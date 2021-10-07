//Initilization section
// Here at the top we delare variabeles to be used
// trhought the whole code 

unsigned long current_time = 0; //can hold values from 0 - 4,294,967,295 32bits https://www.arduino.cc/reference/en/language/variables/data-types/unsignedlong/
unsigned long prev_time = 0 ;
int led_state = 0; //-32,768 to 32,767. 16 bits https://www.arduino.cc/reference/en/language/variables/data-types/int/

void setup() {
  //Setup exectures code once upon startup 
  // We don't have anything that needs to be run here yet
}

void loop() {
  //store how many millisecond have past since the device restarted
  // in the variable "current_time"
  current_time = millis();  

  //timing if
  // Is the current time minus the time recorded the last time the
  // code inside the if was run greater than 1000?
  if (current_time - prev_time > 1000) {
    prev_time = current_time; //remeber the time
    led_state = !led_state; //make led_state the opposite of waht it is. 0 becomes 1, 1 becomes 0
    digitalWrite(13, led_state); //output "led_state" to pin 13 which hs an LED attached
  }

}
