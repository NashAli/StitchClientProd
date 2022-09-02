#ifndef _SUPPORT_H
#define _SUPPORT_H
/*
  Created:      19:55 2021.11.2
  Last Updated: 14:45 2021.11.25
  MIT License

  Copyright (c) 2021 Zulfikar Naushad Ali

  Permission is hereby granted, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  This project uses the Devkit esp32 WROOM 32 module.
  See docs for more info.
  I2C Device Listing
  0x20 - MCP23017 Port Expander - Stepper Motor Drivers
  0x21 - MCP23017 - Aux Controller
  0x3C - OLED - used
  0x68 - MPU6050 - Head motion detect.
  ULN2803 buffer driver
  DRV8825 - stepper motor driver PCB

   Modes:
  0 - 1/1
  1 - 1/2
  2 - 1/4
  3 - 1/8
  4 - 1/16  (not used)
  5 - 1/32  (not used)

  Subject to change, without notice!
*/

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <MCP23017.h>

Adafruit_MPU6050 mpu;
MCP23017 mcp = MCP23017(MotorControl);
MCP23017 acp = MCP23017(AuxControl);

//  These are the operational routines for STITCH
/*
  *************************************** INTERRUPT HANDLER *****************************
*/
void IRAM_ATTR checkLimits()
{
  limits = mcp.readPort(MCP23017Port::A);
  // set a flag for OS, new limits is available and needs attention quickly.
  LIMITS_FLAG = true;
}
/* -------------------------- MCP23017 INIT ------------------------------------------------------- */
/*
   Dear Complier, Error Messages, please let's have none of those.
   Please don't ignore me, I think you're ignoring me! I've seen you here before!
*/
/*
   Title:       mark/get CurrentPlace
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: secure 50 bytes(0-24) for the ssid,pswd combo in EEPROM.
                Byte 25 CalGood byte 0xA5, From Bytes 26-27(XOFFSET-16bit),Bytes 28-29(YOFFSET-16bit).
                Byte 32 GoodPosition byte 0x5A 33-34 XPOS, 35-36 YPOS.
   Input:
   Returns:
*/
void markCurrentPlace()
{
  if(!EEPROM.begin(50))
  {
    EEPROM.begin(50);  /// 
  }
  EEPROM.write(32,0x5A);
  uint8_t xposHigh, xposLow, yposHigh, yposLow;
  xposHigh = XCPOS >> 8 & 0xFF;
  xposLow = XCPOS >> 0 & 0xFF;
  yposHigh = YCPOS >> 8 & 0xFF;
  yposLow = YCPOS >> 0 & 0xFF;
  EEPROM.write(33, xposHigh);
  EEPROM.write(34, xposLow);
  EEPROM.write(35, yposHigh);
  EEPROM.write(36, yposLow);
  EEPROM.end();
}

bool getCurrentPlace()
{
  uint8_t GoodPosition = 0;
  uint8_t xValueHigh, xValueLow, yValueHigh, yValueLow;
  GoodPosition = EEPROM.read(32);
  if(GoodPosition == 0x5A)
  {
    xValueHigh = EEPROM.read(33);
    xValueLow = EEPROM.read(34);
    XCPOS = xValueHigh << 8 | xValueLow;
    yValueHigh = EEPROM.read(35);
    yValueLow = EEPROM.read(36);
    YCPOS = yValueHigh << 8 | yValueLow;
    EEPROM.end();
    display1.clearDisplay();
    display1.drawRect(0, 0, 127, 63, WHITE);
    display1.setCursor(30, 5);
    display1.print(OSNAME);
    display1.display();
    display1.setCursor(20, 20);
    display1.print("Values loaded");
    display1.display();
    delay(500);
    return true;
  }else
  EEPROM.end();
  return false;
}


void testSensor(){
 Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  //setupt motion detection
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);

  Serial.println("");
  delay(100);
  }

