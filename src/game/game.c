#include "game.h"
#include "init.h"
#include "sheet.h"
#include "text.h"
#include "util.h"
#include <GLFW/glfw3.h>



//internal function
void HandleInput(GameState* s) {
    return;
}


void Update(GameState* s, float dt) {
    return;
}


void InitGame(GameState* state, TextRenderer* t, SheetRenderer* s) {

    glfwSetWindowUserPointer(sr_context.w, &state->wininfo);

    //init for editing
    state->wininfo.b.typingEnable = FALSE;
    state->wininfo.b.bufend = 0;
    state->wininfo.b.lineend = 0;
    glfwSetCharCallback(sr_context.w, CharHandler);
    glfwSetKeyCallback(sr_context.w, KeyHandler);

    state->t = t;
    state->s = s;

    //state->mode = PLACEHOLDER_EDIT;
    state->mode = LEVEL1_EDIT;
}

void DestroyGame(GameState* state, TextRenderer* t, SheetRenderer* s) {
}

//main game tick. This is for gameplay only.
//Will create a menu tick for menu stuff
void UItick(GameState* state, TextRenderer* t, SheetRenderer* s, double dt) {
    return;
}
