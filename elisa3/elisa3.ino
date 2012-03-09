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

void setup() {

  initPeripherals();
  calibrateSensors();
  initBehaviors();
  startTime = getTime100MicroSec();  

}

void loop() {

  currentSelector = getSelector();	// update selector position

  readAccelXYZ();						// update accelerometer values to compute the angle

  computeAngle();
	
  endTime = getTime100MicroSec();
  if((endTime-startTime) >= (PAUSE_2_SEC)) {
    readBatteryLevel();				// the battery level is updated every two seconds
    startTime = getTime100MicroSec();
  }

  handleIRRemoteCommands();

  handleRFCommands();

  handleMotorsWithSpeedController();  
  
}

