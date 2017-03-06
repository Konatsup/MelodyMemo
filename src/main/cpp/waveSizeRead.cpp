//
// Created by 西澤佳祐 on 2017/03/06.
//

#include "waveSizeRead.h"
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include "wave.h"
double Java_keisukenishizawa_melodymemo_MainActivity_waveSizeRead(
        JNIEnv *env,
        char *waveFile,
        jobject /* this */){
    MONO_PCM pcm0;
    mono_wave_read(&pcm0, waveFile);
        return pcm0.length;
};