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


//***********************************************************************
//  DEFINES
//***********************************************************************
#define OFF 0
#define ON 1
#define MotorControl 0x20
#define AuxControl 0x21
#define Accel 0x68
#define ML_INTA 32
#define MVOLT_PIN 35
#define LED_BUILTIN 2   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED
#define LAMP 34 //  GPIO34 - work lights via the ULN2803A pin 1.

/*
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
//#define IOCON     0x0A  // Configuration register //conflict with library macro??
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
*/
//MCP23017 - MOTOR_CONTROLLER @ 0x20
//NOTE: INPUTS MASK (are all active low signals!(for now))
#define NEEDLE_UP   0b10000000
#define NEEDLE_DOWN 0b01000000
#define XHOME       0b00100000
#define XMAX        0b00010000
#define YHOME       0b00001000
#define YMAX        0b00000100
//OUTPUTS MASKS
#define NEEDLE_DRV  0b00000001  //  active High
#define ENAYN       0b00000010  //  active low
#define ENAXN       0b00000100  //  active low
#define STEP        0b00001000  //  rising edge
#define DIR         0b00010000
#define MODE        0b11100000

#define STEP_FULL           0b00000000
#define STEP_ONE_HALF       0b00100000
#define STEP_ONE_QUARTER    0b01000000
#define STEP_ONE_EIGHT      0b01100000
#define STEP_ONE_SIX        0b10000000
#define STEP_ONE_THIRTY_TWO 0b10100000

#define FORWARD  0b1
#define REVERSE  0b0

#define STEPPERREV  400       //  set the steps per revolution of the motor for 1:1 mode

/*
   ANSI Terminal ESC sequences
*/
String ADarkGrey =      "\u001b[1;30m";
String ABlack =         "\u001b[30m";
String ARed =           "\u001b[31m";
String AGreen =         "\u001b[32m";
String AYellow =        "\u001b[33m";
String ABlue =          "\u001b[34m";
String AMagenta =       "\u001b[35m";
String ACyan =          "\u001b[36m";
String AWhite =         "\u001b[37m";
String AReset =         "\u001b[0m";
String ABrightBlack =   "\u001b[30;1m";
String ABrightRed =     "\u001b[31;1m";
String ABrightGreen =   "\u001b[32;1m";
String ABrightYellow =  "\u001b[33;1m";
String ABrightBlue =    "\u001b[34;1m";
String ABrightMagenta = "\u001b[35;1m";
String ABrightCyan =    "\u001b[36;1m";
String ABrightWhite =   "\u001b[37;1m";

String ABold =          "\u001b[1m";
String AClearScreen =   "\u001b[2J";
String AHomeCursor =    "\u001b[;H";
String ASlowBlink =     "\u001b[5m";
String AReverse =       "\u001b[7m";
String AUnderline =     "\u001b[4m";
String AUpaline =       "\u001b[1A";

String tab =            "\u0009";
String nl =             "\n";
String cr =             "\r";


// unicode special symbols

String revso_sym =      "\u005c";
String cr_sym =         "\u00A9";
String pat_sym =        "\u1360";
String tm_sym =         "\u2122";
String anchor_sym =     "\u2693";
String ard_sym =        "\u267E";
String sball_sym =      "\u26BD";
String bball_sym =      "\u26BE";
String sman_sym =       "\u26C4";
String psun_sym =       "\u26C5";
String golf_sym =       "\u26F3";
String sboat_sym =      "\u26F5";
String tent_sym =       "\u26FA";
String stars_sym =      "\u2728";

/*
  ORG - starting positions
  POS - current positions
  DEST - destination positions
  OFFSET -  relative offset coordinates
  once POS gets to DEST from ORG then DEST becomes the new ORG after the position move
  Offsets are values relative to fabric position and soft-limits so a home point can be
  assigned on the work surface.
  SYSTEM_BUSY - the highest priority tasks are running when true.
  MachineRun - true while a move and cycle operation is in effect.
  Verbosity - sets the level of verbosness from numeric code to alpha strings.
  NeedleUp  - checks the logic for Up condition.
  NeedleDown  - Down condition.
  Forward - motor step dirction.
  CalGood - if the system hommed X and Y successfully. Certain test operations will cause
  the calibrations to be off or just may require re-calibration.
*/
//  MACHINE GLOBAL VARIABLES  ********************************************************

String OSNAME = "eMB-OS";
String _version_0 = AClearScreen + AHomeCursor;
String _version_1 = ABrightCyan + ABold + tab + golf_sym + ABrightRed + ABold + pat_sym + ASlowBlink + OSNAME + AReset;
String _version_2 = ABrightYellow + ABold + tm_sym + AReset + ABrightGreen + "   V1.0.2" + AReset;
String _version_3 = tab + ABrightYellow + cr_sym + ABrightCyan + "Graffiti Softwerks" + ABrightGreen + " All Rights Reserved 2021/2022";
String _commandhelptitle = tab + tab + tab + ABrightBlue + "COMMAND HELP";


