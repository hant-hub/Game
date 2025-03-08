#ifndef ELI_H
#define ELI_H
#include "game.h"

//slides -> cool text demo -> game demo -> maybe battleship if time permits

//slides
void EliInit(GameState* state);
void EliUpdate(GameState* state, PresentInfo* p);
void EliDestroy(GameState* state);

//text demo
void EliTextInit(GameState* state);
void EliTextUpdate(GameState* state, PresentInfo* p);
void EliTextDestroy(GameState* state);

//poem
void EliPoemInit(GameState* state);
void EliPoemUpdate(GameState* state, PresentInfo* p);
void EliPoemDestroy(GameState* state);

#endif
