#ifndef _TEL_S_H
#define _TEL_S_H
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
  ULN2803 buffr driver
  DRV8825 - stepper motor driver PCB

  Modes:
  0 - 1/1
  1 - 1/2
  2 - 1/4
  3 - 1/8   (not used)
  4 - 1/16  (not used)
  5 - 1/32  (not used)



  This unit's primary role is to provide direct builder diagnostics to the machine
  from the PC utilizing Putty to communicate.

  Available System Commands:

  netscan
  status
  logs
  run
  files
  jobs
  cancel
  selftest
  calibrate

  Machine Controls:

  W - goto coords and wait
  G - goto coords and cycle needle
  T - thread change
  C - continue, after programmed hold
  M - open macro
  V - response verbosity setting

  Subject to change, without notice!
*/
#include "ESPTelnet.h"            //  Telnet

ESPTelnet telnet;       //  setup telnet


/*
   SYSTEM ***************************************************************************************************
   rr - response required if '1'
   rt - 0 "OK", 1 "error", 2 "busy", 3 "Access Denied", 4 "Not Found", 5 "unknown".
   pt - prompt type (symbol) 0 - 4. none,top,machine,file,misc.
*/
void responsePrompt(int rr, int rt, int ptr) {

  //  outcomes for system response. eg. OK, error, busy, Access denied!, Not Found!
  if (rr == 1) {
    switch (rt) {
      case 0:
        if (Verbosity) {
          telnet.println(ABrightGreen + "OK");
        } else {
          telnet.println("0");
        }
        break;
      case 1:
        if (Verbosity) {
          telnet.println(ABrightRed + ASlowBlink + "error" + AReset);
        } else {
          telnet.println("1");
        }
        break;
      case 2:
        if (Verbosity) {
          telnet.println(ABrightBlue + "busy" + AReset);
        } else {
          telnet.println("2");
        }
        break;
      case 3:
        if (Verbosity) {
          telnet.println(ABrightMagenta  + ASlowBlink + "Access Denied!" + AReset);
        } else {
          telnet.println("3");
        }
        break;
      case 4:
        if (Verbosity) {
          telnet.println(ABrightYellow + "Not Found!" + AReset);
        } else {
          telnet.println("4");
        }
        break;
      default:
        if (Verbosity) {
          telnet.println(ABrightYellow + "unknown" + AReset);
        } else {
          telnet.println("5");
        }
        break;
    }
  }
  switch (ptr) {
    case 0:
      telnet.println(AReset);                     //  no prompt
      break;
    case 1:
      telnet.print(ABrightYellow + "$" + AReset); //  main prompt - top level
      break;
    case 2:
      telnet.print(ABrightCyan + "~" + AReset);   //  machine related
      break;
    case 3:
      telnet.print(ABrightRed + ">" + AReset);    //  used by filesystem
      break;
    case 4:
      telnet.print(ABrightYellow + "-" + AReset); //  misc.
      break;
    default:
      // if nothing else matches, do this
      telnet.print(ABrightYellow + "$" + AReset); //  default prompt
      break;
  }
}
//    NETWORK *********************************************************************************************************
/*
  - needs work !!!
  now if you are connected via telnet via the AP then the "No networks found" might make sense
  but on the other hand, if connected via a Router then No Networks found, doesn't get there!!!
*/
void TelNetScan() {
  responsePrompt(0, 0, 4); //subsystem prompt
  telnet.println( ACyan + "Scanning the Local Area Network..." + AReset);
  WiFi.scanDelete();
  int nn = WiFi.scanNetworks();
  responsePrompt(0, 0, 4); //subsystem prompt
  telnet.println( ABrightWhite + "Currently there are " + ABrightGreen + String(nn) + ABrightWhite + " networks in the neighbourhood" + AReset);

  if (nn == 0) {
    //no networks found go AP
    telnet.println(ABrightRed + "No Local Networks Found? - Connect to AP address: 192.168.5.1");
    telnet.println(ABrightGreen + "Open your wifi settings and find the address above." + AReset);
    logToSD(AWhite + getASCIITime() + ABrightRed + " --- no networks found.");
  } else {
    telnet.println(ABrightWhite + "List of available wifi connections:" + AReset);
    for (int i = 0; i < nn; ++i) {
      // Print SSID and RSSI for each network found
      String ss = String( WiFi.RSSI(i));
      String sc = String(i + 1);
      String sn = String(WiFi.SSID(i));
      responsePrompt(0, 0, 3); //system prompt
      if (i < 9) {
        telnet.print("0");
      }
      telnet.print(sc);
      telnet.print(ABrightBlue + ":" + AReset);
      if (sn == " " || sn == "") {
        sn = "unknown";
        telnet.print(ABrightYellow + sn + AReset);
      } else {
        telnet.print(ABrightCyan + sn + AReset);
      }
      telnet.print(ABrightYellow + "(" + AReset);
      telnet.print(ABrightBlue + ss);
      telnet.print(ABrightYellow + ")" + AReset);
      if (sn.length() < 7 ) {
        telnet.print(tab);
      }
      if (sn.length() < 15 ) {
        telnet.print(tab);
      }
      if (sn.length() < 23 ) {
        telnet.print(tab);
      }
      if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) {
        telnet.println(ABrightGreen + "Open");
      }
      if (WiFi.encryptionType(i) == WIFI_AUTH_WEP) {
        telnet.println(AMagenta + "WEP");
      }
      if (WiFi.encryptionType(i) == WIFI_AUTH_WPA_PSK) {
        telnet.println(ARed + "WPA-PSK");
      }
      if (WiFi.encryptionType(i) == WIFI_AUTH_WPA2_PSK) {
        telnet.println(ABrightRed + "WPA2-PSK");
      }
      if (WiFi.encryptionType(i) == WIFI_AUTH_WPA_WPA2_PSK) {
        telnet.println(ARed + "WPA-WPA2-PSK");
      }
      if (WiFi.encryptionType(i) == WIFI_AUTH_WPA2_ENTERPRISE) {
        telnet.println(ABrightMagenta + "WPA2-ENTERPRISE");
      }
    }
  }
  telnet.println("");
  responsePrompt(1, 0, 1); //OK,system prompt
}
/*
   Displays system & network status.
*/

