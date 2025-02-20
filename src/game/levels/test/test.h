#ifndef TEST_H
#define TEST_H
#include "game.h"
#include "frame.h"



void TestInit(GameState* state);
void TestUpdate(GameState* state, PresentInfo* p);
void TestDestroy(GameState* state);

void TestPromptInit(GameState* state);
void TestPromptUpdate(GameState* state, PresentInfo* p);
void TestPromptDestroy(GameState* state);


#endif
