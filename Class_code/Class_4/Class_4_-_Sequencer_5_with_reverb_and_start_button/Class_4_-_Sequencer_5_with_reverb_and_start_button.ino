/*
 key 0 plays the noise sound
 key 1 starts and stops the sequence
 pot 0 feedback
 pot 1 seq rate
 pot 2 filter cutoff
 pot 3 out volume
 pot 4 lfo amount
 pot 5 lfo rate
 pot 6 wet dry
 pot 7 delay time


*/

// The block below is copied from the design tool: https://www.pjrc.com/teensy/gui/
// "#include" means add another file to our sketch
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=105.00569152832031,143
AudioSynthWaveform       waveform2;      //xy=105.00569152832031,191
AudioSynthNoiseWhite     noise1;         //xy=113,244
AudioMixer4              mixer1;         //xy=312.0057067871094,182
AudioSynthWaveform       waveform3;      //xy=321.0056915283203,337
AudioSynthSimpleDrum     drum1;          //xy=353,404
AudioSynthSimpleDrum     drum2;          //xy=369,450
AudioFilterStateVariable filter1;        //xy=471.0057067871094,210
AudioMixer4              mixer2;         //xy=544,328
AudioEffectEnvelope      envelope1;      //xy=629.0056762695312,218
AudioEffectDelay         delay1;         //xy=730,496
AudioMixer4              delay_mixer;         //xy=742,386
AudioMixer4              wet_dry_mixer;         //xy=912,297
AudioEffectFreeverb      freeverb1;      //xy=915,459
AudioAmplifier           amp1;           //xy=1082,277
AudioOutputI2S           i2s1;           //xy=1117.005615234375,343
AudioConnection          patchCord1(waveform1, 0, mixer1, 0);
AudioConnection          patchCord2(waveform2, 0, mixer1, 1);
AudioConnection          patchCord3(noise1, 0, mixer1, 2);
AudioConnection          patchCord4(mixer1, 0, filter1, 0);
AudioConnection          patchCord5(waveform3, 0, filter1, 1);
AudioConnection          patchCord6(drum1, 0, mixer2, 1);
AudioConnection          patchCord7(drum2, 0, mixer2, 2);
AudioConnection          patchCord8(filter1, 0, envelope1, 0);
AudioConnection          patchCord9(mixer2, 0, wet_dry_mixer, 0);
AudioConnection          patchCord10(mixer2, 0, delay_mixer, 0);
AudioConnection          patchCord11(envelope1, 0, mixer2, 0);
AudioConnection          patchCord12(delay1, 0, delay_mixer, 1);
AudioConnection          patchCord13(delay1, 1, freeverb1, 0);
AudioConnection          patchCord14(delay_mixer, 0, wet_dry_mixer, 1);
AudioConnection          patchCord15(delay_mixer, delay1);
AudioConnection          patchCord16(wet_dry_mixer, amp1);
AudioConnection          patchCord17(freeverb1, 0, wet_dry_mixer, 2);
AudioConnection          patchCord18(amp1, 0, i2s1, 0);
AudioConnection          patchCord19(amp1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=978.0056762695312,94
// GUItool: end automatically generated code



#include "bleep_base.h" //Contains functions we'll need when using the bleep base

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float freq1; //frequencies should always be stored as floats
float freq2;
float amplitude1, amplitude2; //you can also declare variables like this. Both will be floats
float cuttoff_freq;
int gate1;
float resonance_control;
int note_sel,note_shift;
//const means it's stores in program memory, not RAM
// all the frequencies on a full sized piano.
const float chromatic[88] = {55.00000728, 58.27047791, 61.73542083, 65.40639999, 69.29566692, 73.4162017, 77.78175623, 82.40690014, 87.30706942, 92.49861792, 97.99887197, 103.8261881, 110.0000146, 116.5409558, 123.4708417, 130.8128, 138.5913338, 146.8324034, 155.5635124, 164.8138003, 174.6141388, 184.9972358, 195.9977439, 207.6523763, 220.0000291, 233.0819116, 246.9416833, 261.6255999, 277.1826676, 293.6648067, 311.1270248, 329.6276005, 349.2282776, 369.9944716, 391.9954878, 415.3047525, 440.0000581, 466.1638231, 493.8833665, 523.2511997, 554.3653352, 587.3296134, 622.2540496, 659.2552009, 698.4565551, 739.9889431, 783.9909755, 830.6095048, 880.0001162, 932.3276461, 987.7667329, 1046.502399, 1108.73067, 1174.659227, 1244.508099, 1318.510402, 1396.91311, 1479.977886, 1567.981951, 1661.219009, 1760.000232, 1864.655292, 1975.533466, 2093.004798, 2217.46134, 2349.318453, 2489.016198, 2637.020803, 2793.82622, 2959.955772, 3135.963901, 3322.438019, 3520.000464, 3729.310584, 3951.066931, 4186.009596, 4434.92268, 4698.636906, 4978.032395, 5274.041605, 5587.652439, 5919.911543, 6271.927802, 6644.876037, 7040.000927, 7458.621167, 7902.133861, 8372.019192};

int seq1[16] = {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0};
int seq2[16] = {0, 0, 220, 0, 0, 0, 440, 0, 220, 0, 0, 0, 880, 0, 0, 0};

int seq_index;
int mod_offset;
int mod_index;
int seq_mode;

void setup() {

  start_bleep_base(); //Gets the LEDs, pots, and buttons ready

  //there's a lot we need to do in setup now but most of it is just copy paste.
  // This first group should only be done in setup how much RAM to set aside for the audio library to use.
  // The audio library uses blocks of a set size so this is not a percentage or kilobytes, just a kind of arbitrary number.
  // On our Teensy 4.1 we can go up to about 1900 but that won't leave any RAM for anyone else.
  // Most objects only need a single block. It's usually the delay and reverb that hog it.
  AudioMemory(1000);

  sgtl5000_1.enable(); //Turn the adapter board on
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN); //Tell it what input we want to use. Not necessary is you're not using the ins
  sgtl5000_1.lineInLevel(10); //The volume of the input. 0-15 with 15 bing more amplifications
  //sgtl5000_1.inputSelect(AUDIO_INPUT_MIC);
  //sgtl5000_1.micGain(13); //0 - 63bd of gain.

  //headphone jack output volume. Goes from 0.0 to 1.0 but a 100% signal will clip over .8 or so.
  // For headphones it's pretty loud at .4
  // There are lots of places we can change the final volume level.
  // For now lets set this one once and leave it alone.
  sgtl5000_1.volume(0.4);

  //The line out has a separate level control but it's not meant to be adjusted like the volume function above.
  // If you're not using the line out don't worry about it.
  sgtl5000_1.lineOutLevel(21); //11-32, the smaller the louder. 21 is about 2 Volts peak to peak


  //This next group can be done anywhere in the code but we want to start things with these
  // values and change some of them in the loop.

  //Notice we start by writing the object we want, then a period, then the function
  // begin(volume from 0.0-1.0 , frequency , shape of oscillator)
  waveform1.begin(1, 440.0, WAVEFORM_SAWTOOTH);
  waveform2.begin(1, 220.01, WAVEFORM_SAWTOOTH);
  waveform3.begin(1, .25, WAVEFORM_SINE);
  //See the tool for more info https://www.pjrc.com/teensy/gui/?info=AudioSynthWaveform

  filter1.octaveControl(7);

  //The mixer has four inputs we can change the volume of
  // gain.(channel from 0 to 3, gain from 0.0 to a large number)
  // A gain of 1 means the output is the same as the input.
  // .5 would be half volume and 2 would be double
  // -1.0 would mean the same volume but the signal is upside down, aka 180 degrees out of phase

  //Since we have two oscillators coming in that are already "1" We should take them down by half so we don't clip.
  // If you go over "1" The top or bottom of the wave is just slammed against a wall
  mixer1.gain(0, 0); //wave1
  mixer1.gain(1, 0); //wave2
  mixer1.gain(2, .3); //pink1

  mixer2.gain(0, .3); //noise voice
  mixer2.gain(1, .3); //drum1
  mixer2.gain(2, .3); //drum2

  //the other channels of the mixer aren't used so don't need to be set
  //This really isn't necessary since we're changing them in the loop but it's here for reference

  //envelope info https://www.pjrc.com/teensy/gui/?info=AudioEffectEnvelope
  envelope1.attack(1); //time in milliseconds
  envelope1.decay(10); //time in milliseconds
  envelope1.sustain(0); //amplitude 0-1.0
  envelope1.release(1000);//time in milliseconds

  noise1.amplitude(1);

  drum1.frequency(440);
  drum1.length(250);
  drum1.pitchMod(.2);

  delay1.delay(0, 500);

  delay_mixer.gain(0, .7); //from voice and drums
  delay_mixer.gain(1, .3); //feedback

  freeverb1.roomsize(.5); //0-1.0
  freeverb1.damping(.5); //0.1.0 with 1 severely reducing high frequencies

} //setup is over

