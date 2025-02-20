#include "game.h"
#include "level1.h"
#include "sheet.h"
#include "text.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"

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
    curr->graphics[SPRITE_EDIT] = CreateSpriteSh(state->s, (sm_vec2f){1080.0/1.5, 0}, (sm_vec2f){1080*2.49,1080*2}, TEX_EDIT, 5);
    curr->graphics[SPRITE_PROMPT] = CreateSpriteSh(state->s, (sm_vec2f){-1350, 0}, (sm_vec2f){1080*1.35, 1080*2}, TEX_PROMPT, 5);
    curr->graphics[SPRITE_SUBMIT] = CreateSpriteSh(state->s, (sm_vec2f){-1380, 850}, (sm_vec2f){1000, 350}, TEX_SUBMIT, 4);
    curr->graphics[SPRITE_CURSOR] = CreateSpriteSh(state->s, (sm_vec2f){}, (sm_vec2f){40,150}, TEX_PROMPT, 3);
    {
        SheetEntry* e = GetSpriteSh(state->s, curr->graphics[SPRITE_CURSOR]);
        e->scale = (sm_vec2f){29, 23};
        e->selection = (sm_vec2f){0, 0};
    }

    //important, transparent textures must be rendered last
    //This is hacky but required for now
    curr->graphics[SPRITE_CHISEL] = CreateSpriteSh(state->s, (sm_vec2f){1800, -900}, (sm_vec2f){1000, 1000}, TEX_CHISEL, 3);
    {
        SheetEntry* e = GetSpriteSh(state->s, curr->graphics[SPRITE_SUBMIT]);
        e->scale = (sm_vec2f){3, 1};
        e->selection = (sm_vec2f){2, 0};
    }

    //disabled for now
    curr->graphics[SPRITE_JUDGE] = CreateSpriteSh(state->s, (sm_vec2f){}, (sm_vec2f){}, TEX_JUDGE, -10);
    curr->graphics[SPRITE_SPEECH_TOP] = CreateSpriteSh(state->s, (sm_vec2f){}, (sm_vec2f){}, TEX_SPEECH, -10);
    curr->graphics[SPRITE_SPEECH_MIDDLE] = CreateSpriteSh(state->s, (sm_vec2f){}, (sm_vec2f){}, TEX_SPEECH, -10);
    curr->graphics[SPRITE_SPEECH_BOTTOM] = CreateSpriteSh(state->s, (sm_vec2f){}, (sm_vec2f){}, TEX_SPEECH, -10);
    {
        SheetEntry* e = GetSpriteSh(state->s, curr->graphics[SPRITE_SPEECH_TOP]);
        e->scale = (sm_vec2f){3, 1};
        e->selection = (sm_vec2f){0, 0};
    }
    {
        SheetEntry* e = GetSpriteSh(state->s, curr->graphics[SPRITE_SPEECH_MIDDLE]);
        e->scale = (sm_vec2f){3, 1};
        e->selection = (sm_vec2f){1, 0};
    }
    {
        SheetEntry* e = GetSpriteSh(state->s, curr->graphics[SPRITE_SPEECH_BOTTOM]);
        e->scale = (sm_vec2f){3, 1};
        e->selection = (sm_vec2f){2, 0};
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


    while (!glfwWindowShouldClose(sr_context.w)) {
        double start = glfwGetTime();
        //poll events
        glfwPollEvents();

        sm_vec2d cpos; 
        glfwGetCursorPos(sr_context.w, &cpos.x, &cpos.y);
        cpos = sm_vec2_f64_sub(cpos, (sm_vec2d){winWIDTH/2.0, winHEIGHT/2.0});
        cpos = sm_vec2_f64_mul(cpos, (sm_vec2d){1.0/winHEIGHT, 1.0/winHEIGHT});
        cpos = sm_vec2_f64_mul(cpos, (sm_vec2d){2, 2});
        cpos = sm_vec2_f64_mul(cpos, (sm_vec2d){1080, 1080});
        //SR_LOG_DEB("cpos: %f %f", cpos.x, cpos.y);

        accum += dt;
        ClearText(state->t);
        SetColor(state->t, (sm_vec3f){0.3, 0.3, 0.3});
        EditArea(state, &c, (sm_vec2f){-400, -820}, 2.0, sc->graphics[SPRITE_CURSOR], accum);
        SetColor(state->t, (sm_vec3f){1.0, 1.0, 1.0});
        if (Button(state, &c, (sm_vec2f){-1380, 850}, (sm_vec2f){1000, 350}, sc->graphics[SPRITE_SUBMIT])) {
            SR_LOG_DEB("submit");
            state->mode = PLACEHOLDER_PROMPT;
            return;
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
