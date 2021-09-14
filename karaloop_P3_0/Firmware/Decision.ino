
float MOVE_STEP_2;

boolean right_click_on = true;
boolean left_click_on = true;

boolean state_change = false;
boolean stateDelay = false;
boolean ModeState = false;

unsigned int  MemTimeDelay;
unsigned int MemTimeDelayMode;
unsigned int MemTimeDelayShortMov;
unsigned int  delayTime = 0;
static float MouseGainRight = 1;
static float MouseGainLeft = 1;

//--------------------------------------------------------------------------------------//
// Affect an action when magnetometers'signal are passing through a threshold
// Mouse MODE : moving the pointer UP - DOWN and LEFT - RIGHT
// Keyboard MODE : send keypress w for UP, s for DOWN, a for LEFT and d for RIGHT
//--------------------------------------------------------------------------------------//
void Decision(void)
{
  static int delayValue = 100;
  static int shortDetection = 500;
  delayTime = millis();

  // Détection d'un changement d'état
  if (valueStateRight != OldvalueStateRight || valueStateLeft != OldvalueStateLeft) {
    state_change = true;
    stateDelay = false;
    MemTimeDelay = delayTime;
    //blehid.keyRelease();
  }

  // Waiting a delay if both movements are not synchronized
  if (((delayTime - MemTimeDelay) > delayValue) && state_change == true) {
    state_change = false;
    stateDelay = true;
    Acquisition();
    //DisplayMag();

    MemTimeDelayMode = delayTime; //delay for changing mode (KEYBOARD<->MOUSE)
    MemTimeDelayShortMov = delayTime; //short movement detection : between 0 to 0.5 sec
    ModeState = true;
  }

  //Classification
  if (current_value_right > Tmax && current_value_left > Tmax && stateDelay == true) { //Y+ --> Blowing both cheeks
    MOVE_STEP_2 = MouseGainRight*ExponentialSpeedMouse(current_value_right, true);
    MouseMove = true;
    if (MouseToKeyboard) {
      //      if ((delayTime - MemTimeDelayShortMov) < shortDetection) {
      //        blehid.mouseMove(0, -2);
      //      }
      //      else {
      blehid.mouseMove(0, -MOVE_STEP_2);
      //      }
    }
    else {
      blehid.keyPress(0x77);
      //blehid.keyRelease();
//      delay(500);
    }
  }
  else if (current_value_right < Tmin && current_value_left < Tmin && stateDelay == true) { //Y- --> Sucking both lips
    MOVE_STEP_2 = MouseGainRight*ExponentialSpeedMouse(current_value_right, false);
    MouseMove = true;
    if (MouseToKeyboard) {
      //      if ((delayTime - MemTimeDelayShortMov) < shortDetection) {
      //        blehid.mouseMove(0, 2);
      //      }
      //      else {
      blehid.mouseMove(0, MOVE_STEP_2);
      //      }
    }
    else {
      blehid.keyPress(0x73);
      //blehid.keyRelease();
//      delay(500);
    }
  }
  else if (current_value_right > Tmax && current_value_left < Tmax && stateDelay == true) { //X+ --> Blowing cheek right
    MOVE_STEP_2 = MouseGainRight*ExponentialSpeedMouse(current_value_right, true);
    MouseMove = true;
    if (MouseToKeyboard) {
      //      if ((delayTime - MemTimeDelayShortMov) < shortDetection) {
      //        blehid.mouseMove(2, 0);
      //      }
      //      else {
      blehid.mouseMove(MOVE_STEP_2, 0);
      //      }
    }
    else {
      blehid.keyPress(0x64);
      //blehid.keyRelease();
//      delay(500);
    }
  }
  else if (current_value_right < Tmax && current_value_left > Tmax && stateDelay == true) { //X- --> Blowing cheek left
    MOVE_STEP_2 = MouseGainLeft*ExponentialSpeedMouse(current_value_left, true);
    MouseMove = true;
    if (MouseToKeyboard) {
      //      if ((delayTime - MemTimeDelayShortMov) < shortDetection) {
      //        blehid.mouseMove(-2, 0);
      //      }
      //      else {
      blehid.mouseMove(-MOVE_STEP_2, 0);
      //      }
    }
    else {
      blehid.keyPress(0x61);
      //blehid.keyRelease();
//      delay(500);
    }
  }
  else {
    blehid.mouseMove(0, 0);
    blehid.keyRelease();
    MouseMove = false;
  }

  // Keep the last update for both sensor
  OldvalueStateRight = valueStateRight;
  OldvalueStateLeft = valueStateLeft;

  //Serial.println(MOVE_STEP_2);

}
