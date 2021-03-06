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
  3 - 1/8   (not used)
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
/* -------------------------- MCP23017 INIT ------------------------------------------------------- */
/*
   Dear Complier, Error Messages, please let's have none of those.
   Please don't ignore me, I think you're ignoring me! I've seen you here before!
*/
void errorMsg(String error, bool restart = true) {
  if (restart) {
    DrawBanner();
    display1.setCursor(5, 15);
    display1.print(error);
    display1.display();
    delay(2000);
    ESP.restart();
    delay(2000);
  }
}
/*
  ************************************  Logic for the work light.
*/
void Lamp(int v) {
  if (v == 0) {
    digitalWrite(WorkLights, LOW);
  } else {
    digitalWrite(WorkLights, HIGH);       // set pin high
  }
}
/*
   *************************************  TIMESTAMP for logs.
*/
String GetASCIITime() {
  time_t rawtime;
  struct tm *info;
  time( &rawtime );
  info = localtime( &rawtime );
  String mt = asctime(info);
  mt.replace("\n", "");
  return mt;
}
/*
 * ****************************************** Init i2c Motors.
 */
void InitMotorsPort() {
  if (MOTORS_ACTIVE) {
    mcp.init();
    mcp.portMode(MCP23017Port::A, 0xFF);  //Port A as inputs
    mcp.portMode(MCP23017Port::B, 0);     //Port B as outputs
    mcp.writeRegister(MCP23017Register::GPIO_A, 0x00);  //Reset port A
    mcp.writeRegister(MCP23017Register::GPIO_B, 0x06);  //Reset port B to default value()
    //set the limits interrupt to GPIO34
    DrawBanner();
    display1.setCursor(20, 20);
    display1.print("Motors in!");
    display1.display();
    delay(500);
  } else {
    DrawBanner();
    display1.setCursor(20, 20);
    display1.print("No Motors!");
    display1.display();
    delay(500);
  }
}
/*
 * ********************************* Init Remote controller.
 */
void InitControllerPort() {
  if (AUX_ACTIVE) {
    acp.init();
    acp.portMode(MCP23017Port::A, 0xFF);  //Port A as in
    acp.portMode(MCP23017Port::B, 0);           //Port B as out
    acp.writeRegister(MCP23017Register::GPIO_A, 0x00);  //Reset port A
    acp.writeRegister(MCP23017Register::GPIO_B, 0x00);  //Reset port B
    //set the control interrupt to GPIO35
    DrawBanner();
    display1.print("Remote Control in");
    display1.display();
    delay(500);
  } else {
    DrawBanner();
    display1.setCursor(25, 20);
    display1.print("No Remote");
    display1.display();
    delay(500);
  }
}

void SetPortPin(int pn, bool v) {

}

bool GetPortPin(int pn) {
  int comval = 0;
  comval = mcp.readPort(MCP23017Port::A);

}

void SetPort(int pn, int v) {
  if (pn == 1) {
    mcp.writeRegister(MCP23017Register::GPIO_A, v);//  Set port A
  }
  else {
    mcp.writeRegister(MCP23017Register::GPIO_B, v);  //  Set port B
  }
}
/*
   Returns with the value of the port as a hexadecimal string.
*/
String GetPort(int pn) {
  int comval;
  if (pn == 1) {
    comval = mcp.readPort(MCP23017Port::A);
  } else {
    comval = mcp.readPort(MCP23017Port::B);
  }
  return IntToHexStr(comval);
}



/*
   Dear compiler... Am I there yet? Please go easy on me today, my morning has been rough.
*/
bool IsAtDestination() {
  if ((XPOS = XDEST) && (YPOS = YDEST)) {
    return true;
  } else {
    return false;
  }

}

/*
   Read the limits port A of the MCP23017 Motors control. An interrupt can set the flag to read all limits
   
*/
void ReadLimits() {
  limits = mcp.readPort(MCP23017Port::A);
}
/*
   Read the controller port A of the MCP23017 onboard the remote link
*/
void ReadController() {
  comval = acp.readPort(MCP23017Port::A);
}
/*

*/
void SetController() {

}
/*
   Reads the active LOW Needle Up position sensor
*/
bool IsNeedleUp() {
  ReadLimits();
  if (limits & NEEDLE_UP) {
    return false;
  } else return true;
}
/*
   Reads the active LOW Needle Down position sensor
*/
bool IsNeedleDown() {
  ReadLimits();
  if (limits & NEEDLE_DOWN) {
    return false;
  } else return true;
}
/*
   Command to set the needle to the up position.
*/
void SetNeedleUp() {
  // while
  // !IsNeedleUp()
  // run needle motor
}
/*
   Command to set the needle to the down position.
*/
void SetNeedleDown() {
  // while
  // !IsNeedleDown()
  // run needle motor
}
/*
   Command to full cycle of the needle from the Needle up position.
*/
void CycleNeedle() {
  //  if needle is at NeedleUp then cycle the needle.
}

/*
   command to move X-motor one step!
   param: dir = true is forward else reverse.
   param: sm = 0 is full step (400 steps/rev)
   param: sm = 1 is 1/2 step (800 steps/rev)
   param: sm = 2 is 1/4 step(1600 steps/rev)
*/
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
  ShowPosition();
  delay(MWAIT); //wait  milliseconds for the motor to move
}

