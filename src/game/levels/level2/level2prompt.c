#include "game.h"
#include "init.h"
#include "log.h"
#include "sheet.h"
#include "text.h"
#include "util.h"
#include "vec2.h"
#include "level2.h"
#include <string.h>
#include <GLFW/glfw3.h>


//texture and sprite
typedef enum {
    TEX_BACKGROUND = 0,
    TEX_JUDGE,
    TEX_DIALOGUE,
    TEX_NAMETAG,
    NUM_TEX
} TexID;

typedef enum {
    SPRITE_BACKGROUND = 0,
    SPRITE_JUDGE,
    SPRITE_DIALOGUE,
    SPRITE_NAMETAG_LEFT,
    SPRITE_NAMETAG_MIDDLE,
    SPRITE_NAMETAG_RIGHT,
    NUM_SPRITES
} SpriteID;


static const char* TexPaths[] = {
    TEXPATH("level2/Background_2.png"),
    TEXPATH("level2/Judge_2.png"),
    TEXPATH("UI/Dialogue_box.png"),
    TEXPATH("UI/name_tag.png")
};

//dialogue data
typedef enum {
    JUDGE_ID = 0,
    YOU_ID
} NameID;

static const char* Names[] = {
    "Ex Girlfriend??",
    "You"
};

static const u32 NameLens[] = {
    16,
    3
};

static const sm_vec3f NameColor[] = {
    (sm_vec3f){0.7, 1.0, 0.7},
    (sm_vec3f){0.7, 0.7, 0.7},
};

static const sm_vec2f NamePos[] = {
    (sm_vec2f){-1080 * 1.2, 800 - (250 + 13*7.5 - 1.5*30)},
    (sm_vec2f){1080*1.2 - 300, 800 - (250 + 13*7.5 - 1.5*30)},
};

static const sm_vec2f TagPos[] = {
    (sm_vec2f){-1080 * 1.2, 800 - (250 + 13*7.5)}, //judge
    (sm_vec2f){1080 * 1.2 - 300, 800 - (250 + 13*7.5)}, //you
};

static const u32 DialogueConf[] = {
    YOU_ID,
    YOU_ID,
    YOU_ID,
    JUDGE_ID,
    JUDGE_ID,
    //stage 1 --------------
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    //stage 2 --------------
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    //stage 3 --------------
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    //stage 4 --------------
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    //stage 5 --------------

};


static const char* Dialogue[] = {
    "*BZZZ BZZZ BZZZ*",
    "*Please Leave A Message For, 310-92 BZZZ*",
    "*Straight to voice mail again...*",
    "I don't want to answer, can you just leave me\nalone",
    "It's the least you can do after what you've done",
    //stage 1 -----------
    "You've known forever that today was my big gig",
    "I was struggling to play up there, I kept\nexpecting to see you walk through those doors\nto support me",
    "After everything you've put me through, I'm not\neven sure if you love me anymore",
    //stage 2 -----------
    "I wish you would ever tell me you love me\nwithout saying that word",
    "The word 'love' means so much, but I think you\ncan see how you've broken my trust",
    "When you say stuff like that, it doesn't mean\nanything anymore",
    "I wish you could tell me how you felt",
    "Everytime you say I love you over and over, and\nI just don't know what it means to me anymore",
    //stage 3 -----------
    "But I still feel like you've been absent all week",
    "Our nightly calls, our evening texts",
    "I'll be honest, as much as I am upset at you for\nditching me, I'm also just upset because I miss\nyou",
    "You never gave me updates, you just left me\ncompletely alone for seven days straight",
    "It hurt a lot to wake up without you each day",
    //stage 4 -----------
    "I appreciate all the messages, and I think I'd\nlike to talk until my bus arrives",
    "Until then, I was thinking you could maybe help\nme out with something I'm writing",
    "The song is called, \"Will They, Won't They\"",
    "The song is about a, uhh, fictional couple, and I\ndon't know if they should remain 'together' or\n'apart'",
    "I was wondering if you could write the last\nverse and decide for me",
    //stage 5 -----------
};

