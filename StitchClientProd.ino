/*
  Created:      19:55 2021.11.2
  Last Updated: 14:45 2021.11.25
  MIT License

  Copyright (c) 2021 Zulfikar Naushad Ali
  This is NOT Free software!
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


#include <Wire.h>
#include <WiFi.h>                 //  Wifi
#include <WiFiClient.h>           //  HTTP
#include <WiFiAP.h>               //  AP
//#include "libssh_esp32.h"       //  SSH
#include <NTPClient.h>            //  time server
#include <time.h>                 //  standard time stuff
#include "system.h"               //  basic
#include "display_support.h"      //  oled
#include "support.h"              //  general support routines and functions for eMB-OS.
#include "SDC_Support.h"          //  goodies for the SD Card
#include "Telnet_Support.h"       //  Telnet
#include "FTP_Support.h"          //  FTP



void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  InitDisplay();
  ScanI2CBus();
  InitializeSDCard(SDCardSelect);
  InitMotorsPort();
  InitControllerPort();
  //ScanNetworks();
  JoinLocalNetwork();
  InitAP();
  SetupTelnet();
  InitFTP();
  //StartSSH();
  ShowTime();
  //LogToSD(TimeStamp);
}

void loop() {
  CheckStatus();
  touchValue = touchRead(EstopPin);
  if (touchValue < touch_threshold) {
    EStopMachine();
  }
  telnet.loop();
  //CheckForHTTPRequest();
}
