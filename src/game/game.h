#ifndef GAME_H
#define GAME_H
#include "common.h"
#include "sheet.h"
#include "text.h"
#include "texture.h"
#include "util.h"
#include "miniaudio/miniaudio.h"
#include <GLFW/glfw3.h>

#define TEXPATH(x) "resources/textures/" x
#define FONTPATH(x) "resources/fonts/" x
#define SOUNDPATH(x) "resources/sounds/" x

typedef enum {
    MAIN_MENU = 0, //reserve negative values for invalid modes
    PLACEHOLDER_PROMPT,
    PLACEHOLDER_EDIT,
    PLACEHOLDER_FEEDBACK,
    LEVEL1_PROMPT,
    LEVEL1_EDIT,
    LEVEL2_PROMPT,
    LEVEL2_EDIT,
} ScreenState;

typedef struct GameState GameState;

#define NUM_AUDIO_SOURCES 20
typedef struct {
    ma_context context;
    ma_device device;
    ma_decoder_config dconf;
    ma_device_config config;
    ma_decoder sources[NUM_AUDIO_SOURCES];
    float vols[NUM_AUDIO_SOURCES];
    bool loop[NUM_AUDIO_SOURCES];
    bool playing[NUM_AUDIO_SOURCES];
    bool valid[NUM_AUDIO_SOURCES];
} GameAudio;

typedef enum {
    TEST_MUSIC = 0,
    TEST_CARVING
} SoundID;

void audio_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
bool LoadSound(GameAudio* a, SoundID id, const char* filepath, bool loop, float vol);
void PlaySound(GameAudio* a, SoundID id);
void ResetSound(GameAudio* a, SoundID id);
void StopSound(GameAudio* a, SoundID id);

void CharHandler(GLFWwindow* window, u32 code);
void KeyHandler(GLFWwindow* window, int key, int scancode, int action, int mods);

void InitGame(GameState* state, TextRenderer* t, SheetRenderer* s);
void DestroyGame(GameState* state, TextRenderer* t, SheetRenderer* s);

void Update(GameState* s, float dt);
void UItick(GameState* state, TextRenderer* t, SheetRenderer* s, double dt);

//widgets

#define MAX_TYPE_SIZE 1500
#define MAX_TYPE_LINES 100

typedef struct {
    bool typingEnable;
    u32 bufend;
    u32 lineend;
    u32 lines[MAX_TYPE_LINES];
    char typingBuffer[MAX_TYPE_SIZE];
} EditBox;

typedef struct {
    i32 hot;
    i32 active;
} UIContext;

typedef struct {
    EditBox b;
} WindowInfo;

typedef struct {
    SheetHandle graphics[20];
    Texture textures[20];
    u32 stage; //carries between screens
} Scene;


typedef enum {
    BUTTON_INACTIVE = 0,
    BUTTON_HOVER,
    BUTTON_ACTIVE
} ButtonAnim;

//subject to change
static const sm_vec2f ButtonTable[] = {
    {2, 0},
    {0, 0},
    {1, 0}
};

void EditArea(GameState* state, UIContext* c, sm_vec2f pos, u32 layer, float scale, SheetHandle cursor, double accum);
bool Button(GameState* s, UIContext* c, sm_vec2f pos, sm_vec2f size, SheetHandle uuid);

bool AnimateText(GameState* s, const char* text, u32 size, sm_vec2f pos, i32 layer, float scale, double accum, double rate);
void SetNameTag(GameState* s, u32 left, u32 middle, u32 right, double width, sm_vec2f pos, sm_vec3f color);

struct GameState {
    //texture id
    WindowInfo wininfo;
    GameAudio audio;
    SheetRenderer* s;
    TextRenderer* t;
    UIContext ucon;
    ScreenState mode;
    Scene prev;
    Scene curr;
    Font f;
};

typedef struct {
    u32 startidx;
    u32 num;
} Substage;


typedef void (*essentialreq)(char*,u32,u32*);
typedef u32 (*optionreq)(const char*, u32);

#endif
