#include "game.h"
#include "parse.h"
#include "util.h"

static const char* Requirements[] = {
    "Say you're Sorry",
    
    "Tell her you\nLove Her",
    
    "Hide LOVE in\nyour message",
    "L",
    "O",
    "V",
    "E",
};

static const char* feedbacktext[] = {
    "Sorry doesn't make it up to\nme",
    "Sorry doesn't make it up to\nme no matter how many times\nyou say it",

    "I... I still don't know if\nthat makes up for it",

    "Thank you for saying it like that", //think you're clever huh kid XD
    "Thank you for saying it like that, your choice of words helps me understand",
};

static const Substage stagerequirements[] = {
    (Substage){0,1},
    (Substage){1,1},
    (Substage){2,5},
};

static const Substage stagefeedback[] = {
    (Substage){0,2},
    (Substage){2,1},
    (Substage){3,2},
};


static void EssentialStub(char* buffer, u32 size, u32* results) {
    return;
}
static u32 OptionalStub(const char* buffer, u32 size) {
    return 0;
}

static void Essential1(char* buffer, u32 size, u32* results) {
    Tokenizer tok = (Tokenizer) {
        .size = size,
        .Base = buffer,
        .At = buffer,
    };


    Token t;
    //states
    MatchStringState matcher = {0}; 

    while (1) {
        t = GetToken(&tok);
        //functions
        
        MatchString(&matcher, "Sorry", t);
        MatchString(&matcher, "sorry", t);

        if (t.type == TOKEN_EOB) break;
    }

    //write results

    results[0] = !(matcher.matched);
}


static u32 Optional1(char* buffer, u32 size) {
    Tokenizer tok = (Tokenizer) {
        .size = size,
        .Base = buffer,
        .At = buffer,
    };


    Token t;
    //states
    MatchStringState matcher = {0}; 

    while (1) {
        t = GetToken(&tok);
        //functions
        
        MatchString(&matcher, "Sorry", t);
        MatchString(&matcher, "sorry", t);
        MatchString(&matcher, "apologize", t);
        MatchString(&matcher, "regret", t);

        if (t.type == TOKEN_EOB) break;
    }

    //write results
    return matcher.matched > 1;
}

static void Essential2(char* buffer, u32 size, u32* results) {
    Tokenizer tok = (Tokenizer) {
        .size = size,
        .Base = buffer,
        .At = buffer,
    };


    Token t;
    //states
    MatchStringState matcher = {0}; 

    while (1) {
        t = GetToken(&tok);
        //functions
        
        MatchString(&matcher, "love you", t);
        MatchString(&matcher, "Love you", t);
        MatchString(&matcher, "Love", t);
        MatchString(&matcher, "love", t);

        if (t.type == TOKEN_EOB) break;
    }

    //write results

    results[0] = !(matcher.matched);
}


static u32 Optional2(char* buffer, u32 size) {
    return 0;
    Tokenizer tok = (Tokenizer) {
        .size = size,
        .Base = buffer,
        .At = buffer,
    };


    Token t;
    //states
    MatchStringState matcher = {0}; 

    while (1) {
        t = GetToken(&tok);
        //functions
        
        MatchString(&matcher, "Sorry", t);
        MatchString(&matcher, "sorry", t);
        MatchString(&matcher, "apologize", t);
        MatchString(&matcher, "regret", t);

        if (t.type == TOKEN_EOB) break;
    }

    //write results
    return 0;
}

static void Essential3(char* buffer, u32 size, u32* results) {
    Tokenizer tok = (Tokenizer) {
        .size = size,
        .Base = buffer,
        .At = buffer,
    };


    Token t;
    //states
    MatchSubState substate = {0};

    while (1) {
        t = GetToken(&tok);
        //functions
        
        MatchSubstr(&substate, "LOVE", t);

        if (t.type == TOKEN_EOB) break;
    }

    //write results

    results[0] = substate.needlepos < 4;
    results[1] = substate.needlepos < 1;
    results[2] = substate.needlepos < 2;
    results[3] = substate.needlepos < 3;
    results[4] = substate.needlepos < 4;
}


static u32 Optional3(char* buffer, u32 size) {
    return 0;
    Tokenizer tok = (Tokenizer) {
        .size = size,
        .Base = buffer,
        .At = buffer,
    };


    Token t;
    //states
    MatchStringState matcher = {0}; 

    while (1) {
        t = GetToken(&tok);
        //functions
        
        MatchString(&matcher, "Sorry", t);
        MatchString(&matcher, "sorry", t);
        MatchString(&matcher, "apologize", t);
        MatchString(&matcher, "regret", t);

        if (t.type == TOKEN_EOB) break;
    }

    //write results
    return 0;
}

static const essentialreq essentialfuncs[] = {
    Essential1,
    Essential2,
    Essential3,
};

static const optionreq optionalfuncs[] = {
    Optional1,
    Optional2,
    Optional3,
};

static const u32 numstages = ARRAY_SIZE(essentialfuncs);
