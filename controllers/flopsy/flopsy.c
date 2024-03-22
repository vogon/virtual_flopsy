/*
 * File:          flopsy.c
 * Date:
 * Description:
 * Author:
 * Modifications:
 */

/*
 * You may need to add include files like <webots/distance_sensor.h> or
 * <webots/motor.h>, etc.
 */
#include <webots/robot.h>
#include <webots/motor.h>
#include <webots/keyboard.h>

/*
 * You may want to add macros here.
 */
#define TIME_STEP 64
#define FULL_SPEED 10.0

enum FlipperState {
  Armed,
  Firing,
  Reloading
};

/*
 * This is the main program.
 * The arguments of the main function can be specified by the
 * "controllerArgs" field of the Robot node
 */
int main(int argc, char **argv) {
  /* necessary to initialize webots stuff */
  wb_robot_init();
  wb_keyboard_enable(TIME_STEP);

  /*
   * You should declare here WbDeviceTag variables for storing
   * robot devices like this:
   *  WbDeviceTag my_sensor = wb_robot_get_device("my_sensor");
   *  WbDeviceTag my_actuator = wb_robot_get_device("my_actuator");
   */
  WbDeviceTag l_motor = wb_robot_get_device("l_motor");
  WbDeviceTag r_motor = wb_robot_get_device("r_motor");
  WbDeviceTag flipper_motor = wb_robot_get_device("flipper_motor");
  
  // switch wheel motors into velocity control mode
  wb_motor_set_position(l_motor, INFINITY);
  wb_motor_set_position(r_motor, INFINITY);
  
  enum FlipperState flipper_state = Armed;
  int flipper_time_left_in_state = 0;

  /* main loop
   * Perform simulation steps of TIME_STEP milliseconds
   * and leave the loop when the simulation is over
   */
  while (wb_robot_step(TIME_STEP) != -1) {
    /*
     * Read the sensors :
     * Enter here functions to read sensor data, like:
     *  double val = wb_distance_sensor_get_value(my_sensor);
     */

    /* Process sensor data here */

    /*
     * Enter here functions to send actuator commands, like:
     * wb_motor_set_position(my_actuator, 10.0);
     */ 
    double commanded_l = 0.0;
    double commanded_r = 0.0;
    
    while (true) {
      int key = wb_keyboard_get_key();
      bool done = false;
      
      switch (key) {
        case 'W':
          commanded_l = commanded_r = FULL_SPEED;
          break;
        case 'S':
          commanded_l = commanded_r = -FULL_SPEED;
          break;
        case 'A':
          commanded_l = -FULL_SPEED;
          commanded_r = FULL_SPEED;
          break;
        case 'D':
          commanded_l = FULL_SPEED;
          commanded_r = -FULL_SPEED;
          break;
        case ' ':
          if (flipper_state == Armed) {
            flipper_state = Firing;
            flipper_time_left_in_state = 5;
          }
          break;
        case -1:
          done = true;
          break;
      }
      
      if (done) break;
    }
    
    wb_motor_set_velocity(l_motor, commanded_l);
    wb_motor_set_velocity(r_motor, commanded_r);
    
    switch (flipper_state) {
      case Firing:
        wb_motor_set_torque(flipper_motor, 0.1);
        flipper_time_left_in_state--;
        if (flipper_time_left_in_state == 0) {
          flipper_state = Reloading;
          flipper_time_left_in_state = 30;
        }
        break;
      case Reloading:
        wb_motor_set_position(flipper_motor, 0);
        flipper_time_left_in_state--;
        if (flipper_time_left_in_state == 0) {
          flipper_state = Armed;
        }
        break;
      default:
        wb_motor_set_position(flipper_motor, 0);
        break;
    }
  };

  /* Enter your cleanup code here */

  /* This is necessary to cleanup webots resources */
  wb_keyboard_disable();
  wb_robot_cleanup();

  return 0;
}