/*
   Title:       
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
*/
void putCalibrationValues(uint16_t xCalValue, uint16_t yCalValue)
{
  if(!EEPROM.begin(50))
  {
    EEPROM.begin(50);  /// secure 25 bytes(0-24) for the ssid,pswd combo in EEPROM. From Bytes 26-27(XOFFSET-16bit),Bytes 28-29(YOFFSET-16bit).
  }
  EEPROM.write(25,0xA5);
  uint8_t xCalHigh, xCalLow, yCalHigh, yCalLow;
  xCalHigh = xCalValue >> 8 & 0xFF;
  xCalLow = xCalValue >> 0 & 0xFF;
  yCalHigh = yCalValue >> 8 & 0xFF;
  yCalLow = yCalValue >> 0 & 0xFF;
  EEPROM.write(26, xCalHigh);
  EEPROM.write(27, xCalLow);
  EEPROM.write(28, yCalHigh);
  EEPROM.write(29, yCalLow);
  EEPROM.end();
}

bool getCalibrationValues()
{
  if(!EEPROM.begin(50))
  {
    EEPROM.begin(50);  /// secure 25 bytes(0-24) for the ssid,pswd combo in EEPROM. (Byte 25 is Flags) From Bytes 26-27(XOFFSET-16bit),Bytes 28-29(YOFFSET-16bit).
  }
  uint8_t goodCal = 0;
  uint8_t xValueHigh, xValueLow, yValueHigh, yValueLow;
  goodCal = EEPROM.read(25);
  if(goodCal == 0xA5){
    xValueHigh = EEPROM.read(26);
    xValueLow = EEPROM.read(27);
    XOFFSET = xValueHigh << 8 | xValueLow;
    yValueHigh = EEPROM.read(28);
    yValueLow = EEPROM.read(29);
    YOFFSET = yValueHigh << 8 | yValueLow;
    EEPROM.end();
    display1.clearDisplay();
    display1.drawRect(0, 0, 127, 63, WHITE);
    display1.setCursor(30, 5);
    display1.print(OSNAME);
    display1.display();
    display1.setCursor(20, 20);
    display1.print("Values loaded");
    display1.display();
    delay(500);
    return true;
  }else
  EEPROM.end();
  return false;
}




/*
   Title:       InitMotorsPort
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Initializes the MCP23017 assigned to controlling the stepper motor drivers and reading the limit sensors.
   Input:       nothing
   Returns:     nothing
*/
void initializeMotorsPort()
{
  if (MOTORS_ACTIVE)
  {
    mcp.init();
    mcp.portMode(MCP23017Port::A, 0xFF);  //Port A as inputs
    mcp.portMode(MCP23017Port::B, 0);     //Port B as outputs
    mcp.interruptMode(MCP23017InterruptMode::Separated);
    mcp.interrupt(MCP23017Port::A, CHANGE);  // trigger an interrupt when an input pin CHANGE is detected on PORTA
    mcp.clearInterrupts();
    mcp.writeRegister(MCP23017Register::IPOL_A, 0x00);
    mcp.writeRegister(MCP23017Register::IPOL_B, 0x00);
    mcp.writeRegister(MCP23017Register::GPIO_A, 0x00);  //Reset port A
    mcp.writeRegister(MCP23017Register::GPIO_B, 0x06);  //Reset port B to default value()
    for (int i = 0; i < 8; i++)
    {
      mcp.pinMode(i, INPUT_PULLUP, true);
    }
    for (int i = 8; i < 16; i++)
    {
      mcp.pinMode(i, OUTPUT);
    }
    attachInterrupt(digitalPinToInterrupt(ML_INTA), checkLimits, FALLING);
    //set the limits interrupt to GPIO32
    MOTORS_READY = true;
    drawBanner();
    display1.setCursor(20, 20);
    display1.print("Motors in!");
    display1.display();
    delay(500);
  } else
  {
    MOTORS_READY = false;
    drawBanner();
    display1.setCursor(20, 20);
    display1.print("No Motors!");
    display1.display();
    delay(500);
  }
}
/*
   Title:       InitControllerPort
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Initializes the aux port if it is installed
   Input:       nothing
   Returns:     nothing
*/
void initializeControllerPort() {
  if (AUX_ACTIVE)
  {
    acp.init();
    acp.portMode(MCP23017Port::A, 0xFF);  //Port A as in
    acp.portMode(MCP23017Port::B, 0);           //Port B as out
    acp.writeRegister(MCP23017Register::GPIO_A, 0x00);  //Reset port A
    acp.writeRegister(MCP23017Register::GPIO_B, 0x00);  //Reset port B
    //set the control interrupt to GPIO35
    drawBanner();
    display1.print("Remote Control in");
    display1.display();
    delay(500);
  } else
  {
    drawBanner();
    display1.setCursor(25, 20);
    display1.print("No Remote");
    display1.display();
    delay(500);
  }
}
/*
   Title:       SetPortPin
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Sets a pin on the specified port
   Input:       port name<A:B>, pin number<0-7>, value<H:L>
   Returns:     nothing
*/
void setPortPin(int pn, int pp, bool v)
{
  //mcp.setPin(pn,pp,v);
}
/*
   Title:       GetPortPin
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Initializes the aux port if it is installed
   Input:       port name<A:B>,<0-7>
   Returns:     pin value <H:L>
*/
bool getPortPin(int pn, int pp)
{
  int comval = 0;
  comval = mcp.readPort(MCP23017Port::A);
}
/*
   Title:       SetPort
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Initializes the aux port if it is installed
   Input:       port name<A:B>, value<0-7>
   Returns:     nothing
*/
void setPort(int pn, int v)
{
  if (pn == 1) {
    mcp.writeRegister(MCP23017Register::GPIO_A, v);//  Set port A
  }
  else {
    mcp.writeRegister(MCP23017Register::GPIO_B, v);  //  Set port B
  }
}
/*
   Title:       GetPort
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Gets the port value port if it is installed
   Input:       port name<A:B>
   Returns:     Hex string value
*/
String getPort(int pn)
{
  int comval;
  if (pn == 1) {
    comval = mcp.readPort(MCP23017Port::A);
  } else {
    comval = mcp.readPort(MCP23017Port::B);
  }
  return integerToHexString(comval);
}



