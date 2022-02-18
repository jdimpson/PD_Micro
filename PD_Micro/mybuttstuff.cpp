
/*******
 * Copyright 2021 Jeremy D. Impson <jdimpson at acm.org>
 * mybuttstuff is a button library for arduino  and circuitpython 
 * that makes it easy to detech when momentary switches are held 
 * for specific lengths of time, and to avoid unwanted repeating
 * repetition when a button is held down.
 * 
 * This software is available to be used under the terms described
 * in what is commonly knownas the MIT License, which can be found
 * in the file LICENSE.
 * 
 * To use the arduino code, open mybuttstuff.cpp and mybuttstuff.h
 * with Sketch->Add File in the Arduino IDE. Then in your main code:
 *      include mybuttstuff.h
 *      declare "extern struct mybuttstuff mbs;" as a global variable, 
 *      call mybuttsetup(pin, pull) in setup(), where pin is the digital 
 *          pin with your button, and pull is INPUT, INPUT_PULLUP, or 
 *          INPUT_PULLDOWN as appropriate
 *      call mybuttloop() in loop() before you want to check the button
 *      call mybuttheldfor(msecs, reset) to find out if the button was 
 *          pressed. msecs is the debounce time to wait before deciding 
 *          the button was pressed, and reset is a boolean that determines 
 *          whether the next check of the pin should be treated as a different
 *          press or the same press.
 * Not well tested with reset = False or with pulled down pins.
 *******/

#include <Arduino.h>
#include "mybuttstuff.h"

struct mybuttstuff mbs = {
  0, 0, 0, 0, false, false, false, false
};

void mybuttsetup(int pin, int pull) {
  pinMode(pin, pull);
  mbs.buttpin = pin;
  mbs.wastouched = mbs.tchanged = false;
  mbs.holding = 0;
  mbs.mono = 0;
  mbs.pressed = mbs.released = false;
}

void mybuttloop() {
  mbs.mono = millis();
  bool touched = digitalRead(mbs.buttpin) == 0;
  if (touched) {
    if (!mbs.wastouched) {
      mbs.tchanged = true;
    } else {
      mbs.tchanged = false;
    }
  }else {
    if (mbs.wastouched) {
      mbs.tchanged = true;
    } else {
      mbs.tchanged = false;
    }
  }
  mbs.wastouched = touched;
  mbs.pressed = (mbs.tchanged && touched);
  mbs.released = (mbs.tchanged && !touched);
  if ((mbs.holding > 0) && !touched) {
    mbs.holding = 0;
  }
  if (mbs.pressed) {
    if (mbs.holding == 0) {
      mbs.holding = mbs.mono;
    }
  }
  if (mbs.released) {
    mbs.holding = 0;
  }
}

bool mybuttheldfor(unsigned long msecs, bool reset) {
  bool bo = (mbs.holding > 0) && (mbs.holding + msecs < mbs.mono);
  if (bo && reset) {
    mbs.holding = 0;
  }
  return bo;
}