void TelSysStat() {
  responsePrompt(0, 0, 2); //subsystem prompt
  telnet.println(ABrightCyan + "Network Status:" + AReset);
  telnet.println(ABrightYellow + "Current Local Time :     " + ABrightWhite + getASCIITime());
  telnet.println(ABrightBlue +   "Machine Local IP:        " + ABrightYellow + WiFi.localIP().toString() + AReset);
  telnet.println(ABrightGreen +  "Connected to IP Address: " + ABrightRed + telnet.getIP());
  telnet.println(ABrightGreen +  "Gateway IP Address:      " + ABrightCyan + WiFi.gatewayIP().toString());
  telnet.println(ABrightGreen +  "Access Point Address:    192.168.5.1");
  telnet.println();
  telnet.println(ABrightCyan + "System Status:" + AReset);
  telnet.println(ABrightGreen +  "SDK Ver:                 " + ABrightWhite + ESP.getSdkVersion() + AReset);
  telnet.println(ABrightGreen +  "CPU Freq:                " + ABrightMagenta + ESP.getCpuFreqMHz() + ABrightRed + " Mhz." + AReset);
  telnet.println(ABrightGreen +  "Sketch Size:             " + ABrightCyan + ESP.getSketchSize() + ABrightYellow + " Bytes." + AReset);
  telnet.println(ABrightGreen +  "Free Heap Size:          " + ABrightBlue + ESP.getFreeHeap() + ABrightYellow + " Bytes." + AReset);
  responsePrompt(1, 0, 1); //OK,system prompt
}
//  FILESYSTEM ***********************************************************************************
/*

   Resursive function to retreive sd card file listing
*/
void TelFileDirectory(File dir, int numTabs) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      telnet.print(tab);
    }
    String en = String(entry.name());
    if (en.startsWith("/System Volume Information")) {
      en.replace("/System Volume Information", ".");
      en.replace("/WPSettings.dat", ".");
      en.replace("/IndexerVolumeGuid", ".");
      en.replace("/System Volume Information\\", ".");
      //telnet.print(ABrightRed + revso_sym);
      telnet.print(ABrightWhite + en + AReset);
    } else {
      en.replace("/", "");
      /*
        if (en.length() < 4) {
        telnet.print(tab);
        }
      */
      if (entry.isDirectory()) {
        String enu = String(entry.name());
        enu.toUpperCase();
        telnet.print(ABrightYellow + enu + AReset);
      } else {
        telnet.print(ABrightWhite + en + AReset);
      }
    }
    if (entry.isDirectory()) {
      telnet.println(ABrightGreen + tab + tab + "<DIR>" + AReset);
      TelFileDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      telnet.print(tab + tab);
      if (en.length() < 8) {
        telnet.print(tab);
      }
      String es = String(entry.size());
      telnet.println(AYellow + es + ACyan + " bytes" + AReset);
    }
    entry.close();
  }
}
/*
  ********************************************* List the files on SD card.
*/
void TelSysFiles(String dir) {
  if (dir == ""){
    dir = "/";
  }
  telnet.println(ABrightGreen + "SD Card File Listings..." + AReset);
  //SD.begin(5);
  File root = SD.open(dir);
  TelFileDirectory(root, 0);
  responsePrompt(1, 0, 1); //OK,system prompt
}
/*
   ********************************************** Make Dir on SD card
*/
void TelMakeDir(String param) {
  telnet.println("Creating Dir: " + param);
  if (SD.mkdir("/" + param)) {
    responsePrompt(0, 0, 3); //file prompt
    telnet.println("Dir created!");
    logToSD(AWhite + getASCIITime() + ABrightGreen + " +++ new dir created --> " + param);
  } else {
    responsePrompt(0, 0, 3); //file prompt
    telnet.println("mkdir failed");
    logToSD(AWhite + getASCIITime() + ABrightRed + " --- make dir fail --> " + param);
  }
  responsePrompt(1, 0, 1); //ok, top prompt
}
/*
  **************************************************  Delete Dir on SD card
*/
void TelRemoveDir(String param) {
  responsePrompt(0, 0, 3); //file prompt
  telnet.println("Removing Dir: " + param);
  if (SD.rmdir("/" + param)) {
    responsePrompt(0, 0, 3); //file prompt
    telnet.println("Dir removed!");
    logToSD(AWhite + getASCIITime() + ABrightGreen + " +++ dir removed --> " + param);
  } else {
    responsePrompt(0, 0, 3); //file prompt
    telnet.println("rmdir failed");
    logToSD(AWhite + getASCIITime() + ABrightRed + " --- remove dir fail --> " + param);
  }
  responsePrompt(1, 0, 1); //ok, top prompt
}
/*
  ****************************************************  List a file's contents to the terminal(dump)
*/
void TelListFile(String param) {
  String fn =   param ;
  File myfile = SD.open(fn, FILE_READ);
  // if the file is available, read it:
  if (myfile) {
    while (myfile.available()) {
      telnet.print(myfile.read());
    }
    myfile.close();
    logToSD(AWhite + getASCIITime() + ABrightBlue + " +++ file looked at--> " + param);
    responsePrompt(1, 0, 1); //OK, system prompt
  } else {
    responsePrompt(0, 0, 2);
    telnet.println("----fail");
    logToSD(AWhite + getASCIITime() + ABrightRed + " --- list file - fail.");
    responsePrompt(0, 0, 1); // just system prompt
  }
}
void TelRenameFile(String oldname, String newname) {

}
/*
  ********************************************************* Show the system log
*/
void TelShowSysLogs() {
  telnet.println(ACyan + "Logs..." + AReset);
  responsePrompt(0, 0, 2); //subsystem prompt only
  if (SDCARD_READY) {
    if (! SD.exists("/syslog.txt")) {
      telnet.println(ABrightBlue + "creating new logfile..." + AReset);
      // only open a new file if it doesn't exist
      File myfile = SD.open("/syslog.txt", FILE_WRITE);
      String header = ABrightMagenta + "eMBOS -" + ABrightRed + " SYSTEM LOG - " + ABrightCyan + "MASTER";
      myfile.println(header);
      myfile.println(ABrightCyan + "System Log Started@:" + AWhite + getASCIITime());
      String user = AWhite + getASCIITime() + tab + ABrightYellow + "Telnet User@" + telnet.getIP() + tab + ABrightGreen + "logged in sucessfully!";
      myfile.println(user);
      myfile.close();
    }
    telnet.println(ABrightBlue + "-->log found!" + AGreen + " dumping log..." + AReset);
    String fn = String("/syslog.txt" );
    File myfile = SD.open(fn, FILE_READ);
    // if the file is available, read it:
    if (myfile) {
      while (myfile.available()) {
        telnet.print(myfile.read());
      }
      myfile.close();
    } else {
      responsePrompt(0, 0, 2);
      telnet.println(ABrightRed + "unknown error opening --> syslog.txt" + AReset);
      responsePrompt(1, 3, 1);  //  access denied, system prompt.
    }
  }
  responsePrompt(1, 0, 1); //OK, system prompt
}