/*
   Dear compiler... Am I there yet? Please go easy on me today, my morning has been rough.
*/

/*
   Title:       IsAtDestination
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Checks to see if X an Y location has been reached.
   Input:       nothing
   Returns:     true if successful
*/
bool isAtDestination()
{
  if ((XCPOS == XDEST) && (YCPOS == YDEST))
  {
    return true;
  } else
  {
    return false;
  }

}

/*
   Title:       ReadLimits
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Initializes the aux port if it is installed
   Input:       nothing
   Returns:     nothing
*/
void readLimits()
{
  limits = mcp.readPort(MCP23017Port::A);
}
/*
   Title:       ReadController
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Reads the aux port if it is installed
   Input:       nothing
   Returns:     Port A value
*/
void readController()
{
  comval = acp.readPort(MCP23017Port::A);
}
/*
   Title:       SetController
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Sets the aux port if it is installed
   Input:       nothing
   Returns:     nothing
*/
void setController()
{

}

/*
   Title:       IsXHome
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
*/
bool isXHome()
{
  readLimits();
  if (limits && XHOME)
  {
    return false;
  } else return true;
}
/*
   Title:       isXMax
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:     true if successful
*/
bool isXMax()
{
  readLimits();
  if (limits && XMAX)
  {
    return false;
  } else return true;
}
/*
   Title:       IsYHome
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:     true if successful
*/
bool isYHome()
{
  readLimits();
  if (limits && YHOME) {
    return false;
  } else return true;
}
/*
   Title:       IsYMax
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:     true if successful
*/
bool isYMax()
{
  readLimits();
  if (limits && YMAX) {
    return false;
  } else return true;
}

/*
   Title:       isDeadStop
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Reads the accelerometer;returns true if not moving!
                their respective axis.
   Input:       nothing
   Returns:     true if successful

*/
bool isDeadStop()
{
  //reads the accellerometer, if not moving, return true.
  return true;
}


/*
   Title:       IsNeedleUp
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Tests for NEEDLE_UP
   Input:       nothing
   Returns:     true if needle is up
*/
bool isNeedleUp()
{
  readLimits();
  if (limits && NEEDLE_UP) {
    return false;
  } else return true;
}
/*
   Title:       IsNeedleDown
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Test for NEEDLE_DOWN
   Input:       nothing
   Returns:     true if needle is down
*/
bool isNeedleDown()
{
  readLimits();
  if (limits && NEEDLE_DOWN) {
    return false;
  } else return true;
}
/*
   Title:       SetNeedleUp
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: send needle up
   Input:       nothing
   Returns:     nothing
*/
void setNeedleUp()
{
  // while
  // !IsNeedleUp()
  // run needle motor
}
/*
   Title:       SetNeedleDown
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: send needle down.
   Input:       nothing
   Returns:     nothing
*/
void setNeedleDown()
{
  // while
  // !IsNeedleDown()
  // run needle motor
}
/*
   Title:       CycleNeedle
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Runs the Needle through one cycle.
   Input:       nothing
   Returns:     nothing
*/
void cycleNeedle()
{
  //  if needle is at NeedleUp then cycle the needle.
}