static const Substage dialoguestages[] = {
    (Substage){0, 5},
    (Substage){5, 3},
    (Substage){8, 5},
    (Substage){13, 5},
    (Substage){18, 5},
};

static  u32 DialogLens[ARRAY_SIZE(Dialogue)] = {
};

static const u32 numDialogue = ARRAY_SIZE(Dialogue);

void Level2PromptInit(GameState* state) {
    for (int i = 0; i < NUM_TEX; i++) {
        SR_LOG_DEB("TexID: %d", i);
        CRASH_CALL(LoadTextureConfig(&state->curr.textures[i], TexPaths[i], (TextureConfig) {
                    .anisotropy = FALSE,
                    .accessmode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                    .filter = VK_FILTER_NEAREST,
                    }));
    }

    CRASH_CALL(LoadFont(FONTPATH("dogicapixel.ttf"), 60, &state->f));
    SetFont(state->t, &state->f);

    SHSetTextureSlots(state->s, state->curr.textures, ARRAY_SIZE(TexPaths));

    //Disabled for prompt
    state->wininfo.b.typingEnable = FALSE;
    state->wininfo.b.bufend = 0;
    state->wininfo.b.lineend = 0;


    //create sprites
    Scene* sc = &state->curr;

    //background
    sc->graphics[SPRITE_BACKGROUND] = CreateSpriteSh(state->s, (sm_vec2f){0,0}, (sm_vec2f){1920*2, 1080*2}, TEX_BACKGROUND, 20);
    sc->graphics[SPRITE_JUDGE] = CreateSpriteSh(state->s, (sm_vec2f){-1080*1.15, 0}, (sm_vec2f){106*13, 168*13}, TEX_JUDGE, 19);

    sc->graphics[SPRITE_DIALOGUE] = CreateSpriteSh(state->s, (sm_vec2f){0, 800}, (sm_vec2f){1080*2.5, 500}, TEX_DIALOGUE, 18);
    sc->graphics[SPRITE_NAMETAG_LEFT] = CreateSpriteSh(state->s, (sm_vec2f){-1080*1.1, 800 - (250 + 13*7.5)}, (sm_vec2f){7*15,13*15}, TEX_NAMETAG, 17);
    sc->graphics[SPRITE_NAMETAG_MIDDLE] = CreateSpriteSh(state->s, (sm_vec2f){-1080*1.1 + 7*15, 800 - (250 + 13*7.5)}, (sm_vec2f){7*15,13*15}, TEX_NAMETAG, 17);
    sc->graphics[SPRITE_NAMETAG_RIGHT] = CreateSpriteSh(state->s, (sm_vec2f){}, (sm_vec2f){7*15,13*15}, TEX_NAMETAG, 17);
    {
        SheetEntry* e = GetSpriteSh(state->s, sc->graphics[SPRITE_NAMETAG_LEFT]);
        e->scale = (sm_vec2f){3,1};
        e->selection = (sm_vec2f){0,0};
        e->color = (sm_vec3f){0.0, 0.0, 0.0};
    }
    {
        SheetEntry* e = GetSpriteSh(state->s, sc->graphics[SPRITE_NAMETAG_MIDDLE]);
        e->scale = (sm_vec2f){3,1};
        e->selection = (sm_vec2f){2,0};
        e->color = (sm_vec3f){0.0, 0.0, 0.0};
    }
    {
        SheetEntry* e = GetSpriteSh(state->s, sc->graphics[SPRITE_NAMETAG_RIGHT]);
        e->scale = (sm_vec2f){3,1};
        e->selection = (sm_vec2f){1,0};
        e->color = (sm_vec3f){0.0, 0.0, 0.0};
    }

    //initialize Dialogue sizes

    for (int i = 0; i < ARRAY_SIZE(Dialogue); i++) {
        DialogLens[i] = strlen(Dialogue[i]);
    }

    //reset sound
    for (int i = 0; i < NUM_SOUNDS; i++) {
        //skip the sounds which should be playing
        if (i != LEVEL2_INTRO && i != LEVEL2_LOOP) {
            ma_StopSound(&state->audio, i);
        }
    }

    //start sounds if not already playing
    if (!state->audio.playing[LEVEL2_LOOP] && !state->audio.playing[LEVEL2_INTRO]) {
        ma_PlaySound(&state->audio, LEVEL2_INTRO);
    }
}

