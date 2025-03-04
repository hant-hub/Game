#include "game.h"
#include "parse.h"
#include "util.h"


static const char* Requirements[] = {
    "Write Exactly 8 Lines",

    "Write 6 Lines",
    "5 Words Per Line",

    "Write Exactly 8\nLines",
    "Odd Lines Have 6\nWords",
    "Even Lines Have 4\nWords",

    "Write at Least 1\nWord",
    "Each Line has 1 More\nWord Than The Last",

    "Write At Least 3\nLines",
    "The First Line Must\nHave 5 Words",
    "Each Line Has 1 Less\nWord Than The Last",

    "Write At least 4\nLines",
    "The First Two Lines\nHave 1 Word",
    "Every Line Is The\nSum of The Last\nTwo Lines"
};

static const char* feedbacktext[] = {
    "Hmm, very not sure I'd have\nchosen the same words, but\ngood work",

    "The pace you wrote that, I can\ntell you're starting to\ncare more about rhythm",

    "Even for a linear poem, your\nimaginative word play is\nglorious",

    "Well done, the sin wave is no\nmatch against you, my\nfriend",

    "Wow, I love the way it\nconcludes at the very end",

    "Wow. It's fantastic. I'm\nnow speechless. An\nabsolute piece of beauty",
};

static const Substage stagerequirements[] = {
    (Substage){0, 1},
    (Substage){1, 2},
    (Substage){3, 3},
    (Substage){6, 2},
    (Substage){8, 3},
    (Substage){11, 3},
};

static const Substage stagefeedback[] = {
    (Substage){0, 1},
    (Substage){1, 1},
    (Substage){2, 1},
    (Substage){3, 1},
    (Substage){4, 1},
    (Substage){5, 1},
};


//8 lines
static void EssentialRequirements1(char* buffer, u32 size, u32* results) {

    Tokenizer tok = (Tokenizer) {
        .size = size,
        .Base = buffer,
        .At = buffer,
    };

    Token t;
    //states
    LineCounterState counter = {0};    

    while (1) {
        t = GetToken(&tok);
        //functions
        CountLines(&counter, t);
        if (t.type == TOKEN_EOB) break;
    }

    //write results
    results[0] = counter.linecount < 8;
}

//6 lines 5 words per
static void EssentialRequirements2(char* buffer, u32 size, u32* results) {

    Tokenizer tok = (Tokenizer) {
        .size = size,
        .Base = buffer,
        .At = buffer,
    };

    Token t;
    //states
    LineCounterState counter = {0};    
    WordLineCounterState wordsper = {0};
    int vals[] = {5, 5, 5, 5, 5, 5};
    u32 numwords = 0;
    bool valid = TRUE;

    while (1) {
        t = GetToken(&tok);
        //functions
        CountLines(&counter, t);
        CountWordsPerLine(&wordsper, vals, ARRAY_SIZE(vals), t);
        if (t.type == TOKEN_WORD || t.type == TOKEN_NUMERIC) numwords++;
        if (t.type == TOKEN_NEWLINE) {
            if (numwords != 5) valid = FALSE;
            numwords = 0;
        }
        if (t.type == TOKEN_EOB) break;
    }

    //write results
    results[0] = counter.linecount < 6;
    results[1] = wordsper.incorrect;
}

//8 lines odd 6, even 4
static void EssentialRequirements3(char* buffer, u32 size, u32* results) {

    Tokenizer tok = (Tokenizer) {
        .size = size,
        .Base = buffer,
        .At = buffer,
    };

    Token t;
    //states
    LineCounterState counter = {0};    
    u32 numwords = 0;
    u32 numlines = 0;
    bool validOdd = TRUE;
    bool validEven = TRUE;

    while (1) {
        t = GetToken(&tok);
        //functions
        CountLines(&counter, t);
        if (t.type == TOKEN_WORD || t.type == TOKEN_NUMERIC) numwords++;
        if (t.type == TOKEN_NEWLINE || t.type == TOKEN_EOB) {
            numlines++;
            if (numlines % 2) {
                if (numwords != 6) validOdd = FALSE;
            } else {
                if (numwords != 4) validEven = FALSE;
            }
            numwords = 0;
        }
        if (t.type == TOKEN_EOB) break;
    }

    //write results
    results[0] = counter.linecount < 8;
    results[1] = !validOdd;
    results[2] = !validEven;
}

//each line must have 1 more words than the last and at least 1 word
static void EssentialRequirements4(char* buffer, u32 size, u32* results) {

    Tokenizer tok = (Tokenizer) {
        .size = size,
        .Base = buffer,
        .At = buffer,
    };

    Token t;
    //states
    u32 numwords = 0;
    u32 totalwords = 0;
    u32 targetwords = 1;
    bool valid = TRUE;

    while (1) {
        t = GetToken(&tok);
        //functions
        if (t.type == TOKEN_WORD || t.type == TOKEN_NUMERIC) {
            numwords++;
            totalwords++;
        }
        if (t.type == TOKEN_NEWLINE || t.type == TOKEN_EOB) {
            if (numwords != targetwords) valid = FALSE;
            targetwords++;
            numwords = 0;
        }
        if (t.type == TOKEN_EOB) break;
    }

    //write results
    results[0] = totalwords < 1;
    results[1] = !valid;
}

//each line must have 1 less words than the last and at least 7 words
static void EssentialRequirements5(char* buffer, u32 size, u32* results) {

    Tokenizer tok = (Tokenizer) {
        .size = size,
        .Base = buffer,
        .At = buffer,
    };

    Token t;
    //states

    LineCounterState lines = {0};
    WordLineCounterState wordsper = {0};
    int vals[] = {5, 4, 3, 2, 1};

    bool valid = TRUE;

    while (1) {
        t = GetToken(&tok);
        //functions
        if (lines.linecount == 0) {
            valid = wordsper.wordcount == 5;
        }
        CountLines(&lines, t);
        CountWordsPerLine(&wordsper, vals, ARRAY_SIZE(vals), t);

        if (t.type == TOKEN_EOB) break;
    }

    //write results
    results[0] = lines.linecount < 3;
    results[1] = !valid;
    results[2] = (wordsper.incorrect != 0) || (lines.linecount > 5);
}

//each line follows the fibbonnaci rule, first two lines are at one word
static void EssentialRequirements6(char* buffer, u32 size, u32* results) {

    Tokenizer tok = (Tokenizer) {
        .size = size,
        .Base = buffer,
        .At = buffer,
    };

    Token t;
    //states
    WordLineCounterState wordsper = {0};
    int vals[] = {
        1, 1, 2, 3, 5, 8, 13, 21, 34, 55
    };

    bool valid = TRUE;

    while (1) {
        t = GetToken(&tok);
        //functions
        CountWordsPerLine(&wordsper, vals, ARRAY_SIZE(vals), t);
        if (t.type == TOKEN_EOB) break;
    }

    //write results
    results[0] = wordsper.linecount < 4;
    results[1] = wordsper.incorrect;
    results[2] = wordsper.incorrect;
}

static u32 OptionalRequirements1(char* buffer, u32 size) {
    return 0;
}

static const essentialreq essentialfuncs[] = {
    EssentialRequirements1,
    EssentialRequirements2,
    EssentialRequirements3,
    EssentialRequirements4,
    EssentialRequirements5,
    EssentialRequirements6,
};

static const optionreq optionalfuncs[] = {
    OptionalRequirements1,
    OptionalRequirements1,
    OptionalRequirements1,
    OptionalRequirements1,
    OptionalRequirements1,
    OptionalRequirements1,
};

static const u32 numstages = 6;