/*
   Title:       MoveXMotor OLD
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Move the X motor, command to move X-motor one step!
   Input:       param: dir = true is forward else reverse.
                param: sm = 0 is full step (400 steps/rev)
                param: sm = 1 is 1/2 step (800 steps/rev)
                param: sm = 2 is 1/4 step(1600 steps/rev)
   Returns:     nothing

void MoveXMotor(bool dir, int sm) {
  if (dir) {
    switch (sm) {
      case 0:
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x02);  // send pre-step X STEP SIGNAL DIR - FWD STEP 1/1 - 400 steps/rev
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x0A);  // send post-step
        delayMicroseconds(3);                               // step pulse width 1.9uSec (min)
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x02);  // send pre-step
        XPOS = XPOS + 4;
        break;
      case 1:
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x22);  // send pre-step X STEP SIGNAL DIR - FWD STEP 1/2 - 800 steps/rev
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x2A);  // send post-step
        delayMicroseconds(3);                               // step pulse width 1.9uSec (min)
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x22);  // send pre-step
        XPOS = XPOS + 2;
        break;
      case 2:
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x42);  // send pre-step X STEP SIGNAL DIR - FWD STEP 1/4 - 1600 steps/rev
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x4A);  // send post-step
        delayMicroseconds(3);                               // step pulse width 1.9uSec (min)
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x42);  // send pre-step
        XPOS = XPOS + 1;
        break;
    }
  } else {
    switch (sm) {
      case 0:
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x12);  // send pre-step X STEP SIGNAL DIR - REV STEP 1/1
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x1A);  // send post-step
        delayMicroseconds(3);                               // step pulse width 1.9uSec (min)
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x12);  // send pre-step
        XPOS = XPOS - 4;
        break;
      case 1:
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x32);  // send pre-step X STEP SIGNAL DIR - REV STEP 1/2
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x3A);  // send post-step
        delayMicroseconds(3);                               // step pulse width 1.9uSec (min)
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x32);  // send pre-step
        XPOS = XPOS - 2;
        break;
      case 2:
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x52);  // send pre-step X STEP SIGNAL DIR - REV STEP 1/4
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x5A);  // send post-step
        delayMicroseconds(3);                               // step pulse width 1.9uSec (min)
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x52);  // send pre-step
        XPOS = XPOS - 1;
        break;
    }
  }
}
*/




