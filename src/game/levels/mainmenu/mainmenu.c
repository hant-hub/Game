#include "mainmenu.h"
#include "game.h"
#include "init.h"
#include "log.h"
#include "sheet.h"
#include "vec2.h"
#include <GLFW/glfw3.h>
#include <math.h>

typedef enum {
    TEX_BACKGROUND = 0,
} TexID;

typedef enum {
    SPRITE_BACKGROUND = 0,
    NUM_SPRITES
} SpriteID;

static const char* TexPaths[] = {
    TEXPATH("Title.png"),
};



void MainMenuInit(GameState* state) {
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

    //sprites
    Scene* sc = &state->curr;

    float scale = 1080.0/195 * 2;
    sc->graphics[SPRITE_BACKGROUND] = CreateSpriteSh(state->s, (sm_vec2f){0,0}, (sm_vec2f){345*scale, 195*scale}, TEX_BACKGROUND, 10);
    ma_PlaySound(&state->audio, TITLE_INTRO);
    
}

void MainMenuUpdate(GameState* state, PresentInfo* p) {
    u32 frameCounter = 0;
    double dt = 0;
    double accum = 0;

    Scene* sc = &state->curr;
    UIContext c = {0};

    int counter = 15;
    double period = 0;
    bool cont = FALSE;

    while (!glfwWindowShouldClose(sr_context.w)) {
        double start = glfwGetTime();
        //poll events
        glfwPollEvents();

        accum += dt;
        ClearText(state->t);

        if (glfwGetKey(sr_context.w, GLFW_KEY_SPACE) == GLFW_PRESS) {
            cont = TRUE;
            period = accum;
        }

        if (cont) {
            if (accum > period && counter) {
                period = accum + 0.06;
                counter--;
                SheetEntry* e = GetSpriteSh(state->s, SPRITE_BACKGROUND);
                double i;
                if (counter % 2) e->color = HSLToRGB(0, 1.0, 7);
                else e->color = HSLToRGB(1.0, 1.0, 1.0);
                SR_LOG_DEB("color: (%f, %f, %f)", e->color.x, e->color.y, e->color.z);
            }

            if (!counter && accum > period + 0.5) {
                state->mode = CARMEN_SLIDES;
                sc->stage = 0;
                return;
            }

        }

        if (!state->audio.playing[TITLE_INTRO]) {
            ma_PlaySound(&state->audio, TITLE_LOOP);
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

void MainMenuDestroy(GameState* state) {
    for (int i = 0; i < NUM_SPRITES; i++) {
        DestroySpriteSh(state->s, state->curr.graphics[i]);
    }
    for (int i = 0; i < ARRAY_SIZE(TexPaths); i++) {
        DestroyTexture(&state->curr.textures[i]);
    }
    DestroyFont(&state->f);
}
