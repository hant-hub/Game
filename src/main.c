#include "config.h"
#include "frame.h"
#include "init.h"
#include "level1/level1.h"
#include "level2/level2.h"
#include "levelselect.h"
#include "test.h"
#include "util.h"
#include <GLFW/glfw3.h>
#include <log.h>
#include <sheet.h>
#include <stdlib.h>
#include <text.h>
#include "game/game.h"
#include "test.h"

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
                .size = (sm_vec2f){1080, 1080}}, 20));

    CRASH_CALL(TextInit(t, NULL, &p.p, 1));
    SetArea(t, (sm_vec2f){1080, 1080});


    GameState state = {0};



    InitGame(&state, t, s);
    while (!glfwWindowShouldClose(sr_context.w)) {
        switch (state.mode) {
            case LEVEL1_EDIT:
                {
                    Level1Init(&state);
                    Level1Update(&state, &p); 
                    Level1Destroy(&state);
                }
            break;
            case LEVEL1_PROMPT:
                {
                    Level1PromptInit(&state);
                    Level1PromptUpdate(&state, &p); 
                    Level1PromptDestroy(&state);
                }
            break;
            case LEVEL2_PROMPT:
                {
                    Level2PromptInit(&state);
                    Level2PromptUpdate(&state, &p); 
                    Level2PromptDestroy(&state);
                }
            break;
            case LEVEL2_EDIT:
                {

                    Level2Init(&state);
                    Level2Update(&state, &p); 
                    Level2Destroy(&state);
                }
            break;
            case PLACEHOLDER_EDIT: 
                {
                    TestInit(&state);
                    TestUpdate(&state, &p); 
                    TestDestroy(&state);
                }
            break;
            case PLACEHOLDER_PROMPT:
                {
                    TestPromptInit(&state);
                    TestPromptUpdate(&state, &p); 
                    TestPromptDestroy(&state);
                }
            break;
            case LEVEL_SELECT:
            {
                    LevelSelectInit(&state);
                    LevelSelectUpdate(&state, &p); 
                    LevelSelectDestroy(&state);
            }
            break;
            default:
                {
                    SR_LOG_ERR("Invalid Game Mode");
                    glfwSetWindowShouldClose(sr_context.w, GLFW_TRUE);
                }
            break;
        }
    }

    DestroyGame(&state, t, s);
    TextDestroy(t);
    SheetDestroy(s);
    free(s);
    free(t);
    DestroyPresent(&p);
    DestroyVulkan();
    return 0;
}