/*
SubCommandLevel
0-System,1-Machine,2-FileSystem,3-Misc.


*/

int SubCommandLevel = 0;
bool HELP_SYS = false;
String commDesc[25];
String commExample[25];
String commExtra[25];
//  Status and Data from sensors initialization values
bool MPU6050_READY = false;
bool MPU6050_ACTIVE = false;
bool MOTORS_READY = false;
bool MOTORS_ACTIVE = false;
bool AUX_READY = false;
bool AUX_ACTIVE = false;
bool ABORT_TEST = false;
int XORG, YORG, XCPOS, YCPOS, XDEST, YDEST, XOFFSET, YOFFSET, SXHOME, SYHOME;
bool SYSTEM_BUSY = false;     //  OS is busy.
bool MachineRun = false;      //  a motor is cycling (X-Y-Z).
bool isSubCommand = false;    //  active sub command.
bool isProcessing = false;    // check for command processor BUSY.
bool LIMITS_FLAG = false;     //  a boundary has been breached.
bool CalGood = false;         //  calibration has been completed sucessfully.
// MCP23017 - AUXILLARY_CONTROLLER @ 0x21
bool Verbosity = true;        //  setting for communications protocol. Application can control response type ALPHA/NUMERIC

uint8_t limits, comval;       //  read mcp23017 ports' values.

//  ESP32 PINS
const int EstopPin = 4;       //  GPIO4 - EMERGENCY STOP PIN !!
const int SDCardSelect = 5;   //  GPIO5 on esp32
int MWAIT = 10;               //  motor wait timing

// change your threshold value here
const int touch_threshold = 20;
// variable for storing the touch pin temporary value
int touchValue;
// some network specific parameters.
const int httpPort = 80;
const int telnetPort = 23;
const int serverPort = 4080;
const char* ssidAP = "STITCH";
const char* appwd = "sewpatch";
const char* ntpServer = "ca.pool.ntp.org";
const int LOCAL_TIME_OFFSET = -5;
const long utcOffsetInSeconds = 3600 * LOCAL_TIME_OFFSET;
// my watchdog.
unsigned long startMillis;
unsigned long currentMillis;

// bluetooth vars
String buffer_in;
unsigned long previousMillis = 0;
byte valu;
int addr = 0;
String stream;
String temp;
String temp2;
unsigned int interval = 30000;

BluetoothSerial bt; //Object for Bluetooth
IPAddress ip;
AsyncWebServer httpServer(80);
WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


struct Position  
{
  uint16_t X;
  uint16_t Y;
};
Position pos;

enum class SYSTEM_STATUS : byte
{
	Ready = 0,
	MachineBusy = 1,
	CommsBusy = 2,
	SDBusy = 3
};


/*
   Title:       Lamp
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Turns on/off the work lamp.
   Input:       <on:off>
   Returns:     nothing
*/
void lamp(int v)
{
  if (v == 0) {
    digitalWrite(LAMP, LOW);
  } else {
    digitalWrite(LAMP, HIGH);       // set pin high
  }
}
/*
   Title:       GetASCIITime
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Gets the time and converts to a string.
   Input:       nothing
   Returns:     time in ascii string format.
*/
String getASCIITime()
{
  time_t rawtime;
  struct tm *info;
  time( &rawtime );
  info = localtime( &rawtime );
  String mt = asctime(info);
  mt.replace("\n", "");
  return mt;
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
String getMotorVoltage()
{
  uint16_t vbat = analogRead(MVOLT_PIN);
  float battery_voltage = ((float)vbat / 4095.0) * 3.3 * (1184 / 1000.0);
  String voltage =  String(battery_voltage);
  return voltage;
}

/*


   Title:       converts a String to Integer value, m is the base. ie. 16 = hexadecimal
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
*/
int stringToInteger(String cpv, int m)
{
  int iv;
  return iv = strtoul(cpv.c_str(), 0, m);
}
/*

   Title:       Convert integer to hexadecimal string
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
*/
String integerToHexString(int v)
{
  char hex_string[20];
  sprintf(hex_string, "%2X", v); //convert number to hex
  return hex_string;
}
/*
   Title:       LoadHelp
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Loads the help system arrays
   Input:       none
   Returns:     HelpArray
*/
void loadHelp()
{

}
/*
   Title:       UnloadHelp
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Unloads the help system arrays to free up memory.
   Input:       none
   Returns:     unloads the HelpArray
*/
void unloadHelp()
{

}
#endif
