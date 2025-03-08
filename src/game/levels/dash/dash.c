#include "dash.h"
#include "game.h"
#include "sheet.h"

typedef enum {
    SLIDE_1 = 0,
    SLIDE_2,
    SLIDE_3,
    NUM_TEX
} TexID;

typedef enum {
    SPRITE_MAIN = 0,
    NUM_SPRITES,
} SpriteID;


static const char* TexPaths[] = {
    TEXPATH("dash/slide1.png"),
    TEXPATH("dash/slide2.png"),
    TEXPATH("dash/slide3.png"),
};

void DashInit(GameState* state) {
    for (int i = 0; i < NUM_TEX; i++) {
        SR_LOG_DEB("TexID: %d", i);
        CRASH_CALL(LoadTextureConfig(&state->curr.textures[i], TexPaths[i], (TextureConfig) {
                    .anisotropy = FALSE,
                    .accessmode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                    .filter = VK_FILTER_NEAREST,
                    }));
    }

    CRASH_CALL(LoadFont(FONTPATH("dogicapixel.ttf"), 60, &state->f));
    SetFont(state->t, &state->f);

    SHSetTextureSlots(state->s, state->curr.textures, ARRAY_SIZE(TexPaths));

    //Disabled for prompt
    state->wininfo.b.typingEnable = FALSE;
    state->wininfo.b.bufend = 0;
    state->wininfo.b.lineend = 0;


    Scene* sc = &state->curr;

    sc->graphics[SPRITE_MAIN] = CreateSpriteSh(state->s, (sm_vec2f){0,0}, (sm_vec2f){1920*2,1080*2}, SLIDE_1, 10);
}
void DashDestroy(GameState* state) {
    for (int i = 0; i < NUM_SPRITES; i++) {
        DestroySpriteSh(state->s, state->curr.graphics[i]);
    }
    for (int i = 0; i < ARRAY_SIZE(TexPaths); i++) {
        DestroyTexture(&state->curr.textures[i]);
    }
    DestroyFont(&state->f);
}
void DashUpdate(GameState* state, PresentInfo* p) {
    u32 frameCounter = 0;
    double dt = 0;
    double accum = 0;

    Scene* sc = &state->curr;
    UIContext c = {0};

    while (!glfwWindowShouldClose(sr_context.w)) {
        double start = glfwGetTime();
        //poll events
        glfwPollEvents();

        accum += dt;

        bool advance = KeyButtonEx(state, &c, GLFW_KEY_N, 0);
        bool back = KeyButtonEx(state, &c, GLFW_KEY_P, 1);


        if (advance) {
            SheetEntry* e = GetSpriteSh(state->s, sc->graphics[SPRITE_MAIN]);
            e->texture++;
            if (e->texture >= NUM_TEX) {
                state->mode = WU_SLIDE;
                return;
            }
        }

        if (back) {
            SheetEntry* e = GetSpriteSh(state->s, sc->graphics[SPRITE_MAIN]);
            e->texture = e->texture ? e->texture - 1 : 0;
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
