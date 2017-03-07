//
// Created by 西澤佳祐 on 2017/03/07.
//

//
// Created by 西澤佳祐 on 2017/03/06.
//
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include "wave.h"
#include "fft.h"
#include "window_function.h"
#define PAI 3.14159265358979323846264338 // PAI
#define SAMPLE_RATE 44100 //sampling rate
#define LENGTH 3 // 3 seconds
#define AMPLITUDE 1.0// 16bit
#define FREQUENCY 440 // frequency Hz
#define RESOLUTION 1000
#define FFTSIZE 2048
#define OVERLAP 44000
#define THRESHOLD 0.85

extern "C"

void Java_keisukenishizawa_melodymemo_MainActivity_pitch(
        JNIEnv *env,
        double *j_pitch,
        char file,
        jobject this) {
    MONO_PCM pcm0;
    int n, k, N;
    double *x_real, *x_imag, *w, *powerspec, *rt, *autocorrelation, *NSDF, *peakBin, *pitch_;
    int maxdB = 0;
    int maxN = 0;
    double maxBin = 0.0;
    int peakCnt = 0;
    int *peakTime;
    int overlap = 0;
    int incr = 1000;
    //double pitch;

    int frame_num = SAMPLE_RATE * FFTSIZE;
    mono_wave_read(&pcm0, "voice2.wav"); /* WAVE?t?@?C?????烂?m?????̉??f?[?^???͂???*/

    printf("(%d)\n", pcm0.length);

    N = FFTSIZE;
    pitch_ = (double *) calloc(pcm0.length/incr, sizeof(double));
    peakTime = (int *) calloc(N, sizeof(int));
    x_real = (double *) calloc(N, sizeof(double)); /* ???????̊m??*/
    x_imag = (double *) calloc(N, sizeof(double)); /* ???????̊m??*/
    powerspec = (double *) calloc(N, sizeof(double)); /* ???????̊m??*/
    rt = (double *) calloc(N, sizeof(double));
    autocorrelation = (double *) calloc(N, sizeof(double)); /* ???????̊m??*/
    NSDF = (double *) calloc(N, sizeof(double));
    peakBin = (double *) calloc(N, sizeof(double));
    w = (double *) calloc(N, sizeof(double)); /* ???????̊m??*/




    Hanning_window(w, N); /* ?n?j???O?? */
    for (overlap = 0; overlap < pcm0.length; overlap = overlap + incr) {
        for (n = 0; n < N; n++) {
            x_real[n] = pcm0.s[n + overlap] * w[n]; /* x(n)?̎????? */
            x_imag[n] = 0.0; /* x(n)?̋????? */
        }

        FFT(x_real, x_imag, FFTSIZE); /* FFT?̌v?Z???ʂ?_real??_imag?ɏ㏑?????? */


        for (n = 0; n < FFTSIZE / 2; n++) {
            powerspec[n] = sqrt(x_real[n] * x_real[n] + x_imag[n] * x_imag[n]);
        }

        for (n = 0; n < FFTSIZE / 2; n++) {
            x_real[n] = powerspec[n];
            x_imag[n] = 0.0;
        }

        IFFT(x_real, x_imag, FFTSIZE);

        for (n = 0; n < FFTSIZE / 2; n++) {
            rt[n] = x_real[0] * x_real[n];
        }


        for (n = 0; n < FFTSIZE / 2; n++) {
            autocorrelation[n] = x_real[0] * x_real[0] + x_real[n] * x_real[n];
        }


        for (n = 0; n < FFTSIZE / 2; n++) {
            if (autocorrelation[n]) {
                NSDF[n] = 2 * rt[n] / autocorrelation[n];
            } else {
                NSDF[n] = 0.0;
            }
        }

        for (n = 0; n < FFTSIZE / 2; n++) {
            if (NSDF[n] >= 0.0) {
                if (NSDF[n] > peakBin[peakCnt]) {
                    peakBin[peakCnt] = NSDF[n];
                    peakTime[peakCnt] = n;
                }
            } else if (NSDF[n - 1] >= 0.0) {
                // printf("%d: %lf\n",peakCnt,peakBin[peakCnt]);
                peakCnt++;
            }
        }

        for (k = 1; k < peakCnt; k++) {
            if (peakBin[k] > THRESHOLD) {
                break;
            }
        }

        //ここで余裕があれば補間処理を書く

//ハイCがC5
//人間の最高音であるF6が1400Hz
        //人間の最低音であるC2が65Hz
        if (peakTime[k] != 0.0 && SAMPLE_RATE / peakTime[k] < 1000.0 &&
            SAMPLE_RATE / peakTime[k] > 65.0) {
            j_pitch[overlap / incr] = SAMPLE_RATE / peakTime[k];
        } else {
            j_pitch[overlap / incr] = 0.0;//スレッショルドを超えるものがなかった時
        }
        for (n = 0; n < FFTSIZE / 2; n++) {
            peakBin[n] = 0.0;
            peakTime[n] = 0;
        }
        peakCnt = 0;
        printf("count: %d, pitch:%.2lf\n", overlap / incr, j_pitch[overlap / incr]);
    }
//    Write_Data2(pitch_,overlap/incr);


    free(rt);
    free(powerspec);
    free(autocorrelation);
    free(NSDF);
    free(w);
    free(pitch_);
    free(pcm0.s); /* ???????̉? */
    free(x_real); /* ???????̉? */
    free(x_imag); /* ???????̉? */

    //  return env->;
}
