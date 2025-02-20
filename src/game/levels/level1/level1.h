#ifndef LEVEL1_H
#define LEVEL1_H
#include "game.h"

void Level1Init(GameState* state);
void Level1Update(GameState* state, PresentInfo* p);
void Level1Destroy(GameState* state);

void Level1PromptInit(GameState* state);
void Level1PromptUpdate(GameState* state, PresentInfo* p);
void Level1PromptDestroy(GameState* state);



#endif