/*
   *********************************************  Delete System logs
*/
void TelDeleteSysLogs() {
  responsePrompt(0, 0, 2);// subsystem prompt only
  telnet.println(ABrightRed + "Deleted," + ABrightGreen + " this cannot be undone! --> syslog.txt" + AReset);
  SD.remove("/syslog.txt");
  drawBanner();
  String user = "" + telnet.getIP();
  showBigMessage(user, "destroyed logs!");
  showTime();
  responsePrompt(1, 0, 1);//  OK, system prompt
}
/*
  ************************************************  SD Card Info
*/
void  TelSDCardInfo() {
  responsePrompt(0, 0, 2); // just sub-system prompt
  telnet.println(ABrightGreen + "SD Card Information" + AReset);
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_MMC) {
    telnet.println(ABrightBlue + "CardType - MMC" + AReset);
  }
  else if (cardType == CARD_SD) {
    telnet.println(ABrightBlue + "CardType - SD" + AReset);
  }
  else if (cardType == CARD_SDHC) {
    telnet.println(ABrightBlue + "CardType - SDHC" + AReset);
  }
  else {
    telnet.println(ABrightRed + "CardType - unknown" + AReset);
  }
  int cardSize = SD.cardSize() / (1024 * 1024);
  int tbu = SD.totalBytes() / (1024 * 1024);
  int ubu = SD.usedBytes() / (1024 * 1024);
  String sdcs = String(cardSize);
  telnet.println(AWhite + "SD Card Size: " + sdcs + " MB");
  String tb = String(tbu);
  String ub = String(ubu);
  telnet.println("Total space: " + tb + " MB");
  telnet.println("Used space: " + ub + " MB" );
  responsePrompt(1, 0, 1); // OK, system prompt
}

//  MACHINE  ******************************************************************************************************
/*

   run the calibration sequence. This homes the X & Y axes and sets all respective counters to zero!
*/
void TelCalibrate() {
  telnet.println(ABrightCyan + "Calibrate Machine.." + AReset);
  responsePrompt(1, 0, 2);  //OK,subsystem prompt
  telnet.println(ABrightRed + "Running Calibration Sequence!! - Standby..." + AReset);
  showPosition();
  delay(3000);
  responsePrompt(1, 0, 2);  //OK,subsystem prompt
  telnet.println(ABrightRed + "Calibration Done!" + AReset);
  logToSD(AWhite + getASCIITime() + ABrightBlue + " +++ calibration done.");
  responsePrompt(1, 0, 1);  //OK,system prompt
}
/*
  ********************************************************* Limits test
*/
void TelLimitsTest() {
  showLimits();
  responsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightYellow + "See OLED," + ABrightRed + " done!" + AReset);
  logToSD(AWhite + getASCIITime() + ABrightGreen + " --> limits test done.");
  responsePrompt(1, 0, 1); //system prompt
}
/*
   SELFTEST ************************************************************************************************
   Selftest Help
*/
void TelSelfTestHelp() {
  telnet.println(AClearScreen + AHomeCursor);
  telnet.print(ABrightCyan + ABold + tab + golf_sym);
  telnet.print( ABrightRed + ABold + tab + tab + pat_sym + tab + ASlowBlink + "eMBOS" + AReset);
  telnet.println(ABrightYellow + ABold + tm_sym + AReset + ABrightGreen + "   V1.0.1" + AReset);
  telnet.println(tab + ABrightYellow + cr_sym + ABrightCyan + "Graffiti Softwerks" + ABrightGreen + " All Rights Reserved 2021/2022");
  telnet.println();
  telnet.println(tab + tab + tab + ABrightYellow + "SelfTest Help!");
  telnet.println();
  telnet.println(ABrightBlue + " selftest -<help:?>" + AWhite + "    - Shows this command help.");
  telnet.println(ABrightBlue + " selftest -M <-X:Y:B>" + AWhite + "  - Selects the motor axis to test, X, Y or both.");
  telnet.println(ABrightBlue + " selftest -N <-c:o>" + AWhite + "    - Needle motor test, o - oneshot or a 1 sec burst.");
  telnet.println(ABrightBlue + " selftest -I" + AWhite + "           - I2c system discovery test.");
  telnet.println(ABrightBlue + " selftest -L" + AWhite + "           - Limit sensor test.");
  telnet.println(ABrightBlue + " selftest -C" + AWhite + "           - Calibration plot test.(requires a paper paddle)" + AReset);
}

