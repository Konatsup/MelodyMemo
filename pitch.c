#include <stdio.h>
#include <stdlib.h>
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
#define THRESHOLD 0.80
void Write_Data(double *NSDF,int N){

  FILE *fpout;

  if((fpout = fopen("pitch.dat","w"))== NULL){
    fprintf(stderr,"FILE open ERROR\n");
    exit(-1);
  }
  for(int i=0;i<N;i++){
    fprintf(fpout,"%d %lf\n", i,NSDF[i]);
  }
/*
  for(int i=0;i<N/2;i++){
    fprintf(fpout,"%d %lf\n", (i+N/2)*(SAMPLE_RATE/FFTSIZE/2),NSDF[i]);
  }
  for(int i=0;i<N/2;i++){
    fprintf(fpout,"%d %lf\n", i*(SAMPLE_RATE/FFTSIZE/2),NSDF[i+N/2]);
  }*/
  fclose(fpout);

}

void Write_Data2(double *NSDF,int N){

  FILE *fpout;

  if((fpout = fopen("pitch_i.dat","w"))== NULL){
    fprintf(stderr,"FILE open ERROR\n");
    exit(-1);
  }

  for(int i=0;i<N;i++){
 //  fprintf(fpout,"%d %lf\n", i*(SAMPLE_RATE/FFTSIZE),NSDF[i]);
   fprintf(fpout,"%d %lf\n", i,NSDF[i]);
  }
 /* for(int i=0;i<N/2;i++){
    fprintf(fpout,"%d %lf\n", (i+N/2)*(SAMPLE_RATE/FFTSIZE/2),NSDF[i]);
  }
  for(int i=0;i<N/2;i++){
    fprintf(fpout,"%d %lf\n", i*(SAMPLE_RATE/FFTSIZE/2),NSDF[i+N/2]);
  }
*/
  fclose(fpout);

}

