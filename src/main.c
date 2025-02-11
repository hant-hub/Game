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

int main() {

    CRASH_CALL(CreateVulkan());

    SubPass passes[2] = {0};
    Attachment attachments[SR_SHEET_ATTACHMENT_NUM + SR_TEXT_NUM_ATTACHMENTS] = {0};

    SpriteGetSubpass(&passes[0], attachments, 0);
    TextGetSubpass(&passes[1], attachments, SR_SHEET_ATTACHMENT_NUM);

    PresentInfo p = {0};
    CRASH_CALL(InitPresent(&p, passes, 2, attachments, SR_SHEET_ATTACHMENT_NUM + SR_TEXT_NUM_ATTACHMENTS));

    SpriteRenderer* s = calloc(sizeof(SpriteRenderer), 1);
    TextRenderer* t = calloc(sizeof(TextRenderer), 1);
    CRASH_CALL(SpriteInit(s, &p.p, 0, (Camera){
                .pos = (sm_vec2f){0, 0},
                .rotation = 0,
                .size = (sm_vec2f){1080, 1080}}, 1));

    CRASH_CALL(TextInit(t, "resources/fonts/JetBrainsMonoNLNerdFontPropo-Regular.ttf", &p.p, 1));

    Texture textures[1] = {0};
    CRASH_CALL(LoadTexture(&textures[0], "resources/textures/texture.jpg"))
    CRASH_CALL(SetTextureSlot(s, &textures[0], 0))

    SheetHandle s1 = CreateSprite(s, (sm_vec2f){0,0}, (sm_vec2f){400,400}, 0, 0);
    {
        SheetEntry* e = GetSprite(s, s1);
        e->scale = (sm_vec2f){2, 2};
    }

    u32 frameCounter = 0;
    while (!glfwWindowShouldClose(sr_context.w)) {
        //poll events
        glfwPollEvents();

        //game logic

        //Set UI

        //draw frame
        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        StartFrame(&p, frameCounter);
        SpriteDrawFrame(s, &p, frameCounter);
        NextPass(&p, frameCounter);
        TextDrawFrame(t, &p, frameCounter);
        SubmitFrame(&p, frameCounter);
    }

    DestroyTexture(&textures[0]);
    DestroySprite(s, s1);
    TextDestroy(t);
    SpriteDestroy(s);
    free(s);
    free(t);
    DestroyPresent(&p);
    DestroyVulkan();
    return 0;
}
