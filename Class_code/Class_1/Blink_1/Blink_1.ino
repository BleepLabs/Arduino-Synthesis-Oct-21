unsigned long current_time = 0;
unsigned long prev_time = 0 ;
int led_state = 0;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  current_time = millis();

  //timing if
  if (current_time - prev_time > 1000) {
    prev_time = current_time;
    led_state = !led_state;
    digitalWrite(13, led_state);

  }

}