void loop() {
  current_time = millis();

  delay_mixer.gain(1, potRead(0));
  float dt = potRead(7) * 500.0;
  delay1.delay(0, dt);
  delay1.delay(1, dt * .8);


  float dry = map(potRead(6), 0, 1.0, .6, 0);
  float wet = map(potRead(6), 0, 1.0, 0, .6);
  wet_dry_mixer.gain(0, dry);
  wet_dry_mixer.gain(1, wet);
  wet_dry_mixer.gain(2, .4);


  int seq1_rate =  potRead(1) * 500.0;


//not used
  freq1 = chromatic[note_sel+40]; //set the frequency using the button's "i" + 40 so it's in the middle of the piano range
  freq2 = freq1 * 2.0;
  waveform1.frequency(freq1);
  waveform2.frequency(freq2);
//

  for (int i = 0; i < 8; i = i + 1)  {
    buttons[i].update();
  }

  if (buttons[0].fell()) {
    envelope1.noteOn(); //start the attack section of the envelope
  }
  if (buttons[0].rose()) {
    envelope1.noteOff(); //end the sustain section and start the release
  }

  if (buttons[1].fell()) {
    seq_mode++;
    if (seq_mode > 1) { //only 2 modes right now, 0 and 1 but easily expandable
      seq_mode = 0;
    }
    if (seq_mode == 1) {
      seq_index = 0; //start at the first step of the sequence
      prev_time[1] = 0; //make is so the seq timing iff happens this loop
    }

  }


//this is being used
  amp1.gain(potRead(3));

  cuttoff_freq = map(potRead(2), 0, 1.0, 0, 15000.0);
  filter1.frequency(cuttoff_freq);

  //resonance_control = map(potRead(6), 0, 1.0, .7, 5.0);
  filter1.resonance(3.0);

  waveform3.amplitude(potRead(4));
  waveform3.frequency(potRead(5) * 10.0);


  /////////////////////////////////////////////////////////////////sequencer
  if (current_time - prev_time[1] > seq1_rate) {
    prev_time[1] = current_time;
    if (seq_mode == 1) {
      if (seq1[seq_index] == 1) {
        envelope1.noteOn();
      }

      if (seq2[seq_index] > 0) {
        drum1.frequency(seq2[seq_index]);
        drum1.noteOn();
      }
      else {
        drum1.frequency(seq2[seq_index]);
        drum1.noteOn();
      }

      seq_index = seq_index + 1;  //we want to increment AFTER we play the sounds so we play the first step before advancing

      if (seq_index > 15) {
        seq_index = 0;
      }

    }

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
