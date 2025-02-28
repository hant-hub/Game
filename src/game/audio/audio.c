#include "common.h"
#include "game.h"
#include "log.h"
#include "util.h"
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdlib.h>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"






void audio_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    float tempbuf[4096] = {0};    
    GameAudio* a = (GameAudio*)pDevice->pUserData;
    float* vols = a->vols;
    ma_decoder* decoders = a->sources;
    float* out = (float*)pOutput;

    ma_result result;
    u32 temp_cap = ma_countof(tempbuf); //monosound
    

    //initialize to zero
    for (int i = 0; i < frameCount; i++) {
        out[i] = 0;
    }

    for (int i = 0; i < NUM_AUDIO_SOURCES; i++) {
        u32 totalframesread = 0;
        if (!a->valid[i] || !a->playing[i]) {
            continue; //skip non playing
        }

        //static u64 c = 0;
        //if (i == 1) SR_LOG_DEB("hit %ld", c++);

        //read data in tempbuf sized chunks
        while (totalframesread < frameCount) {
            ma_uint64 framesRead = 0; // a little annoying but whatever
            u32 framesRemaining = frameCount - totalframesread;
            u32 framesToRead = temp_cap;

            if (framesToRead > framesRemaining) {
                framesToRead = framesRemaining;
            }

            result = ma_decoder_read_pcm_frames(&decoders[i], tempbuf, framesToRead, &framesRead);

            //if (result != MA_SUCCESS || framesRead == 0) {
            //    break; //failed for whatever reason
            //}

            for (u64 j = 0; j < framesRead; ++j) {
                out[totalframesread + j] += tempbuf[j] * vols[i]; //add sound with volume
            }


            totalframesread += framesRead;
            if (framesRead < framesToRead) {
                //EOF
                ma_decoder_seek_to_pcm_frame(&decoders[i], 0); //reset decoders
                if (a->loop[i]) {
                    continue;
                }
                a->playing[i] = FALSE; //mark finished
                break;
            }
        }
        //SR_LOG_DEB("valid: %d", a->valid[1]);
        //SR_LOG_DEB("playing: %d", a->playing[1]);
        //SR_LOG_DEB("vol: %f", a->vols[1]);
    }

}

bool LoadSound(GameAudio* a, SoundID id, const char* filepath, bool loop, float vol) {
    a->playing[id] = FALSE;
    a->valid[id] = TRUE;
    a->loop[id] = loop;
    a->vols[id] = vol;
    return ma_decoder_init_file(filepath, &a->dconf, &a->sources[id]);
}
void PlaySound(GameAudio* a, SoundID id) {
    a->playing[id] = TRUE;
}
void StopSound(GameAudio* a, SoundID id) {
    a->playing[id] = FALSE;
}

void ResetSound(GameAudio* a, SoundID id) {
    ma_decoder_seek_to_pcm_frame(&a->sources[id], 0);
}
