//Audio input with AM modulation
// envelope follower using peak

// The block below is copied from the design tool: https://www.pjrc.com/teensy/gui/
// "#include" means add another file to our sketch
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s2;           //xy=152.33332061767578,236.5
AudioSynthWaveform       waveform1;      //xy=173.66666412353516,381.6666564941406
AudioAnalyzePeak         peak1;          //xy=335,111
AudioEffectMultiply      multiply1;      //xy=367.5,277.5
AudioMixer4              mixer1;         //xy=552.6666717529297,206.8333282470703
AudioOutputI2S           i2s1;           //xy=838,206
AudioConnection          patchCord1(i2s2, 0, multiply1, 0);
AudioConnection          patchCord2(i2s2, 0, mixer1, 1);
AudioConnection          patchCord3(i2s2, 0, peak1, 0);
AudioConnection          patchCord4(waveform1, 0, multiply1, 1);
AudioConnection          patchCord5(waveform1, 0, mixer1, 2);
AudioConnection          patchCord6(multiply1, 0, mixer1, 0);
AudioConnection          patchCord7(mixer1, 0, i2s1, 0);
AudioConnection          patchCord8(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=781,379
// GUItool: end automatically generated code




#include "bleep_base.h" //Contains functions we'll need when using the bleep base

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float freq1; //frequencies should always be stored as floats
float freq2;
float amp1, amp2; //you can also declare variables like this. Both will be floats
float peak1_reading;
float follower1;

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

  //See the tool for more info https://www.pjrc.com/teensy/gui/?info=AudioSynthWaveform

  //The mixer has four inputs we can change the volume of
  // gain.(channel from 0 to 3, gain from 0.0 to a large number)
  // A gain of 1 means the output is the same as the input.
  // .5 would be half volume and 2 would be double
  // -1.0 would mean the same volume but the signal is upside down, aka 180 degrees out of phase

  //Since we have two oscillators coming in that are already "1" We should take them down by half so we don't clip.
  // If you go over "1" The top or bottom of the wave is just slammed against a wall
  mixer1.gain(0, 0); //et from multiply
  mixer1.gain(1, 0); //dry input
  mixer1.gain(2, 1);
  //the other channels of the mixer aren't used so don't need to be set
  //This really isn't necessary since we're changing them in the loop but it's here for reference

} //setup is over

void loop() {
  current_time = millis();

  for (int i = 0; i < 8; i = i + 1)  {
    buttons[i].update();
  } 
  //Pot 0 sets the base frequency
  // Then the follower is added to it but can be attenuated with pot 1
  freq1 = (potRead(0) * 1000.0) + (follower1 * 1000.0 * potRead(1)); //add and multiply by floats to make sure the output is a float
  waveform1.frequency(freq1); //change the pitch of the oscillator

  amp1 = potRead(4); //returns 0-1.0 already

  mixer1.gain(0, amp1); //wet
  mixer1.gain(1, 1.0-amp1); //dry
  mixer1.gain(2, 0); //output of waveform1
  
  if (peak1.available()) {
    peak1_reading = peak1.read();
  }
  //Rather than using the peak reading directly, we can follow it 
  // When the incoming audio goes above the follower, it jumps up immediately
  // When the incoming audio drops again, the follower takes time to fade out
  if (follower1 < peak1_reading) {
    follower1 = peak1_reading;
  }
  if (follower1 > peak1_reading) {
    //follower1-=.0001;  //linear
    follower1*=.999;  //exponential 
  }

  if (current_time - prev_time[1] > 33) {
    prev_time[1] = current_time;
    Serial.print(peak1_reading*100.0); //*100.0 so it's easier to see in the monitor
    Serial.print(" ");
    Serial.println(follower1*100.0);
    set_LED(0, .3, 1, peak1_reading);
    LEDs.show();
  }

  if (0) { //1 it will happen 0 nope
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
  }

}// loop is over
