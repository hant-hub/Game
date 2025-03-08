#include "game.h"
#include "config.h"
#include "fractal.h"
#include "frame.h"
#include "init.h"
#include "sheet.h"
#include "util.h"
#include <GLFW/glfw3.h>
#include "vec3.h"

static sm_vec2f pos = {0};
static sm_vec2f zinit = {0};
static float zoom = 1.5f;
static float pw = 4;


void EliPoemUpdate(GameState* state, PresentInfo* p){
}
void EliPoemDestroy(GameState* state){
}

void EliPoemInit(GameState* state) {

    DestroyPresent(state->p);
    SheetDestroy(state->s);
    TextDestroy(state->t);

    FractalRenderer ren = {0};
    FractalRenderer* r = &ren;
    PresentInfo p = {0};
    Attachment attachments[SR_SHEET_ATTACHMENT_NUM] = {0};
    SubPass passes[1] = {0};

    FractalGetSubpass(passes, attachments, 0);
    CRASH_CALL(InitPresent(&p, passes, 1, attachments, 1));
    CRASH_CALL(FractalInit(r,"julia", &p.p, 0));

    FractalSetPos(r, (sm_vec2f){0.0f, 0.0f});
    FractalSetZoom(r, 1.5f);
    FractalSetColor(r, (sm_vec3f){2.0f, 0.0f, 2.0f});

    //build multipass

    unsigned int frameCounter = 0;
    double last = 0.0;
    bool flip = FALSE;
    
    float dt = 0.0f;
    zoom = 2.2;

    while (!glfwWindowShouldClose(sr_context.w)) {
        double start = glfwGetTime();
        glfwPollEvents();

        zinit.x = 0.8*cos(glfwGetTime()*0.55);
        zinit.y = 0.8*sin(glfwGetTime()*0.5);




//        sm_vec3f color = (sm_vec3f) {
//            sin(glfwGetTime()),
//            cos(glfwGetTime()) * sin(glfwGetTime() + SM_PI/2),
//            cos(glfwGetTime()) * cos(glfwGetTime() + SM_PI/2)
//        };
//
//        color = (sm_vec3f) {
//            color.x * color.x + 0.1,
//            color.y * color.y + 0.1,
//            color.z * color.z + 0.1f
//        };
//
        sm_vec3f color = HSLToRGB(cos(glfwGetTime()), 0.8, 2.0);
        FractalSetPos(r, pos);
        FractalSetZoom(r, zoom);
        FractalSetColor(r, color);
        FractalSetInit(r, zinit);

        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        StartFrame(&p, frameCounter);
        FractalDrawFrame(r, &p, frameCounter);
        SubmitFrame(&p, frameCounter); 

        dt = glfwGetTime() - start;

    }

    FractalDestroy(r);
    DestroyPresent(&p);
    DestroyVulkan();
    exit(EXIT_SUCCESS);
}
