#include "naga8_audio.h"
#include "raylib.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>

#define TONE_SAMPLE_RATE 22050

typedef struct {
    Sound sound;
    bool  used;
} ToneChannel;

static ToneChannel g_channels[4];
static bool        g_seeded = false;

/* --- Генераторы --- */

static void gen_pulse(short *samples, int count, float freq, int volume, float duty)
{
    for (int i = 0; i < count; i++) {
        float t     = (float)i / TONE_SAMPLE_RATE;
        float phase = fmodf(t * freq, 1.0f);
        float v     = (phase < duty) ? 1.0f : -1.0f;
        samples[i]  = (short)(v * volume * 327.67f);
    }
}

static void gen_triangle(short *samples, int count, float freq, int volume)
{
    for (int i = 0; i < count; i++) {
        float t     = (float)i / TONE_SAMPLE_RATE;
        float phase = fmodf(t * freq, 1.0f);
        float v     = 2.0f * fabsf(2.0f * phase - 1.0f) - 1.0f;
        samples[i]  = (short)(v * volume * 327.67f);
    }
}

static void gen_noise(short *samples, int count, int volume)
{
    for (int i = 0; i < count; i++) {
        float v    = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
        samples[i] = (short)(v * volume * 327.67f);
    }
}

/* --- Init / Close --- */

void naga8_audio_init(void)
{
    InitAudioDevice();
    for (int i = 0; i < 4; i++) {
        g_channels[i].used = false;
    }
}

void naga8_audio_close(void)
{
    for (int i = 0; i < 4; i++) {
        if (g_channels[i].used) {
            StopSound(g_channels[i].sound);
            UnloadSound(g_channels[i].sound);
            g_channels[i].used = false;
        }
    }
    CloseAudioDevice();
}

/* --- Tone --- */

void naga8_tone(int frequency, int duration, int volume, int channel)
{
    if (duration <= 0 || volume <= 0) return;
    if (channel < 0 || channel > 3) return;
    if (!IsAudioDeviceReady()) return;

    if (!g_seeded) {
        srand((unsigned)time(NULL));
        g_seeded = true;
    }

    float freq         = (float)frequency;
    float duration_sec = duration / 60.0f;
    int   sample_count = (int)(TONE_SAMPLE_RATE * duration_sec);
    if (sample_count <= 0) return;

    short *samples = (short *)malloc(sample_count * sizeof(short));
    if (!samples) return;

    switch (channel) {
        case 0: gen_pulse(samples, sample_count, freq, volume, 0.125f); break;
        case 1: gen_pulse(samples, sample_count, freq, volume, 0.250f); break;
        case 2: gen_triangle(samples, sample_count, freq, volume);      break;
        case 3: gen_noise(samples, sample_count, volume);               break;
    }

    Wave wave = {
        .frameCount = (unsigned int)sample_count,
        .sampleRate = TONE_SAMPLE_RATE,
        .sampleSize = 16,
        .channels   = 1,
        .data       = samples
    };

    Sound s = LoadSoundFromWave(wave);
    free(samples);
    if (s.frameCount == 0) return;

    if (g_channels[channel].used) {
        StopSound(g_channels[channel].sound);
        UnloadSound(g_channels[channel].sound);
        g_channels[channel].used = false;
    }

    g_channels[channel].sound = s;
    g_channels[channel].used  = true;
    PlaySound(s);
}