/*
   *****************************************************  X Motor Test DRV8825
*/
void TelRunXTest() {
  startMillis = millis();
  logToSD(AWhite + getASCIITime() + ABrightBlue + " --> x-motor test started.");
  responsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightRed + "running motor test X axis!" + AReset);
  telnet.println(ABrightYellow + "reverse - 1/1 step!" + AReset);
  for (int ctr = 1; ctr < STEPPERREV; ctr++) {
    MoveXMotor(!Forward, 0);
  }
  responsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightYellow + "reverse - 1/2 step!" + AReset);
  for (int ctr = 1; ctr < STEPPERREV; ctr++) {
    MoveXMotor(!Forward, 1);
  }
  responsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightYellow + "reverse - 1/4 step!" + AReset);
  for (int ctr = 1; ctr < STEPPERREV; ctr++) {
    MoveXMotor(!Forward, 2);
  }
  responsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightBlue + "forward - 1/1 step!" + AReset);
  for (int ctr = 1; ctr < STEPPERREV; ctr++) {
    MoveXMotor(Forward, 0);
  }
  responsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightBlue + "forward - 1/2 step!" + AReset);
  for (int ctr = 1; ctr < STEPPERREV; ctr++) {
    MoveXMotor(Forward, 1);
  }
  responsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightBlue + "forward - 1/4 step!" + AReset);
  for (int ctr = 1; ctr < STEPPERREV; ctr++) {
    MoveXMotor(Forward, 2);
  }
  responsePrompt(1, 0, 2); //OK, subsystem prompt
  telnet.println(ABrightGreen + "completed." + AReset);
  responsePrompt(1, 0, 1);
}
/*
   ******************************************************** Y Motor Test DRV8825
*/
void TelRunYTest() {
  logToSD(AWhite + getASCIITime() + ABrightBlue + " --> y-motor test started.");
  responsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightRed + "running motor test Y axis!" + AReset);
  telnet.println(ABrightYellow + "reverse - 1/1 step!" + AReset);
  for (int ctr = 1; ctr < STEPPERREV; ctr++) {
    MoveYMotor(!Forward, 0);
  }
  responsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightYellow + "reverse - 1/2 step!" + AReset);
  for (int ctr = 1; ctr < (STEPPERREV * 2); ctr++) {
    MoveYMotor(!Forward, 1);
  }
  responsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightYellow + "reverse - 1/4 step!" + AReset);
  for (int ctr = 1; ctr < (STEPPERREV * 4); ctr++) {
    MoveYMotor(!Forward, 2);
  }
  responsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightBlue + "forward - 1/1 step!" + AReset);
  for (int ctr = 1; ctr < STEPPERREV; ctr++) {
    MoveYMotor(Forward, 0);
  }
  responsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightBlue + "forward - 1/2 step!" + AReset);
  for (int ctr = 1; ctr < (STEPPERREV * 2); ctr++) {
    MoveYMotor(Forward, 1);
  }
  responsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightBlue + "forward - 1/4 step!" + AReset);
  for (int ctr = 1; ctr < ( STEPPERREV * 4); ctr++) {
    MoveYMotor(Forward, 2);
  }
  responsePrompt(1, 0, 2); //OK, subsystem prompt
  telnet.println(ABrightGreen + "completed." + AReset);
  responsePrompt(1, 0, 1);
}
/*
   ***********************************************************  I2C Bus scan
*/
void TelI2CBusScan() {
  responsePrompt(1, 0, 2); //OK, subsystem prompt only
  telnet.println(ABrightYellow + "Running i2c bus scan!.. Standby." + AReset);
  logToSD(AWhite + getASCIITime() + ABrightBlue + " --> i2c test started.");

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
      telnet.print(ABrightGreen + "I2C device at 0x");
      if (address < 0x10) {
        display1.print("0");
        telnet.print("0");
      }
      char charVal[4];
      sprintf(charVal, "%04X", address);
      telnet.println(charVal + AReset);
      display1.print(address, HEX);
      display1.print(" hex!");
      display1.display();

      if (address == 0x20) {
        telnet.println(ABrightYellow + "PortExpander @" + ABrightBlue + " address" + ABrightCyan + " 0x20" + AReset);
        display1.setCursor(10, 35);
        display1.print("PortExpander");
        display1.display();
      }

      else if (address == 0x3C) {
        telnet.println(ABrightYellow + "OLED @" + ABrightBlue + " address" + ABrightCyan + " 0x3C" + AReset);
        display1.setCursor(10, 35);
        display1.print("OLED - Me!");
        display1.display();
      }

      nDevices++;
      delay(500);
    } else if (error == 4) {
      display1.setCursor(5, lastRow);
      display1.print("Error at address 0x");
      telnet.println(ABrightRed + "Error at address 0x");
      if (address < 16) {
        display1.print("0");
        telnet.print("0");

      }
      char charVal[4];
      sprintf(charVal, "%04X", address);
      telnet.println(charVal + AReset);
      display1.println(address, HEX);
      display1.display();
    }
  }
  if (nDevices == 0) {
    drawBanner();
    display1.setCursor(10, lastRow);
    display1.print("No I2C devices found");
    telnet.print( ABrightRed + "No I2C devices found" + AReset);
    display1.display();
  } else {
    display1.setCursor(10, lastRow);
    display1.print("That's all. Done.");
    telnet.println( ABrightWhite + "That's all. Done." + AReset);
    display1.display();
  }
  responsePrompt(1, 0, 1); //OK!, system prompt
}
/*
 ********************************** Telnet Motor Voltage Info
*/
void TelMotorVoltageInfo() {
  responsePrompt(0, 0, 2); //subsystem prompt
  float battery_voltage = ((float)analogRead(MVOLT_PIN) / 4095.0) * 2.0 * 3.3 * (1100 / 1000.0);
  String voltage =  String(battery_voltage) + ABrightYellow + " Volts";
  telnet.println(ABrightBlue + "Battery Voltage :" + ABrightGreen + voltage + AReset);
  responsePrompt(1, 0, 1); //OK,system prompt
}
void TelResetPorts() {
  initializeMotorsPort();
  responsePrompt(0, 0, 2);//  sub-prompt
  telnet.println(ABrightMagenta +  ABold + "Ports RESET" + AReset);
  responsePrompt(1, 0, 1);//  OK, system prompt
}
/*
  Gets the hexadecimal value of the specified port.
*/
void TelGetPort(String p) {
  String pf = "";
  if (p == "A" || p == "a") {
    pf = GetPort(1);
    telnet.println( ABrightCyan + "Port A:" + ABrightWhite + " 0x" + pf + AReset);
  }
  else {
    pf = GetPort(2);
    telnet.println( ABrightCyan + "Port B:"  + ABrightWhite + " 0x" + pf + AReset);
  }
  responsePrompt(1, 0, 1); //OK,system prompt
}
/*
  Sets the hexadecimal value of a port.
*/
void TelSetPort(String p, int pv) {
  if (p == "A" || p == "a") {
    SetPort(1, pv);
  } else {
    SetPort(2, pv);
  }
  responsePrompt(0, 0, 1); // just system prompt
}

void TelGetPin(String param, String pin) {

  responsePrompt(1, 0, 1); //OK, system prompt - normal completion.
}

void TelSetPin(String param, String pin, String pval) {

  responsePrompt(1, 0, 1); //OK, system prompt - normal completion.
}

//********************************************************** System Diagnostics - motor test, remote test,...

void TelSelfTest(String command) {
  int se = command.length();
  int xTag = command.indexOf('-');
  int yTag = command.indexOf('-', xTag + 1);
  int zTag = command.indexOf(' ', yTag + 1);

  String tsp1 = ABrightGreen + "p1= " + command.substring(xTag + 1, yTag - 1);
  String p1 = command.substring(xTag + 1, yTag - 1);
  String tsp2 = ABrightGreen + "p2= " + command.substring(yTag + 1, zTag);
  String p2 = command.substring(yTag + 1, zTag);
  //telnet.println(tsp1);
  //telnet.println(tsp2);
  telnet.print(AReset);
  if (p1 == "?" || p1 == "help" || p1 == "selftest") {
    TelSelfTestHelp();
  }
  if (p1 == "L" || p1 == "l") {
    TelLimitsTest();
  }
  if (p1 == "M" || p1 == "m") {
    telnet.println(ABrightYellow + "SelfTest " + ACyan + "set to test motor: " + ABrightBlue + p2 + AReset);
    if (p2 == "-X" || p2 == "-x") {
      TelRunXTest();
    }
    if (p2 == "-Y" || p2 == "-y") {
      TelRunYTest();
    }
    if (p2 == "B" || p2 == "b") {
      TelRunXTest();
      TelRunYTest();
    }
  }
  if (p1 == "I" || p1 == "i") {
    TelI2CBusScan();
  }
  if (p1 == "N" || p1 == "n") {
    telnet.println(ABrightYellow + "SelfTest " + ACyan + "- set to test needle motor: " + ABrightRed + "CAUTION!"  + AReset);
    if (p2 == "o" || p2 == "O") {
      telnet.println(AYellow + "One-Shot/Single Cycle"  + AReset);
      logToSD(AWhite + getASCIITime() + ABrightBlue + " +++ needle - one shot.");
      RunNeedleTest(0); //  see support.h
    }
    if (p2 == "c" || p2 == "C") {
      telnet.println(AYellow + "1 second continous run"  + AReset);
      logToSD(AWhite + getASCIITime() + ABrightBlue + " +++ needle - one second burst.");
      RunNeedleTest(1); //  see support.h
    }
  }
  if (p1 == "C" || p1 == "c") {
    TestCalibration();  //  see support.h
  }
}


/*
  ********************************************* Jobs
*/
void TelSysJobs() {
  telnet.println( AGreen + "Jobs..." + AReset);
  responsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println("no jobs in list");
  responsePrompt(1, 0, 1); //system prompt
}
/*
*************************************************** Run
*/
void TelSysRun(String param) {
  telnet.println(ABrightGreen + "Run Job..." + AReset);
  responsePrompt(0, 1, 2); //system prompt only
  telnet.println("none to select");
  responsePrompt(1, 0, 1); //system prompt
  logToSD(AWhite + getASCIITime() + ABrightBlue + " +++ run job --> " + param);
}
/*
********************************************  cancel
*/
void TelSysCancel() {
  responsePrompt(0, 0, 2);
  telnet.println(ABrightYellow + "Cancelling any running test.");
  ABORT_TEST = true;
  logToSD( AWhite + getASCIITime() + ABrightBlue + " --- cancelled operation --> " );
  responsePrompt(1, 0, 1);
}

