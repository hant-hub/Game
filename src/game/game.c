#include "game.h"
#include "init.h"
#include "log.h"
#include "sheet.h"
#include "text.h"
#include "util.h"
#include <GLFW/glfw3.h>
#include <time.h>



//internal function
void HandleInput(GameState* s) {
    return;
}


void Update(GameState* s, float dt) {
    return;
}


void InitGame(GameState* state, TextRenderer* t, SheetRenderer* s) {

    glfwSetWindowUserPointer(sr_context.w, &state->wininfo);

    //init for editing
    state->wininfo.b.typingEnable = FALSE;
    state->wininfo.b.bufend = 0;
    state->wininfo.b.lineend = 0;
    glfwSetCharCallback(sr_context.w, CharHandler);
    glfwSetKeyCallback(sr_context.w, KeyHandler);

    state->t = t;
    state->s = s;

    //init sound stuff
    GameAudio* a = &state->audio;
    ma_result result;
    if (ma_context_init(NULL, 0, NULL, &a->context) != MA_SUCCESS) {
        SR_LOG_ERR("Failed to init audio");
        exit(EXIT_FAILURE);
    }
    
    a->config = ma_device_config_init(ma_device_type_playback);
    a->config.playback.format   = ma_format_f32; //floats for simplicity
    a->config.playback.channels = 1; //monosound
    a->config.sampleRate        = 48000; //standard 48KHz sample rate

    a->config.dataCallback      = audio_callback;
    a->config.pUserData         = a;

    a->dconf = ma_decoder_config_init(ma_format_f32, 1, 48000);

    if (ma_device_init(&a->context, &a->config, &a->device) != MA_SUCCESS) {
        SR_LOG_ERR("Failed to init Device");
        exit(EXIT_FAILURE);
    }
    
    //load audio

    ma_LoadSound(a, TITLE_INTRO, SOUNDPATH("home_intro.wav"), FALSE, 0.5);
    ma_LoadSound(a, TITLE_LOOP, SOUNDPATH("home_loop.wav"), TRUE, 0.5);

    ma_LoadSound(a, LEVEL1_INTRO, SOUNDPATH("level1_intro.wav"), FALSE, 0.5);
    ma_LoadSound(a, LEVEL1_LOOP, SOUNDPATH("level1_loop.wav"), TRUE, 0.5);

    ma_LoadSound(a, LEVEL2_INTRO, SOUNDPATH("level2_intro.wav"), FALSE, 0.5);
    ma_LoadSound(a, LEVEL2_LOOP, SOUNDPATH("level2_loop.wav"), TRUE, 0.5);

    ma_LoadSound(a, CARVING_1, SOUNDPATH("carving.wav"), FALSE, 3.0);
    ma_LoadSound(a, CARVING_2, SOUNDPATH("carving2.wav"), FALSE, 1.0);
    ma_LoadSound(a, CARVING_3, SOUNDPATH("carving3.wav"), FALSE, 1.0);

    ma_LoadSound(a, TYPING, SOUNDPATH("typing.wav"), FALSE, 2.5);

    ma_device_start(&a->device);


    //state->mode = PLACEHOLDER_EDIT;
    //state->mode = LEVEL2_EDIT;
    
    //state->mode = LEVEL2_PROMPT;
    //state->mode = LEVEL1_PROMPT;
    //state->curr.stage = 4;
    //state->mode = LEVEL1_EDIT;
    state->mode = MAIN_MENU;
    state->mode = ELI_POEM;
}

void DestroyGame(GameState* state, TextRenderer* t, SheetRenderer* s) {
}

//main game tick. This is for gameplay only.
//Will create a menu tick for menu stuff
void UItick(GameState* state, TextRenderer* t, SheetRenderer* s, double dt) {
    return;
}
