#ifndef UTIL_H
#define UTIL_H

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


#endif