/*
   Title:       MoveYMotor
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Move the Y motor, command to move Y-motor one step!
   Input:       param: dir = true is forward else reverse.
                param: sm = 0 is full step (400 steps/rev)
                param: sm = 1 is 1/2 step (800 steps/rev)
                param: sm = 2 is 1/4 step(1600 steps/rev)
   Returns:     nothing

void MoveYMotor(bool dir, int sm) {
  if (dir) {

    switch (sm) {
      case 0:
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x04);  // send pre-step Y STEP SIGNAL DIR - FWD STEP 1/1
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x0C);  // send post-step
        delayMicroseconds(3);                               // step pulse width 1.9uSec (min)
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x04);  // send pre-step
        YPOS = YPOS + 4;
        break;
      case 1:
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x24);  // send pre-step Y STEP SIGNAL DIR - FWD STEP 1/2
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x2C);  // send post-step
        delayMicroseconds(3);                               // step pulse width 1.9uSec (min)
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x24);  // send pre-step
        YPOS = YPOS + 2;
        break;
      case 2:
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x44);  // send pre-step Y STEP SIGNAL DIR - FWD STEP 1/4
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x4C);  // send post-step
        delayMicroseconds(3);                               // step pulse width 1.9uSec (min)
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x44);  // send pre-step
        YPOS = YPOS + 1;
        break;
    }
    YPOS = YPOS + 1;
  } else {
    switch (sm) {
      case 0:
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x14);  // send pre-step Y STEP SIGNAL DIR - REV STEP 1/1
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x1C);  // send post-step
        delayMicroseconds(3);                               // step pulse width 1.9uSec (min)
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x14);  // send pre-step
        YPOS = YPOS - 4;
        break;
      case 1:
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x34);  // send pre-step Y STEP SIGNAL DIR - REV STEP 1/2
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x3C);  // send post-step
        delayMicroseconds(3);                               // step pulse width 1.9uSec (min)
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x34);  // send pre-step
        YPOS = YPOS - 2;
        break;
      case 2:
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x54);  // send pre-step Y STEP SIGNAL DIR - REV STEP 1/4
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x5C);  // send post-step
        delayMicroseconds(3);                               // step pulse width 1.9uSec (min)
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x54);  // send pre-step
        YPOS = YPOS - 1;
        break;
    }
  }
  showPosition();
  delay(MWAIT);  //wait  milliseconds for motor to move or we can check the MPU5060 later
}

   Compiler, I'll be back shortly. Just going out to clear my head. I'm going to
   the python club!! Where whitespace rules!
*/
/*
    Title:          moveMotor
    Author:         zna
    Date:           01.21.22
    Revision:       1.0.0
    Description:    move the motor. Replaces moveXMotor & moveYMotor
    Inputs:         <axis>,<steps>,<direction>
    Returns:        nothing
*/
void moveMotor(uint8_t axis, uint8_t steps, uint8_t dir)
{
  // read the axis and dir values
  // translate to correct pre and post val
  
  uint8_t setupvalue = ((axis | steps | dir) | 0b00000000);
  uint8_t cyclevalue = ((axis | steps | dir) | 0b00001000);

  mcp.writeRegister(MCP23017Register::GPIO_B, setupvalue);      // send pre-step  AXIS STEP DIR - FWD STEP 1/4
  mcp.writeRegister(MCP23017Register::GPIO_B, cyclevalue);     // send post-step
  delayMicroseconds(2);                                   // step pulse width 1.9uSec (min. as per DRV8825 specs.)
  mcp.writeRegister(MCP23017Register::GPIO_B, setupvalue);      // send pre-step
  showPosition();
}

/*
   Title:       HomeX
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Instructs the X-Motor to return to the Absolute Home position.
   Input:       nothing
   Returns:     nothing
*/
void homeX()
{

do
{
  moveMotor(ENAXN,STEP_FULL,REVERSE);
  XCPOS = XCPOS - 1;
  showPosition();
} while (!isXHome() && isNeedleUp() && XCPOS > 0);
  yield();
}
/*
   Title:       HomeY
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Instructs the Y-Motor to return to the Absolute Home position.
   Input:       nothing
   Returns:     nothing
*/
void homeY()
{

  do
  {
    moveMotor(ENAYN,STEP_FULL,REVERSE);
    YCPOS = YCPOS - 1;
    showPosition();
  } while (!isYHome() && isNeedleUp() && YCPOS > 0);
  yield();
}
/*
   Title:       HomeAll
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Instructs the X-Motor & Y-Motor to return to the Absolute Home position on
                their respective axis.
   Input:       nothing
   Returns:     nothing
*/
void homeAll()
{
  homeX();
  homeY();
}

/*
   Title:       GotoPoint
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Move the gantry to the specified position
   Input:
   Returns:
*/
void gotoPoint(int xpos, int ypos)
{

}
/*
   Title:       RunNeedleTest
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Needle Test bypasses MotionSense!
   Input:       function number
   Returns:     nothing
*/
void runNeedleTest(int m)
{
  switch (m) {
    case 0:
      if (isNeedleUp()) {
        //run motor till IsNeedleDown
        //run motor to IsNeeldeUp, then stop
      }
      break;
    case 1:
      //set motor on
      mcp.writeRegister(MCP23017Register::GPIO_B, 0x07);  // set needle motor on, set port (no xy motors on!)
      delay(1000);
      //set motor off
      mcp.writeRegister(MCP23017Register::GPIO_B, 0x06);  // set needle motor off, set port to (no xy motors on!) default state.
      break;
  }
}

/*

   Title:       calibration
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:

   confirms the success of calibration by plotting a small grid which can be measured for accuracy!
*/
void calibration()
{

}

