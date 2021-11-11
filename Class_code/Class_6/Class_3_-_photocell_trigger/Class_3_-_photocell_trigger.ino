/*
   using the pcell to trigger a sound when it goes past a threshold but without double tiggering
*/


// The block below is copied from the design tool: https://www.pjrc.com/teensy/gui/
// "#include" means add another file to our sketch
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform2;      //xy=147,139
AudioSynthKarplusStrong  string1;        //xy=155,190
AudioSynthSimpleDrum     drum1;          //xy=157,232
AudioSynthWaveform       waveform1;      //xy=163,95
AudioMixer4              mixer1;         //xy=376,168
AudioAmplifier           amp1;           //xy=549,154
AudioOutputI2S           i2s1;           //xy=757,130
AudioConnection          patchCord1(waveform2, 0, mixer1, 1);
AudioConnection          patchCord2(string1, 0, mixer1, 2);
AudioConnection          patchCord3(drum1, 0, mixer1, 3);
AudioConnection          patchCord4(waveform1, 0, mixer1, 0);
AudioConnection          patchCord5(mixer1, amp1);
AudioConnection          patchCord6(amp1, 0, i2s1, 0);
AudioConnection          patchCord7(amp1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=607,269
// GUItool: end automatically generated code



#include "bleep_base.h" //Contains functions we'll need when using the bleep base

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float freq1; //frequencies should always be stored as floats
float freq2;
int photocell_read, prev_photocell_read;
int string_threshold = 300;
int drum_threshold = 100;
int drum_flag, string_flag;
int drum_off;
#define array_size 20
int photocell_read_array[array_size];
int photocell_read_array_index;
unsigned long drum_flag_time;
unsigned long string_time;
int pot_switch1;

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


  //This next group can be done anywhere in the code but we want to start things with these
  // values and change some of them in the loop.

  //Notice we start by writing the object we want, then a period, then the function
  // begin(volume from 0.0-1.0 , frequency , shape of oscillator)
  waveform1.begin(1, 440.0, WAVEFORM_SINE);
  waveform2.begin(1, 220.01, WAVEFORM_SINE);
  //See the tool for more info https://www.pjrc.com/teensy/gui/?info=AudioSynthWaveform

  //The mixer has four inputs we can change the volume of
  // gain.(channel from 0 to 3, gain from 0.0 to a large number)
  // A gain of 1 means the output is the same as the input.
  // .5 would be half volume and 2 would be double
  // -1.0 would mean the same volume but the signal is upside down, aka 180 degrees out of phase

  //Since we have two oscillators coming in that are already "1" We should take them down by half so we don't clip.
  // If you go over "1" The top or bottom of the wave is just slammed against a wall
  mixer1.gain(0, 0); //wave1
  mixer1.gain(1, 0); //wave2
  mixer1.gain(2, .5); //string
  mixer1.gain(3, .5); //drum
  //the other channels of the mixer aren't used so don't need to be set
  //This really isn't necessary since we're changing them in the loop but it's here for reference

} //setup is over

void loop() {
  current_time = millis();

  for (int i = 0; i < 8; i = i + 1)  {
    buttons[i].update();
  }

  freq1 = (potRead(0) * 500.0) + 100.0; //add and multiply by floats to make sure the output is a float
  waveform1.frequency(freq1); //change the pitch of the oscillator

  freq2 = (potRead(1) * 500.0) + 100.0;
  waveform2.frequency(freq2);

  pot_switch1 = potRead(2) * 3.0;

  if (current_time - prev_time[2] > 5 && 1) {
    prev_time[2] = current_time;

    photocell_read_array_index++;
    if (photocell_read_array_index > array_size - 1) {
      photocell_read_array_index = 0;
    }
    photocell_read_array[photocell_read_array_index] = photocell_read;
    photocell_read = analogRead(A8);
    int index2 = photocell_read_array_index + 1;
    if (index2 > array_size - 1) {
      index2 -= array_size;
    }
    prev_photocell_read = photocell_read_array[index2];

    if (photocell_read > drum_threshold && prev_photocell_read < drum_threshold) {
      if (current_time - drum_flag_time > 200) {
        drum_flag = 1;
        drum_flag_time = current_time;
      }
    }

    if (drum_flag == 1) {
      if (pot_switch1 == 0) {
        drum1.frequency(220);
        int r1 = random(2);
        if (r1 == 0) {
          drum1.length(250);
        }
        if (r1 >= 1) {
          drum1.length(500);
        }
        drum1.pitchMod(.3);
        drum1.noteOn();
      }

      if (pot_switch1 == 1) {
        string1.noteOn(220.0, .8);
        string_time = current_time;
        string_flag = 1;
      }
      drum_flag = 0;
    }
  }

  if (current_time - string_time > 20 && string_flag == 1) {
    string1.noteOff(.8);
    string_flag = 0;
  }


  if (current_time - prev_time[1] > 20 && 1) {
    prev_time[1] = current_time;
    Serial.print(pot_switch1);
    Serial.print(" ");
    Serial.print(photocell_read);
    Serial.print(" ");
    Serial.print(prev_photocell_read);
    Serial.print(" ");
    Serial.println(drum_threshold);
  }

  if (current_time - prev_time[0] > 500 && 0) {
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
