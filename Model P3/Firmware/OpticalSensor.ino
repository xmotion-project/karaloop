unsigned int  delayTime_optique = 0;
unsigned int  MemTimeDelay_optique;
const int optical_threshold = 10;
const int delay_relaxation = 500;
float time_passed;

// Reference : https://github.com/bitcraze/Bitcraze_PMW3901/blob/master/examples/flow/flow.ino
//--------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------//
void OpticalSensor()
{
  static int delayValue_optique = 20;
  delayTime_optique = millis();

  // Waiting a delay if both movements are not synchronized
  if (((delayTime_optique - MemTimeDelay_optique) > delayValue_optique))
  {
    // Get motion count since last call
    flow.readMotionCount(&deltaX, &deltaY);
    //        Serial.print(deltaX);
    //        Serial.print(" ");
    //        Serial.println(deltaY);
    time_passed = millis();
    MemTimeDelay_optique = delayTime_optique;


    if (deltaY < -optical_threshold && MouseMove == false) {



      while (millis() < time_passed + delay_relaxation) {
        delay(20);
        flow.readMotionCount(&deltaX, &deltaY);
        //      Serial.print(deltaX);
        //      Serial.print(" ");
        //      Serial.println(deltaY);
        if (deltaY > optical_threshold) {
          blehid.mouseButtonPress(MOUSE_BUTTON_LEFT);
          delay(1);
          blehid.mouseButtonRelease();
          break;
        }
      }

      while (millis() > time_passed + delay_relaxation && millis() < time_passed + 2 * delay_relaxation) {
        //      Serial.print(deltaX);
        //      Serial.print(" ");
        //      Serial.println(deltaY);
        delay(20);
        flow.readMotionCount(&deltaX, &deltaY);

        if (deltaY > optical_threshold) {
          blehid.mouseButtonPress(MOUSE_BUTTON_RIGHT);
          delay(1);
          blehid.mouseButtonRelease();
          break;
        }
      }


    while (millis() > time_passed + 2 * delay_relaxation) {
      blehid.mouseButtonPress(MOUSE_BUTTON_LEFT);
      break;
    }

    if (deltaY < -optical_threshold)
    {
      blehid.mouseButtonRelease();
      }



    }

  }

}
