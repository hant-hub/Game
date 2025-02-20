#include "../game.h"
#include "init.h"
#include "log.h"
#include "sheet.h"
#include "text.h"
#include "vec2.h"
#include <GLFW/glfw3.h>


//The softlinewidth only breaks
//on spaces
static const u32 softlinewidth = 20;
//linewidth is the absolute max line
//width, all characters beyond
//this point are reflowed.
static const u32 linewidth = 25;



void CharHandler(GLFWwindow* window, u32 code) {

    WindowInfo* winfo = glfwGetWindowUserPointer(window);
    
    bool* typingEnable = &winfo->b.typingEnable;
    u32* bufend = &winfo->b.bufend;
    u32* lineend = &winfo->b.lineend;
    u32* lines = winfo->b.lines;
    char* typingBuffer = winfo->b.typingBuffer;

    if (!(*typingEnable)) return;

    if (*bufend >= MAX_TYPE_SIZE) {
        return;
    }
    if (bufend && lines[*lineend] >= softlinewidth && code == ' ') {
        if (*bufend >= MAX_TYPE_SIZE || *lineend >= MAX_TYPE_LINES) {
            return;
        }
        typingBuffer[(*bufend)++] = '\n';
        (*lineend)++;
        return;
    }
    if (bufend && lines[*lineend] >= linewidth) {
        if (*bufend >= MAX_TYPE_SIZE || *lineend >= MAX_TYPE_LINES) {
            return;
        }
        typingBuffer[(*bufend)++] = '\n';
        (*lineend)++;
    }
    typingBuffer[(*bufend)++] = code;
    lines[*lineend]++;
    //SR_LOG_DEB("char");
    //SR_LOG_DEB("last: %c", code);
    //SR_LOG_DEB("bufend: %d", bufend);
}

//bind this for typing, will type into a fixed size buffer
//than can be read from
void KeyHandler(GLFWwindow* window, int key, int scancode, int action, int mods) {
    WindowInfo* winfo = glfwGetWindowUserPointer(window);
    
    bool* typingEnable = &winfo->b.typingEnable;
    u32* bufend = &winfo->b.bufend;
    u32* lineend = &winfo->b.lineend;
    u32* lines = winfo->b.lines;
    char* typingBuffer = winfo->b.typingBuffer;

    if (!(*typingEnable)) return;

    if (action != GLFW_PRESS && action != GLFW_REPEAT) {
        return;
    }
    if (key == GLFW_KEY_BACKSPACE) {
        //SR_LOG_DEB("line: %d", lines[lineend]);
        if (*lineend && typingBuffer[(*bufend)-1] == '\n') (*lineend)--;
        else if (lines[*lineend]) lines[*lineend]--;
        if (*bufend) (*bufend)--;
        //SR_LOG_DEB("lineend: %d", lineend);
        return;
    }
    if (key == GLFW_KEY_ENTER) {
        if (*bufend < MAX_TYPE_SIZE && *lineend < MAX_TYPE_LINES) {
            typingBuffer[(*bufend)++] = '\n';
            lines[(*lineend)++]++;
        }
        return;
    }
}

void EditArea(GameState* state, UIContext* c, sm_vec2f pos, float scale, SheetHandle cursor, double accum) {
    WindowInfo* winfo = glfwGetWindowUserPointer(sr_context.w);
    
    bool typingEnable = winfo->b.typingEnable;
    u32* bufend = &winfo->b.bufend;
    char* typingBuffer = (char*)&winfo->b.typingBuffer;

    AppendText(state->t, (char*)typingBuffer, *bufend, pos, 1.0, scale);
    {
        static double thresh = 0;
        static bool flip = FALSE;
        SheetEntry* e = GetSpriteSh(state->s, cursor);
        e->pos = GetTextPos(state->t);
        e->pos.y -= e->size.y/2.5;
        e->pos.x += e->size.x;

        if (typingEnable && accum > thresh) {
            thresh = accum + 0.5;
            e->layer = flip ? -10 : 0;
            flip = !flip;
            return;
        }
        if (!typingEnable) {
            e->layer = -10;
        }
    }
}

bool Button(GameState* s, UIContext* c,
            sm_vec2f pos, sm_vec2f size,
            SheetHandle uuid) {

    //render
    SheetEntry* e = GetSpriteSh(s->s, uuid);    
    e->pos = pos;
    e->size = size;
    
    sm_vec2d cpos; 
    glfwGetCursorPos(sr_context.w, &cpos.x, &cpos.y);

    //SR_LOG_DEB("Screen: %d %d", winWIDTH, winHEIGHT);
    //SR_LOG_DEB("cpos: %f %f", cpos.x, cpos.y);

    cpos = sm_vec2_f64_sub(cpos, (sm_vec2d){winWIDTH/2.0, winHEIGHT/2.0});
    cpos = sm_vec2_f64_mul(cpos, (sm_vec2d){1.0/winHEIGHT, 1.0/winHEIGHT});
    cpos = sm_vec2_f64_mul(cpos, (sm_vec2d){2, 2});
    cpos = sm_vec2_f64_mul(cpos, (sm_vec2d){1080, 1080});
    
    //button logic
    bool result = FALSE;
    if (c->active == uuid) {
        if (glfwGetMouseButton(sr_context.w, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
            c->active = -1;
            result = TRUE;
            e->selection = ButtonTable[BUTTON_INACTIVE];
        }
    } else if (c->hot == uuid) {
        if (glfwGetMouseButton(sr_context.w, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
            e->selection = ButtonTable[BUTTON_ACTIVE];
            c->active = uuid;
        }
    }

    if (
            cpos.x >= (pos.x - size.x/2) &&
            cpos.x <= (pos.x + size.x/2) &&
            cpos.y >= (pos.y - size.y/2) &&
            cpos.y <= (pos.y + size.y/2)
       ) {
        if (glfwGetMouseButton(sr_context.w, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
            e->selection = ButtonTable[BUTTON_HOVER];
            c->hot = uuid;
        }
    } else {
        e->selection = ButtonTable[BUTTON_INACTIVE];
        if (c->hot == uuid) {
            c->hot = -1;
        }
        if (c->active == uuid) {
            c->active = -1;
        }
    }


    return result;
}

bool AnimateText(GameState* s, sm_vec2f pos, double accum) {



    return FALSE;
}
