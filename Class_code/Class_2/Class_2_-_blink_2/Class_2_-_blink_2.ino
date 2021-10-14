unsigned long current_time;
unsigned long prev_time;
unsigned long prev_time2;
int led_state;
int rate1 = 200;

void setup() {
  pinMode(13, OUTPUT); //only for LEDs

}

void loop() {
  current_time = millis();

  rate1 = 1023 - analogRead(A10); //analogRead returns 0-1023 10b ADC

  if (current_time - prev_time2 > 100) {
    prev_time2 = current_time;
    Serial.println(rate1);
  }

  if (current_time - prev_time > rate1) {
    prev_time = current_time;


    //led_state = !led_state;

    if (led_state == 0) {
      led_state = 1;
    }
    else {
      led_state = 0;
    }

  }

  digitalWrite(13, led_state);


}
