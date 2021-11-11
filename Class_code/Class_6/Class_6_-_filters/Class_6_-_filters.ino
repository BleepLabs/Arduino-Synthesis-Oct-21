//Playing notes in different scales

// The block below is copied from the design tool: https://www.pjrc.com/teensy/gui/
// "#include" means add another file to our sketch
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthKarplusStrong  string1;        //xy=135,185
AudioSynthWaveform       waveform2;      //xy=147,139
AudioSynthWaveform       waveform1;      //xy=163,95
AudioMixer4              mixer1;         //xy=363,122
AudioFilterStateVariable filter1;        //xy=435,231
AudioFilterLadder        ladder1;        //xy=472,377
AudioFilterStateVariable filter2;        //xy=584,221
AudioMixer4              mixer2;         //xy=661,127
AudioAmplifier           amp1;           //xy=810,126
AudioOutputI2S           i2s1;           //xy=993,121
AudioConnection          patchCord1(string1, 0, mixer1, 2);
AudioConnection          patchCord2(waveform2, 0, mixer1, 1);
AudioConnection          patchCord3(waveform1, 0, mixer1, 0);
AudioConnection          patchCord4(mixer1, 0, filter1, 0);
AudioConnection          patchCord5(mixer1, 0, mixer2, 0);
AudioConnection          patchCord6(mixer1, 0, ladder1, 0);
AudioConnection          patchCord7(filter1, 0, filter2, 0);
AudioConnection          patchCord8(ladder1, 0, mixer2, 2);
AudioConnection          patchCord9(filter2, 2, mixer2, 1);
AudioConnection          patchCord10(mixer2, amp1);
AudioConnection          patchCord11(mixer2, 0, mixer1, 3);
AudioConnection          patchCord12(amp1, 0, i2s1, 0);
AudioConnection          patchCord13(amp1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=805,315
// GUItool: end automatically generated code




#include "bleep_base.h" //Contains functions we'll need when using the bleep base

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float freq1; //frequencies should always be stored as floats
float freq2;

//starts at midi note 12, C0 https://newt.phys.unsw.edu.au/jw/notes.html
// cosnt static means it will be put in program memory where there's plenty of space. This saves 121 * 4 bytes of ram
const static float chromatic[121] = {16.3516, 17.32391673, 18.35405043, 19.44543906, 20.60172504, 21.82676736, 23.12465449, 24.499718, 25.95654704, 27.50000365, 29.13523896, 30.86771042, 32.7032, 34.64783346, 36.70810085, 38.89087812, 41.20345007, 43.65353471, 46.24930897, 48.99943599, 51.91309407, 55.00000728, 58.27047791, 61.73542083, 65.40639999, 69.29566692, 73.4162017, 77.78175623, 82.40690014, 87.30706942, 92.49861792, 97.99887197, 103.8261881, 110.0000146, 116.5409558, 123.4708417, 130.8128, 138.5913338, 146.8324034, 155.5635124, 164.8138003, 174.6141388, 184.9972358, 195.9977439, 207.6523763, 220.0000291, 233.0819116, 246.9416833, 261.6255999, 277.1826676, 293.6648067, 311.1270248, 329.6276005, 349.2282776, 369.9944716, 391.9954878, 415.3047525, 440.0000581, 466.1638231, 493.8833665, 523.2511997, 554.3653352, 587.3296134, 622.2540496, 659.2552009, 698.4565551, 739.9889431, 783.9909755, 830.6095048, 880.0001162, 932.3276461, 987.7667329, 1046.502399, 1108.73067, 1174.659227, 1244.508099, 1318.510402, 1396.91311, 1479.977886, 1567.981951, 1661.219009, 1760.000232, 1864.655292, 1975.533466, 2093.004798, 2217.46134, 2349.318453, 2489.016198, 2637.020803, 2793.82622, 2959.955772, 3135.963901, 3322.438019, 3520.000464, 3729.310584, 3951.066931, 4186.009596, 4434.92268, 4698.636906, 4978.032395, 5274.041605, 5587.652439, 5919.911543, 6271.927802, 6644.876037, 7040.000927, 7458.621167, 7902.133861, 8372.019192, 8869.845359, 9397.273811, 9956.06479, 10548.08321, 11175.30488, 11839.82309, 12543.8556, 13289.75207, 14080.00185, 14917.24233, 15804.26772, 16744.03838};

//notes contained in these scales. Stupid way I made them: https://docs.google.com/spreadsheets/d/1TM-jySsLeFPi6fl0kGjl9rvmG9vqZZXBw7Mcckain8I/edit?usp=sharing
const static int major[71] = {0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23, 24, 26, 28, 29, 31, 33, 35, 36, 38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79, 81, 83, 84, 86, 88, 89, 91, 93, 95, 96, 98, 100, 101, 103, 105, 107, 108, 110, 112, 113, 115, 117, 119, 120};
const static int minor[71] = {0, 2, 3, 5, 7, 8, 10, 12, 14, 15, 17, 19, 20, 22, 24, 26, 27, 29, 31, 32, 34, 36, 38, 39, 41, 43, 44, 46, 48, 50, 51, 53, 55, 56, 58, 60, 62, 63, 65, 67, 68, 70, 72, 74, 75, 77, 79, 80, 82, 84, 86, 87, 89, 91, 92, 94, 96, 98, 99, 101, 103, 104, 106, 108, 110, 111, 113, 115, 116, 118, 120};
const static int diminished[73] = {0, 2, 3, 5, 6, 8, 9, 11, 12, 14, 15, 17, 18, 20, 21, 23, 24, 26, 27, 29, 30, 32, 33, 35, 36, 38, 39, 41, 42, 44, 45, 47, 48, 50, 51, 53, 54, 56, 57, 59, 60, 62, 63, 65, 66, 68, 69, 71, 72, 74, 75, 77, 78, 80, 81, 83, 84, 86, 87, 89, 90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 108};
const static int augmented[55] = {0, 3, 4, 7, 10, 11, 14, 15, 18, 21, 22, 25, 26, 29, 32, 33, 36, 37, 40, 43, 44, 47, 48, 51, 54, 55, 58, 59, 62, 65, 66, 69, 70, 73, 76, 77, 80, 81, 84, 87, 88, 91, 92, 95, 98, 99, 102, 103, 106, 109, 110, 113, 114, 117, 120};
const static int pentatonic[51] = {0, 3, 5, 7, 10, 12, 15, 17, 19, 22, 24, 27, 29, 31, 34, 36, 39, 41, 43, 46, 48, 51, 53, 55, 58, 60, 63, 65, 67, 70, 72, 75, 77, 79, 82, 84, 87, 89, 91, 94, 96, 99, 101, 103, 106, 108, 111, 113, 115, 118, 120};
const static int locrian[71] = {0, 1, 3, 5, 6, 8, 10, 12, 13, 15, 17, 18, 20, 22, 24, 25, 27, 29, 30, 32, 34, 36, 37, 39, 41, 42, 44, 46, 48, 49, 51, 53, 54, 56, 58, 60, 61, 63, 65, 66, 68, 70, 72, 73, 75, 77, 78, 80, 82, 84, 85, 87, 89, 90, 92, 94, 96, 97, 99, 101, 102, 104, 106, 108, 109, 111, 113, 114, 116, 118, 120};

int arp_order_index;
int arp_order[4][4] = {
  {2, -1, 2, -1},
  {2, 3, -4, 1},
  {1, 3, 4, -5},
  {2, 3, 1, 1},
};

int scale_index;
int note1;
int scale_pot;
unsigned long env_time;
int env_flag;
int rate1, gate_length;
int button_sel;
int gate_enable;
float detune;
int arp_sel;
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
  waveform1.begin(1, 440.0, WAVEFORM_BANDLIMIT_SAWTOOTH);
  waveform2.begin(1, 220.01, WAVEFORM_BANDLIMIT_SAWTOOTH);
  //See the tool for more info https://www.pjrc.com/teensy/gui/?info=AudioSynthWaveform

  //The mixer has four inputs we can change the volume of
  // gain.(channel from 0 to 3, gain from 0.0 to a large number)
  // A gain of 1 means the output is the same as the input.
  // .5 would be half volume and 2 would be double
  // -1.0 would mean the same volume but the signal is upside down, aka 180 degrees out of phase

  //Since we have two oscillators coming in that are already "1" We should take them down by half so we don't clip.
  // If you go over "1" The top or bottom of the wave is just slammed against a wall
  mixer1.gain(0, .5);
  mixer1.gain(1, .5);
  //the other channels of the mixer aren't used so don't need to be set
  //This really isn't necessary since we're changing them in the loop but it's here for reference

  envelope1.attack(5);
  envelope1.decay(25);
  envelope1.sustain(.4);
  envelope1.release(200);

} //setup is over

