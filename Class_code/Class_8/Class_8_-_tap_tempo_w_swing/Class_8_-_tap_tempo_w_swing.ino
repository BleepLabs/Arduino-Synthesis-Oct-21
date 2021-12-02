/*
  Button 30 is the tap input. You'll need to tap it at least 4 times to get it going
  Pot 0 is the amounf od swing from fully clickwise straigh to 50% swing on the right
  
*/
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=457.00004959106445,154.00000190734863
AudioSynthWaveform       waveform1;      //xy=475.0000457763672,237.0000286102295
AudioAnalyzePeak         peak1;          //xy=573.000129699707,84.00000095367432
AudioMixer4              mixer1;         //xy=644.0000610351562,195
AudioEffectEnvelope      envelope1;      //xy=763,291
AudioAmplifier           amp1;           //xy=849.4000244140625,202
AudioOutputI2S           i2s2;           //xy=1000.8109760284424,199.7143096923828
AudioConnection          patchCord1(i2s1, 0, mixer1, 0);
AudioConnection          patchCord2(i2s1, 0, peak1, 0);
AudioConnection          patchCord3(waveform1, 0, mixer1, 1);
AudioConnection          patchCord4(mixer1, envelope1);
AudioConnection          patchCord5(envelope1, amp1);
AudioConnection          patchCord6(amp1, 0, i2s2, 0);
AudioConnection          patchCord7(amp1, 0, i2s2, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=805.0966644287109,338.99997997283936
// GUItool: end automatically generated code


#include "bleep_base.h" //Contains functions we'll need when using the bleep base

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float freq1; //frequencies should always be stored as floats
float freq2;
float peak1_reading;

#define tap_array_length 4
unsigned long tap_array[tap_array_length];
unsigned long prev_tap_time;
int tap_array_index;
int tap_ave = 420;
int led_tick;
float bpm, hz;
int loop_tick;
unsigned long blink_time;
unsigned long swing_time;
int swing_flag;
int quarter_count;

void setup() {
  pinMode(3, OUTPUT);

  start_bleep_base(); //Gets the LEDs, pots, and buttons ready

  //there's a lot we need to do in setup now but most of it is just copy paste.
  // This first group should only be done in setup how much RAM to set aside for the audio library to use.
  // The audio library uses blocks of a set size so this is not a percentage or kilobytes, just a kind of arbitrary number.
  // On our Teensy 4.1 we can go up to about 1900 but that won't leave any RAM for anyone else.
  // Most objects only need a single block. It's usually the delay and reverb that hog it.
  AudioMemory(10);

  sgtl5000_1.enable(); //Turn the adapter board on
  //sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN); //Tell it what input we want to use. Not necessary is you're not using the ins
  //sgtl5000_1.lineInLevel(10); //The volume of the input. 0-15 with 15 bing more amplifications
  sgtl5000_1.inputSelect(AUDIO_INPUT_MIC);
  sgtl5000_1.micGain(63); //0 - 63db of gain.

  //headphone jack output volume. Goes from 0.0 to 1.0 but a 100% signal will clip over .8 or so.
  // For headphones it's pretty loud at .4
  // There are lots of places we can change the final volume level.
  // For now lets set this one once and leave it alone.
  sgtl5000_1.volume(0.25);

  //The line out has a separate level control but it's not meant to be adjusted like the volume function above.
  // If you're not using the line out don't worry about it.
  sgtl5000_1.lineOutLevel(21); //11-32, the smaller the louder. 21 is about 2 Volts peak to peak


  waveform1.begin(1, 440.0, WAVEFORM_SINE);

  mixer1.gain(0, 0);  //audio in
  mixer1.gain(1, 1); //waveform1
  amp1.gain(1); //out volume

  envelope1.attack(1);
  envelope1.decay(100);
  envelope1.sustain(0); //we'll just use attack and decay so we jsut need to worry about noteOn
  envelope1.attack(1);

} //setup is over

void loop() {
  current_time = millis();

  for (int i = 0; i < 8; i = i + 1)  {
    buttons[i].update();
  }

  if (buttons[0].fell()) {
    tap_array_index++;
    if (tap_array_index > tap_array_length - 1) {
      tap_array_index = 0;
    }
    //save the differnece between the current time and the last time the button was pressed
    tap_array[tap_array_index] = current_time - prev_tap_time;
    prev_tap_time = current_time; //save the current time
  }

  if (current_time - prev_time[1] > 20 && 1) {  //quickly average and print
    prev_time[1] = current_time;
    tap_ave = 0; //set this to 0 so we don't average the current reading with the last one
    for (int j = 0; j < tap_array_length; j++) {
      tap_ave += tap_array[j];
      Serial.print(tap_array[j]);
      Serial.print(" ");
    }
    tap_ave = tap_ave / tap_array_length;
    hz = (1.0 / tap_ave) * 1000.0; //convert milliseconds the Hz
    //convert Hz (cycles per second) to BPM
    // since we're tapping in quarter notes we don't multiply by 4. If we were taping once per measure we would
    bpm = (hz * 60.0);
    
    Serial.println();
    Serial.println(tap_ave);
    Serial.println(hz, 6);  //,6 give 6 decimal places
    Serial.println(bpm);
    Serial.println();
  }

  if (current_time - prev_time[2] > tap_ave) {
    prev_time[2] = current_time;
    quarter_count++;
    if (quarter_count > 3) {
      quarter_count = 0;
    }
    if (quarter_count == 0 || quarter_count == 2) {  //if its 0 or 1 play the note and turn the light on
      set_LED(0, 0, 0, .25);
      envelope1.noteOn();
      blink_time = current_time; //remeber the time so we can turn the light off
    }
    if (quarter_count == 1 || quarter_count == 3) { //if its odd, remeber the time and turn the flag on
      swing_time = current_time;
      swing_flag = 1;
    }
    waveform1.frequency((quarter_count + 1) * 110.0);
  }
  //if the flag is on and it's been X amount of time then play the note and turn the light on
  if (swing_flag == 1 && current_time - swing_time > tap_ave * potRead(0)*.5) { 
    set_LED(0, 0, 0, .25);
    envelope1.noteOn();
    swing_flag = 0;
    blink_time = current_time;
  }

  if (current_time - blink_time > 60) { //turn the light off 60 millis after it's been turned on
    set_LED(0, 0, 0, 0);
  }


  if (current_time - prev_time[3] > 30) {
    prev_time[3] = current_time;
    LEDs.show();
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