void Level2PromptDestroy(GameState* state) {
    for (int i = 0; i < NUM_SPRITES; i++) {
        DestroySpriteSh(state->s, state->curr.graphics[i]);
    }
    for (int i = 0; i < ARRAY_SIZE(TexPaths); i++) {
        DestroyTexture(&state->curr.textures[i]);
    }
    DestroyFont(&state->f);
}

void Level2PromptUpdate(GameState* state, PresentInfo* p) {
    u32 frameCounter = 0;
    double dt = 0;
    double accum = 0;

    Scene* sc = &state->curr;
    UIContext c = {0};

    //set nametag
    u32 dialogCounter = dialoguestages[sc->stage].startidx;
    u32 textpos = 0;
    SetNameTag(state, 
            sc->graphics[SPRITE_NAMETAG_LEFT],
            sc->graphics[SPRITE_NAMETAG_MIDDLE],
            sc->graphics[SPRITE_NAMETAG_RIGHT],
            14*15 + NameLens[DialogueConf[dialogCounter]]*50,
            TagPos[DialogueConf[dialogCounter]],
            NameColor[DialogueConf[dialogCounter]]
            );


    double cooldown2 = 0.5;
    double thresh = 1.0;
    while (!glfwWindowShouldClose(sr_context.w)) {
        double start = glfwGetTime();
        //poll events
        glfwPollEvents();

        accum += dt;

        ClearText(state->t);
        SetColor(state->t, (sm_vec3f){0.0, 0.0, 0.0});
        u32 name = DialogueConf[dialogCounter];
        AppendText(state->t, Names[name], NameLens[name], NamePos[name], 16, 1.5);


        AppendText(state->t, Dialogue[dialogCounter], textpos, (sm_vec2f){-540*2.5 + 10, 660}, 1.0, 1.2);

        bool advance = KeyButton(state, &c, 10);

        {
            if (
                textpos >= DialogLens[dialogCounter] && 
                advance
               ) {
                advance = FALSE;
                textpos = 0;
                dialogCounter++;
                if (dialogCounter >= dialoguestages[sc->stage].startidx + dialoguestages[sc->stage].num) {
                    state->mode = LEVEL2_EDIT;
                    return;
                }
                u32 name = DialogueConf[dialogCounter];
                SetNameTag(state, 
                        sc->graphics[SPRITE_NAMETAG_LEFT],
                        sc->graphics[SPRITE_NAMETAG_MIDDLE],
                        sc->graphics[SPRITE_NAMETAG_RIGHT],
                        14*15 + NameLens[name]*50, TagPos[name],
                        NameColor[name]);

            }
        }

        {
            if (advance) {
                advance = FALSE;
                textpos = DialogLens[dialogCounter];
            }
        }

        {
            if (textpos < DialogLens[dialogCounter] && accum > thresh) {
                thresh = accum + 0.05;
                textpos++;
            }
        }

        if (glfwGetKey(sr_context.w, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(sr_context.w, GLFW_TRUE);
            return;
        }

        //start loop when intro finishes
        if (!state->audio.playing[LEVEL2_INTRO]) {
            ma_PlaySound(&state->audio, LEVEL2_LOOP);
        }

        //draw frame
        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        StartFrame(p, frameCounter);
        SheetDrawFrame(state->s, p, frameCounter);
        NextPass(p, frameCounter);
        TextDrawFrame(state->t, p, frameCounter);
        SubmitFrame(p, frameCounter);

        double end = glfwGetTime();
        dt = end - start;
    }
}


