#include "game.h"
#include "level1.h"
#include "parse.h"


static const char* Requirements[] = {
    "3 lines or more",
    "At least six words", 
    "3 lines again",
    "six words",
};

static const char* feedbacktext[] = {
    "Huh, a little short\nisn't it?",
    "Decent, nothing noteworthy\nhowever.",
    "again",
    "and again",
};

static const Substage stagerequirements[] = {
    (Substage){0, 2},
    (Substage){2, 2},
};

static const Substage stagefeedback[] = {
    (Substage){0, 2},
    (Substage){2, 2},
};


void EssentialRequirements1(char* buffer, u32 size, u32* results) {

    Tokenizer tok = (Tokenizer) {
        .size = size,
        .Base = buffer,
        .At = buffer,
    };

    Token t;
    //states
    LineCounterState counter = {0};    
    u32 numwords = 0;

    int i = 0;
    while (1) {
        i++;
        t = GetToken(&tok);
        //functions
        CountLines(&counter, t);
        if (t.type == TOKEN_WORD || t.type == TOKEN_NUMERIC) numwords++;
        if (t.type == TOKEN_EOB) break;
    }

    //write results
    results[0] = counter.linecount < 3;
    results[1] = numwords < 6;
}
u32 OptionalRequirements1(const char* buffer, u32 size) {
    return 1;
}

static const essentialreq essentialfuncs[] = {
    EssentialRequirements1,
    EssentialRequirements1,
};

static const optionreq optionalfuncs[] = {
    OptionalRequirements1,
    OptionalRequirements1,
};

static const u32 numstages = 2;
