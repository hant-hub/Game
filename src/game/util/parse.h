#ifndef PARSE_H
#define PARSE_H

#include "common.h"

typedef enum {
    TOKEN_UNINITIALIZED = 0,
    TOKEN_NEWLINE,
    TOKEN_WHITESPACE,
    TOKEN_WORD,
    TOKEN_NUMERIC,
    TOKEN_PUNCTUATION,
    TOKEN_EOB
} TokenType;


typedef struct {
    char* pos;
    u32 length;
    TokenType type;
} Token;

typedef struct {
    u64 size;
    char* Base;
    char* At;
} Tokenizer;

//maybe later cause its cool
//Could be useful for like evaluating math
//expressions which would be sick
Token GetToken(Tokenizer* t);

//prebuilt parsers + structs

typedef struct {
    int linecount;
    int wordcount;
    int incorrect;
} WordLineCounterState;
void CountWordsPerLine(WordLineCounterState* state, int* vals, int numvals, Token t);

typedef struct {
    int linecount;
} LineCounterState;
void CountLines(LineCounterState* state, Token t);

typedef struct {
    int needlepos; 
} MatchSubState;
void MatchSubstr(MatchSubState* state, char* needle, Token t);

typedef struct {
    int mismatch;
} StrInSetState;
void StrInSet(StrInSetState* state, char** set, int size, Token t);

typedef struct {
    int matched;//counts matches
} MatchStringState;
void MatchString(MatchStringState* state, char* string, Token t);

#endif
