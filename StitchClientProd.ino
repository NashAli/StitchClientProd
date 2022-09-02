/*
  Created:      19:55 2021.11.2
  Last Updated: 04:29 2022.01.20
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

  DRV8825 - stepper motor driver PCB

  12:13   12/10/2021
  759,826 bytes Program
  045,036 bytes Ram (14%)

  16:50   12/12/2021  - running!
  773,494 bytes  Program
  045,180 bytes RAM (13%)

  19:21   12/16/2021  - added BlueTooth serial support.
  1,471,158 bytes  Program (~46%) (Max 3,145,728 bytes)
  058,988 bytes RAM (18%)

  17:31   01/25/2022  - updates
  1,528,366 bytes  Program (~48%) (Max 3,145,728 bytes)
  054,824 bytes RAM (16%)
*/

//  DO NOT ALTER THE ORDER OF THESE INCLUDES
#include <Wire.h>
#include <WiFi.h>                 //  Wifi
#include "esp_event.h"
#include <WiFiClient.h>           //  HTTP
#include <WiFiAP.h>               //  AP
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <EEPROM.h>               //  manage the auto-l
#include "BluetoothSerial.h"      //  BT for login
#include <NTPClient.h>            //  time server
#include <time.h>                 //  standard time stuff

#include "system.h"               //  basic
#include "display_support.h"      //  oled
#include "support.h"              //  general support routines and functions for eMB-OS.
#include "SDC_Support.h"          //  goodies for the SD Card
#include "comms.h"                //  all other network
#include "Telnet_Support.h"       //  Telnet


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);     //  for testing & debug
  initializeDisplay();
  initializeSDCard(SDCardSelect);
  configNetwork();
  pinMode(LAMP, OUTPUT);           // set pin to output
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ML_INTA, INPUT_PULLUP);
  scanI2CBus();
  //initializeSensorGroup();
  initializeMotorsPort();
  initializeControllerPort();
  initializeSensorGroup();
  setupTelnet();
  String logthis = ACyan + "System started@:" + AWhite + getASCIITime();
  logToSD(logthis);
  showFreeMemory();
  if (getCalibrationValues()){
    //HomeAll
    homeAll();
  }
}

void loop() { 
  touchValue = touchRead(EstopPin);
  if (touchValue < touch_threshold) {
    eStopMachine();
  }
  telnet.loop();
}
