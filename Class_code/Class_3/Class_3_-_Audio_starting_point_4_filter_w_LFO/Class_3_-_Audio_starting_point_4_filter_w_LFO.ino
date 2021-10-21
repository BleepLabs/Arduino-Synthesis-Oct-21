//Sound only outputs when left most button is pressed
// Filter cuttof frequency is controlled with pot 2
// filter resonance is controlled with pot 6
// LFO modulates cutoff freq. Pot 4 is amount, pot 5 is rate

// The block below is copied from the design tool: https://www.pjrc.com/teensy/gui/
// "#include" means add another file to our sketch
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=130.28572845458984,186.14282035827637
AudioSynthWaveform       waveform2;      //xy=132.84127807617188,258.92057037353516
AudioSynthWaveform       waveform3;      //xy=258.8238334655762,356.00562953948975
AudioMixer4              mixer1;         //xy=293.6031723022461,231.11104202270508
AudioFilterStateVariable filter1;        //xy=452.666446685791,296.0792999267578
AudioEffectEnvelope      envelope1;      //xy=604.8232765197754,211.00537300109863
AudioAmplifier           amp1;           //xy=768.6823654174805,281.38093185424805
AudioOutputI2S           i2s1;           //xy=937.7299156188965,361.57139015197754
AudioConnection          patchCord1(waveform1, 0, mixer1, 0);
AudioConnection          patchCord2(waveform2, 0, mixer1, 1);
AudioConnection          patchCord3(waveform3, 0, filter1, 1);
AudioConnection          patchCord4(mixer1, 0, filter1, 0);
AudioConnection          patchCord5(filter1, 0, envelope1, 0);
AudioConnection          patchCord6(envelope1, amp1);
AudioConnection          patchCord7(amp1, 0, i2s1, 0);
AudioConnection          patchCord8(amp1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=595.2856292724609,467.1428394317627
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
  mixer1.gain(0, .5);
  mixer1.gain(1, .5);
  //the other channels of the mixer aren't used so don't need to be set
  //This really isn't necessary since we're changing them in the loop but it's here for reference

  envelope1.attack(1000);
  envelope1.decay(1000);
  envelope1.sustain(1);
  envelope1.release(2000);
} //setup is over

void loop() {
  current_time = millis();
  for (int i = 0; i < 8; i = i + 1)  {
    buttons[i].update();
  }
  if (buttons[0].fell()) {
    //gate1 = 1;
    envelope1.noteOn();
  }
  if (buttons[0].rose()) {
    //gate1 = 0;
    envelope1.noteOff();
  }

  freq1 = (potRead(0) * 500.0) + 100.0; //add and multiply by floats to make sure the output is a float
  waveform1.frequency(freq1); //change the pitch of the oscillator

  freq2 = (potRead(1) * 500.0) + 100.0;
  waveform2.frequency(freq2);

  mixer1.gain(0, .22); //resonance adds a lot of volume to a small band of frequencies so we need to turn the input into it down 
  mixer1.gain(1, .22);

  amp1.gain(potRead(3));

  cuttoff_freq = map(potRead(2), 0, 1.0, 0, 15000.0); //crazy noise over 15000
  filter1.frequency(cuttoff_freq);

  resonance_control = map(potRead(6), 0, 1.0, .7, 5.0); //resonance is from .7 to 5.0
  filter1.resonance(resonance_control);

  //LFO that also controls cutoff freq
  waveform3.amplitude(potRead(4)); //amount
  waveform3.frequency(potRead(5) * 10.0);  //frequency

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
