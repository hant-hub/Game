#include "eli.h"
#include "game.h"
#include <GLFW/glfw3.h>


typedef enum {
    SPRITE_MAIN = 0,
    NUM_SPRITES,
} SpriteID;

void EliTextInit(GameState* state) {
    //for (int i = 0; i < NUM_TEX; i++) {
    //    SR_LOG_DEB("TexID: %d", i);
    //    CRASH_CALL(LoadTextureConfig(&state->curr.textures[i], TexPaths[i], (TextureConfig) {
    //                .anisotropy = FALSE,
    //                .accessmode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    //                .filter = VK_FILTER_NEAREST,
    //                }));
    //}

    CRASH_CALL(LoadFont(FONTPATH("dogicapixel.ttf"), 60, &state->f));
    SetFont(state->t, &state->f);

    //SHSetTextureSlots(state->s, state->curr.textures, ARRAY_SIZE(TexPaths));

    //Disabled for prompt
    state->wininfo.b.typingEnable = FALSE;
    state->wininfo.b.bufend = 0;
    state->wininfo.b.lineend = 0;

}

void EliTextDestroy(GameState* state) {
    DestroyFont(&state->f);
}

void EliTextUpdate(GameState* state, PresentInfo* p) {
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


        ClearText(state->t);
        for (int i = 0; i < 50; i++) {
            for (int j = 0; j < 60; j++) {
                float shift = glfwGetTime();
                float scale;
                shift = shift - floor(shift);
                {
                    double temp;
                    int hash = j;
                    hash = ((hash << 5) + hash) + i;
                    hash = (hash << 5) + hash;
                    hash ^= 230957;
                    scale = hash;
                    scale += glfwGetTime();
                }
                SetColor(state->t, HSLToRGB(sin(scale/10.0 + glfwGetTime()), 1.0, 0.7 * cos(scale/10.0 + glfwGetTime()) + 0.7));
                AppendText(state->t, "Blam", 4, (sm_vec2f){60 * 2 * i - 1080*2, 60 * j + shift - 1080*2}, i + 10, 0.3 * sin(scale) + 0.5);
            }
        }

        bool advance = KeyButtonEx(state, &c, GLFW_KEY_N, 0);
        if (advance) {
            state->mode = LEVEL2_PROMPT;
            sc->stage = 3;
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



