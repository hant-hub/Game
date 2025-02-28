#ifndef LEVEL2_H
#define LEVEL2_H

#include "game.h"

void Level2Init(GameState* state);
void Level2Update(GameState* state, PresentInfo* p);
void Level2Destroy(GameState* state);

void Level2PromptInit(GameState* state);
void Level2PromptUpdate(GameState* state, PresentInfo* p);
void Level2PromptDestroy(GameState* state);



#endif
