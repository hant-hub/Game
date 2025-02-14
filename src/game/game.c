#include "game.h"
#include "log.h"
#include "text.h"
#include <GLFW/glfw3.h>

#define MAX_TYPE_SIZE 1500
#define MAX_TYPE_LINES 100

//The softlinewidth only breaks
//on spaces
static u32 softlinewidth = 30;
//linewidth is the absolute max line
//width, all characters beyond
//this point are reflowed.
static u32 linewidth = 40;

//This is the buffer
static u32 bufend = 0;
static u32 lineend = 0;
static u32 lines[MAX_TYPE_LINES] = {0};
static char typingBuffer[MAX_TYPE_SIZE] = {0};


void CharHandler(GLFWwindow* window, u32 code) {
    if (bufend >= MAX_TYPE_SIZE) {
        return;
    }
    if (bufend && lines[lineend] >= softlinewidth && code == ' ') {
        if (bufend >= MAX_TYPE_SIZE || lineend >= MAX_TYPE_LINES) {
            return;
        }
        typingBuffer[bufend++] = '\n';
        lineend++;
        return;
    }
    if (bufend && lines[lineend] >= linewidth) {
        if (bufend >= MAX_TYPE_SIZE || lineend >= MAX_TYPE_LINES) {
            return;
        }
        typingBuffer[bufend++] = '\n';
        lineend++;
    }
    typingBuffer[bufend++] = code;
    lines[lineend]++;
    //SR_LOG_DEB("char");
    //SR_LOG_DEB("last: %c", code);
    //SR_LOG_DEB("bufend: %d", bufend);
}

//bind this for typing, will type into a fixed size buffer
//than can be read from
void KeyHandler(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT) {
        return;
    }
    if (key == GLFW_KEY_BACKSPACE) {
        //SR_LOG_DEB("line: %d", lines[lineend]);
        if (lineend && typingBuffer[bufend-1] == '\n') lineend--;
        else if (lines[lineend]) lines[lineend]--;
        if (bufend) bufend--;
        //SR_LOG_DEB("lineend: %d", lineend);
        return;
    }
    if (key == GLFW_KEY_ENTER) {
        if (bufend < MAX_TYPE_SIZE && lineend < MAX_TYPE_LINES) {
            typingBuffer[bufend++] = '\n';
            lines[lineend++]++;
        }
        return;
    }
}

//internal function
void HandleInput(GameState* s) {
    return;
}

//this is just for the demo. Probably replace this
//with an array of function pointers
//This is just a demo
//This one requires a poem to have lines which match the fibbonacci sequence
//ie: 1, 2, 3, 5, 8, 13, etc.
int DemoJudge() {
    const static int fib[6] = {
        1, 2, 3, 5, 8, 13
    };

    int lines = 0;
    int spaces = 1;
    int incorrect = 0;
    for (int i = 0; i < bufend; i++) {
        if (lines >= 6) break;
        if (typingBuffer[i] == ' ') spaces++;
        if (typingBuffer[i] == '\n') {
            if (spaces != fib[lines++]) incorrect++;
            spaces = 1;
        }
    }
    SR_LOG_DEB("lines: %d", lines);
    SR_LOG_DEB("Incorrect: %d", incorrect);
    return incorrect;
}

void Update(GameState* s, float dt) {
    static int lastbuf = 0;

    if (lastbuf != bufend) {
        int i = DemoJudge();
        lastbuf = bufend;
    }
    return;
}


//main game tick. This is for gameplay only.
//Will create a menu tick for menu stuff
void UItick(GameState* state, TextRenderer* t, SpriteRenderer* s) {
    ClearText(t);

    AppendText(t, typingBuffer, bufend, (sm_vec2f){-1180, -800}, 1);
    return;
}
