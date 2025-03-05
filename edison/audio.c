#define MINIAUDIO_IMPLEMENTATION

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "audio.h"

ma_device sine_audio_device;
playback_data sine_audio_data;
ma_result sine_audio_device_result;
ma_device_config sine_audio_device_config;

ma_device tri_audio_device;
playback_data tri_audio_data;
ma_result tri_audio_device_result;
ma_device_config tri_audio_device_config;

ma_device sine_audio_device_1;
playback_data sine_audio_data_1;
ma_result sine_audio_device_result_1;
ma_device_config sine_audio_device_config_1;

ma_device tri_audio_device_1;
playback_data tri_audio_data_1;
ma_result tri_audio_device_result_1;
ma_device_config tri_audio_device_config_1;

ma_result audio_device_result;

int old_frequency = 0;
int old_callback = 0;
int old_frequency_1 = 0;
int old_callback_1 = 0;

int audio_device_init(void){
    
    /* Initialize sine wave configuration */
    sine_audio_data.wave_position = 0.0f;
    sine_audio_data.wave_increment = (float)420 / SAMPLE_RATE;

    sine_audio_device_config = ma_device_config_init(ma_device_type_playback); 

    sine_audio_device_config.playback.format = ma_format_f32;  // 32-bit floating-point samples
    sine_audio_device_config.playback.channels = 2;            // Stereo
    sine_audio_device_config.sampleRate = SAMPLE_RATE;

    sine_audio_device_config.dataCallback = sine_wave_callback;

    sine_audio_device_config.pUserData = &sine_audio_data;

    sine_audio_device_result = ma_device_init(NULL, &sine_audio_device_config, &sine_audio_device);
    if (sine_audio_device_result  != MA_SUCCESS) {
        printf("Failed to initialize sine playback device.\n");
        return EXIT_FAILURE;
    }

    /* Initialize sine wave 1 configuration */
    sine_audio_data_1.wave_position = 0.0f;
    sine_audio_data_1.wave_increment = (float)420 / SAMPLE_RATE;

    sine_audio_device_config_1 = ma_device_config_init(ma_device_type_playback); 

    sine_audio_device_config_1.playback.format = ma_format_f32;  // 32-bit floating-point samples
    sine_audio_device_config_1.playback.channels = 2;            // Stereo
    sine_audio_device_config_1.sampleRate = SAMPLE_RATE;

    sine_audio_device_config_1.dataCallback = sine_wave_callback;

    sine_audio_device_config_1.pUserData = &sine_audio_data_1;

    sine_audio_device_result_1 = ma_device_init(NULL, &sine_audio_device_config_1, &sine_audio_device_1);
    if (sine_audio_device_result_1  != MA_SUCCESS) {
        printf("Failed to initialize sine playback device.\n");
        return EXIT_FAILURE;
    }


    /* Initialize triangle wave configuration */
    tri_audio_data.wave_position = 0.0f;
    tri_audio_data.wave_increment = (float)420 / SAMPLE_RATE;

    tri_audio_device_config = ma_device_config_init(ma_device_type_playback); 

    tri_audio_device_config.playback.format = ma_format_f32;  // 32-bit floating-point samples
    tri_audio_device_config.playback.channels = 2;            // Stereo
    tri_audio_device_config.sampleRate = SAMPLE_RATE;

    tri_audio_device_config.dataCallback = triangle_wave_callback;

    tri_audio_device_config.pUserData = &tri_audio_data;

    tri_audio_device_result = ma_device_init(NULL, &tri_audio_device_config, &tri_audio_device);
    if (tri_audio_device_result  != MA_SUCCESS) {
        printf("Failed to initialize triangle playback device.\n");
        return EXIT_FAILURE;
    }

    /* Initialize triangle wave configuration */
    tri_audio_data_1.wave_position = 0.0f;
    tri_audio_data_1.wave_increment = (float)420 / SAMPLE_RATE;

    tri_audio_device_config_1 = ma_device_config_init(ma_device_type_playback); 

    tri_audio_device_config_1.playback.format = ma_format_f32;  // 32-bit floating-point samples
    tri_audio_device_config_1.playback.channels = 2;            // Stereo
    tri_audio_device_config_1.sampleRate = SAMPLE_RATE;

    tri_audio_device_config_1.dataCallback = triangle_wave_callback;

    tri_audio_device_config_1.pUserData = &tri_audio_data_1;

    tri_audio_device_result_1 = ma_device_init(NULL, &tri_audio_device_config_1, &tri_audio_device_1);
    if (tri_audio_device_result_1  != MA_SUCCESS) {
        printf("Failed to initialize triangle playback device.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}   

void play_sound(int frequency,int channel){

    /* Get old frequencys */
    int* old_freq  = &old_frequency;
    int* old_call  = &old_callback;

    /* Mask away control bit */
    int real_frequency = frequency & 0x7FFF;

    /* Get control bit */
    int ctrl = frequency & 0x8000;


    ma_device* audio_device = ctrl ? &tri_audio_device : &sine_audio_device;
    playback_data* audio_data = ctrl ? &tri_audio_data : &sine_audio_data;
    if(channel){
        audio_device = ctrl ? &tri_audio_device_1 : &sine_audio_device_1;
        audio_data = ctrl ? &tri_audio_data_1 : &sine_audio_data_1;
        old_freq  = &old_frequency_1;
        old_call  = &old_callback_1;
    }

    /* Stop device on zero frequency */
    if(real_frequency == 0){
        *old_freq = real_frequency;
        if(channel){
            ma_device_stop(&tri_audio_device_1);
            ma_device_stop(&sine_audio_device_1);
        }else{
            ma_device_stop(&tri_audio_device);
            ma_device_stop(&sine_audio_device);
        }
        return;
    } 

    if(ctrl != *old_call){
        ma_device* old_device = *old_call ? &tri_audio_device : &sine_audio_device;

        if(channel) old_device = *old_call ? &tri_audio_device_1 : &sine_audio_device_1;

        ma_device_stop(old_device);
    }

    if(*old_freq != real_frequency || ctrl != *old_call) {
        audio_data->wave_position = 0.0f;
        audio_data->wave_increment = (float)real_frequency / SAMPLE_RATE;
        audio_device_result = ma_device_start(audio_device);
    }

    if (audio_device_result != MA_SUCCESS && *old_freq == 0) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(audio_device);
        return;
    }

    *old_call = ctrl;
    *old_freq = real_frequency;
}

void free_audio(void){
    ma_device_uninit(&sine_audio_device);
    ma_device_uninit(&tri_audio_device);
    ma_device_uninit(&sine_audio_device_1);
    ma_device_uninit(&tri_audio_device_1);
}

/* Callback for sine wave audio buffer */ 
void sine_wave_callback(ma_device* device, void* output, const void* input, ma_uint32 frame_count) {
    playback_data* data = (playback_data*)device->pUserData;
    float* out = (float*)output;

    for (ma_uint32 i = 0; i < frame_count; ++i) {
        float sample = AMPLITUDE * sinf(2 * M_PI * data->wave_position);
        data->wave_position += data->wave_increment;
        if (data->wave_position >= 1.0f) {
            data->wave_position -= 1.0f;
        }

        // Stereo output (duplicate to left and right channel)
        out[i * 2 + 0] = sample;  // Left channel
        out[i * 2 + 1] = sample;  // Right channel
    }

    (void)input;
}

/* Callback for triangle wave audio buffer */ 
void triangle_wave_callback(ma_device* device, void* output, const void* input, ma_uint32 frame_count) {
    playback_data* data = (playback_data*)device->pUserData;
    float* out = (float*)output;

    for (ma_uint32 i = 0; i < frame_count; ++i) {
        // Generate the triangle wave sample
        float sample = 2.0f * fabsf(2.0f * (data->wave_position - floorf(data->wave_position + 0.5f))) - 1.0f;
        sample *= AMPLITUDE;

        // Move the triangle wave position
        data->wave_position += data->wave_increment;
        if (data->wave_position >= 1.0f) {
            data->wave_position -= 1.0f;
        }

        // Stereo output (duplicate to left and right channel)
        out[i * 2 + 0] = sample;  // Left channel
        out[i * 2 + 1] = sample;  // Right channel
    }

    (void)input;
}
