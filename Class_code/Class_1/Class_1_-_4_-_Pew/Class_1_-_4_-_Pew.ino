//This Is a quick example of using the LFO from the previous code to 
// modulate audio. We'll talk about how it works in class 3

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthNoisePink      pink1;          //xy=162,278
AudioSynthWaveform       waveform2;      //xy=163,166
AudioSynthWaveform       waveform1;      //xy=171,114
AudioMixer4              mixer1;         //xy=379,144
AudioOutputI2S           i2s1;           //xy=587,143
AudioConnection          patchCord1(pink1, 0, mixer1, 2);
AudioConnection          patchCord2(waveform2, 0, mixer1, 1);
AudioConnection          patchCord3(waveform1, 0, mixer1, 0);
AudioConnection          patchCord4(mixer1, 0, i2s1, 0);
AudioConnection          patchCord5(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=480,254
// GUItool: end automatically generated code

//initlaization
// just variables
unsigned long current_time = 0;
unsigned long prev_time = 0 ;
float led_state = 1;
int rate_pot;
int bright_pot;
int raw_pot;
int latch1;
int pot3;

void setup() {

  //there's a lot we need to do in setup now but some of it is just copy paste.
  // This first group should only be done in setup

  //How much RAM to set aside for the audio library to use.
  // The audio library uses blocks of a set size so this is not a percentage or kilobytes, just a kind of arbitrary number.
  // On our Teensy 3.2 we can go up to about 200 but that won't leave any RAM for anyone else.
  // It's usually the delay and reverb that hog it.
  AudioMemory(10);

  sgtl5000_1.enable(); //Turn the adapter board on
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN); //Tell it what input we want to use. Not necessary yet but good to have
  sgtl5000_1.lineInLevel(13); //The volume of the input. Again we'll get to this later

  //Output volume. Goes from 0.0 to 1.0 but a fully loud signal will clip over .8 or so.
  // For headphones it's pretty loud at .4
  // There are lots of places we can change the final volume level. This one you set once and leave alone.
  sgtl5000_1.volume(0.25);

  //This next group can be done anywhere in the code but we want to start things with these
  // values and change some of them in the loop.

  //Notice we start by writing the object we want, then a period, then the function
  // begin(volume from 0.0-1.0 , frequency , shape of oscillator)
  // See the tool for more info https://www.pjrc.com/teensy/gui/?info=AudioSynthWaveform
  waveform1.begin(.5, 220.0, WAVEFORM_SINE);
  waveform2.begin(.5, 440.0, WAVEFORM_SINE);

  //The mixer has four inputs we can change the volume of
  // gain.(channel from 0 to 3, gain from 0.0 to a large number)
  // A gain of 1 means the output is the same as the input.
  // .5 would be half volume and 2 would be double
  // Since we have two oscillators coming in that are already "1" We should take them down by half so we don't clip.
  // If you go over "1" The top or bottom of the wave is just slammed against a wall
  mixer1.gain(0, .5);
  mixer1.gain(1, .5);
  mixer1.gain(2, 0);
  mixer1.gain(3, 0);

  analogReadAveraging(64);
}

void loop() {
  current_time = millis();

  rate_pot = analogRead(A10) / 4;
  raw_pot = analogRead(A11); //0-1023
  pot3 = analogRead(A12); //0-1023
  bright_pot = map(raw_pot, 0, 1023, 0, 255); //https://www.arduino.cc/reference/en/language/functions/math/map/

  //timing if
  if (current_time - prev_time > rate_pot) {
    prev_time = current_time;

    if (latch1 == 1) {
      //led_state = led_state + 1;
      float up = pot3 / 1023.0;
      led_state = led_state * (1.0 + up);
    }
    if (latch1 == 0) {
      //led_state = led_state - 1;
      led_state = led_state * 0.95;
    }

    if (led_state > 254) {
      latch1 = 0;
    }

    if (led_state < 1) {
      led_state = 1;
      latch1 = 1;
    }

    analogWrite(13, led_state); //0-255
    Serial.println(led_state);
  }
  
  waveform1.frequency((led_state * 4) + 100);
  mixer1.gain(0, led_state / 255.0 / 2.0);
  mixer1.gain(1, led_state / 255.0 / 2.0);

}
