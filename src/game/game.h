#ifndef GAME_H
#define GAME_H
#include "common.h"
#include "sheet.h"
#include "text.h"
#include <GLFW/glfw3.h>

typedef struct {
    //texture id
    u32 background;

} GameState;


void CharHandler(GLFWwindow* window, u32 code);
void KeyHandler(GLFWwindow* window, int key, int scancode, int action, int mods);

void Update(GameState* s, float dt);
void UItick(GameState* state, TextRenderer* t, SheetRenderer* s);

//todo: implement via counter
void FixedUpdate(GameState* s);

//widgets
void DrawTasks();


#endif