//**********************************  HELP SYSTEM ************************************************

void TelGenHelp() {
  telnet.println(AClearScreen + AHomeCursor);
  telnet.print(ABrightCyan + ABold + tab + golf_sym + ABrightRed + ABold + pat_sym + ASlowBlink + "eMBOS" + AReset);
  telnet.println(ABrightYellow + ABold + tm_sym + AReset + ABrightGreen + "   V1.0.1" + AReset);
  telnet.println(tab + ABrightYellow + cr_sym + ABrightCyan + "Graffiti Softwerks" + ABrightGreen + " All Rights Reserved 2021/2022");

  telnet.println(tab + tab + ABrightYellow + "General Help System!" + AReset);
  telnet.println();
  telnet.println( tab + tab + tab + AMagenta + "Commands:");
  telnet.println();
  telnet.println(tab + AGreen + "netscan" + AWhite + "      - returns scan of network.");
  telnet.println(tab + AGreen + "status" + AWhite + "       - returns complete status on system.");
  telnet.println(tab + AGreen + "log" + AWhite + "          - system log.");
  telnet.println(tab + AGreen + "clr" + AWhite + "          - clears the screen.");
  telnet.println(tab + AGreen + "run" + AWhite + "          - runs a job on the system.");
  telnet.println(tab + AGreen + "sdcardinfo" + AWhite + "   - returns SD card info.");
  telnet.println(tab + AGreen + "files" + AWhite + "        - list file system on machine.");
  telnet.println(tab + AGreen + "list" + AWhite + "         - dumps file to screen.");
  telnet.println(tab + AGreen + "mkdir" + AWhite + "        - creates a new directory.");
  telnet.println(tab + AGreen + "rmdir" + AWhite + "        - deletes named directory.");
  telnet.println(tab + AGreen + "rename" + AWhite + "       - renames a file.");
  telnet.println(tab + AGreen + "jobs" + AWhite + "         - show available jobs.");
  telnet.println(tab + AGreen + "cancel" + AWhite + "       - cancels job if one is running.");
  telnet.println(tab + AGreen + "i2c" + AWhite + "          - i2c discovery");
  telnet.println(tab + AGreen + "motorv" + AWhite + "       - gets the motors supply voltage.");
  telnet.println(tab + AGreen + "selftest" + AWhite + "     - selftest diagnostics on system.");
  telnet.println(tab + AGreen + "calibrate" + AWhite + "    - runs calibration on system.");
  telnet.println(tab + AGreen + "worklight" + AWhite + "    - turns on/off the worklight.");
  telnet.println();
  telnet.println(tab + tab + ABrightRed + "PortExpander Direct Commands: - experimental");
  telnet.println();
  telnet.println(tab + AGreen + "resetports" + AWhite + "   - resetports PortExpander system, see help docs.");
  telnet.println(tab + AGreen + "getport" + AWhite + "      - getport PortExpander system, see help docs.");
  telnet.println(tab + AGreen + "setport" + AWhite + "      - setport PortExpander system, see help docs.");
  telnet.println(tab + AGreen + "getpin" + AWhite + "       - getpin PortExpander system, see help docs.");
  telnet.println(tab + AGreen + "setpin" + AWhite + "       - setpin PortExpander system, see help docs.");
  telnet.println();
  telnet.println(tab + tab + ABrightRed + "SewCNC Direct Commands:");
  telnet.println();
  telnet.println(tab + ABrightBlue + "W> X:<n> Y:<n>" + AWhite + "  - Hover to that location and wait.");
  telnet.println(tab + ABrightBlue + "G> X:<n> Y:<n>" + AWhite + "  - cycle NEEDLE at that location.");
  telnet.println(tab + ABrightBlue + "T> <colour>" + AWhite + "     - thread change.");
  telnet.println(tab + ABrightBlue + "C>" + AWhite + "              - continue.");
  telnet.println(tab + ABrightBlue + "M> <name>" + AWhite + "       - call macro - .msf loads file.");
  telnet.println(tab + ABrightBlue + "V> <0:1>" + AWhite + "        - set verbosity.");
  telnet.println();
  telnet.println(tab + tab + ABrightWhite + "Type" + ACyan + " [command]" + AYellow + "-" + ABrightGreen + "help" + AYellow + " / -" + ABrightGreen + "?" + ABrightBlue + " for more details." + AReset);
  telnet.println();
  responsePrompt( 1, 0, 1); //system prompt
}
/*
   Title:       TelHelp
   Author:      zna
   Date:        01-26-22
   Version:     1.0.0
   Description: provides user help.
   Input:       <command>
   Returns:     ASCII string.
*/
void TelHelp(String comm) {
  // check if help-sys loaded
  if (!HELP_SYS) {
    loadHelp();
  }
  // lookup the comm and get its result number
  int x = SDHelpLookup(comm);
  commDesc[x] = "- scans the neighbourhood for networks.";
  commExample[x] = "nets/netscan";
  commExtra[x] = "<Y:N>";
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.print(tab + ABrightRed + comm + cr + tab + tab + tab + ABrightWhite + commDesc[x] + nl + cr);
  telnet.print(tab + ABrightGreen  + "usage:" + cr + tab + tab + tab + ABrightCyan + commExample[x] + ABrightMagenta + commExtra[x] + nl + cr + AReset);
  responsePrompt( 1, 0, 1); //system prompt
}

/*
   Title:       WriteHelp
   Author:      zna
   Date:        01-27-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
*/
void WriteHelp(String h) {
  File root;
  root.rewindDirectory();
  root = SD.open("/qhelp.bin", FILE_WRITE);
  //create new one here
  root.println(h);
  root.close();
}
/*
   Title:       ReadHelp
   Author:      zna
   Date:        01-27-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
*/
void TelReadQuickHelp() {
  File root;
  root.rewindDirectory();
  root = SD.open("/qhelp.bin", FILE_READ);
  if (root) {
    while (root.available()) {
      telnet.print(root.read());
    }
    root.close();
  }
  responsePrompt( 1, 0, 1); //system prompt
}
void TelCalibrateHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.print(tab + ABrightRed + "calibrate" + cr + tab + tab + tab + ABrightWhite + "- calibrates the X & Y axis." + nl + cr);
  telnet.print(tab + ABrightGreen  + "usage:" + cr + tab + tab + tab + ABrightCyan + "calibrate-" + AMagenta + "<y>" + nl + cr + AReset);
  responsePrompt( 1, 0, 1); //system prompt
}

void TelNetScanHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.print(tab + ABrightRed + "netscan" + cr + tab + tab + tab + ABrightWhite + "- scans the neighbourhood for networks." + nl + cr);
  telnet.print(tab + ABrightGreen  + "usage:" + cr + tab + tab + tab + ABrightCyan + "nets/netscan-" + AMagenta + "<y>" + nl + cr + AReset);
  responsePrompt( 1, 0, 1); //system prompt
}
void TelSysNetInfoHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightRed + "status" + ABrightWhite + tab + "- reports system and network status.");
  telnet.println(tab + ABrightGreen  + "usage:" + ABrightCyan + tab + "status-" + AMagenta + "<y>" + AReset);
  responsePrompt( 1, 0, 1); //system prompt
}
void TelCancelHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightRed + "cancel" + ABrightWhite + tab + "- cancels any running test.");
  telnet.println(tab + ABrightGreen  + "usage:" + ABrightCyan + tab + "cancel-" + AMagenta + "<y>" + AReset);
  responsePrompt( 1, 0, 1); //system prompt
}
void TelResetPortsHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightRed + "resetports" + ABrightWhite + tab + "- resets all ports to default configuration.");
  telnet.println(tab + ABrightGreen  + "usage:" + ABrightCyan + tab + "resetports-" + AMagenta + "<y>" + AReset);
  responsePrompt( 1, 0, 1); //system prompt
}
void TelSetPortHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightRed + "setport" + ABrightWhite + tab + "- sets a port value of the port expander(A/B port).");
  telnet.println(tab + ABrightGreen  + "usage:" + ABrightCyan + tab + "setport-" + AMagenta + "<a:b>,<0-FF>" + AReset);
  responsePrompt( 1, 0, 1); //system prompt
}
void TelGetPortHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightRed + "getport" + ABrightWhite + tab + "- gets a port value of the port expander(A/B port).");
  telnet.println(tab + ABrightGreen  + "usage:" + ABrightCyan + tab + "getport-" + AMagenta + "<a:b>" + AReset);
  responsePrompt( 1, 0, 1); //system prompt
}
void TelSetPinHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightRed + "setpin" + ABrightWhite + tab + "- sets a pin value of the port expander(A/B port).");
  telnet.println(tab + ABrightGreen  + "usage:" + ABrightCyan + tab + "setpin-" + AMagenta + "<a:b>,<pin#>,<H:L>" + AReset);
  responsePrompt( 1, 0, 1); //system prompt
}
void TelGetPinHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightRed + "getpin" + ABrightWhite + tab + "- gets a pin value of the port expander(A/B port).");
  telnet.println(tab + ABrightGreen  + "usage:" + ABrightCyan + tab + "getpin-" + AMagenta + "<a:b>,<pin#>" + AReset);
  responsePrompt( 1, 0, 1); //system prompt
}
void TelSDCardInfoHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightRed + "sdcardinfo" + ABrightWhite + tab + "- shows sd card information.");
  telnet.println(tab + ABrightGreen  + "usage:" + ABrightCyan + tab + tab + "sdci/sdcardinfo-" + AMagenta + "<y>" + AReset);
  responsePrompt(1, 0, 1);  //system prompt
}
void TelRemoveDirHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightRed + "rmdir" + ABrightWhite + tab + "- deletes named directory.");
  telnet.println(tab + ABrightGreen  + "usage:" + ABrightCyan + tab + "rmdir-" + AMagenta + "<dirname>" + AReset);
  responsePrompt( 1, 0, 1); //system prompt
}
void TelMakeDirHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightRed + "mkdir" + ABrightWhite + tab + "- makes named directory.");
  telnet.println(tab + ABrightGreen  + "usage:" + ABrightCyan + tab + "mkdir-" + AMagenta + "<dirname>" + AReset);
  responsePrompt( 1, 0, 1); //system prompt
}
void TelLogsHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightRed + "log" + ABrightWhite + tab + "- displays/deletes log information.");
  telnet.println(tab + ABrightGreen  + "usage:" + ABrightCyan + tab + "log-" + AMagenta + "<del:list>" + AReset);
  responsePrompt( 1, 0, 1); //system prompt
}
void TelListFileHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightRed + "list" + ABrightWhite + tab + "- dumps file to screen.");
  telnet.println(tab + ABrightGreen  + "usage:" + ABrightCyan + tab + "list-" + AMagenta + "<dir><filename>" + AReset);
  responsePrompt(1, 0, 1);  //system prompt
}
void TelI2CHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightRed + "i2cdetect" + ABrightWhite + tab + "- scans the i2c bus for devices.");
  telnet.println(tab + ABrightGreen  + "usage:" + ABrightCyan + tab + "i2c/i2cdetect" + AMagenta + "<y>" + AReset);
  responsePrompt(1, 0, 1);  //system prompt
}
void TelBatteryHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightRed + "battery" + ABrightWhite + tab + "- reports the status of the battery.");
  telnet.println(tab + ABrightGreen  + "usage:" + ABrightCyan + tab + "battery/volts/bat/voltage" + AMagenta + "<y>" + AReset);
  responsePrompt(1, 0, 1);  //system prompt
}
void TelRenameFileHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightRed + "list" + ABrightWhite + tab + "- dumps file contents to screen.");
  telnet.println(tab + ABrightGreen  + "usage:" + ABrightCyan + tab + "rename-" + AMagenta + "<old filename>,<new filename>" + AReset);
  responsePrompt(1, 0, 1);  //system prompt
}
void TelFilesHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightRed + "files" + ABrightWhite + tab + "- list files to screen.");
  telnet.println(tab + ABrightGreen  + "usage:" + ABrightCyan + tab + "files-" + AMagenta + "<dir>" + AReset);
  responsePrompt(1, 0, 1);  //system prompt
}
void TelWorklightHelp() {
  telnet.println(tab + tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightRed + "worklight" + ABrightWhite + cr + tab + tab + tab + "- turns on or off, machine work lamp.");
  telnet.println(tab + ABrightGreen  + "usage:" + ABrightCyan + tab + "worklight-" + AMagenta + "<ON:OFF>" + AReset);
  responsePrompt(1, 0, 1);  //system prompt
}
void TelHelpHelp() {
  TelNetScanHelp();
  TelSysNetInfoHelp();
  TelCancelHelp();
  TelResetPortsHelp();
  TelSetPortHelp();
  TelGetPortHelp();
  TelSetPinHelp();
  TelGetPinHelp();
  TelMakeDirHelp();
  TelRemoveDirHelp();
  TelLogsHelp();
  TelListFileHelp();
  TelI2CHelp();
  TelBatteryHelp();
  TelRenameFileHelp();
  TelFilesHelp();
  TelWorklightHelp();
}
//  DIRECT  ************************************************************************************************
void TelDirectG(String command) {
  int se = command.length();
  int xTag = command.indexOf('X:');
  int yTag = command.indexOf('Y:', xTag + 1);
  int endTag = command.indexOf(' ', yTag + 1);
  String x_param = ABrightMagenta + "X= " + command.substring(xTag + 1, yTag - 1);
  XDEST = command.substring(xTag + 1, yTag - 1).toInt();
  String y_param = ABrightMagenta + "Y= " + command.substring(yTag + 1, endTag);
  YDEST = command.substring(yTag + 1, endTag).toInt();
  GotoPoint(XDEST, YDEST);
  responsePrompt(0, 0, 2);// machine prompt only.
  telnet.println(x_param);
  responsePrompt(0, 0, 2);// machine prompt only.
  telnet.println(y_param);
  telnet.print(AReset);
  responsePrompt(1, 0, 1);//OK, system prompt - normal completion.
}

