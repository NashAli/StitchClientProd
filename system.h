#ifndef _SYSTEM_H
#define _SYSTEM_H
/*
  Created:      19:55 2021.12.07
  Last Updated: 14:45 2021.12.07
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

  I2C Device Listing
  0x20 - MCP23017 Port Expander - Stepper Motor Drivers
  0x21 - MCP23017 - Aux Controller
  0x3C - OLED - used
  0x68 - MPU6050 - Head motion detect.

  DRV8825 - stepper motor driver PCB


*/


//***********************************************************************
//  DEFINES
//***********************************************************************
#define OFF 0
#define ON 1
#define MotorControl 0x20
#define AuxControl 0x21
#define Accel 0x68
#define LimitsIRQ 34
#define AuxControlIRQ 35
#define LED_BUILTIN 2   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

// Define MCP23017 registers values from datasheet
#define IODIRA    0x00  // IO direction A - 1= input 0 = output - set as all INPUT 0xFF 
#define IODIRB    0x01  // IO direction B - 1= input 0 = output - set as all OUTPUT 0x00
#define IPOLA     0x02  // Input polarity A
#define IPOLB     0x03  // Input polarity B
#define GPINTENA  0x04  // Interrupt-onchange A
#define GPINTENB  0x05  // Interrupt-onchange B
#define DEFVALA   0x06  // Default value for port A
#define DEFVALB   0x07  // Default value for port B
#define INTCONA   0x08  // Interrupt control register for port A
#define INTCONB   0x09  // Interrupt control register for port B
//#define IOCON     0x0A  // Configuration register //conflict with library macro
#define GPPUA     0x0C  // Pull-up resistors for port A
#define GPPUB     0x0D  // Pull-up resistors for port B
#define INTFA     0x0E  // Interrupt condition for port A
#define INTFB     0x0F  // Interrupt condition for port B
#define INTCAPA   0x10  // Interrupt capture for port A
#define INTCAPB   0x11  // Interrupt capture for port B
#define GPIOA     0x12  // Data port A
#define GPIOB     0x13  // Data port B
#define OLATA     0x14  // Output latches A - all limit sw. - X,Y,Needle Position
#define OLATB     0x15  // Output latches B  - stepper motor controllers
//MCP23017 - MOTOR_CONTROLLER @ 0x20
//NOTE: INPUTS MASK (are all active low signals!)
#define NEEDLE_UP   0b10000000
#define NEEDLE_DOWN 0b01000000
#define XHOME       0b00100000
#define XMAX        0b00010000
#define YHOME       0b00001000
#define YMAX        0b00000100
//OUTPUTS MASKS
#define NEEDLE_DRV  0b00000001  //  active High
#define ENAXN       0b00000010  //  active low
#define ENAYN       0b00000100  //  active low
#define STEP        0b00001000  //  rising edge
#define DIR         0b00010000
#define MODE        0b11100000

#define STEP_FULL           0b00000000
#define STEP_ONE_HALF       0b00100000
#define STEP_ONE_QUARTER    0b01000000
#define STEP_ONE_EIGHT      0b01100000
#define STEP_ONE_SIX        0b10000000
#define STEP_ONE_THIRTY_TWO 0b10100000

#define FOREWARD  0b1
#define BACKWARD  0b0

#define STEPPERREV  400       //  set the steps per revolution of the motor for 1:1 mode

// MCP23017 - AUXILLARY_CONTROLLER @ 0x21

//  Status and Data from sensors initial values
bool MPU6050_READY = false;
bool MPU6050_ACTIVE = false;
bool MOTORS_READY = false;
bool MOTORS_ACTIVE = false;
bool AUX_READY = false;
bool AUX_ACTIVE = false;


//  MACHINE GLOBAL VARIABLES  ********************************************************

String OSNAME = "eMB-OS V1.1";
String TimeStamp = "";
int XORG, YORG, XPOS, YPOS, XDEST, YDEST; // start,current,dest coordinates
bool SYSTEM_BUSY = false;
bool MachineRun = false;
bool Verbosity = true;
bool NeedleUp = false;
bool NeedleDown = false;
bool Forward = true;          //  direction logic for DRV8845
bool CalGood = false;         //  calibration has been completed sucessfully.


uint8_t limits, comval;
const int EstopPin = 4; //GPIO4 - EMERGENCY STOP PIN !!
const int SDCardSelect = 5;
const int WorkLights = 34;  //  GPIO34 - work lights via the ULN2803A (IC-3 BOM)
int MWAIT = 10;    //  motor wait timing
// change your threshold value here
const int touch_threshold = 20;
// variable for storing the touch pin value
int touchValue;
const int httpPort = 80;
const int serverPort = 4080;
const char* ssidAP = "STITCH";
const char* appwd = "sewpatch";
char* ssid     = "ETMS";      //  add your own credentials.
char* password = "dodobird";  //  here.
const char* ntpServer = "ca.pool.ntp.org";
const int LOCAL_TIME_OFFSET = -5;
const long utcOffsetInSeconds = 3600 * LOCAL_TIME_OFFSET;

IPAddress ip;
WiFiServer server(httpPort);  //  setup the http server
WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

#endif
