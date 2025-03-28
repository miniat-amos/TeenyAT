#ifndef __AUDIO_H__
#define __AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "miniaudio.h"

#define SAMPLE_RATE 44100 // A4 note
#define AMPLITUDE 0.5f

typedef struct {
    float wave_position;
    float wave_increment;
} playback_data;

int audio_device_init(void);
void play_sound(int,int);
void free_audio(void);
void sine_wave_callback(ma_device*, void*, const void*, ma_uint32 );
void triangle_wave_callback(ma_device*, void*, const void*, ma_uint32);

#ifdef __cplusplus
}
#endif

#endif  /* __AUDIO_H__ */ 
