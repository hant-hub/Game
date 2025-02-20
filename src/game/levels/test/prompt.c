#include "init.h"
#include "sheet.h"
#include "test.h"
#include "text.h"
#include <GLFW/glfw3.h>


typedef enum {
    TEMP_BACKGROUND = 0,
    PLACEHOLDER_1
} TexID;

static const char* TexPaths[] = {
    TEXPATH("prompt.png"),
    TEXPATH("texture.jpg"),
};

typedef enum {
    BACKGROUND = 0,

} SpriteID;

static const char* text = "This is some sample text";
static const u32 textlen = 24;


void TestPromptInit(GameState* state) {
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
    state->curr.graphics[BACKGROUND] = CreateSpriteSh(state->s, (sm_vec2f){0, 0}, (sm_vec2f){1920*2, 1080*2}, TEMP_BACKGROUND, 10);
}
void TestPromptUpdate(GameState* state, PresentInfo* p) {
    u32 frameCounter = 0;
    double dt = 0;
    double accum = 0;
    UIContext context = {0};

    Scene* sc = &state->curr;

    u32 textpos = 0;

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

        {
            static double thresh = 0;
            if (textpos < textlen && accum > thresh) {
                thresh = accum + 0.01;
                textpos++;
            }
        }
        AppendText(state->t, text, textpos, (sm_vec2f){-1500, 500}, 1.0, 1.5);


        if (glfwGetKey(sr_context.w, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(sr_context.w, GLFW_TRUE);
            return;
        }
        if (glfwGetMouseButton(sr_context.w, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
            textpos = textlen;
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
void TestPromptDestroy(GameState* state) {
    for (int i = 0; i < ARRAY_SIZE(TexPaths); i++) {
        DestroyTexture(&state->curr.textures[i]);
    }
}
