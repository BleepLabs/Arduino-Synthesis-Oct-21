/*
  Using John-Mikes audio libraries
  you no longer have to have a tab with bleep_base.h in it, now it's just a regualr library

  download libraries
  bleep_base_2v1 https://downgit.github.io/#/home?url=https://github.com/BleepLabs/Arduino-Synthesis-Oct-21/tree/main/libraries/bleep_base_2v1
  bleep_audio https://downgit.github.io/#/home?url=https://github.com/BleepLabs/Arduino-Synthesis-Oct-21/tree/main/libraries/bleep_audio

  since my objects aren't in the tool you'll need to replace the names
  So you could use the delay object but change what's imported from:
  AudioEffectDelay delay1;
  to
  AudioEffectTapeDelay delay1;
  the origial tool output is after the loop for reference

  find all the info on the audio library here https://github.com/BleepLabs/Arduino-Synthesis-Oct-21/wiki/Bleep-Audio
*/

#include <MIDI.h>
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); //You can have multiple DIN MIDI systems setup in  the same device using different pins but this is the one on the Bleep Base and most other systems


#include <bleep_audio.h>
//must be before the block from the tool

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthNoiseWhite     noise1;         //xy=87,317
AudioSynthWaveform       waveform1;      //xy=88,370
AudioInputI2S            i2s1;           //xy=222,39
AudioMixer4              envelope_mixer;         //xy=310,368
MemSampler            sampler1;      //xy=456,110
AudioEffectEnvelopeAdjustable      envelope1;      //xy=461,449
AudioMixer4              mixer1;         //xy=547,290
AudioMixer4              delay_mixer;         //xy=721,411
AudioEffectTapeDelay         delay1;         //xy=721,523
AudioMixer4              wet_dry_mixer;         //xy=964,369
AudioAmplifier           output_amp1;           //xy=983,214
AudioOutputI2S           i2s2;           //xy=1110,122

