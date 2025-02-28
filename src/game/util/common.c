#include "parse.h"
#include <string.h>


void CountWordsPerLine(WordLineCounterState* state, int* vals, int numvals, Token t) {
    switch (t.type) {
        case TOKEN_WORD:
        case TOKEN_NUMERIC:
            {
                state->wordcount++; //counting numbers as words
                break;
            }
        case TOKEN_EOB:
        case TOKEN_NEWLINE:
            {
                if (state->linecount >= numvals) break;
                if (state->wordcount != vals[state->linecount]) state->incorrect++;

                state->linecount++;
                state->wordcount = 0;
                break;
            }
        default:
            {
                break;
            }
    }
}

void CountLines(LineCounterState* state, Token t) {
    if (t.type == TOKEN_NEWLINE || t.type == TOKEN_EOB) {
        state->linecount++;
    }
}

//this one is weird, I think for now I will
//essentially 'detokenize' the string, but
//that might cripple it's performance
void MatchSubstr(MatchSubState* state, char* needle, Token t) {
    if (t.type != TOKEN_WORD && t.type != TOKEN_NUMERIC) return;

    for (int i = 0; i < t.length; i++) {
        if (t.pos[i] == needle[state->needlepos]) {
            state->needlepos++;
        }
    }

}

void StrInSet(StrInSetState* state, char** set, int size, Token t) {
    if (t.type != TOKEN_WORD) return;

    int matched = 0;
    for (int i = 0; i < size; i++) {
        int counter = 0;
        if (strncmp(set[i], t.pos, t.length) != 0) continue;
        if (strlen(set[i]) != t.length) continue;
        matched = 1;
        break;
    }

    if (!matched) state->mismatch++;
}

void MatchString(MatchStringState* state, char* string, Token t) {
    if (t.type != TOKEN_WORD && t.type != TOKEN_NUMERIC) return;

    if (strncmp(string, t.pos, t.length) != 0) return;
    if (strlen(string) != t.length) return;
    state->matched++;
}


