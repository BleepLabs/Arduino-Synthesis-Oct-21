// all the functions I made to check the controls and wok the LEDs live here
// We'll talk about whats they are really doing on later in the course
#include <Bounce2.h>

Bounce * buttons = new Bounce[8];
#define BOUNCE_LOCK_OUT
byte pot_pins[8] = {A10, A11, A12, A13, A14, A15, A16, A17};
byte button_pins[8] = {30, 31, 32, 33, 34, 35, 36, 37};


#include <WS2812Serial.h>
#define num_of_leds 5 //increase for more LEDs
#define led_data_pin 29 // only these pins can be used on the Teensy 3.2:  1, 5, 8, 10, 31
byte drawingMemory[num_of_leds * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[num_of_leds * 12]; // 12 bytes per LED
WS2812Serial LEDs(num_of_leds, displayMemory, drawingMemory, led_data_pin, WS2812_GRB);
float max_brightness; //Change this to increase the max brightness of the LEDs. 1.0 is very bright


byte mux_select, prev_mux_select;
int raw_pot_reading[8];
int sm_pot_reading[8];
float pot_reading[8];
int button_reading[8];
int prev_button_reading[8];
int button_state[8];
uint32_t pm[8];
int pcellRaw, pcellSmooth;

#define ctrl_pin1 2
#define ctrl_pin2 3
#define ctrl_pin3 4

#define digital_read_pin 5
#define analog_pin1 A1

#define FELL 4
#define ROSE 5
#define IS_PRESSED 0
#define NOT_PRESSED 1

#define  bounce_lockout_length 10
byte first_time = 1;

void start_bleep_base() {

  for (int i = 0; i < 8; i++) {
    pinMode(button_pins[i], INPUT_PULLUP);
  }

  for (int i = 0; i < 8; i++) {
    buttons[i].attach( button_pins[i] , INPUT_PULLUP  );       //setup the bounce instance for the current button
    buttons[i].interval(5);              // interval in ms
  }

  LEDs.begin(); //must be done in setup for the LEDs to work.
  LEDs.setPixelColor(0, 0, 0, 0);
  LEDs.setPixelColor(1, 0, 0, 0);
  LEDs.show(); // after we've set what we want all the LEDs to be we send the data out through this function

  max_brightness = .2;
  analogReadAveraging(64);
}


//Smooth
// based on https://playground.arduino.cc/Main/DigitalSmooth/
// This function continuously samples an input and puts it in an array that is "samples" in length.
// This array has a new "raw_in" value added to it each time "smooth" is called and an old value is removed
// It throws out the top and bottom 15% of readings and averages the rest

#define maxarrays 9 //max number of different variables to smooth
#define maxsamples 21 //max number of points to sample and 
//reduce these numbers to save RAM

unsigned int smoothArray[maxarrays][maxsamples];

// sel should be a unique number for each occurrence
// samples should be an odd number greater that 7. It's the length of the array. The larger the more smooth but less responsive
// raw_in is the input. positive numbers in and out only.

unsigned int smooth(byte sel, unsigned int samples, unsigned int raw_in) {
  int j, k, temp, top, bottom;
  long total;
  static int i[maxarrays];
  static int sorted[maxarrays][maxsamples];
  boolean done;

  i[sel] = (i[sel] + 1) % samples;    // increment counter and roll over if necessary. -  % (modulo operator) rolls over variable
  smoothArray[sel][i[sel]] = raw_in;                 // input new data into the oldest slot

  for (j = 0; j < samples; j++) { // transfer data array into anther array for sorting and averaging
    sorted[sel][j] = smoothArray[sel][j];
  }

  done = 0;                // flag to know when we're done sorting
  while (done != 1) {      // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (samples - 1); j++) {
      if (sorted[sel][j] > sorted[sel][j + 1]) {    // numbers are out of order - swap
        temp = sorted[sel][j + 1];
        sorted[sel] [j + 1] =  sorted[sel][j] ;
        sorted[sel] [j] = temp;
        done = 0;
      }
    }
  }

  //I changed this to just 3 off the top and bottom
  bottom = 3;
  top = samples - 3;
  k = 0;
  total = 0;
  for ( j = bottom; j < top; j++) {
    total += sorted[sel][j];  // total remaining indices
    k++;
  }
  return total / k;    // divide by number of samples
}

float potRead(byte sel) {
  if (sel > 7) {
    sel = 7;
  }
  float sm = smooth(sel, 17, analogRead(pot_pins[sel]));
  sm = (1023 - sm) / 1023.0;
  return sm;
}

float potReadRaw(byte sel) {
  if (sel > 7) {
    sel = 7;
  }
  return 1023 - analogRead(pot_pins[sel]);
}

float pcellReadRaw() {
  return analogRead(A8) / 1023.0;
}

float pcellRead() {
  float sm = smooth(8, 17, analogRead(A8));
  sm = (1023 - sm) / 1023.0;
  return sm;
}

int buttonState(byte num) {
  return button_state[num];
}
int buttonRead(byte num) {
  return button_reading[num];
}
void setMaxBrightness(float b) {
  max_brightness = b;
}

//This function is a little different than you might see in other libraries but it works pretty similar
// instead of 0-255 you see in other libraries this is all 0-1.0
// you can copy this to the bottom of any code as long as the declarations at the top in "led biz" are done

//set_pixel_HSV(led to change, hue,saturation,value aka brightness)
// led to change is 0-63
// all other are 0.0 to 1.0
// hue - 0 is red, then through the ROYGBIV to 1.0 as red again
// saturation - 0 is fully white, 1 is fully colored.
// value - 0 is off, 1 is the value set by max_brightness
// (it's not called brightness since, unlike in photoshop, we're going from black to fully lit up

//based on https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both

void set_LED(int pixel, float fh, float fs, float fv) {
  byte RedLight;
  byte GreenLight;
  byte BlueLight;

  byte h = fh * 255;
  byte s = fs * 255;
  byte v = fv * max_brightness * 255;

  h = (h * 192) / 256;  // 0..191
  unsigned int i = h / 32;   // We want a value of 0 thru 5
  unsigned int f = (h % 32) * 8;   // 'fractional' part of 'i' 0..248 in jumps

  unsigned int sInv = 255 - s;  // 0 -> 0xff, 0xff -> 0
  unsigned int fInv = 255 - f;  // 0 -> 0xff, 0xff -> 0
  byte pv = v * sInv / 256;  // pv will be in range 0 - 255
  byte qv = v * (256 - s * f / 256) / 256;
  byte tv = v * (256 - s * fInv / 256) / 256;

  switch (i) {
    case 0:
      RedLight = v;
      GreenLight = tv;
      BlueLight = pv;
      break;
    case 1:
      RedLight = qv;
      GreenLight = v;
      BlueLight = pv;
      break;
    case 2:
      RedLight = pv;
      GreenLight = v;
      BlueLight = tv;
      break;
    case 3:
      RedLight = pv;
      GreenLight = qv;
      BlueLight = v;
      break;
    case 4:
      RedLight = tv;
      GreenLight = pv;
      BlueLight = v;
      break;
    case 5:
      RedLight = v;
      GreenLight = pv;
      BlueLight = qv;
      break;
  }
  LEDs.setPixelColor(pixel, RedLight, GreenLight, BlueLight);
}