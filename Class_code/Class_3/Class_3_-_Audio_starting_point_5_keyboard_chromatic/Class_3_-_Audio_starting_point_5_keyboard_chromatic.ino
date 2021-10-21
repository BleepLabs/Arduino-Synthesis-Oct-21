//Keys play different notes using an envelope

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

//const means it's stores in program memory, not RAM
// all the frequencies on a full sized piano.
const float chromatic[88] = {55.00000728, 58.27047791, 61.73542083, 65.40639999, 69.29566692, 73.4162017, 77.78175623, 82.40690014, 87.30706942, 92.49861792, 97.99887197, 103.8261881, 110.0000146, 116.5409558, 123.4708417, 130.8128, 138.5913338, 146.8324034, 155.5635124, 164.8138003, 174.6141388, 184.9972358, 195.9977439, 207.6523763, 220.0000291, 233.0819116, 246.9416833, 261.6255999, 277.1826676, 293.6648067, 311.1270248, 329.6276005, 349.2282776, 369.9944716, 391.9954878, 415.3047525, 440.0000581, 466.1638231, 493.8833665, 523.2511997, 554.3653352, 587.3296134, 622.2540496, 659.2552009, 698.4565551, 739.9889431, 783.9909755, 830.6095048, 880.0001162, 932.3276461, 987.7667329, 1046.502399, 1108.73067, 1174.659227, 1244.508099, 1318.510402, 1396.91311, 1479.977886, 1567.981951, 1661.219009, 1760.000232, 1864.655292, 1975.533466, 2093.004798, 2217.46134, 2349.318453, 2489.016198, 2637.020803, 2793.82622, 2959.955772, 3135.963901, 3322.438019, 3520.000464, 3729.310584, 3951.066931, 4186.009596, 4434.92268, 4698.636906, 4978.032395, 5274.041605, 5587.652439, 5919.911543, 6271.927802, 6644.876037, 7040.000927, 7458.621167, 7902.133861, 8372.019192};

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

  //envelope info https://www.pjrc.com/teensy/gui/?info=AudioEffectEnvelope
  envelope1.attack(10); //time in milliseconds
  envelope1.decay(1000); //time in milliseconds
  envelope1.sustain(1); //amplitude 0-1.0
  envelope1.release(1000);//time in milliseconds
} //setup is over

void loop() {
  current_time = millis();
  for (int i = 0; i < 8; i = i + 1)  {
    buttons[i].update();

    if (buttons[i].fell()) { //if ANY button fell..
      
      freq1 = chromatic[i + 30]; //set the frequency using the button's "i"
      //freq2 = freq1 * .501;
      freq2 = chromatic[i + 35];
      waveform1.frequency(freq1);
      waveform2.frequency(freq2);

      envelope1.noteOn(); //start the attack section of the envelope
    }
    if (buttons[i].rose()) {
      envelope1.noteOff(); //end the sustain section and start the release 
    }
  }

  // freq1 = (potRead(0) * 500.0) + 100.0; //add and multiply by floats to make sure the output is a float
  // freq2 = (potRead(1) * 500.0) + 100.0;


  mixer1.gain(0, .22); //resonance adds a lot of volume to a small band of frequencies so we need to turn the input into it down 
  mixer1.gain(1, .22);

  amp1.gain(potRead(3));

  cuttoff_freq = map(potRead(2), 0, 1.0, 0, 15000.0);
  filter1.frequency(cuttoff_freq);

  resonance_control = map(potRead(6), 0, 1.0, .7, 5.0);
  filter1.resonance(resonance_control);

  waveform3.amplitude(potRead(4));
  waveform3.frequency(potRead(5) * 1000.0);

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