AudioConnection          patchCord1(noise1, 0, envelope_mixer, 0);
AudioConnection          patchCord2(waveform1, 0, envelope_mixer, 1);
AudioConnection          patchCord3(i2s1, 0, sampler1, 0);
AudioConnection          patchCord4(i2s1, 1, wet_dry_mixer, 2);
AudioConnection          patchCord5(envelope_mixer, envelope1);
AudioConnection          patchCord6(sampler1, 0, mixer1, 0);
AudioConnection          patchCord7(envelope1, 0, mixer1, 1);
AudioConnection          patchCord8(mixer1, 0, delay_mixer, 0);
AudioConnection          patchCord9(mixer1, 0, wet_dry_mixer, 0);
AudioConnection          patchCord10(delay_mixer, delay1);
AudioConnection          patchCord11(delay_mixer, 0, wet_dry_mixer, 1);
AudioConnection          patchCord12(delay1, 0, delay_mixer, 1);
AudioConnection          patchCord13(wet_dry_mixer, output_amp1);
AudioConnection          patchCord14(output_amp1, 0, i2s2, 0);
AudioConnection          patchCord15(output_amp1, 0, i2s2, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=911,521
// GUItool: end automatically generated code

//starts at midi note 12, C0 https://newt.phys.unsw.edu.au/jw/notes.html
PROGMEM const static float chromatic[121] = {16.3516, 17.32391673, 18.35405043, 19.44543906, 20.60172504, 21.82676736, 23.12465449, 24.499718, 25.95654704, 27.50000365, 29.13523896, 30.86771042, 32.7032, 34.64783346, 36.70810085, 38.89087812, 41.20345007, 43.65353471, 46.24930897, 48.99943599, 51.91309407, 55.00000728, 58.27047791, 61.73542083, 65.40639999, 69.29566692, 73.4162017, 77.78175623, 82.40690014, 87.30706942, 92.49861792, 97.99887197, 103.8261881, 110.0000146, 116.5409558, 123.4708417, 130.8128, 138.5913338, 146.8324034, 155.5635124, 164.8138003, 174.6141388, 184.9972358, 195.9977439, 207.6523763, 220.0000291, 233.0819116, 246.9416833, 261.6255999, 277.1826676, 293.6648067, 311.1270248, 329.6276005, 349.2282776, 369.9944716, 391.9954878, 415.3047525, 440.0000581, 466.1638231, 493.8833665, 523.2511997, 554.3653352, 587.3296134, 622.2540496, 659.2552009, 698.4565551, 739.9889431, 783.9909755, 830.6095048, 880.0001162, 932.3276461, 987.7667329, 1046.502399, 1108.73067, 1174.659227, 1244.508099, 1318.510402, 1396.91311, 1479.977886, 1567.981951, 1661.219009, 1760.000232, 1864.655292, 1975.533466, 2093.004798, 2217.46134, 2349.318453, 2489.016198, 2637.020803, 2793.82622, 2959.955772, 3135.963901, 3322.438019, 3520.000464, 3729.310584, 3951.066931, 4186.009596, 4434.92268, 4698.636906, 4978.032395, 5274.041605, 5587.652439, 5919.911543, 6271.927802, 6644.876037, 7040.000927, 7458.621167, 7902.133861, 8372.019192, 8869.845359, 9397.273811, 9956.06479, 10548.08321, 11175.30488, 11839.82309, 12543.8556, 13289.75207, 14080.00185, 14917.24233, 15804.26772, 16744.03838};


#include "bleep_base_2v1.h" //Contains functions we'll need when using the bleep base

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float freq1; //frequencies should always be stored as floats
float freq2;
float amp1, amp2; //you can also declare variables like this. Both will be floats
int loop_state = 0;
int rev_state = 0;
byte dm_type, dm_note, dm_velocity, dm_channel_recived, dm_data1, dm_data2, dm_cc_num, dm_cc_val;

#define sample_bank_length 96000 //1 second
unsigned int sampler_bank1[sample_bank_length];

#define delay_bank_length 1000 //1 second
short delay_bank[delay_bank_length];
float midi_delay_len;

void setup() {

  start_bleep_base(); //Gets the LEDs, pots, and buttons ready

  MIDI.begin(); //turn on DIN MIDI
  MIDI.turnThruOff(); //if you want the incoming MIDI notes to be copied and sent out remove this line


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

  envelope1.attack(1);
  envelope1.decay(100);
  envelope1.sustain(.5);
  envelope1.release(100);
  envelope1.shape(-.9); //-1.0 is exp, 1 is log 0 is linear

  noise1.amplitude(1);
  waveform1.begin(1, 110, WAVEFORM_BANDLIMIT_SQUARE);
  envelope_mixer.gain(0, .5);
  envelope_mixer.gain(1, .5);

  mixer1.gain(0, .5);
  mixer1.gain(1, .5);

  sampler1.begin(sampler_bank1, sample_bank_length); //array, array length
  sampler1.frequency(1.0);

  delay1.begin(delay_bank, delay_bank_length, 0, 0, 0); //(delay bank, length, delay length, redux, lerp)

  delay_mixer.gain(0, .75); //input
  delay_mixer.gain(1, 0); //feedback

  wet_dry_mixer.gain(0, .5);
  wet_dry_mixer.gain(1, .5);
  wet_dry_mixer.gain(2, 0);
  wet_dry_mixer.gain(3, 0);

  output_amp1.gain(1);

} //setup is over

void loop() {
  current_time = millis();

  for (int i = 0; i < 8; i = i + 1)  {
    buttons[i].update();
  }

  if (buttons[0].fell()) {
    envelope1.trigger();
  }

  if (buttons[1].fell()) {
    sampler1.play();
  }
  if (buttons[2].fell()) {
    sampler1.record();
  }
  if (buttons[3].fell()) {
    sampler1.stop();
  }

  if (buttons[4].fell()) {
    loop_state = !loop_state;
  }
  sampler1.loop(loop_state);

  if (buttons[5].fell()) {
    rev_state = !rev_state;
  }
  sampler1.reverse(rev_state);

  float sf = map(potRead(4), 0, 1.0, .1, 4.0);
  sampler1.frequency(sf);

  float len1 = potRead(0) * delay_bank_length;
  delay1.length(len1);
  delay_mixer.gain(1, potRead(5)); //feedback


  envelope1.attack(1);
  envelope1.decay(potRead(1) * 1000.0);
  //float shape_adj = (potRead(2) * 2.0) - 1.0;
  float shape_adj = map(potRead(2), 0, 1.0, -1.0, 1.0);
  envelope1.shape(shape_adj);

  if (current_time - prev_time[1] > 10 && 1) { //slow it down a little to avoid pops and clicks
    prev_time[1] = current_time;
    sampler1.start_location(potRead(6));  //0-1.0 input. 0 is beginnign of sample, 1 is end
    sampler1.play_length(potRead(7));  //0 is shortest possible length, 1 is entire sample
  }

  if (current_time - prev_time[0] > 500 && 1) {
    prev_time[0] = current_time;
    Serial.print("processor: ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print("%    Memory: ");
    Serial.print(AudioMemoryUsageMax());
    Serial.println();
    AudioProcessorUsageMaxReset(); //We need to reset these values so we get a real idea of what the audio code is doing rather than just peaking in every half a second
    AudioMemoryUsageMaxReset();
  }

}// loop is over



//from the tool before I changed th names
/*
   #include <Audio.h>
  #include <Wire.h>
  #include <SPI.h>
  #include <SD.h>
  #include <SerialFlash.h>

  // GUItool: begin automatically generated code
  AudioSynthWaveform       waveform1;      //xy=124,441
  AudioSynthNoiseWhite     noise1;         //xy=129,390
  AudioInputI2S            i2s1;           //xy=261,218
  AudioMixer4              envelope_mixer;         //xy=304,434
  AudioEffectMultiply      sampler1;      //xy=419,293
  AudioEffectEnvelope      envelope1;      //xy=475,427
  AudioMixer4              mixer1;         //xy=594,300
  AudioEffectDelay         delay1;         //xy=738,535
  AudioMixer4              delay_mixer;         //xy=744,395
  AudioMixer4              wet_dry_mixer;         //xy=988,403
  AudioAmplifier           output_amp1;           //xy=1037,302
  AudioOutputI2S           i2s2;           //xy=1072,224
  AudioConnection          patchCord1(waveform1, 0, envelope_mixer, 1);
  AudioConnection          patchCord2(noise1, 0, envelope_mixer, 0);
  AudioConnection          patchCord3(i2s1, 0, sampler1, 0);
  AudioConnection          patchCord4(i2s1, 1, wet_dry_mixer, 2);
  AudioConnection          patchCord5(envelope_mixer, envelope1);
  AudioConnection          patchCord6(sampler1, 0, mixer1, 0);
  AudioConnection          patchCord7(envelope1, 0, mixer1, 1);
  AudioConnection          patchCord8(mixer1, 0, delay_mixer, 0);
  AudioConnection          patchCord9(mixer1, 0, wet_dry_mixer, 0);
  AudioConnection          patchCord10(delay1, 0, delay_mixer, 1);
  AudioConnection          patchCord11(delay_mixer, delay1);
  AudioConnection          patchCord12(delay_mixer, 0, wet_dry_mixer, 1);
  AudioConnection          patchCord13(wet_dry_mixer, output_amp1);
  AudioConnection          patchCord14(output_amp1, 0, i2s2, 0);
  AudioConnection          patchCord15(output_amp1, 0, i2s2, 1);
  AudioControlSGTL5000     sgtl5000_1;     //xy=863,653
  // GUItool: end automatically generated code
*/
