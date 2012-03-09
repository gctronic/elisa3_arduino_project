#include <adc.h>
#include <behaviors.h>
#include <constants.h>
#include <ir_remote_control.h>
#include <leds.h>
#include <mirf.h>
#include <motors.h>
#include <nRF24L01.h>
#include <ports_io.h>
#include <sensors.h>
#include <speed_control.h>
#include <spi.h>
#include <twimaster.h>
#include <usart.h>
#include <utility.h>
#include <variables.h>

unsigned long int startTime = 0, endTime = 0;
unsigned char prevSelector = 0;

void setup() {

  initPeripherals();
  calibrateSensors();
  initBehaviors();
  startTime = getTime100MicroSec();  

}

void loop() {

  currentSelector = getSelector();	          // update selector position

  readAccelXYZ();			          // update accelerometer values to compute the angle

  computeAngle();
	
  endTime = getTime100MicroSec();
  if((endTime-startTime) >= (PAUSE_2_SEC)) {
    readBatteryLevel();			          // the battery level is updated every two seconds
			
    if(currentSelector==4 || currentSelector==5) {
      pwm_red = rand() % 255;
      pwm_green = rand() % 255;
      pwm_blue = rand() % 255;
    }    
    
    startTime = getTime100MicroSec();
  }

  handleIRRemoteCommands();

  handleRFCommands();

  usartTransmit(currentSelector);                // send the current selector position through uart as debug info

  switch(currentSelector) {
    
    case 0: // motors in direct power control (no speed control)
            handleMotorsWithNoController();
            break;
             
    case 1: // obstacle avoidance enabled (the robot does not move untill commands are 
            // received from the radio or tv remote)
            enableObstacleAvoidance();
            break;
             
    case 2: // cliff avoidance enabled (the robot does not move untill commands are 
	    // received from the radio or tv remote)
            enableCliffAvoidance();
            break;
    
    case 3: // both obstacle and cliff avoidance enabled (the robot does not move untill commands are
            // received from the radio or tv remote)
            enableObstacleAvoidance();
            enableCliffAvoidance();
            break;
            
    case 4: // random colors on RGB leds; small green leds turned on
            GRREEN_LED0_ON;
            GRREEN_LED1_ON;
            GRREEN_LED2_ON;
            GRREEN_LED3_ON;
            GRREEN_LED4_ON;
            GRREEN_LED5_ON;
            GRREEN_LED6_ON;
            GRREEN_LED7_ON;
            updateRedLed(pwm_red);
            updateGreenLed(pwm_green);
            updateBlueLed(pwm_blue);
            break;
             
    case 5: // random colors on RGB leds; obstacle avoidance enabled; robot start moving automatically
            // (motors speed setting)
            updateRedLed(pwm_red);
            updateGreenLed(pwm_green);
            updateBlueLed(pwm_blue);
            enableObstacleAvoidance();
            setLeftSpeed(25);
            setRightSpeed(25);
            break;
  
  }

  if(currentSelector != 0) {
    handleMotorsWithSpeedController();  
  }

  if(prevSelector != currentSelector) {	// in case the selector is changed, reset the robot state
    disableObstacleAvoidance();
    disableCliffAvoidance();
    GRREEN_LED0_OFF;
    GRREEN_LED1_OFF;
    GRREEN_LED2_OFF;
    GRREEN_LED3_OFF;
    GRREEN_LED4_OFF;
    GRREEN_LED5_OFF;
    GRREEN_LED6_OFF;
    GRREEN_LED7_OFF;
    pwm_red = 255;
    pwm_green = 255;
    pwm_blue = 255;
    updateRedLed(pwm_red);
    updateGreenLed(pwm_green);
    updateBlueLed(pwm_blue);
  }
  prevSelector = currentSelector;

}

