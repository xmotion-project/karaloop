//--------------------------------------------------------------------------------------//
// Initialize the I2C communication for both magnetometers
//--------------------------------------------------------------------------------------//
void Init_mlx(void)
{
  //  // MLX Right
  mlx_right.begin(0, 0); //Assumes I2C jumpers are GND. No DRDY pin used.
  mlx_right.setOverSampling(3);
  mlx_right.setDigitalFiltering(0);
  mlx_right.setGainSel(3);//3
  mlx_right.setResolution(0, 0, 0); //x, y, z

  // MLX Left
  mlx_left.begin(0, 1);
  mlx_left.setOverSampling(3);
  mlx_left.setDigitalFiltering(0);
  mlx_left.setGainSel(3);//3
  mlx_left.setResolution(0, 0, 0); //x, y, z

  old_state_right = 0;
  old_state_left = 0;
  current_state_right = 0;
  current_state_left = 0;

//    mlx_right.reset();
//    mlx_left.reset();

  mlx_right.reset();
  mlx_right.readData(data_right);

  mlx_left.reset();
  mlx_left.readData(data_left);

  init_mlx_left = data_left.y;/*sqrt(pow(float(data_left.y), 2) + pow(float(data_left.z), 2));*/
  init_mlx_right = data_right.y; /*sqrt(pow(float(data_right.x), 2) + pow(float(data_right.y), 2));*/

  Tinit_min = (Tmax - Tmin) / 2 - Ttolerance + Tmin + Toffset;
  Tinit_max = (Tmax - Tmin) / 2 + Ttolerance + Tmin + Toffset;

}
