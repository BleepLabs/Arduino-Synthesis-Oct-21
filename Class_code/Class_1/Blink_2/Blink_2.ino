unsigned long current_time = 0;
unsigned long prev_time = 0 ;
int led_state = 0;
int rate_pot;
int bright_pot;
int raw_pot;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  current_time = millis();

  rate_pot = analogRead(A10) * 4;
  raw_pot = analogRead(A11); //0-1023
  bright_pot = map(raw_pot, 0, 1023, 0, 255); //https://www.arduino.cc/reference/en/language/functions/math/map/

  //timing if
  if (current_time - prev_time > rate_pot) {
    prev_time = current_time;

    led_state = !led_state; //! opposite
    analogWrite(13, led_state * bright_pot); //0-255

    Serial.println(current_time);
  }

}
