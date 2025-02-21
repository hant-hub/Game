#include "log.h"
#include "parse.h"
#include "util.h"


bool IsWhiteSpace(char c) {
    return (c == ' ') ||
            (c == '\r');
}

bool IsAlpha(char c) {
    return ((c <= 'Z') && (c >= 'A')) || 
           ((c <= 'z') && (c >= 'a'));
}

bool IsPunctuation(char c) {
    return ((c >= '!') && (c <= '/')) ||
           ((c >= ':') && (c <= '@')) ||
           ((c >= '[') && (c <= '`')) ||
           ((c >= '{') && (c <= '~'));
}

bool IsNumeric(char c) {
    return (c <= '9') && (c >= '0');
}


Token GetToken(Tokenizer* t) {
    Token w;

    if (t->At >= &t->Base[t->size]) {
        w.type = TOKEN_EOB;
        w.pos = t->At;
        w.length = 0;
    }

    //skip whitespace
    if (t->At[0] == '\n') {
        w.pos = t->At++;
        w.length = 1;
        w.type = TOKEN_NEWLINE;
    } else if (IsAlpha(t->At[0])) {
        w.type = TOKEN_WORD;
        w.pos = t->At;

        while (IsAlpha(t->At[0])) t->At++;
        w.length = t->At - w.pos;

    } else if (IsNumeric(t->At[0])) {
        w.type = TOKEN_NUMERIC;
        w.pos = t->At;

        while (IsNumeric(t->At[0])) t->At++;
        w.length = t->At - w.pos;
    } else if (IsPunctuation(t->At[0])) {
        w.type = TOKEN_PUNCTUATION;
        w.length = 1;
        w.pos = t->At++;
    } else if (IsWhiteSpace(t->At[0])) {
        w.type = TOKEN_WHITESPACE;
        w.pos = t->At;
        while(IsWhiteSpace(t->At[0])) t->At++;
        w.length = t->At - w.pos;
    }

    return w;
}



