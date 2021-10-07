//Making an LFO, a slowly changing variable, that modulates the brightness of the LED 

unsigned long current_time = 0;
unsigned long prev_time = 0 ;
int led_state;
int rate_pot;
int bright_pot;
int raw_pot;
int latch1;

void setup() {
}

void loop() {
  current_time = millis();

  rate_pot = analogRead(A10) / 4;
  raw_pot = analogRead(A11); //0-1023
  bright_pot = map(raw_pot, 0, 1023, 0, 255); //https://www.arduino.cc/reference/en/language/functions/math/map/

  //timing if
  if (current_time - prev_time > rate_pot) {
    prev_time = current_time;


    if (latch1 == 1) {
      led_state = led_state + 1;
    }
    if (latch1 == 0) {
      led_state = led_state - 1;
    }
    //once we get to the top we change the latch to 0
    // so next time around led_state is increased
    if (led_state > 254) {
      latch1 = 0;
    }

    // wehn it hits the bottom the latch is changed to 1
    if (led_state < 1) {
      led_state = 1;
      latch1 = 1;
    }

    analogWrite(13, led_state); //0-255

    Serial.println(led_state); //print the number out on the serial monitor
  }

}
