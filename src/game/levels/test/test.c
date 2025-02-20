#include "test.h"
#include "frame.h"
#include "game.h"
#include "init.h"
#include "sheet.h"
#include <GLFW/glfw3.h>


typedef enum {
    PLACEHOLDER_1 = 0,
    TEMP_BACKGROUND
} TexID;

static const char* TexPaths[] = {
    TEXPATH("texture.jpg"),
    TEXPATH("mainscreen.png")
};

#define BACKGROUND_IDX 0
#define CURSOR_IDX 1
#define SUBMIT_IDX 2


void TestInit(GameState* state) {
    //load textures
    
    for (int i = 0; i < ARRAY_SIZE(TexPaths); i++) {
        SR_LOG_DEB("TexID: %d", i);
        CRASH_CALL(LoadTexture(&state->curr.textures[i], TexPaths[i]));
    }

    SHSetTextureSlots(state->s, state->curr.textures, ARRAY_SIZE(TexPaths));

    //init for editing
    state->wininfo.b.typingEnable = TRUE;
    state->wininfo.b.bufend = 0;
    state->wininfo.b.lineend = 0;

    //background image
    state->curr.graphics[BACKGROUND_IDX] = CreateSpriteSh(state->s, (sm_vec2f){0, 0}, (sm_vec2f){1920*2, 1080*2}, TEMP_BACKGROUND, 10);

    //cusor for typing
    state->curr.graphics[CURSOR_IDX] = CreateSpriteSh(state->s, (sm_vec2f){0,0}, (sm_vec2f){20*1.5, 60*1.5}, PLACEHOLDER_1, -10);

    state->curr.graphics[SUBMIT_IDX] = CreateSpriteSh(state->s, (sm_vec2f){0,0}, (sm_vec2f){20*1.5, 60*1.5}, PLACEHOLDER_1, 1);
    {
        SheetEntry* e = GetSpriteSh(state->s, state->curr.graphics[SUBMIT_IDX]);
        e->scale = (sm_vec2f){2,2};
        e->selection = (sm_vec2f){0,0};
    }
}

void TestDestroy(GameState* state) {
    DestroySpriteSh(state->s, state->curr.graphics[BACKGROUND_IDX]);
    DestroySpriteSh(state->s, state->curr.graphics[CURSOR_IDX]);
    DestroySpriteSh(state->s, state->curr.graphics[SUBMIT_IDX]);
    for (int i = 0; i < ARRAY_SIZE(TexPaths); i++) {
        DestroyTexture(&state->curr.textures[i]);
    }
}

void TestUpdate(GameState* state, PresentInfo* p) {
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
        SetColor(state->t, (sm_vec3f){0.0, 0.0, 0.0});
        SetColor(state->t, (sm_vec3f){1.0, 1.0, 1.0});
        if (Button(state, &c, (sm_vec2f){-1380, 850}, (sm_vec2f){1000, 350}, sc->graphics[SUBMIT_IDX])) {
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


