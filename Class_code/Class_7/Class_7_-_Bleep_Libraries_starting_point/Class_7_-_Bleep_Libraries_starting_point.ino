/*
  Using John-Mikes audio libraries
  you no longer have to have a tab with bleep_base.h in it, now it's just a regualr library

  download libraries
  bleep_base_2v1 https://downgit.github.io/#/home?url=https://github.com/BleepLabs/Arduino-Synthesis-Oct-21/tree/main/libraries/bleep_base_2v1
  bleep_audio https://downgit.github.io/#/home?url=https://github.com/BleepLabs/Arduino-Synthesis-Oct-21/tree/main/libraries/bleep_audio
  
  since my objects aren't in the tool you'll need to replace the names
  So you could use the delay object but change what's imported from:
  AudioEffectDelay delay1;
  to
  AudioEffectTapeDelay delay1;
  the origial tool output is after the loop for reference

  find all the info on the audio library here https://github.com/BleepLabs/Arduino-Synthesis-Oct-21/wiki/Bleep-Audio
*/

//must be before the block from the tool
#include <bleep_audio.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=124,441
AudioSynthNoiseWhite     noise1;         //xy=129,390
AudioInputI2S            i2s1;           //xy=261,218
AudioMixer4              envelope_mixer;         //xy=304,434
MemSampler      sampler1;      //xy=419,293
AudioEffectEnvelopeAdjustable      envelope1;      //xy=475,427
AudioMixer4              mixer1;         //xy=594,300
AudioEffectTapeDelay         delay1;         //xy=738,535
AudioMixer4              delay_mixer;         //xy=744,395
AudioMixer4              wet_dry_mixer;         //xy=988,403
AudioAmplifier           output_amp1;           //xy=1037,302
AudioOutputI2S           i2s2;           //xy=1072,224
AudioConnection          patchCord1(waveform1, 0, envelope_mixer, 1);
AudioConnection          patchCord2(noise1, 0, envelope_mixer, 0);
AudioConnection          patchCord3(i2s1, 0, sampler1, 0);
AudioConnection          patchCord4(i2s1, 1, wet_dry_mixer, 2);
AudioConnection          patchCord5(envelope_mixer, envelope1);
AudioConnection          patchCord6(sampler1, 0, mixer1, 0);
AudioConnection          patchCord7(envelope1, 0, mixer1, 1);
AudioConnection          patchCord8(mixer1, 0, delay_mixer, 0);
AudioConnection          patchCord9(mixer1, 0, wet_dry_mixer, 0);
AudioConnection          patchCord10(delay1, 0, delay_mixer, 1);
AudioConnection          patchCord11(delay_mixer, delay1);
AudioConnection          patchCord12(delay_mixer, 0, wet_dry_mixer, 1);
AudioConnection          patchCord13(wet_dry_mixer, output_amp1);
AudioConnection          patchCord14(output_amp1, 0, i2s2, 0);
AudioConnection          patchCord15(output_amp1, 0, i2s2, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=863,653
// GUItool: end automatically generated code


#include "bleep_base_2v1.h" //Contains functions we'll need when using the bleep base

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float freq1; //frequencies should always be stored as floats
float freq2;
float amp1, amp2; //you can also declare variables like this. Both will be floats

#define sample_bank_length 44100 //1 second
unsigned int sampler_bank1[sample_bank_length];

#define delay_bank_length 44100 //1 second
short delay_bank[delay_bank_length];

void setup() {

  start_bleep_base(); //Gets the LEDs, pots, and buttons ready

  //there's a lot we need to do in setup now but most of it is just copy paste.
  // This first group should only be done in setup how much RAM to set aside for the audio library to use.
  // The audio library uses blocks of a set size so this is not a percentage or kilobytes, just a kind of arbitrary number.
  // On our Teensy 4.1 we can go up to about 1900 but that won't leave any RAM for anyone else.
  // Most objects only need a single block. It's usually the delay and reverb that hog it.
  AudioMemory(10);

  sgtl5000_1.enable(); //Turn the adapter board on
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN); //Tell it what input we want to use. Not necessary is you're not using the ins
  sgtl5000_1.lineInLevel(10); //The volume of the input. 0-15 with 15 bing more amplifications
  //sgtl5000_1.inputSelect(AUDIO_INPUT_MIC);
  //sgtl5000_1.micGain(13); //0 - 63bd of gain.

  //headphone jack output volume. Goes from 0.0 to 1.0 but a 100% signal will clip over .8 or so.
  // For headphones it's pretty loud at .4
  // There are lots of places we can change the final volume level.
  // For now lets set this one once and leave it alone.
  sgtl5000_1.volume(0.25);

  //The line out has a separate level control but it's not meant to be adjusted like the volume function above.
  // If you're not using the line out don't worry about it.
  sgtl5000_1.lineOutLevel(21); //11-32, the smaller the louder. 21 is about 2 Volts peak to peak

  envelope1.attack(1);
  envelope1.decay(100);
  envelope1.sustain(.5);
  envelope1.release(100);
  envelope1.shape(-.9); //-1.0 is exp, 1 is log 0 is linear

  noise1.amplitude(1);
  waveform1.begin(1, 110, WAVEFORM_BANDLIMIT_SQUARE);
  envelope_mixer.gain(0, .5);
  envelope_mixer.gain(1, .5);

  mixer1.gain(0, .5);
  mixer1.gain(1, .5);

  sampler1.begin(sampler_bank1, sample_bank_length); //array, array length
  sampler1.frequency(1.0);
  delay1.begin(delay_bank, delay_bank_length, 0, 0, 2); //(delay bank, length, delay length, redux, lerp)

  delay_mixer.gain(0, .5);
  delay_mixer.gain(1, .5);

  wet_dry_mixer.gain(0, .5);
  wet_dry_mixer.gain(1, .5);
  wet_dry_mixer.gain(2, 0);
  wet_dry_mixer.gain(3, 0);

  output_amp1.gain(1);

} //setup is over

