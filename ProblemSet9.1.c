#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sndfile.h>



#define kSampleRate 44100
#define kSec 1
#define kNumFrames (kSampleRate * kSec)
#define kNumChannels 2
#define kFormat (SF_FORMAT_WAV | SF_FORMAT_PCM_24)
#define kFileName "TriWave.wav"

void plotAndPlay();

int main(void){
  SNDFILE *sndFile;
  SF_INFO sfInfo;
  double *buffer;


  double amplitude = 0.25;
  double frequency = 250.0;

  
  buffer = malloc(kNumChannels * kNumFrames * sizeof(double));
  if (!buffer){
    printf ("Error : Malloc failed.\n");
		return 1;
	};

  sfInfo.samplerate = kSampleRate;
	sfInfo.frames = kNumFrames; 
	sfInfo.channels = kNumChannels;
	sfInfo.format = kFormat;

 
  sndFile = sf_open(kFileName, SFM_WRITE, &sfInfo);
  if(!sndFile){
    printf ("Error : could not open file : %s\n", kFileName);
		puts(sf_strerror (NULL));
		return 1;
  }

  //Compute Sawtooth Wave (interleave channels)
  for(int n = 0; n < kNumFrames; n++){
    for(int i = 1; i <= 15; i += 1){ //Up to 15th harmonics 
      if(frequency * (2 * i - 1) < kSampleRate / 2){ 
        double sample = amplitude * cos(2 * M_PI * (2 * i - 1) * frequency * n / kSampleRate) / pow(2 * i - 1, 2);
      for(int c = 0; c < kNumChannels; c++){
        buffer[kNumChannels * n + c] += sample;
      }
      }
    }
  }
  

  sf_count_t count = sf_write_double(sndFile, buffer, sfInfo.channels * kNumFrames);

  //Make sure writing the file succeeeded  
  if(count != sfInfo.channels * kNumFrames){
    puts(sf_strerror(sndFile));
  }

  //Close sound file and buffer
  sf_close(sndFile);
  free(buffer);
  
  plotAndPlay(kFileName);

  return 0;
}

void plotAndPlay(char *fileName){

  char command[100]; 
  sprintf(command, "sox \"%s\" -r 8000 plot.dat", fileName);
  system(command);
  

  system("gnuplot --persist -e \"plot 'plot.dat' every ::1::100 using 1:2 with lines\"");


  sprintf(command, "afplay \"%s\" 2> /dev/null", fileName);
  system(command);
}