int main(void)
{
  MONO_PCM pcm0;
  int n, k, N;
  double *x_real, *x_imag,*w,*powerspec,*rt,*autocorrelation,*NSDF, *peakBin , *pitch_;
  int maxdB=0;
  int maxN = 0;
  double maxBin = 0.0;
  int peakCnt = 0;
  int *peakTime;
  int overlap = 0;
  int incr = 1102;
  //double pitch;

  int frame_num = SAMPLE_RATE*FFTSIZE;
  mono_wave_read(&pcm0, "kaeru2_mix.wav"); /* WAVE?t?@?C?????烂?m?????̉??f?[?^???͂???*/
  //N = SAMPLE_RATE*LENGTH; //?t???[????

  printf("(%d)\n",pcm0.length);

  N = FFTSIZE;
  //N = 64;
  pitch_ = calloc(pcm0.length+10,sizeof(double));
  peakTime = calloc(N, sizeof(int));
  x_real = calloc(N, sizeof(double)); /* ???????̊m??*/
  x_imag = calloc(N, sizeof(double)); /* ???????̊m??*/
  powerspec = calloc(N, sizeof(double)); /* ???????̊m??*/
  rt = calloc(N, sizeof(double));
  autocorrelation = calloc(N, sizeof(double)); /* ???????̊m??*/
  NSDF = calloc(N, sizeof(double));
  peakBin = calloc(N,sizeof(double));
  w = calloc(N, sizeof(double)); /* ???????̊m??*/




  Hanning_window(w, N); /* ?n?j???O?? */
  for(overlap =0;overlap<pcm0.length;overlap=overlap + incr){
    for (n = 0; n < N; n++)
    {
      x_real[n] = pcm0.s[n+overlap] * w[n]; /* x(n)?̎????? */
      x_imag[n] = 0.0; /* x(n)?̋????? */
    }
    
    //Write_Data(x_real,N);
    FFT(x_real, x_imag, FFTSIZE); /* FFT?̌v?Z???ʂ?_real??_imag?ɏ㏑?????? */
    
    //Write_Data(x_real,FFTSIZE);
    //Write_Data2(x_imag,FFTSIZE);
    //?p???[?X?y?N?g???ɕϊ?
    for (n = 0; n < FFTSIZE/2; n++)
    {
      powerspec[n] = sqrt(x_real[n]*x_real[n] + x_imag[n]*x_imag[n]);
    }

  //  Write_Data(x_real,FFTSIZE);
   // Write_Data2(x_imag,FFTSIZE);
  //  Write_Data2(powerspec,FFTSIZE/2);
  //  Write_Data(powerspec,SAMPLE_RATE/2);
  //  printf("N:%d, dB:%d\n",maxN,maxdB);
    for (n = 0; n < FFTSIZE/2; n++)
    {
      x_real[n] = powerspec[n];
      x_imag[n] = 0.0;
    }

    // Write_Data2(x_real,FFTSIZE/2);

   // Write_Data2(autocorrelation,FFTSIZE/2);


    //?t?????t?[???G?ϊ?
    IFFT(x_real, x_imag, FFTSIZE);
    //Write_Data(x_real,N);
   // Write_Data2(x_imag,N);

  for (n = 0; n < FFTSIZE/2; n++)
    {
      rt[n] = x_real[0]*x_real[n];
    }


    //???ȑ??֊֐??̕????v?Z
  /* for (n = 0; n < FFTSIZE/2; n++)
    {
      autocorrelation[n] = sqrt(x_real[n]*x_real[n] + x_imag[n]*x_imag[n]);
    }*/

  /*
   for (n = 0; n < FFTSIZE/2; n++)
    {
      autocorrelation[n] = x_real[n] + x_imag[n];
    }
  */

   for (n = 0; n < FFTSIZE/2; n++)
    {
      autocorrelation[n] = x_real[0]*x_real[0] + x_real[n]*x_real[n];
    }
  //  Write_Data2(autocorrelation,N/2);

    //?W????????
  /*
    for(n=0;n<FFTSIZE/2;n++){
      if(autocorrelation[n]){
        NSDF[n] = autocorrelation[n]/autocorrelation[0];
      }else{
        NSDF[n]= 0.0;
      }  
    }*/

    for(n=0;n<FFTSIZE/2;n++){
      if(autocorrelation[n]){
        NSDF[n] = 2*rt[n]/autocorrelation[n];
      }else{
        NSDF[n]= 0.0;
      }  
    }

  /*
    for(n=0;n<FFTSIZE/2;n++){
        NSDF[n] = 2*nt[n]/autocorrelation[n];
    }
  */
   // Write_Data2(NSDF,FFTSIZE/2);
  for(n=0;n<FFTSIZE/2;n++){
    if(NSDF[n]>=0.0){
      if(NSDF[n]>peakBin[peakCnt]){
        peakBin[peakCnt] = NSDF[n];
        peakTime[peakCnt] = n;
      }
    }else if(NSDF[n-1]>=0.0){
   // printf("%d: %lf\n",peakCnt,peakBin[peakCnt]);
    peakCnt++;
    }
  }

  for(k = 1;k<peakCnt;k++){
    if(peakBin[k]>THRESHOLD){
      break;
    }
  }

  //ここで余裕があれば補間処理を書く

//ハイCがC5
//人間の最高音であるF6が1400Hz
  //人間の最低音であるC2が65Hz
  if(peakTime[k]!=0.0 && SAMPLE_RATE/peakTime[k] <1000.0 && SAMPLE_RATE/peakTime[k] > 65.0){
    pitch_[overlap/incr] = SAMPLE_RATE/peakTime[k];
  }else{
    pitch_[overlap/incr] = 0.0;//スレッショルドを超えるものがなかった時
  }
  for(n=0;n<FFTSIZE/2;n++){
    peakBin[n] = 0.0;
    peakTime[n] = 0;
}
peakCnt = 0;
printf("count: %d, pitch:%.2lf\n",overlap/incr,pitch_[overlap/incr]);
}
Write_Data2(pitch_,overlap/incr);

//printf("pitch:%.2lf\n",pitch);
//Write_Data(NSDF);
  /* ???g?????? 
  for (k = 0; k < N; k++)
  {
    printf("%d %f+j%f\n", k, x_real[k], x_imag[k]);
  }
  */

  free(rt);
  free(powerspec);
  free(autocorrelation);
  free(NSDF);
  free(w);
  free(pitch_);
  free(pcm0.s); /* ???????̉? */
  free(x_real); /* ???????̉? */
  free(x_imag); /* ???????̉? */
  
  return 0;
}