void loop() {
  current_time = millis();

  scale_pot = potRead(0) * 7;
  arp_sel = potRead(5) * 4;
  arp_sel = constrain(arp_sel, 0, 3); //https://www.arduino.cc/reference/en/language/functions/math/constrain/

  rate1 = potRead(1) * 1000.0;
  gate_length = rate1 * .5;

  detune = (potRead(4) * .1) + 1.0;

  for (int i = 0; i < 8; i = i + 1)  {
    buttons[i].update();

    if (buttons[i].fell()) {
      gate_enable = 1;
      button_sel = i;
      scale_index = 0;
      prev_time[1] = 0;
    }

    if (buttons[i].rose()) {
      gate_enable = 0;
      envelope1.noteOff();
    }
  }

  if (current_time - prev_time[1] > rate1) {
    prev_time[1] = current_time;

    if (gate_enable == 1) {

      if (scale_pot == 0) {
        note1 = major[scale_index];
      }
      if (scale_pot == 1) {
        note1 = minor[scale_index];
      }
      if (scale_pot > 1) {
        note1 = pentatonic[scale_index];
      }

      freq1 = chromatic[note1 + 48 + button_sel]; //note 48 is c3
      float freq2 = chromatic[note1 + 48 + 3  + button_sel]; //note 48 is c3
      env_flag = 1;
      env_time = current_time;
      waveform1.frequency(freq1);
      waveform2.frequency(freq1 * detune);
      envelope1.noteOn();
      arp_order_index++;
      if (arp_order_index > 3) {
        arp_order_index = 0;
      }

      scale_index = scale_index + arp_order[arp_sel][arp_order_index];
      if (scale_index > 13) {
        //gate_enable = 0;
        scale_index = 0;
      }
      if (scale_index < 0) {
        scale_index = 0;
      }
      Serial.println(scale_index);
    }
  }
  
  if (current_time - env_time > gate_length && env_flag == 1) {
    env_flag = 0;
    envelope1.noteOff();
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
