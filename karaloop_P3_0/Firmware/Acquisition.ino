//--------------------------------------------------------------------------------------//
// Magnetometers data acquisition and classification based on thresholds
//--------------------------------------------------------------------------------------//
void Acquisition(void)
{
  float K_right = 1;
  float K_left = 1;

  // Data acquisition
  mlx_right.readData(data_right);
  mlx_left.readData(data_left);

  current_value_right = ((float(data_right.y) - init_mlx_right) * K_right) + 2500;
  current_value_left = (float(data_left.y) - init_mlx_left) * K_left ;//- 3000;;

  //HP_filter();

  // Defining the state of both sensors respect their position within the guides
  if (current_value_right > Tmax && current_value_left > Tmax) { // Both at the bottom of the guide (MOUSE UP)
    valueStateRight = 1;
    valueStateLeft = 1;
    //DisplayMag();
  }
  else if (current_value_right < Tmin && current_value_left < Tmin) { // Both at the top of the guide (MOUSE DOWN)
    valueStateRight = -1;
    valueStateLeft = -1;
    //DisplayMag();
  }
  else if (current_value_right > Tmax && current_value_left < Tmax) { // Right at the bottom, left at the top (MOUSE RIGHT)
    valueStateRight = 1;
    valueStateLeft = 0;
    //DisplayMag();
  }
  else if (current_value_left > Tmax && current_value_right < Tmax) { // Right at the top, left at the bottom (MOUSE LEFT)
    valueStateRight = 0;
    valueStateLeft = 1;
    //DisplayMag();
  }
  else if (current_value_left < Tmin && current_value_right > Tmin) { // Right at the bottom, left at the middle (SWITCH MODE)
    valueStateRight = 0;
    valueStateLeft = -1;
  }
  else if (current_value_left > Tmin && current_value_right < Tmin) { // Right at the middle, left at the botom (UNDEFINIED)
    valueStateRight = -1;
    valueStateLeft = 0;
  }
  else if (current_value_left > Tmin && current_value_right > Tmin && current_value_left < Tmax && current_value_right < Tmax) { // Rest position
    valueStateRight = 0;
    valueStateLeft = 0;
  }

}
