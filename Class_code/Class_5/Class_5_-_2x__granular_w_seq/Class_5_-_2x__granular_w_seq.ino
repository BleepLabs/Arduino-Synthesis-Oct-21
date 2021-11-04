//Using the granual effect
//Capture small pieces of audio and repeat and play them at differnt rates.
// This was an early library I did and got changes a lot by Paul
// so it's got it's issues but works ok

// The block below is copied from the design tool: https://www.pjrc.com/teensy/gui/
// "#include" means add another file to our sketch
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s2;           //xy=110,98
AudioEffectGranular      granular1;      //xy=290,251
AudioEffectGranular      granular2;      //xy=298,310
AudioSynthKarplusStrong  string1;        //xy=300,373
AudioMixer4              mixer1;         //xy=505,280
AudioOutputI2S           i2s1;           //xy=778,180
AudioConnection          patchCord1(i2s2, 0, granular1, 0);
AudioConnection          patchCord2(i2s2, 0, mixer1, 0);
AudioConnection          patchCord3(i2s2, 0, granular2, 0);
AudioConnection          patchCord4(granular1, 0, mixer1, 1);
AudioConnection          patchCord5(granular2, 0, mixer1, 2);
AudioConnection          patchCord6(string1, 0, mixer1, 3);
AudioConnection          patchCord7(mixer1, 0, i2s1, 0);
AudioConnection          patchCord8(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=741,413
// GUItool: end automatically generated code





#include "bleep_base.h" //Contains functions we'll need when using the bleep base

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float freq1; //frequencies should always be stored as floats
float freq2;
float amp1, amp2; //you can also declare variables like this. Both will be floats
float grain_length;
float grain_speed;

int16_t granular1_array[32000]; //that's the max the granular libray can use
int16_t granular2_array[32000];

int seq1[8] = {1, 0, 1, 0, 0, 0, 0, 0};
int seq2[8] = {0, 0, 1, 1, 0, 0, 1, 0};
int index1, index2;

int note_state1, note_state2;

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

  granular1.begin(granular1_array, 32000); //array, max_length
  granular2.begin(granular2_array, 32000);

  mixer1.gain(0, 0); //from audio input
  mixer1.gain(1, .3); //from granular1 effect
  mixer1.gain(2, .3); //from granular2 effect
  mixer1.gain(3, .3);  //string1
} //setup is over

void loop() {
  current_time = millis();

  for (int i = 0; i < 8; i = i + 1)  {
    buttons[i].update();
  }

  if (buttons[0].fell()) {
    //chage the pitch of a stream on incoming audio
    granular1.beginPitchShift(grain_length); //input variable is in milliseconds
  }

  if (buttons[1].fell()) {
    //capture x amount of audio and repeat it.
    granular1.beginFreeze(grain_length); //input variable is in milliseconds
  }

  if (buttons[2].fell()) {
    granular1.stop();  //stop eiter effect
  }

  grain_length = (potRead(0) * 700.0); //32000 samples is about 700 millis
  grain_speed = (potRead(1) * 4.0);
  granular1.setSpeed(grain_speed); //resposed to .125 to 8. 1 is regular speed
  granular2.setSpeed(grain_speed);

  amp1 = potRead(2); //returns 0-1.0 already
//  mixer1.gain(0, amp1); //dry
//  mixer1.gain(1, 1.0 - amp1); //granular 1
//  mixer1.gain(2, 1.0 - amp1); //granular 1

  if (current_time - prev_time[2] > 900) {
    prev_time[2] = current_time;
    index1++;
    if (index1 > 7) {
      index1 = 0;
    }
    if (seq1[index1] == 1) {
      note_state1 = !note_state1;
    }
    if (note_state1 == 1) {
      string1.noteOn(random(100, 500), .5);
      granular2.beginFreeze(grain_length);
    }
    if (note_state1 == 0) {
      string1.noteOff(.5);
      granular2.stop();
    }
  }

  if (current_time - prev_time[1] > 1000) {
    prev_time[1] = current_time;
    granular1.beginFreeze(grain_length);

  }


  if (current_time - prev_time[0] > 500) {
    prev_time[0] = current_time;

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