void loop() {
  current_time = millis();

  for (int i = 0; i < 8; i = i + 1)  {
    buttons[i].update();
  }

  if (buttons[0].fell()) {
    envelope1.trigger();
  }
  if (buttons[1].fell()) {
    sampler1.play();
  }
  if (buttons[2].fell()) {
    sampler1.record();
  }
  if (buttons[3].fell()) {
    sampler1.stop();
  }
  float len1 = potRead(0) * delay_bank_length;
  delay1.length(len1);

  if (current_time - prev_time[0] > 500) {
    prev_time[0] = current_time;
    Serial.println(len1);
    
    //Here we print out the usage of the audio library
    // If we go over 90% processor usage or get near the value of memory blocks we set aside in the setup we'll have issues or crash.
    // If you're using too many block, jut increase the number up top until you're over it by a couple
    Serial.print("processor: ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print("%    Memory: ");
    Serial.print(AudioMemoryUsageMax());
    Serial.println();
    AudioProcessorUsageMaxReset(); //We need to reset these values so we get a real idea of what the audio code is doing rather than just peaking in every half a second
    AudioMemoryUsageMaxReset();
  }

}// loop is over



//from the tool before I changed th names
/*
   #include <Audio.h>
  #include <Wire.h>
  #include <SPI.h>
  #include <SD.h>
  #include <SerialFlash.h>

  // GUItool: begin automatically generated code
  AudioSynthWaveform       waveform1;      //xy=124,441
  AudioSynthNoiseWhite     noise1;         //xy=129,390
  AudioInputI2S            i2s1;           //xy=261,218
  AudioMixer4              envelope_mixer;         //xy=304,434
  AudioEffectMultiply      sampler1;      //xy=419,293
  AudioEffectEnvelope      envelope1;      //xy=475,427
  AudioMixer4              mixer1;         //xy=594,300
  AudioEffectDelay         delay1;         //xy=738,535
  AudioMixer4              delay_mixer;         //xy=744,395
  AudioMixer4              wet_dry_mixer;         //xy=988,403
  AudioAmplifier           output_amp1;           //xy=1037,302
  AudioOutputI2S           i2s2;           //xy=1072,224
  AudioConnection          patchCord1(waveform1, 0, envelope_mixer, 1);
  AudioConnection          patchCord2(noise1, 0, envelope_mixer, 0);
  AudioConnection          patchCord3(i2s1, 0, sampler1, 0);
  AudioConnection          patchCord4(i2s1, 1, wet_dry_mixer, 2);
  AudioConnection          patchCord5(envelope_mixer, envelope1);
  AudioConnection          patchCord6(sampler1, 0, mixer1, 0);
  AudioConnection          patchCord7(envelope1, 0, mixer1, 1);
  AudioConnection          patchCord8(mixer1, 0, delay_mixer, 0);
  AudioConnection          patchCord9(mixer1, 0, wet_dry_mixer, 0);
  AudioConnection          patchCord10(delay1, 0, delay_mixer, 1);
  AudioConnection          patchCord11(delay_mixer, delay1);
  AudioConnection          patchCord12(delay_mixer, 0, wet_dry_mixer, 1);
  AudioConnection          patchCord13(wet_dry_mixer, output_amp1);
  AudioConnection          patchCord14(output_amp1, 0, i2s2, 0);
  AudioConnection          patchCord15(output_amp1, 0, i2s2, 1);
  AudioControlSGTL5000     sgtl5000_1;     //xy=863,653
  // GUItool: end automatically generated code
*/
