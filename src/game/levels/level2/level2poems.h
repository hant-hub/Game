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

    "Make up for\nlost days",

    "Decide\nTogether or Apart"
};

static const char* feedbacktext[] = {
    "Sorry doesn't make it up to\nme",
    "Sorry doesn't make it up to\nme no matter how many times\nyou say it",

    "I... I still don't know if\nthat makes up for it",

    "Thank you for saying it like\nthat", //think you're clever huh kid XD
    "Thank you for saying it like\nthat, your choice of words\nhelps me understand",

    "It felt like seven days of\nmisery, and it feels like\nyou're just starting to\nrecognize it",

    "I... that's okay",
    "I... think thats good,\nfor the best",
};

static const Substage stagerequirements[] = {
    (Substage){0,1},
    (Substage){1,1},
    (Substage){2,5},
    (Substage){7,1},
    (Substage){8,1},
};

static const Substage stagefeedback[] = {
    (Substage){0,2},
    (Substage){2,1},
    (Substage){3,2},
    (Substage){5,1},
    (Substage){6,2},
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

    int i = 0;
    while (1) {
        t = GetToken(&tok);
        //functions
        
        MatchSubstr(&substate, "LOVE", t);
        i += t.length;
        if (t.type == TOKEN_EOB) break;
    }

    //write results

    results[0] = i <= 5;
    results[1] = substate.needlepos < 1;
    results[2] = substate.needlepos < 2;
    results[3] = substate.needlepos < 3;
    results[4] = substate.needlepos < 4;
}


static u32 Optional3(char* buffer, u32 size) {
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
        
        MatchString(&matcher, "care", t);
        MatchString(&matcher, "adore", t);
        MatchString(&matcher, "enjoy", t);
        MatchString(&matcher, "appreciate", t);
        MatchString(&matcher, "admire", t);
        MatchString(&matcher, "cherish", t);
        MatchString(&matcher, "value", t);
        MatchString(&matcher, "idolize", t);
        MatchString(&matcher, "affection", t);
        MatchString(&matcher, "devotion", t);

        MatchString(&matcher, "Care", t);
        MatchString(&matcher, "Adore", t);
        MatchString(&matcher, "Enjoy", t);
        MatchString(&matcher, "Appreciate", t);
        MatchString(&matcher, "Admire", t);
        MatchString(&matcher, "Cherish", t);
        MatchString(&matcher, "Value", t);
        MatchString(&matcher, "Idolize", t);
        MatchString(&matcher, "Affection", t);
        MatchString(&matcher, "Devotion", t);

        if (t.type == TOKEN_EOB) break;
    }

    //write results
    return matcher.matched >= 3; //look for at least three power words lmao
}


static void Essential4(char* buffer, u32 size, u32* results) {
    Tokenizer tok = (Tokenizer) {
        .size = size,
        .Base = buffer,
        .At = buffer,
    };


    Token t;
    //states
    LineCounterState lines = {0};
    MatchStringState matcher = {0};

    while (1) {
        t = GetToken(&tok);
        //functions
        
        CountLines(&lines, t);

        MatchString(&matcher, "sorry", t);
        MatchString(&matcher, "Sorry", t);

        MatchString(&matcher, "apologize", t);
        MatchString(&matcher, "Apologize", t);

        MatchString(&matcher, "regret", t);
        MatchString(&matcher, "Regret", t);

        MatchString(&matcher, "do better", t);
        MatchString(&matcher, "Do Better", t);
        MatchString(&matcher, "do Better", t);
        MatchString(&matcher, "do better", t);

        MatchString(&matcher, "promise", t);
        MatchString(&matcher, "Promise", t);
        if (t.type == TOKEN_EOB) break;
    }

    //write results
    results[0] = lines.linecount < 7 && matcher.matched < 3;

}


static u32 Optional4(char* buffer, u32 size) {
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
        
        MatchString(&matcher, "care", t);
        MatchString(&matcher, "adore", t);
        MatchString(&matcher, "enjoy", t);
        MatchString(&matcher, "appreciate", t);
        MatchString(&matcher, "admire", t);
        MatchString(&matcher, "cherish", t);
        MatchString(&matcher, "value", t);
        MatchString(&matcher, "idolize", t);
        MatchString(&matcher, "affection", t);
        MatchString(&matcher, "devotion", t);

        MatchString(&matcher, "Care", t);
        MatchString(&matcher, "Adore", t);
        MatchString(&matcher, "Enjoy", t);
        MatchString(&matcher, "Appreciate", t);
        MatchString(&matcher, "Admire", t);
        MatchString(&matcher, "Cherish", t);
        MatchString(&matcher, "Value", t);
        MatchString(&matcher, "Idolize", t);
        MatchString(&matcher, "Affection", t);
        MatchString(&matcher, "Devotion", t);

        if (t.type == TOKEN_EOB) break;
    }

    //write results
    return matcher.matched >= 3; //look for at least three power words lmao
}

static void Essential5(char* buffer, u32 size, u32* results) {
    Tokenizer tok = (Tokenizer) {
        .size = size,
        .Base = buffer,
        .At = buffer,
    };


    Token t;
    //states
    LineCounterState lines = {0};
    MatchStringState matcher = {0};

    while (1) {
        t = GetToken(&tok);
        //functions
        
        CountLines(&lines, t);

        MatchString(&matcher, "together", t);
        MatchString(&matcher, "Together", t);

        MatchString(&matcher, "apart", t);
        MatchString(&matcher, "Apart", t);
        if (t.type == TOKEN_EOB) break;
    }

    //write results

    results[0] = matcher.matched < 2;
}


static u32 Optional5(char* buffer, u32 size) {
    Tokenizer tok = (Tokenizer) {
        .size = size,
        .Base = buffer,
        .At = buffer,
    };


    Token t;
    //states
    MatchStringState apartmatcher = {0}; 
    MatchStringState togmatcher = {0}; 

    while (1) {
        t = GetToken(&tok);
        //functions
        //
        MatchString(&togmatcher, "together", t);
        MatchString(&togmatcher, "Together", t);

        MatchString(&togmatcher, "stay", t);
        MatchString(&togmatcher, "Stay", t);

        MatchString(&apartmatcher, "don't leave", t);
        MatchString(&apartmatcher, "don't Leave", t);
        MatchString(&apartmatcher, "don't leave", t);
        MatchString(&apartmatcher, "don't Leave", t);

        MatchString(&togmatcher, "don't go", t);
        MatchString(&togmatcher, "don't Go", t);
        MatchString(&togmatcher, "don't go", t);
        MatchString(&togmatcher, "don't Go", t);

        MatchString(&apartmatcher, "apart", t);
        MatchString(&apartmatcher, "Apart", t);

        MatchString(&apartmatcher, "leave", t);
        MatchString(&apartmatcher, "Leave", t);

        MatchString(&togmatcher, "don't stay", t);
        MatchString(&togmatcher, "don't Stay", t);
        MatchString(&togmatcher, "don't stay", t);
        MatchString(&togmatcher, "don't Stay", t);
        
        MatchString(&togmatcher, "go", t);
        MatchString(&togmatcher, "Go", t);

        if (t.type == TOKEN_EOB) break;
    }

    //write results
    return togmatcher.matched <= apartmatcher.matched; //look for at least three power words lmao
}

static const essentialreq essentialfuncs[] = {
    Essential1,
    Essential2,
    Essential3,
    Essential4,
    Essential5,
};

static const optionreq optionalfuncs[] = {
    Optional1,
    Optional2,
    Optional3,
    Optional4,
    Optional5,
};

static const u32 numstages = ARRAY_SIZE(essentialfuncs);
