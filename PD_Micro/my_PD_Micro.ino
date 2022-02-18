
#include <Wire.h>
#include "src/PD_UFP.h"
#include "mybuttstuff.h"

class PD_UFP_c PD_UFP;

int  tacpin = 18;
int   onpin = 9;
bool  onbut = 0;
PD_power_option_t newv = PD_POWER_OPTION_MAX_5V;

extern struct mybuttstuff mbs;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode( onpin, INPUT_PULLUP);  
  mybuttsetup(tacpin, INPUT_PULLUP);
  PD_UFP.init(PD_POWER_OPTION_MAX_20V);
}

void loop() {
  PD_UFP.run();
  mybuttloop();
  onbut = digitalRead( onpin) == 0;


  if (PD_UFP.is_power_ready()) {
    if(onbut && !PD_UFP.is_ps_transition()) {
      PD_UFP.set_output(1);   // Turn on load switch 
      PD_UFP.set_led(1);      // Output reach 20V and 1.5A, set indicators on
    } else {
      PD_UFP.set_output(0);   // Turn off load switch
      PD_UFP.blink_led(400);  // Output less than 20V or 1.5A, blink LED
      // only allow voltage change if load switch is off
      if (mybuttheldfor(50,true)) {
        newv = PD_POWER_OPTION_MAX_5V;
        switch (PD_UFP.get_voltage()) {
          case PD_V(20.0):
            newv = PD_POWER_OPTION_MAX_15V;
            break;
          case PD_V(15.0):
            newv = PD_POWER_OPTION_MAX_12V;
            break;
          case PD_V(12.0):
            newv = PD_POWER_OPTION_MAX_9V;
            break;
          case PD_V(9.0):
            newv = PD_POWER_OPTION_MAX_5V;
            break;
          case PD_V(5.0):
            newv = PD_POWER_OPTION_MAX_20V;
            break;
        }
        PD_UFP.set_power_option(newv);
      }
    }
  }
}
