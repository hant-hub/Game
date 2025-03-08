#include "parse.h"
#include "util.h"
#include "vec3.h"

float HueToRGB(float p, float q, float t) {
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1.0/6) return p + (q - p) * 6 * t;
    if (t < 1.0/2) return q;
    if (t < 2.0/3) return p + (q - p) * (2.0/3 - t) * 6;
    return p;
}

sm_vec3f HSLToRGB(float h, float s, float l) {
    float r, g, b;

    if (s == 0) {
        r = g = b = l; // achromatic
    } else {
        float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        float p = 2 * l - q;
        r = HueToRGB(p, q, h + 1.0/3);
        g = HueToRGB(p, q, h);
        b = HueToRGB(p, q, h - 1.0/3);
    }

    return (sm_vec3f){r, g, b};
}

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
        return w;
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