/*

   Title:       TestCalibration
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:

   confirms the success of calibration by plotting a small grid which can be measured for accuracy!
*/
void testCalibration()
{

}
/*
    Title:          setHardHome
    Author:         zna
    Date:           01.21.22
    Revision:       1.0.0
    Description:    reset the x and y axes, sets the GLOBALS - XORG,YORG and XPOS,YPOS to 0.
    Inputs:         nothing
    Returns:        true if successful
*/
bool setHardHome()
{
  homeX();
  homeY();
  putCalibrationValues(XORG,YORG);
  return true;
}

/*
   Title:       StitcHere
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
*/
void stitchHere()
{

do
{
  /* code */
} while (isDeadStop());


}
/*
   Title:       CheckStatus
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Interrupt Handler
   Input:
   Returns:
*/
void checkStatus()
{

}

/*
   Title:       EStopMachine
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
*/
void eStopMachine()
{
  //stop all motors, preserve all values, set needle up
  setNeedleUp();
  drawBanner();
  display1.setCursor(20, 15);
  display1.print("EMERGENCY STOP!");
  display1.setCursor(20, 26);
  display1.print("MACHINE HALTED!");

  display1.setCursor(30, 40);
  display1.print("Press 'STEP'");
  display1.setCursor(30, 50);
  display1.print("to continue!");
  display1.display();
  display1.startscrollright(0x00, 0x0F);
  delay(5000);
  display1.stopscroll();
  delay(500);
  display1.startscrollleft(0x00, 0x0F);
  delay(5000);
  display1.stopscroll();
}






/* ------------------------------- HARDWARE -------------------------------------------------- */

/*

   Title:       ScanI2CBus
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
  ScanI2CBus is implemented - 9/21/2020
  this has to be done before init sensor group sets the sensor availability so the system can ignore offline sensors
  to manage performance and no bad data woes.  The i2c_scanner uses the return value of the Write.endTransmisstion to
  see if a device did acknowledge to the address.
*/
void scanI2CBus()
{
  int lastRow = 50;
  byte error, address;
  int nDevices;
  Wire.begin();
  drawBanner();
  display1.setCursor(10, 15);
  display1.print("I2C Bus Scan....");
  display1.display();

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      drawBanner();
      display1.setCursor(10, 25);
      display1.print("I2C at 0x");
      if (address < 0x10) {
        display1.print("0");
      }
      display1.print(address, HEX);
      display1.print(" hex!");
      display1.display();

      if (address == 0x20) {
        display1.setCursor(10, 35);
        display1.print("Port - Motors");
        display1.display();
        MOTORS_ACTIVE = true;
      }
      else if (address == 0x21) {
        display1.setCursor(10, 35);
        display1.print("Port - Aux");
        display1.display();
        AUX_ACTIVE = true;
      }
      else if (address == 0x3C) {
        display1.setCursor(10, 35);
        display1.print("OLED - Me!");
        display1.display();
      }
      else if (address == 0x68) {
        display1.setCursor(10, 35);
        display1.print("MPU6050 Accel");
        display1.display();
        MPU6050_ACTIVE = true;
      }
      nDevices++;
      delay(500);
    } else if (error == 4) {
      display1.setCursor(5, lastRow);
      display1.print("Error at address 0x");
      if (address < 16) {
        display1.print("0");
        display1.println(address, HEX);
        display1.display();
      }
    }
  }
  if (nDevices == 0) {
    drawBanner();
    display1.setCursor(10, lastRow);
    display1.print("No I2C devices found");
    display1.display();
  } else {
    display1.setCursor(10, lastRow);
    display1.print("That's all. Done.");
    display1.display();
  }
}




/*
   This initialize handles most of the devices that is available. If it finds a sensor it attempts to initialize or if the
   sensor is not attached it will flag the unit so the system will ignore it.
*/
/*
   Title:       InitializeSensorGroup
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
*/
void initializeSensorGroup() {
  drawBanner();
  display1.setCursor(34, 10);
  display1.print("-STITCH Sensors-");
  display1.display();

  if (!mpu.begin()) {
    while (1)
      yield();
  }
  MPU6050_READY = true;
  // accelerometer test results.
  if (MPU6050_READY) {
    MPU6050_ACTIVE = true;
  } else {
    MPU6050_ACTIVE = false;
  }
  delay(1000);
  Wire.setClock(400000);
  display1.setCursor(30, 30);
  display1.print("-Registered-");
  display1.display();
  delay(2000);
}
/*
   Title:       
   Author:      zna
   Date:        05-18-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
*/
#endif