/*
   command to move Y-motor one step!
   param: dir = true is forward else reverse.
   param: sm = 0 is full step (400 steps/rev)
   param: sm = 1 is 1/2 step (800 steps/rev)
   param: sm = 2 is 1/4 step(1600 steps/rev)
*/
void MoveYMotor(bool dir, int sm) {
  if (dir) {

    switch (sm) {
      case 0:
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x04);  // send pre-step X STEP SIGNAL DIR - FWD STEP 1/1
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x0C);  // send post-step
        delayMicroseconds(3);                               // step pulse width 1.9uSec (min)
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x04);  // send pre-step
        YPOS = YPOS + 4;
        break;
      case 1:
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x24);  // send pre-step X STEP SIGNAL DIR - FWD STEP 1/2
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x2C);  // send post-step
        delayMicroseconds(3);                               // step pulse width 1.9uSec (min)
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x24);  // send pre-step
        YPOS = YPOS + 2;
        break;
      case 2:
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x44);  // send pre-step X STEP SIGNAL DIR - FWD STEP 1/4
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
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x14);  // send pre-step X STEP SIGNAL DIR - REV STEP 1/1
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x1C);  // send post-step
        delayMicroseconds(3);                               // step pulse width 1.9uSec (min)
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x14);  // send pre-step
        YPOS = YPOS - 4;
        break;
      case 1:
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x34);  // send pre-step X STEP SIGNAL DIR - REV STEP 1/2
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x3C);  // send post-step
        delayMicroseconds(3);                               // step pulse width 1.9uSec (min)
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x34);  // send pre-step
        YPOS = YPOS - 2;
        break;
      case 2:
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x54);  // send pre-step X STEP SIGNAL DIR - REV STEP 1/4
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x5C);  // send post-step
        delayMicroseconds(3);                               // step pulse width 1.9uSec (min)
        mcp.writeRegister(MCP23017Register::GPIO_B, 0x54);  // send pre-step
        YPOS = YPOS - 1;
        break;
    }
  }
  ShowPosition();
  delay(MWAIT);  //wait  milliseconds for motor to move or we can check the MPU5060 later
}
/*
   Compiler, I'll be back shortly. Just going out to clear my head. I'm going to
   the python club!! Where whitespace rules!
*/

/*
   checks if the head is at the X axis home position.
*/
bool IsXHome() {
  ReadLimits();
  if (limits & XHOME) {
    return false;
  } else return true;
}
/*
   checks if the head is at the X axis max position.
*/
bool IsXMax() {
  ReadLimits();
  if (limits & XMAX) {
    return false;
  } else return true;
}
/*
   checks if the head is at the Y axis home position.
*/
bool IsYHome() {
  ReadLimits();
  if (limits & YHOME) {
    return false;
  } else return true;
}
/*
   checks if the head is at the Y axis max position.
*/
bool IsYMax() {
  ReadLimits();
  if (limits & YMAX) {
    return false;
  } else return true;
}
/*
   command to send head to the y-axis home position
*/
void HomeX() {
  while (!IsXHome & NeedleUp) {
    MoveXMotor(!Forward, 1);
  }

}
/*
   command to send head to the x-axis home position
*/
void HomeY() {
  while (!IsYHome & NeedleUp) {
    MoveYMotor(!Forward, 1);
  }
}
/*
   command to home X & Y axii
*/
void HomeAll() {
  HomeX();
  HomeY();
}
/*
   Reads the accelerometer;returns true if not moving!
*/
bool IsDeadStop() {
  return true;
}

/*

*/
void GotoPoint(int xpos, int ypos) {

}
/*
  Needle Test bypasses MotionSense!
*/
void RunNeedleTest(int m) {
  switch (m) {
    case 0:
      if (IsNeedleUp) {
        //run motor till IsNeedleDown
        //run motor to IsNeeldeUp, then stop
      }
      break;
    case 1:
      //set motor on
      mcp.writeRegister(MCP23017Register::GPIO_B, 0x07);  // set needle motor on, set port to default(no xy motors on!)
      delay(1000);
      //set motor off
      mcp.writeRegister(MCP23017Register::GPIO_B, 0x06);  // set needle motor off, set port to default
      break;
  }
}
/*
   confirms the success of calibration by plotting a small grid which can be measured for accuracy!
*/
void TestCalibration() {

}


/*

*/
void StitchHere() {
  while (IsDeadStop) {

  }

}
/*
 * An interrupt has been fired and a LIMITS_FLAG has been set
 */
void CheckStatus() {

}


void EStopMachine() {
  //stop all motors, preserve all values, set needle up
  DrawBanner();
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
  ScanI2CBus is implemented - 9/21/2020
  this has to be done before init sensor group sets the sensor availability so the system can ignore offline sensors
  to manage performance and no bad data woes.  The i2c_scanner uses the return value of the Write.endTransmisstion to
  see if a device did acknowledge to the address.
*/
void ScanI2CBus() {
  int lastRow = 50;
  byte error, address;
  int nDevices;
  Wire.begin();
  DrawBanner();
  display1.setCursor(10, 15);
  display1.print("I2C Bus Scan....");
  display1.display();

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      DrawBanner();
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
    DrawBanner();
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
  *************************************** INTERRUPT HANDLER *****************************
*/
void IRAM_ATTR CheckLimits() {
  limits = mcp.readPort(MCP23017Port::A);
  // set a flag for OS, new limits is available and needs attention quickly.
  LIMITS_FLAG = true;
}
/*
  
*/
void InitGPIOInterrupt() {
  pinMode(LimitsIRQ, INPUT_PULLUP);
  //attachInterrupt(LimitsIRQ, CheckLimits, FALLING);
  attachInterrupt(digitalPinToInterrupt(LimitsIRQ), CheckLimits, FALLING);
}

/*
   This initialize handles most of the devices that is available. If it finds a sensor it attempts to initialize or if the
   sensor is not attached it will flag the unit so the system will ignore it.
*/
void InitializeSensorGroup() {
  DrawBanner();
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

#endif
