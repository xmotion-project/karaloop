//--------------------------------------------------------------------------------------//
// Exponential transfer function between magnetometers' signals amplitude and mouse pointer speed
//--------------------------------------------------------------------------------------//
float ExponentialSpeedMouse(float currentValue, boolean Direction)
{
  static float MoveStep;
  static int MoveMin = 0;
  static int TmaxGuide = 9000;
  static int TminGuide = 500;
  static float A = 1;
  static float C = 0.35;
  static int B = 10;
  static float D = 6;

  if (Direction) { //positiv direction
    if (currentValue > TmaxGuide) {
      MoveStep = MoveMax;
    }
    else {
      MoveStep = A * pow(currentValue / Tmax, B) - A + MoveMin;
      if (MoveStep > MoveMax) {
        MoveStep = MoveMax;
      }
    }
  }
  else { //negativ direction
    if (currentValue < TminGuide) {
      MoveStep = MoveMax;
    }
    else {
      MoveStep = C * pow((1 - (currentValue / Tmin) + 1), D) - C + MoveMin;
      if (MoveStep > MoveMax) {
        MoveStep = MoveMax;
      }
    }
  }
  return MoveStep;
}