void TelDirectW(String command) {
  int se = command.length();
  int xTag = command.indexOf('X:');
  int yTag = command.indexOf('Y:', xTag + 1);
  int zTag = command.indexOf(' ', yTag + 1);
  String x_param = ABrightGreen + "X= " + command.substring(xTag + 1, yTag - 1);
  XDEST = command.substring(xTag + 1, yTag - 1).toInt();
  String y_param = ABrightGreen + "Y= " + command.substring(yTag + 1, zTag);
  YDEST = command.substring(yTag + 1, zTag).toInt();
  GotoPoint(XDEST, YDEST);
  responsePrompt(0, 0, 2);// machine prompt only.
  telnet.println(x_param);
  responsePrompt(0, 0, 2);// machine prompt only.
  telnet.println(y_param);
  telnet.print(AReset);
  responsePrompt(1, 0, 1);
}
void TelDirectM(String command) {
  responsePrompt(0, 0, 2);// machine prompt only.
  String param = command.substring(command.indexOf(' ') + 1, command.length());
  telnet.println(ABrightYellow + "MacroName: " + ABrightCyan + param + ".msf" + AReset);
  responsePrompt(1, 0, 1);//OK, system prompt - normal completion.
}
void TelDirectT(String command) {
  responsePrompt(0, 0, 2);// machine prompt only.
  //TODO: add a GLOBAL THREAD VALUE in "system.h"
  String t_param = command.substring(command.indexOf(' ') + 1, command.length());
  telnet.println(ABrightRed + "Thread Change - " + ABrightWhite + t_param + AReset);
  responsePrompt(1, 0, 1);//OK, system prompt - normal completion.
}
void TelDirectC(String command) {
  if (MachineRun) {
    responsePrompt(0, 0, 2);// machine prompt only.
    telnet.println(ABrightRed + "Job Paused" + AReset);
    responsePrompt(1, 0, 1);//OK, system prompt - normal completion.
  }
}
void TelDirectV(String command) {
  String blablabla = command.substring(command.indexOf(' ') + 1, command.length());
  if (blablabla == "0" ) {
    Verbosity = false;
  } else {
    Verbosity = true;
  }
  responsePrompt(1, 0, 1);  //OK, system prompt - normal completion.
}
/*
  ***************************************************** MISC  ******************************************
*/
void TelQuit() {
  telnet.println("There is NO quit option!");
  responsePrompt(1, 0, 1); //OK, system prompt
}
/*
   Title:       TelUpdateInfo
   Author:      zna
   Date:        01-27-22
   Version:     1.0.0
   Description: Writes the qhelp.bin file to disk
   Input:
   Returns:
*/
void TelUpdateInfo() {

}

// -------------------------- COMMAND PARSER -------------------------------------------------------//

