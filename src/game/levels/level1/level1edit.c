#include "game.h"
#include "init.h"
#include "log.h"
#include "sheet.h"
#include "text.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include <stdio.h>
#include <string.h>
#include <GLFW/glfw3.h>
#include "level1poems.h"

typedef enum {
    TEX_CHISEL = 0,
    TEX_JUDGE,
    TEX_PROMPT,
    TEX_EDIT,
    TEX_SPEECH,
    TEX_SUBMIT,
} TexID;

typedef enum {
    SPRITE_CHISEL = 0,
    SPRITE_EDIT,
    SPRITE_PROMPT,
    SPRITE_SPEECH_TOP,
    SPRITE_SPEECH_MIDDLE,
    SPRITE_SPEECH_BOTTOM,
    SPRITE_SUBMIT,
    SPRITE_JUDGE,
    SPRITE_CURSOR,
    NUM_SPRITES
} SpriteID;

static const char* TexPaths[] = {
    TEXPATH("level1/Chisel.png"),
    TEXPATH("level1/Judge_1.png"),
    TEXPATH("level1/L_window_1.png"),
    TEXPATH("level1/R_window_1.png"),
    TEXPATH("level1/speech_bubble.png"),
    TEXPATH("level1/submit_button_1.png"),
};


void Level1Init(GameState* state) {
    //load textures
    
    for (int i = 0; i < ARRAY_SIZE(TexPaths); i++) {
        SR_LOG_DEB("TexID: %d", i);
        CRASH_CALL(LoadTextureConfig(&state->curr.textures[i], TexPaths[i], (TextureConfig) {
                    .anisotropy = FALSE,
                    .accessmode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                    .filter = VK_FILTER_NEAREST,
                    }));
    }

    //load font
    CRASH_CALL(LoadFont(FONTPATH("dogicapixel.ttf"), 60, &state->f));
    SetFont(state->t, &state->f);

    SHSetTextureSlots(state->s, state->curr.textures, ARRAY_SIZE(TexPaths));

    //init for editing
    state->wininfo.b.typingEnable = TRUE;
    state->wininfo.b.bufend = 0;
    state->wininfo.b.lineend = 0;

    //reserve lower ten layers for text
    //upper layers for sprites
    //Will have to break that for the Feedback
    
    //Create Sprites
    Scene* curr = &state->curr;
    curr->graphics[SPRITE_EDIT] = CreateSpriteSh(state->s, (sm_vec2f){1080.0/1.5, 0}, (sm_vec2f){1080*2.49,1080*2}, TEX_EDIT, 6);
    curr->graphics[SPRITE_PROMPT] = CreateSpriteSh(state->s, (sm_vec2f){-1350, 0}, (sm_vec2f){1080*1.35, 1080*2}, TEX_PROMPT, 6);
    curr->graphics[SPRITE_SUBMIT] = CreateSpriteSh(state->s, (sm_vec2f){-1380, 850}, (sm_vec2f){1000, 350}, TEX_SUBMIT, 5);
    curr->graphics[SPRITE_CURSOR] = CreateSpriteSh(state->s, (sm_vec2f){}, (sm_vec2f){30,70}, TEX_PROMPT, 4);
    {
        SheetEntry* e = GetSpriteSh(state->s, curr->graphics[SPRITE_CURSOR]);
        e->scale = (sm_vec2f){29, 23};
        e->selection = (sm_vec2f){0, 0};
    }

    //important, transparent textures must be rendered last
    //This is hacky but required for now
    curr->graphics[SPRITE_CHISEL] = CreateSpriteSh(state->s, (sm_vec2f){1900, -900}, (sm_vec2f){1000, 1000}, TEX_CHISEL, 4);
    {
        SheetEntry* e = GetSpriteSh(state->s, curr->graphics[SPRITE_SUBMIT]);
        e->scale = (sm_vec2f){3, 1};
        e->selection = (sm_vec2f){2, 0};
    }

    //disabled for now
    curr->graphics[SPRITE_JUDGE] = CreateSpriteSh(state->s, (sm_vec2f){-1080*3, 0}, (sm_vec2f){1080*2, 1080*2}, TEX_JUDGE, -10);
    curr->graphics[SPRITE_SPEECH_TOP] = CreateSpriteSh(state->s, (sm_vec2f){0, -700}, (sm_vec2f){50*35, 6*35}, TEX_SPEECH, -10);
    curr->graphics[SPRITE_SPEECH_MIDDLE] = CreateSpriteSh(state->s, (sm_vec2f){0, -420}, (sm_vec2f){50*35, 14*35}, TEX_SPEECH, -10);
    curr->graphics[SPRITE_SPEECH_BOTTOM] = CreateSpriteSh(state->s, (sm_vec2f){-75, -420 + 9*35}, (sm_vec2f){50*38, 6*35}, TEX_SPEECH, -10);
    {
        SheetEntry* e = GetSpriteSh(state->s, curr->graphics[SPRITE_SPEECH_TOP]);
        e->scale = (sm_vec2f){3, 1};
        e->selection = (sm_vec2f){0, 0};
    }
    {
        SheetEntry* e = GetSpriteSh(state->s, curr->graphics[SPRITE_SPEECH_MIDDLE]);
        e->scale = (sm_vec2f){3, 1};
        e->selection = (sm_vec2f){2, 0};
    }
    {
        SheetEntry* e = GetSpriteSh(state->s, curr->graphics[SPRITE_SPEECH_BOTTOM]);
        e->scale = (sm_vec2f){3, 1};
        e->selection = (sm_vec2f){1, 0};
    }

}
void Level1Destroy(GameState* state) {
    for (int i = 0; i < NUM_SPRITES; i++) {
        DestroySpriteSh(state->s, state->curr.graphics[i]);
    }
    for (int i = 0; i < ARRAY_SIZE(TexPaths); i++) {
        DestroyTexture(&state->curr.textures[i]);
    }
    DestroyFont(&state->f);
}
void Level1Update(GameState* state, PresentInfo* p) {
    u32 frameCounter = 0;
    double dt = 0;
    double accum = 0;

    Scene* sc = &state->curr;
    UIContext c = {0};

    bool feedback = FALSE;
    double slide = 0; //for animation
    u32 textpos = 0; //text animation
    
    u32 feedbackidx = 0;
    u32 results[10] = {0};

    u32 substage = sc->stage;
    SR_LOG_DEB("stage: %d", substage);

    while (!glfwWindowShouldClose(sr_context.w)) {
        double start = glfwGetTime();
        //poll events
        glfwPollEvents();

        accum += dt;
        ClearText(state->t);
        SetColor(state->t, (sm_vec3f){0.3, 0.3, 0.3});
        EditArea(state, &c, (sm_vec2f){-400, -820}, 3, 1.2, sc->graphics[SPRITE_CURSOR], accum);
        SetColor(state->t, (sm_vec3f){1.0, 1.0, 1.0});

        if (results[0]) SetColor(state->t, (sm_vec3f){0.5, 0.0, 0.0});
        else SetColor(state->t, (sm_vec3f){0.0, 0.5, 0.0});
        AppendText(state->t, 
                Requirements[stagerequirements[substage].startidx],
                strlen(Requirements[stagerequirements[substage].startidx]), 
                (sm_vec2f){-1080*1.8, -800}, 3.5, 1.3);
        for (int i = 1; i < stagerequirements[substage].num; i++) {
            if (results[i]) SetColor(state->t, (sm_vec3f){0.5, 0.0, 0.0});
            else SetColor(state->t, (sm_vec3f){0.0, 0.5, 0.0});
            float y = GetTextPos(state->t).y;
            AppendText(state->t, 
                    Requirements[stagerequirements[substage].startidx + i],
                    strlen(Requirements[stagerequirements[substage].startidx + i]), 
                    (sm_vec2f){-1080*1.8, y + 60*1.5}, 3.5, 1.3);
        }

        if (!feedback) {
            sm_vec2d cpos; 
            glfwGetCursorPos(sr_context.w, &cpos.x, &cpos.y);
            cpos = sm_vec2_f64_sub(cpos, (sm_vec2d){winWIDTH/2.0, winHEIGHT/2.0});
            cpos = sm_vec2_f64_mul(cpos, (sm_vec2d){1.0/winHEIGHT, 1.0/winHEIGHT});
            cpos = sm_vec2_f64_mul(cpos, (sm_vec2d){2, 2});
            cpos = sm_vec2_f64_mul(cpos, (sm_vec2d){1080, 1080});
            //SR_LOG_DEB("cpos: %f %f", cpos.x, cpos.y);

            for (int i = 0; i < ARRAY_SIZE(results); i++) {
                results[i] = 0;
            }
            essentialfuncs[substage](state->wininfo.b.typingBuffer, state->wininfo.b.bufend, results);

            bool enableButton = TRUE;
            for (int i = 0; i < ARRAY_SIZE(results); i++) {
                if (results[i]) enableButton = FALSE;
            }
            if (enableButton && Button(state, &c, (sm_vec2f){-1380, 850}, (sm_vec2f){1000, 350}, sc->graphics[SPRITE_SUBMIT])) {
                char savename[32];
                snprintf(savename, 32, "./save/level1-%d.txt", substage + 1);
                FILE* save = fopen(savename, "w");
                SR_LOG_DEB("save: %ld", (u64)save);
                fprintf(save, "\n\n%.*s", state->wininfo.b.bufend, state->wininfo.b.typingBuffer);
                fclose(save);
                feedback = TRUE;
                state->wininfo.b.typingEnable = FALSE;
                {
                    SheetEntry* e = GetSpriteSh(state->s, sc->graphics[SPRITE_JUDGE]);
                    e->layer = 3;
                }
                feedbackidx = optionalfuncs[substage](state->wininfo.b.typingBuffer, state->wininfo.b.bufend);
                feedbackidx += stagefeedback[substage].startidx;
            } else if (!enableButton) {
                SheetEntry* e = GetSpriteSh(state->s, sc->graphics[SPRITE_SUBMIT]);
                e->selection = ButtonTable[BUTTON_INACTIVE];
            }


        } else {
            const sm_vec2f start = (sm_vec2f){-1080*2.5, 0};
            const sm_vec2f end = (sm_vec2f){-1080, 0};

            if (slide < 1.0) slide += 0.9 * dt;

            sm_vec2f judgepos = sm_vec2_f32_add(
                    sm_vec2_f32_mul((sm_vec2f){slide, slide}, end),
                    sm_vec2_f32_mul((sm_vec2f){1-slide, 1-slide}, start)
                    );

            {
                SheetEntry* e = GetSpriteSh(state->s, sc->graphics[SPRITE_JUDGE]);
                e->pos = judgepos;
            }

            if (slide >= 1.0) {
                //do feedback

                {
                    SheetEntry* eb = GetSpriteSh(state->s, sc->graphics[SPRITE_SPEECH_BOTTOM]);
                    SheetEntry* em = GetSpriteSh(state->s, sc->graphics[SPRITE_SPEECH_MIDDLE]);
                    SheetEntry* et = GetSpriteSh(state->s, sc->graphics[SPRITE_SPEECH_TOP]);

                    eb->layer = 2;
                    em->layer = 1;
                    et->layer = 2;
                }

                SetColor(state->t, (sm_vec3f){0.0, 0.0, 0.0});
                u32 textlen = strlen(feedbacktext[feedbackidx]);
                if (glfwGetMouseButton(sr_context.w, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
                    textpos = textlen;
                }
                                
                {
                    static double thresh = 0;
                    if (textpos < textlen && accum > thresh) {
                        thresh = accum + 0.01;
                        textpos++;
                    }
                }
                AppendText(state->t, feedbacktext[feedbackidx], textpos, (sm_vec2f){-75 - 50*15, -560}, 0.6, 1.2);

                if (textpos >= textlen) {
                    if (glfwGetKey(sr_context.w, GLFW_KEY_SPACE) == GLFW_PRESS ||
                        glfwGetMouseButton(sr_context.w, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
                        sc->stage++;
                        if (sc->stage >= numstages) {
                            state->mode = LEVEL2_PROMPT;
                        } else {
                            state->mode = LEVEL1_PROMPT;
                        }
                        return;
                    }
                }

            }

        }

        if (glfwGetKey(sr_context.w, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(sr_context.w, GLFW_TRUE);
            return;
        }

        //draw frame
        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        StartFrame(p, frameCounter);
        SheetDrawFrame(state->s, p, frameCounter);
        NextPass(p, frameCounter);
        TextDrawFrame(state->t, p, frameCounter);
        SubmitFrame(p, frameCounter);

        double end = glfwGetTime();
        dt = end - start;
    }
}
