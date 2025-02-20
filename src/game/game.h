#ifndef GAME_H
#define GAME_H
#include "common.h"
#include "sheet.h"
#include "text.h"
#include "texture.h"
#include "util.h"
#include <GLFW/glfw3.h>

#define TEXPATH(x) "resources/textures/" x
#define FONTPATH(x) "resources/fonts/" x

typedef enum {
    MAIN_MENU,
    PLACEHOLDER_PROMPT,
    PLACEHOLDER_EDIT,
    PLACEHOLDER_FEEDBACK,
    LEVEL1_EDIT,
} ScreenState;

typedef struct GameState GameState;



void CharHandler(GLFWwindow* window, u32 code);
void KeyHandler(GLFWwindow* window, int key, int scancode, int action, int mods);

void InitGame(GameState* state, TextRenderer* t, SheetRenderer* s);
void DestroyGame(GameState* state, TextRenderer* t, SheetRenderer* s);

void Update(GameState* s, float dt);
void UItick(GameState* state, TextRenderer* t, SheetRenderer* s, double dt);

//widgets

#define MAX_TYPE_SIZE 1500
#define MAX_TYPE_LINES 100

typedef struct {
    bool typingEnable;
    u32 bufend;
    u32 lineend;
    u32 lines[MAX_TYPE_LINES];
    char typingBuffer[MAX_TYPE_SIZE];
} EditBox;

typedef struct {
    i32 hot;
    i32 active;
} UIContext;

typedef struct {
    EditBox b;
} WindowInfo;

typedef struct {
    SheetHandle graphics[20];
    Texture textures[20];
} Scene;


typedef enum {
    BUTTON_INACTIVE = 0,
    BUTTON_HOVER,
    BUTTON_ACTIVE
} ButtonAnim;

//subject to change
static const sm_vec2f ButtonTable[] = {
    {2, 0},
    {0, 0},
    {1, 0}
};

void EditArea(GameState* state, UIContext* c, sm_vec2f pos, float scale, SheetHandle cursor, double accum);
bool Button(GameState* s, UIContext* c, sm_vec2f pos, sm_vec2f size, SheetHandle uuid);

bool AnimateText(GameState* s, sm_vec2f pos, double accum);


struct GameState {
    //texture id
    SheetRenderer* s;
    TextRenderer* t;
    Font f;
    UIContext ucon;
    WindowInfo wininfo;
    ScreenState mode;
    Scene prev;
    Scene curr;
};

#endif
