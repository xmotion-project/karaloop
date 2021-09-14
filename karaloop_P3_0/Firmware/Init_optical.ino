//--------------------------------------------------------------------------------------//
//
//--------------------------------------------------------------------------------------//
void Init_optical()
{
  // Reference : https://github.com/bitcraze/Bitcraze_PMW3901
  if (!flow.begin()) {
    Serial.println("Initialization of the flow sensor failed");
    while (1) { }
  }

  // Reference : https://forum.arduino.cc/index.php?topic=112074.0
  // initialize SPI:
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  //!!! Clock Polarity 0, Clock Phase is 1
  //!!! See http://en.wikipedia.org/wiki/File:SPI_timing_diagram2.svg
  //!!!SCAT datasheet page 11, figure 10
  SPI.setDataMode(1);

}
