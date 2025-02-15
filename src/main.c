#include "config.h"
#include "frame.h"
#include "init.h"
#include "texture.h"
#include "util.h"
#include <GLFW/glfw3.h>
#include <log.h>
#include <sheet.h>
#include <stdlib.h>
#include <text.h>
#include "game/game.h"

int main() {

    CRASH_CALL(CreateVulkan());

    SubPass passes[2] = {0};
    Attachment attachments[SR_SHEET_ATTACHMENT_NUM + SR_TEXT_NUM_ATTACHMENTS] = {0};

    SheetGetSubpass(&passes[0], attachments, 0);
    TextGetSubpass(&passes[1], attachments, SR_SHEET_ATTACHMENT_NUM);

    PresentInfo p = {0};
    CRASH_CALL(InitPresent(&p, passes, 2, attachments, SR_SHEET_ATTACHMENT_NUM + SR_TEXT_NUM_ATTACHMENTS));

    SheetRenderer* s = calloc(sizeof(SheetRenderer), 1);
    TextRenderer* t = calloc(sizeof(TextRenderer), 1);
    CRASH_CALL(SheetInit(s, &p.p, 0, (Sh_Camera){
                .pos = (sm_vec2f){0, 0},
                .rotation = 0,
                .size = (sm_vec2f){1080, 1080}}, 1));

    CRASH_CALL(TextInit(t, "resources/fonts/JetBrainsMonoNLNerdFontPropo-Regular.ttf", 60, &p.p, 1));
    SetArea(t, (sm_vec2f){1080, 1080});

    Texture textures[1] = {0};
    CRASH_CALL(LoadTexture(&textures[0], "resources/textures/texture.jpg"))
    CRASH_CALL(SHSetTextureSlot(s, &textures[0], 0))

    SheetHandle s1 = CreateSpriteSh(s, (sm_vec2f){0,0}, (sm_vec2f){400,400}, 0, 0);
    {
        SheetEntry* e = GetSpriteSh(s, s1);
        e->scale = (sm_vec2f){2, 2};
    }

    u32 frameCounter = 0;
    SetColor(t, (sm_vec3f){1.0, 1.0, 1.0});
    GameState state = {0};
    double dt = 0;
    glfwSetCharCallback(sr_context.w, CharHandler);
    glfwSetKeyCallback(sr_context.w, KeyHandler);
    while (!glfwWindowShouldClose(sr_context.w)) {
        double start = glfwGetTime();
        //poll events
        glfwPollEvents();

        //game logic
        Update(&state, dt);

        //Set UI
        UItick(&state, t, s);

        //draw frame
        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        StartFrame(&p, frameCounter);
        SheetDrawFrame(s, &p, frameCounter);
        NextPass(&p, frameCounter);
        TextDrawFrame(t, &p, frameCounter);
        SubmitFrame(&p, frameCounter);

        double end = glfwGetTime();
        dt = end - start;
    }

    DestroyTexture(&textures[0]);
    DestroySpriteSh(s, s1);
    TextDestroy(t);
    SheetDestroy(s);
    free(s);
    free(t);
    DestroyPresent(&p);
    DestroyVulkan();
    return 0;
}