void parseCommand(String command) {
  //parses the telnet entry and locates instructions and completes the tasks

  if (command.length() == 0) {
    responsePrompt(1, 5, 1); //unknown - system prompt
  }

  else if (command.startsWith("selft")) {
    TelSelfTest(command);
    responsePrompt(1, 0, 1);
  }
  else if (command == "jobs") {
    TelSysJobs();
  }

  else if (command == "read") {
    TelReadQuickHelp();
  }

  else if (command.startsWith("sdci")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    telnet.println(param);
    if (param == command || param == "?" || param == "help") {
      TelSDCardInfoHelp();
    } else if (param == "y" || param == "Y") {
      TelSDCardInfo();
    }
  }

  else if (command.startsWith("nets")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    telnet.println(param);
    if (param == command || param == "?" || param == "help") {
      TelNetScanHelp();
    } else if (param == "y" || param == "Y") {
      TelNetScan();
    }
  }

  else if (command.startsWith("cali")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      TelCalibrateHelp();
    } else {
      TelCalibrate();
    }
  }


  else if (command.startsWith("stat")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      TelSysNetInfoHelp();
    } else {
      TelSysStat();
    }
  }

  else if (command == "clr" || command == "clear") {
    telnet.println(AClearScreen + AHomeCursor);
    responsePrompt(0, 0, 1);
  }

  else if (command.startsWith("run")) {
    responsePrompt(1, 0, 1);
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      responsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "usage: run -<jobname>");
      responsePrompt(1, 0, 1);
    } else {
      responsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "JobName: " + ABrightCyan + param + AReset);
      TelSysRun(param);
    }
  }

  else if (command.startsWith("log")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    telnet.println(param);
    if (param == "del" || param == "DEL" || param == "delete" || param == "DELETE") {
      TelDeleteSysLogs();

    }
    else if (param == "l" || param == "L" || param == "List" || param == "list") {
      TelShowSysLogs();
    }
    else if (param == command || param == "?" || param == "help") {
      TelLogsHelp();
    }
    responsePrompt(1, 0, 1);
  }

  else if (command.startsWith("list")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      TelListFileHelp();
    } else {
      responsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "Screen Dump FileName: " + ABrightCyan + param + AReset);
      TelListFile(param);
    }
  }

  else if (command.startsWith("mkdir")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      TelMakeDirHelp();
      //TelHelp("mkdir");
    } else {
      responsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "Create Directory: " + ABrightCyan + param + AReset);
      TelMakeDir(param);
    }
  }

  else if (command.startsWith("rmdir")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      TelRemoveDirHelp();
    } else {
      responsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "Remove Directory: " + ABrightCyan + param + AReset);
      TelRemoveDir(param);
    }
  }


  else if (command.startsWith("rename")) {
    responsePrompt(1, 0, 1);
    String param = command.substring(command.indexOf(' ') + 1, command.indexOf(',') - 1);
    String newname = command.substring(command.indexOf(',') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      TelRenameFileHelp();
    } else {
      responsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "Renamed: " + ABrightCyan + param + " to " + newname + AReset);
      TelRenameFile(param, newname);
      logToSD(AWhite + getASCIITime() + ABrightRed + " --> file rename - " + param + " to " + newname);
    }
  }

  else if (command.startsWith("files")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if (param == "?" || param == "help") {
      TelFilesHelp();
    } else {
      TelSysFiles(param);
    }

  }


  else if (command.startsWith("cancel")) {
    responsePrompt(1, 0, 1);
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      TelCancelHelp();
    } else {
      TelSysCancel();
    }
    responsePrompt(1, 0, 1);
  }

  else if (command.startsWith("workl")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      TelWorklightHelp();
    } else {
      if (param == "0" || param == "off" || param == "OFF") {
        lamp(0);
        responsePrompt(0, 0, 2);
        telnet.println(ABrightYellow + "Setting LAMP OFF" + AReset);
      }
      if (param == "1" || param == "on" || param == "ON") {
        lamp(1);
        responsePrompt(0, 0, 2 );
        telnet.println(ABrightYellow + "Setting LAMP ON" + AReset);
      }
      responsePrompt(1, 0, 1);
    }
  }
  else if (command == "?" || command == "help") {
    TelGenHelp();
  }

  else if (command.startsWith("help") || command.startsWith("?")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      TelHelpHelp();
    }
  }

  else if (command.startsWith("#")) {
    responsePrompt(1, 0, 1); //OK, system prompt - normal completion.
  }

  else if (command.startsWith("W>")) {
    TelDirectW(command);
  }

  else if (command.startsWith("G>")) {
    TelDirectG(command);
  }

  else if (command.startsWith("M>")) {
    TelDirectM(command);
  }

  else if (command.startsWith("T>")) {
    TelDirectT(command);
  }

  else if (command.startsWith("C>")) {
    TelDirectC(command);
  }

  else if (command.startsWith("V>")) {
    TelDirectV(command);
  }

  else if (command.startsWith("i2c")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if (param == "?" || param == "help") {
      TelI2CHelp();
    } else {
      TelI2CBusScan();
      drawBanner();
      showTime();
    }
  }

  else if (command.startsWith("getport")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if (param == "?" || param == "help") {
      TelGetPortHelp();
      //ResponsePrompt(1, 0, 1); //OK, system prompt - normal completion.
    } else {
      TelGetPort(param);
    }
  }

  else if (command.startsWith("resetports")) {
    String param = command.substring(command.indexOf('-') + 1, command.indexOf(',') - 1);
    if (param == "?" || param == "help") {
      TelResetPortsHelp();
    } else {
      TelResetPorts();
    }
  }


  else if (command.startsWith("setport")) {
    String param = command.substring(command.indexOf('-') + 1, command.indexOf(',') - 1);
    String apv = command.substring(command.indexOf(',') + 1, command.length());
    //  converts hex string to integer value
    int pv = stringToInteger(apv, 16);
    if (pv > 255) {
      responsePrompt(0, 0, 2);
      telnet.println(ABrightMagenta +  ABold + "Input value error - too large!" + AReset);
    } else {
      TelSetPort(param, pv);
    }
    if (param == "?" || param == "help") {
      TelSetPortHelp();
    }
  }

  else if (command.startsWith("getpin")) {
    String param = command.substring(command.indexOf('-') + 1, command.indexOf(',') - 1);
    String pin = command.substring(command.indexOf(',') + 1, command.indexOf(',') - 1);
    if (param == "?" || param == "help") {
      TelGetPinHelp();
    } else {
      TelGetPin(param, pin);
    }
  }

  else if (command.startsWith("setpin")) {
    String param = command.substring(command.indexOf('-') + 1, command.indexOf(',') - 1);
    String pin = command.substring(command.indexOf(',') + 1, command.indexOf(',') - 1);
    String pval = command.substring(command.indexOf(',') + 1, command.indexOf(',') - 1);
    if (param == "?" || param == "help") {
      TelSetPinHelp();
    } else {
      TelSetPin(param, pin, pval);
    }
  }
  //
}
/*
 * *********************************  TELNET SETUP & FUNCTIONS ***********************************************
*/
// callback functions for telnet events
void OnTelnetConnect(String ip) {
  telnet.println(AClearScreen + AHomeCursor);
  telnet.print(ABrightCyan + ABold + tab + golf_sym + ABrightRed + ABold + pat_sym + ASlowBlink + "eMBOS" + AReset);
  telnet.println(ABrightYellow + ABold + tm_sym + AReset + ABrightGreen + "   V1.0.1" + AReset);
  telnet.println(tab + ABrightYellow + cr_sym + ABrightCyan + "Graffiti Softwerks" + ABrightGreen + " All Rights Reserved 2021/2022");

  telnet.print(tab + ABrightCyan + "System Local Time: ");
  String nt = String(getASCIITime());
  telnet.println(AWhite + nt );
  telnet.print(tab + ABrightMagenta +  ABold + "Telnet Service: ");
  telnet.println(ACyan + "Welcome user -> " + AYellow + telnet.getIP());
  String user = AWhite + getASCIITime() + tab + ABrightYellow + "Telnet User@" + telnet.getIP() + tab + ABrightGreen + "logged in sucessfully!";
  logToSD(user);
  telnet.println(tab + AGreen + "Type '?' or 'help' for commands" + AReset);
  responsePrompt(1, 0, 1);
  drawBanner();
  display1.setCursor(45, 15);
  display1.print("Telnet:");
  display1.setCursor(25, 25);
  display1.print("connected to");
  display1.setCursor(30, 40);
  display1.print(telnet.getIP());
  display1.display();
}

void OnTelnetDisconnect(String ip) {
  drawBanner();
  display1.setCursor(45, 15);
  display1.print("Telnet:");
  display1.setCursor(15, 25);
  display1.print("disconnected from");
  display1.setCursor(30, 40);
  display1.print("IP Address:");
  display1.setCursor(27, 50);
  display1.print(telnet.getIP());
  display1.display();
  logToSD(AWhite + getASCIITime() + ABrightRed + " --- user disconnected@" + telnet.getIP() + nl);
}

void OnTelnetReconnect(String ip) {
  drawBanner();
  display1.setCursor(45, 15);
  display1.print("Telnet:");
  display1.setCursor(15, 25);
  display1.print("reconnected to");
  display1.setCursor(30, 40);
  display1.print("IP Address:");
  display1.setCursor(27, 50);
  display1.print(telnet.getIP());
  display1.display();
  logToSD(AWhite + getASCIITime() + ABrightBlue + " +++ user re-connected@" + telnet.getIP() + nl);
}

void OnTelnetConnectionAttempt(String ip) {
  drawBanner();
  display1.setCursor(50, 15);
  display1.print("Telnet:");
  display1.setCursor(15, 25);
  display1.print("rejected");
  display1.setCursor(30, 40);
  display1.print("IP Address:");
  display1.setCursor(27, 50);
  display1.print(telnet.getIP());
  display1.display();
  logToSD(AWhite + getASCIITime() + ABrightRed + " --- user rejected@" + telnet.getIP() + nl);
}

void setupTelnet() {
  // passing on functions for various telnet events
  telnet.onConnect(OnTelnetConnect);
  telnet.onConnectionAttempt(OnTelnetConnectionAttempt);
  telnet.onReconnect(OnTelnetReconnect);
  telnet.onDisconnect(OnTelnetDisconnect);
  // passing a lambda function
  telnet.onInputReceived([](String str) {
    // checks for a command entered by user at terminal.
    parseCommand(str);
  });
  drawBanner();
  display1.setCursor(45, 15);
  display1.print("Telnet:");
  if (telnet.begin()) {
    display1.setCursor(20, 25);
    display1.print("service running");
    display1.display();
  } else {
    errorMsg("Reboot in 2 sec...");
  }
}

#endif
