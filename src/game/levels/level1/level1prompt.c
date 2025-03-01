#include "game.h"
#include "init.h"
#include "level1.h"
#include "sheet.h"
#include "text.h"
#include "util.h"
#include "vec2.h"
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
    TEXPATH("level1/background.png"),
    TEXPATH("level1/Judge_1.png"),
    TEXPATH("UI/Dialogue_box.png"),
    TEXPATH("UI/name_tag.png")
};

//dialogue data
typedef enum {
    JUDGE_ID = 0,
    YOU_ID
} NameID;

static const char* Names[] = {
    "Zeuxippus",
    "You"
};

static const u32 NameLens[] = {
    10,
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
    YOU_ID,
    JUDGE_ID,
    YOU_ID,
    JUDGE_ID,
    JUDGE_ID,
    YOU_ID,
    YOU_ID,
    JUDGE_ID,
    YOU_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    //stage 1 --------------
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    //stage 2 --------------
    YOU_ID,
    JUDGE_ID,
    YOU_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    YOU_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    //stage 3 -----------
    JUDGE_ID,
    YOU_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    //stage 4 -----------
    JUDGE_ID,
    //stage 5 ----------
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    YOU_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    JUDGE_ID,
    YOU_ID,
    JUDGE_ID,
    JUDGE_ID,
};


static const char* Dialogue[] = {
    "*You have arrived early to your first lecture\n of the day*",
    "*With no friends and nowhere to go, you decide\nto simply walk into the class and secure\nyour seat*",
    "*You sit down at the perfect chair, not too far\nto the back, yet close enough to view your\nprofessor*",
    "*Not to mention, a perfect view towards the\nvast villas along the hill*",

    "Hmm?",

    "*Your professor, who was previously engrossed\nin some writing, looks up to you*",

    "Oh, I see you've decided to come in early today",
    "Trying to become one of my most diligent\nstudents, a disciple of mine perhaps?",

    "...",
    "*The near-empty hall engulfs you with its\nsilence*",

    "Well, you're the only one here",

    "*The academy's bourdon bell rings*",

    "The rest of our class will be here any moment",
    "But before the second bell rings, let's do some\npractice, shall we?",
    "This is an applied mathematical poetry class\nafter all!",
    "First, write me a poem of 8 lines",

    //-------------stage 1------------

    "Let's try to fit in one more before class, okay?",
    "This time, don't only focus on the number of\nlines, but also how many words within them",
    "Write me a poem of, hmm",
    "6 lines exact, but each with exactly 5 words\nin each",

    //-------------stage 2------------

    "*Your classmates slowly trickle into the room*",

    "Hello class, settle down, settle down",

    "*The final bell rings as everyone gets to their\nseats*",

    "Thank you all for coming",
    "Let's pick up where we left off, alright",
    "And I expect absolutely NO interruptions this\ntime",
    "I'll see that every miscreant with a loose toga\ngets an additional 5 day suspension",

    "*You can hear some girls in the back of class\ngiggling*",

    "Hmmph",
    "If you all want to know how to write freely, you\nmust learn how to write correctly",
    "By defining your abilities now, you'll find it\nmuch easier to flow later on",
    "So for now, let's practice with simple\noscillation",
    "So I want you to write a poem where every odd\nline, starting at 1, must have 6 words",
    "And every other line, the evens, deliver 4",

    //-------------stage 3---------------
    "Now, let's move on to some linear\nequations, shall we?",

    "*On the board he writes a simple equation,\ny = x + 1*",

    "Let's write a poem following this\nbasic structure",
    "Begin with one word",
    "Each line should be one word longer\nthan the last",
    "Make sure to write at least one line",
    //-------------stage 4---------------
    "Similar poem, minimum lines 3, but now\ny = -x + 5",
    //-------------stage 5---------------
    "Great",
    "Now let's continue onto a more advanced\nfunction",
    "Hmm, for fun, let's conclude with a fun one",
    "Has anyone here worked with the Fibonacci\nsequence?",
    
    "*A few hands across the class raise*",

    "The Fibonacci sequence is defined as follows",
    "Each term is the sum of the preceeding two",
    "For the base case we stipulate that the first\nand second terms are one",
    "Understand?",

    "*A few more hands are raised*",

    "Sorry, we don't have time for further\nexplanation",
    "Build me a poem in which each line's word\ncount follows the Fibonacci sequence",
};

static const Substage dialoguestages[] = {
    (Substage){0, 16},
    (Substage){16, 4},
    (Substage){20, 14},
    (Substage){34, 6},
    (Substage){40, 1},
    (Substage){41, 12},
};

static  u32 DialogLens[ARRAY_SIZE(Dialogue)] = {
};

static const u32 numDialogue = ARRAY_SIZE(Dialogue);

void Level1PromptInit(GameState* state) {
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
    sc->graphics[SPRITE_JUDGE] = CreateSpriteSh(state->s, (sm_vec2f){-1080/1.5, 0}, (sm_vec2f){1080*2, 1080*2}, TEX_JUDGE, 19);

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

}

void Level1PromptDestroy(GameState* state) {
    for (int i = 0; i < NUM_SPRITES; i++) {
        DestroySpriteSh(state->s, state->curr.graphics[i]);
    }
    for (int i = 0; i < ARRAY_SIZE(TexPaths); i++) {
        DestroyTexture(&state->curr.textures[i]);
    }
    DestroyFont(&state->f);
}

void Level1PromptUpdate(GameState* state, PresentInfo* p) {
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


    double cooldown = 0.5;
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


        {
            if (accum > cooldown && glfwGetMouseButton(sr_context.w, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
                cooldown = accum + 0.20;
                textpos = DialogLens[dialogCounter];
            }
        }

        {
            if (textpos < DialogLens[dialogCounter] && accum > thresh) {
                thresh = accum + 0.05;
                textpos++;
            }
        }
        AppendText(state->t, Dialogue[dialogCounter], textpos, (sm_vec2f){-540*2.5 + 10, 660}, 1.0, 1.2);


        {
            if (
                accum > cooldown2 && 
                textpos >= DialogLens[dialogCounter] && 
                glfwGetMouseButton(sr_context.w, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS
               ) {
                textpos = 0;
                cooldown = accum + 0.2;
                dialogCounter++;
                if (dialogCounter >= dialoguestages[sc->stage].startidx + dialoguestages[sc->stage].num) {
                    state->mode = LEVEL1_EDIT;
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


        if (glfwGetKey(sr_context.w, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(sr_context.w, GLFW_TRUE);
            return;
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

