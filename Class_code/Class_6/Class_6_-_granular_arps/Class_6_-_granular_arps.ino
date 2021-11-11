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
AudioInputI2S            i2s2;           //xy=167,175
AudioEffectGranular      granular1;      //xy=337,244
AudioMixer4              mixer1;         //xy=541,232
AudioOutputI2S           i2s1;           //xy=835,250
AudioConnection          patchCord1(i2s2, 0, granular1, 0);
AudioConnection          patchCord2(i2s2, 0, mixer1, 0);
AudioConnection          patchCord3(granular1, 0, mixer1, 1);
AudioConnection          patchCord4(mixer1, 0, i2s1, 0);
AudioConnection          patchCord5(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=781,379
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

int index1;
float speed_array[4] = {1.25, .5, .4, 1.0};
int16_t granular_array[10000]; //taht's the max the granular libray can use

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

  granular1.begin(granular_array, 10000); //array, max_length

  mixer1.gain(0, 0); //from audio input
  mixer1.gain(1, 1); //from granular effect

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
    Serial.println("!");
    granular1.beginFreeze(grain_length); //input variable is in milliseconds
  }

  if (buttons[2].fell()) {
    granular1.stop();  //stop eiter effect
  }

  grain_length = (potRead(0) * 100.0); //32000 samples is about 700 millis
  grain_speed = (potRead(1) * 2.0);
  //granular1.setSpeed(grain_speed); //resposed to .125 to 8. 1 is regular speed

  amp1 = potRead(2); //returns 0-1.0 already
  mixer1.gain(0, amp1); //dry
  mixer1.gain(1, 1.0 - amp1); //wet


  if (current_time - prev_time[1] > 250) {
    prev_time[1] = current_time;
    index1++;
    index1 %= 4;
    
    granular1.setSpeed(speed_array[index1]*grain_speed);
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
