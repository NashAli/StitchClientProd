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
void ResponsePrompt(int rr, int rt, int ptr) {

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
/*
  NETWORK *********************************************************************************************************
  - needs work !!!
  now if you are connected via telnet via the AP then the "No networks found" might make sense
  but on the other hand, if connected via a Router then No Networks found, doesn't get there!!!
*/
void TelNetScan() {
  SYSTEM_BUSY = true;
  ResponsePrompt(0, 0, 4); //subsystem prompt
  telnet.println( ACyan + "Scanning the Local Area Network..." + AReset);
  ResponsePrompt(0, 0, 4); //subsystem prompt
  int n = WiFi.scanNetworks();
  telnet.println( ABrightWhite + "Currently there are " + ABrightGreen + String(n) + ABrightWhite + " networks in the neighbourhood" + AReset);
  if (n == 0) {
    //no networks found go AP
    telnet.println(ABrightRed + "No Local Networks Found? - Connect to AP address: 192.168.5.1");
    telnet.println(ABrightGreen + "Open your wifi settings and find the address above." + AReset);
  } else {
    telnet.println(ABrightWhite + "List of available wifi connections:" + AReset);
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      String ss = String( WiFi.RSSI(i));
      String sc = String(i + 1);
      String sn = String(WiFi.SSID(i));
      ResponsePrompt(0, 0, 3); //system prompt
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
      telnet.print(ss);
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
    LogToSD(AWhite + GetASCIITime() + ABrightBlue + " +++ viewed networks.");
  }
  telnet.println("");
  ResponsePrompt(1, 0, 1); //OK,system prompt
  SYSTEM_BUSY = false;
}
/*
   Displays system network status.
*/

void TelSysStat() {
  startMillis = millis();  //initial start time
  SYSTEM_BUSY = true;
  ResponsePrompt(0, 0, 2); //subsystem prompt
  telnet.println( ACyan + "Status:" + AReset);
  telnet.println(AYellow + "Current Local Time : " + ABrightWhite + GetASCIITime());
  telnet.print(ABrightBlue + "Local IP: " + ABrightYellow);
  telnet.println(WiFi.localIP().toString());
  telnet.println(AGreen + "Connected to IP Address: " + ABrightRed + telnet.getIP());
  telnet.println(AReset + "Access Point Address: 192.168.5.1");
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  String tt = String(currentMillis - startMillis);
  telnet.println(ABrightRed + "TaskTime: " + tt + " mSec" + AReset);
  ResponsePrompt(1, 0, 1); //OK,system prompt
  SYSTEM_BUSY = false;
}
/*
   FILESYSTEM ***********************************************************************************
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
  startMillis = millis();
  telnet.println(ABrightGreen + "SD Card File Listings..." + AReset);
  //SD.begin(5);
  File root = SD.open(dir);
  TelFileDirectory(root, 0);
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  String tt = String(currentMillis - startMillis);
  telnet.println(ABrightRed + "TaskTime: " + tt + " mSec" + AReset);
  ResponsePrompt(1, 0, 1); //OK, system prompt
}
/*
   ********************************************** Make Dir on SD card
*/
void TelMakeDir(String param) {
  telnet.println("Creating Dir: " + param);
  if (SD.mkdir("/" + param)) {
    ResponsePrompt(0, 0, 3); //file prompt
    telnet.println("Dir created!");
    LogToSD(AWhite + GetASCIITime() + ABrightGreen + " +++ new dir created --> " + param);
  } else {
    ResponsePrompt(0, 0, 3); //file prompt
    telnet.println("mkdir failed");
    LogToSD(AWhite + GetASCIITime() + ABrightRed + " --- make dir fail --> " + param);
  }
  ResponsePrompt(1, 0, 1); //ok, top prompt
}
/*
  **************************************************  Delete Dir on SD card
*/
void TelRemoveDir(String param) {
  ResponsePrompt(0, 0, 3); //file prompt
  telnet.println("Removing Dir: " + param);
  if (SD.rmdir("/" + param)) {
    ResponsePrompt(0, 0, 3); //file prompt
    telnet.println("Dir removed!");
    LogToSD(AWhite + GetASCIITime() + ABrightGreen + " +++ dir removed --> " + param);
  } else {
    ResponsePrompt(0, 0, 3); //file prompt
    telnet.println("rmdir failed");
    LogToSD(AWhite + GetASCIITime() + ABrightRed + " --- remove dir fail --> " + param);
  }
  ResponsePrompt(1, 0, 1); //ok, top prompt
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
    LogToSD(AWhite + GetASCIITime() + ABrightBlue + " +++ file looked at--> " + param);
    ResponsePrompt(1, 0, 1); //OK, system prompt
  } else {
    ResponsePrompt(0, 0, 2);
    telnet.println("----fail");
    LogToSD(AWhite + GetASCIITime() + ABrightRed + " --- list file - fail.");
    ResponsePrompt(0, 0, 1); // just system prompt
  }
}
void TelRenameFile(String oldname, String newname) {

}
/*
  ********************************************************* Show the system log
*/
void TelShowSysLogs() {
  telnet.println(ACyan + "Logs..." + AReset);
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  if (SDCARD_READY) {
    if (! SD.exists("/syslog.txt")) {
      telnet.println(ABrightBlue + "creating new logfile..." + AReset);
      // only open a new file if it doesn't exist
      File myfile = SD.open("/syslog.txt", FILE_WRITE);
      String header = ABrightMagenta + "eMBOS -" + ABrightRed + " SYSTEM LOG - " + ABrightCyan + "MASTER";
      myfile.println(header);
      myfile.println(ABrightCyan + "System Log Started@:" + AWhite + GetASCIITime());
      String user = AWhite + GetASCIITime() + tab + ABrightYellow + "Telnet User@" + telnet.getIP() + tab + ABrightGreen + "logged in sucessfully!";
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
      ResponsePrompt(0, 0, 2);
      telnet.println(ABrightRed + "unknown error opening --> syslog.txt" + AReset);
      ResponsePrompt(1, 3, 1);  //  access denied, system prompt.
    }
  }
  ResponsePrompt(1, 0, 1); //OK, system prompt
}

/*
   *********************************************  Delete System logs
*/
void TelDeleteSysLogs() {
  startMillis = millis();  //initial start time
  ResponsePrompt(0, 0, 2);// subsystem prompt only
  telnet.println(ABrightRed + "Deleted, cannot be undone! --> syslog.txt" + AReset);
  SD.remove("/syslog.txt");
  ResponsePrompt(1, 0, 1);//  OK, system prompt
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  String tt = String(currentMillis - startMillis);
  telnet.println(ABrightRed + "TaskTime: " + tt + " mSec" + AReset);
}
/*
  ************************************************  SD Card Info
*/
void  TelSDCardInfo() {
  startMillis = millis();
  ResponsePrompt(0, 0, 2); // just sub-system prompt
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
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  String tt = String(currentMillis - startMillis);
  telnet.println(ABrightRed + "TaskTime: " + tt + " mSec" + AReset);
  ResponsePrompt(1, 0, 1); // OK, system prompt
}


/*
   MACHINE  ****************************************************************************************
   run the calibration sequence.
*/
void TelCalibrate() {
  startMillis = millis();
  telnet.println(ABrightCyan + "Calibrate Machine.." + AReset);
  SYSTEM_BUSY = true;
  ResponsePrompt(1, 0, 2);  //OK,subsystem prompt
  telnet.println(ABrightRed + "Running Calibration Sequence!! - Standby..." + AReset);
  ShowPosition();
  delay(3000);
  ResponsePrompt(1, 0, 2);  //OK,subsystem prompt
  telnet.println(ABrightRed + "Calibration Done!" + AReset);
  LogToSD(AWhite + GetASCIITime() + ABrightBlue + " +++ calibration done.");
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  String tt = String(currentMillis - startMillis);
  telnet.println(ABrightRed + "TaskTime: " + tt + " mSec" + AReset);
  ResponsePrompt(1, 0, 1);  //OK,system prompt
  SYSTEM_BUSY = false;
}
/*
  ********************************************************* Limits test
*/
void TelLimitsTest() {
  startMillis = millis();
  SYSTEM_BUSY = true;
  ShowLimits();
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightYellow + "See OLED," + ABrightRed + " done!" + AReset);
  LogToSD(AWhite + GetASCIITime() + ABrightGreen + " --> limits test done.");
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  String tt = String(currentMillis - startMillis);
  telnet.println(ABrightRed + "TaskTime: " + tt + " mSec" + AReset);
  ResponsePrompt(1, 0, 1); //system prompt
  SYSTEM_BUSY = false;
}
/*
   SELFTEST ************************************************************************************************
   Selftest Help
*/
void ShowTestHelp() {
  telnet.println(ABrightYellow + "           SelfTest Help!");
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
  SYSTEM_BUSY = true;
  LogToSD(AWhite + GetASCIITime() + ABrightBlue + " --> x-motor test started.");
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightRed + "running motor test X axis!" + AReset);
  telnet.println(ABrightYellow + "reverse - 1/1 step!" + AReset);
  for (int ctr = 1; ctr < STEPPERREV; ctr++) {
    MoveXMotor(!Forward, 0);
  }
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightYellow + "reverse - 1/2 step!" + AReset);
  for (int ctr = 1; ctr < STEPPERREV; ctr++) {
    MoveXMotor(!Forward, 1);
  }
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightYellow + "reverse - 1/4 step!" + AReset);
  for (int ctr = 1; ctr < STEPPERREV; ctr++) {
    MoveXMotor(!Forward, 2);
  }
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightBlue + "forward - 1/1 step!" + AReset);
  for (int ctr = 1; ctr < STEPPERREV; ctr++) {
    MoveXMotor(Forward, 0);
  }
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightBlue + "forward - 1/2 step!" + AReset);
  for (int ctr = 1; ctr < STEPPERREV; ctr++) {
    MoveXMotor(Forward, 1);
  }
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightBlue + "forward - 1/4 step!" + AReset);
  for (int ctr = 1; ctr < STEPPERREV; ctr++) {
    MoveXMotor(Forward, 2);
  }
  ResponsePrompt(1, 0, 2); //OK, subsystem prompt
  telnet.println(ABrightGreen + "completed." + AReset);
  SYSTEM_BUSY = false;
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  String tt = String(currentMillis - startMillis);
  telnet.println(ABrightRed + "TaskTime: " + tt + " mSec" + AReset);
  ResponsePrompt(1, 0, 1);
}
/*
   ******************************************************** Y Motor Test DRV8825
*/
void TelRunYTest() {
  startMillis = millis();
  SYSTEM_BUSY = true;
  LogToSD(AWhite + GetASCIITime() + ABrightBlue + " --> y-motor test started.");
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightRed + "running motor test Y axis!" + AReset);
  telnet.println(ABrightYellow + "reverse - 1/1 step!" + AReset);
  for (int ctr = 1; ctr < STEPPERREV; ctr++) {
    MoveYMotor(!Forward, 0);
  }
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightYellow + "reverse - 1/2 step!" + AReset);
  for (int ctr = 1; ctr < (STEPPERREV * 2); ctr++) {
    MoveYMotor(!Forward, 1);
  }
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightYellow + "reverse - 1/4 step!" + AReset);
  for (int ctr = 1; ctr < (STEPPERREV * 4); ctr++) {
    MoveYMotor(!Forward, 2);
  }
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightBlue + "forward - 1/1 step!" + AReset);
  for (int ctr = 1; ctr < STEPPERREV; ctr++) {
    MoveYMotor(Forward, 0);
  }
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightBlue + "forward - 1/2 step!" + AReset);
  for (int ctr = 1; ctr < (STEPPERREV * 2); ctr++) {
    MoveYMotor(Forward, 1);
  }
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightBlue + "forward - 1/4 step!" + AReset);
  for (int ctr = 1; ctr < ( STEPPERREV * 4); ctr++) {
    MoveYMotor(Forward, 2);
  }
  ResponsePrompt(1, 0, 2); //OK, subsystem prompt
  telnet.println(ABrightGreen + "completed." + AReset);
  SYSTEM_BUSY = false;
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  String tt = String(currentMillis - startMillis);
  telnet.println(ABrightRed + "TaskTime: " + tt + " mSec" + AReset);
  ResponsePrompt(1, 0, 1);
}
/*
   ***********************************************************  I2C Bus scan
*/
void TelRunI2CBusScanTest() {
  SYSTEM_BUSY = true;
  ResponsePrompt(1, 0, 2); //OK, subsystem prompt only
  telnet.println(ABrightYellow + "Running i2c bus scan!.. Standby." + AReset);
  LogToSD(AWhite + GetASCIITime() + ABrightBlue + " --> i2c test started.");
  ResponsePrompt(1, 3, 1); //Access Denied!, system prompt
  SYSTEM_BUSY = false;
}
/*
  ********************************************************* System Diagnostics - motor test, remote test,...
*/
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
    ShowTestHelp();
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
    TelRunI2CBusScanTest();
  }
  if (p1 == "N" || p1 == "n") {
    telnet.println(ABrightYellow + "SelfTest " + ACyan + "- set to test needle motor: " + ABrightRed + "CAUTION!"  + AReset);
    if (p2 == "o" || p2 == "O") {
      telnet.println(AYellow + "One-Shot/Single Cycle"  + AReset);
      LogToSD(AWhite + GetASCIITime() + ABrightBlue + " +++ needle - one shot.");
      RunNeedleTest(0); //  see support.h
    }
    if (p2 == "c" || p2 == "C") {
      telnet.println(AYellow + "1 second continous run"  + AReset);
      LogToSD(AWhite + GetASCIITime() + ABrightBlue + " +++ needle - one second burst.");
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
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println("no jobs in list");
  ResponsePrompt(1, 0, 1); //system prompt
}
/*
*************************************************** Run
*/
void TelSysRun(String param) {
  LogToSD(AWhite + GetASCIITime() + ABrightBlue + " +++ run job --> " + param);
  telnet.println(ABrightGreen + "Run Job..." + AReset);
  ResponsePrompt(0, 1, 2); //system prompt only
  telnet.println("none to select");
  ResponsePrompt(1, 0, 1); //system prompt

}
/*
********************************************  cancel
*/
void TelSysCancel(String param) {
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println("nothing to cancel");
  ResponsePrompt(1, 0, 1); //system prompt
}
/*
**********************************  HELP SYSTEM ************************************************
*/
void TelSysHelp() {
  telnet.println(AClearScreen + AHomeCursor);
  telnet.print(ABrightCyan + ABold + " " + golf_sym);
  telnet.print( ABrightRed + ABold + "          " + pat_sym + " " + ASlowBlink + "eMBOS" + AReset);
  telnet.println(ABrightYellow + ABold + tm_sym + AReset + ABrightGreen + "   V1.1" + AReset);
  telnet.println(" " + ABrightYellow + cr_sym + ABrightCyan + "ETM Studios" + ABrightGreen + " All Rights Reserved 2021/2022");

  telnet.println(ABrightYellow + "              Help system!" + AReset);
  telnet.println();
  telnet.println(AMagenta + "    Commands:");
  telnet.println();
  telnet.println(AGreen + " netscan" + AWhite + "      - returns scan of network.");
  telnet.println(AGreen + " status" + AWhite + "       - returns complete status on system.");
  telnet.println(AGreen + " log" + AWhite + "          - system log.");
  telnet.println(AGreen + " clr" + AWhite + "          - clears the screen.");
  telnet.println(AGreen + " run" + AWhite + "          - runs a job on the system.");
  telnet.println(AGreen + " sdcardinfo" + AWhite + "   - returns SD card info.");
  telnet.println(AGreen + " files" + AWhite + "        - list file system on machine.");
  telnet.println(AGreen + " list" + AWhite + "         - dumps file to screen.");
  telnet.println(AGreen + " mkdir" + AWhite + "        - creates a new directory.");
  telnet.println(AGreen + " rmdir" + AWhite + "        - deletes named directory.");
  telnet.println(AGreen + " rename" + AWhite + "       - renames a file.");
  telnet.println(AGreen + " jobs" + AWhite + "         - show available jobs.");
  telnet.println(AGreen + " cancel" + AWhite + "       - cancels job if one is running.");
  telnet.println(AGreen + " selftest" + AWhite + "     - selftest diagnostics on system.");
  telnet.println(AGreen + " calibrate" + AWhite + "    - runs calibration on system.");
  telnet.println(AGreen + " worklight" + AWhite + "    - turns on/off a worklight.");
  telnet.println();
  telnet.println(ABrightRed + "    SewMachine Direct Commands:");
  telnet.println();
  telnet.println(ABrightBlue + " W> X:<n> Y:<n>" + AWhite + "  - Hover to that location and wait.");
  telnet.println(ABrightBlue + " G> X:<n> Y:<n>" + AWhite + "  - cycle NEEDLE at that location.");
  telnet.println(ABrightBlue + " T> <colour>" + AWhite + "     - thread change.");
  telnet.println(ABrightBlue + " C>" + AWhite + "              - continue.");
  telnet.println(ABrightBlue + " M> <name>" + AWhite + "       - call macro - .msf loads file.");
  telnet.println(ABrightBlue + " V> <0:1>" + AWhite + "        - set verbosity.");
  telnet.println();
  telnet.println(ABrightWhite + "    Type" + ACyan + " [command]" + AYellow + "-" + ABrightGreen + "help" + AYellow + " / -" + ABrightGreen + "?" + ABrightBlue + " for more details." + AReset);
  telnet.println();
  ResponsePrompt( 1, 0, 1); //system prompt
}
/*
  ***************************************************** MISC  ******************************************
*/
void TelQuit() {
  telnet.println("Are you sure?");
  ResponsePrompt(1, 0, 0); //no system prompt
}


/* -------------------------- COMMAND PARSER ------------------------------------------------------- */

void ParseCommand(String command) {
  //parses the telnet entry and locates instructions and completes the tasks

  if (command.length() == 0) {
    ResponsePrompt(1, 5, 1); //unknown - system prompt
  }

  else if (command.startsWith("selftest")) {
    TelSelfTest(command);
    ResponsePrompt(1, 0, 1);
  }

  else if (command == "sdcardinfo" || command == "sdc" || command == "sdcard" || command == "sdci") {
    TelSDCardInfo();
  }

  else if (command == "netscan" || command == "net" || command == "nets" || command == "netsc") {
    TelNetScan();
  }

  else if (command == "calibrate" || command == "cal" || command == "cali" || command == "calib") {
    TelCalibrate();
  }

  else if (command == "status" || command == "sta" || command == "stat" || command == "statu") {
    TelSysStat();
  }

  else if (command == "clr") {
    telnet.println(AClearScreen + AHomeCursor);
    ResponsePrompt(0, 0, 1);
  }

  else if (command.startsWith("run")) {
    ResponsePrompt(1, 0, 1);
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      ResponsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "usage: run -<jobname>");
      ResponsePrompt(1, 0, 1);
    } else {
      ResponsePrompt(0, 0, 3);
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
      ResponsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "usage: log -<del:list>");
    }
    ResponsePrompt(1, 0, 1);
  }

  else if (command.startsWith("list")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      ResponsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "usage: list -<filename>");
      ResponsePrompt(1, 0, 1);
    } else {
      ResponsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "Screen Dump FileName: " + ABrightCyan + param + AReset);
      TelListFile(param);
    }
  }

  else if (command.startsWith("mkdir")) {
    ResponsePrompt(1, 0, 1);
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      ResponsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "usage: mkdir -<dirname>");
      //ResponsePrompt(1, 0, 1);
    } else {
      ResponsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "Create Directory: " + ABrightCyan + param + AReset);
      TelMakeDir(param);
    }
    ResponsePrompt(1, 0, 1);  //OK, top level prompt
  }

  else if (command.startsWith("rmdir")) {
    ResponsePrompt(1, 0, 1);
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      ResponsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "usage: rmdir -<dirname>");
      //ResponsePrompt(1, 0, 1);
    } else {
      ResponsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "Remove Directory: " + ABrightCyan + param + AReset);
      TelRemoveDir(param);
    }
    ResponsePrompt(1, 0, 1);  //OK, top level prompt
  }

  else if (command.startsWith("rename")) {
    ResponsePrompt(1, 0, 1);
    String param = command.substring(command.indexOf(' ') + 1, command.length());
    String newname = command.substring(command.indexOf(',') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      ResponsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "usage: rename <oldname,newname>");
      ResponsePrompt(1, 0, 1);
    } else {
      ResponsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "Renamed: " + ABrightCyan + param + " to " + newname + AReset);
      TelRenameFile(param, newname);
      LogToSD(AWhite + GetASCIITime() + ABrightRed + " --> file rename - " + param + " to " + newname);
    }
    ResponsePrompt(1, 0, 1);
  }

  else if (command.startsWith("files")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    TelSysFiles(param);
  }

  else if (command == "jobs") {
    TelSysJobs();
  }

  else if (command.startsWith("cancel")) {
    ResponsePrompt(1, 0, 1);
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      ResponsePrompt(0, 0, 2);
      telnet.println(ABrightYellow + "usage: cancel -<jobname>");
      ResponsePrompt(1, 0, 1);
    } else {
      ResponsePrompt(0, 0, 2);
      telnet.println(ABrightYellow + "Cancel Job: " + ABrightCyan + param + AReset);
      TelSysCancel(param);
      LogToSD(AWhite + GetASCIITime() + ABrightBlue + " --- cancel job --> " + param);
    }
    ResponsePrompt(1, 0, 1);
  }
  else if (command == "worklight") {
    Lamp(1);
    ResponsePrompt(0, 0, 1);
  }
  else if (command == "help" || command == "?" || command == "help?" || command == "Help" || command == "HELP" || command == "HELP!") {
    TelSysHelp();
  }

  else if (command.startsWith("#")) {
    ResponsePrompt(1, 0, 1); //OK, system prompt - normal completion.
  }

  else if (command.startsWith("W>")) {
    int se = command.length();
    int xTag = command.indexOf('X:');
    int yTag = command.indexOf('Y:', xTag + 1);
    int zTag = command.indexOf(' ', yTag + 1);
    String x_param = ABrightGreen + "X= " + command.substring(xTag + 1, yTag - 1);
    XDEST = command.substring(xTag + 1, yTag - 1).toInt();
    String y_param = ABrightGreen + "Y= " + command.substring(yTag + 1, zTag);
    YDEST = command.substring(yTag + 1, zTag).toInt();
    GotoPoint(XDEST, YDEST);
    ResponsePrompt(0, 0, 2);// machine prompt only.
    telnet.println(x_param);
    ResponsePrompt(0, 0, 2);// machine prompt only.
    telnet.println(y_param);
    telnet.print(AReset);
    ResponsePrompt(1, 0, 1);

  }

  else if (command.startsWith("G>")) {
    int se = command.length();
    int xTag = command.indexOf('X:');
    int yTag = command.indexOf('Y:', xTag + 1);
    int endTag = command.indexOf(' ', yTag + 1);
    String x_param = ABrightMagenta + "X= " + command.substring(xTag + 1, yTag - 1);
    XDEST = command.substring(xTag + 1, yTag - 1).toInt();
    String y_param = ABrightMagenta + "Y= " + command.substring(yTag + 1, endTag);
    YDEST = command.substring(yTag + 1, endTag).toInt();
    GotoPoint(XDEST, YDEST);
    ResponsePrompt(0, 0, 2);// machine prompt only.
    telnet.println(x_param);
    ResponsePrompt(0, 0, 2);// machine prompt only.
    telnet.println(y_param);
    telnet.print(AReset);
    ResponsePrompt(1, 0, 1);//OK, system prompt - normal completion.
  }

  else if (command.startsWith("M>")) {
    ResponsePrompt(0, 0, 2);// machine prompt only.
    String param = command.substring(command.indexOf(' ') + 1, command.length());
    telnet.println(ABrightYellow + "MacroName: " + ABrightCyan + param + ".msf" + AReset);
    ResponsePrompt(1, 0, 1);//OK, system prompt - normal completion.
  }

  else if (command.startsWith("T>")) {
    ResponsePrompt(0, 0, 2);// machine prompt only.
    //TODO: add a GLOBAL THREAD VALUE in "system.h"
    String t_param = command.substring(command.indexOf(' ') + 1, command.length());
    telnet.println(ABrightRed + "Thread Change - " + ABrightWhite + t_param + AReset);
    ResponsePrompt(1, 0, 1);//OK, system prompt - normal completion.
  }

  else if (command.startsWith("C>")) {
    if (MachineRun) {
      ResponsePrompt(0, 0, 2);// machine prompt only.
      telnet.println(ABrightRed + "Job Paused" + AReset);
      ResponsePrompt(1, 0, 1);//OK, system prompt - normal completion.
    }
  }

  else if (command.startsWith("V>")) {
    String blablabla = command.substring(command.indexOf(' ') + 1, command.length());
    if (blablabla == "0" ) {
      Verbosity = false;
    } else {
      Verbosity = true;
    }
    ResponsePrompt(1, 0, 1);  //OK, system prompt - normal completion.
  }

}
/*
 * *********************************  TELNET SETUP & FUNCTIONS ***********************************************
*/
// callback functions for telnet events
void OnTelnetConnect(String ip) {
  telnet.println(AClearScreen);
  telnet.println(AHomeCursor);
  telnet.print(ABrightCyan + ABold + " " + sman_sym);
  telnet.print( ABrightRed + ABold + "          " + pat_sym + " " + ASlowBlink + "eMBOS" + AReset);
  telnet.println(ABrightYellow + ABold + tm_sym + AReset + ABrightGreen + " V1.1" + AReset);
  telnet.println(" " + ABrightYellow + cr_sym + ABrightCyan + "ETM Studios" + ABrightGreen + " All Rights Reserved 2021/2022");
  telnet.print(ABrightCyan + " System Local Time: ");
  String nt = String(GetASCIITime());
  telnet.println(AWhite + nt );
  telnet.println(ABrightMagenta +  ABold + " Telnet Service:");
  telnet.println(ACyan + " Welcome user -> " + AYellow + telnet.getIP());
  String user = AWhite + GetASCIITime() + tab + ABrightYellow + "Telnet User@" + telnet.getIP() + tab + ABrightGreen + "logged in sucessfully!";
  LogToSD(user);
  telnet.println(AGreen + " Type '?' or 'help' for commands" + AReset);
  //telnet.println(" (Use ^] + q  to disconnect.)");
  ResponsePrompt(1, 0, 1);
  DrawBanner();
  display1.setCursor(45, 15);
  display1.print("Telnet:");
  display1.setCursor(25, 25);
  display1.print("connected to");
  display1.setCursor(30, 40);
  display1.print(telnet.getIP());
  display1.display();
}

void OnTelnetDisconnect(String ip) {
  DrawBanner();
  display1.setCursor(45, 15);
  display1.print("Telnet:");
  display1.setCursor(15, 25);
  display1.print("disconnected from");
  display1.setCursor(30, 40);
  display1.print("IP Address:");
  display1.setCursor(27, 50);
  display1.print(telnet.getIP());
  display1.display();
  LogToSD(AWhite + GetASCIITime() + ABrightRed + " --- user disconnected@" + telnet.getIP() + nl);
}

void OnTelnetReconnect(String ip) {
  DrawBanner();
  display1.setCursor(45, 15);
  display1.print("Telnet:");
  display1.setCursor(15, 25);
  display1.print("reconnected to");
  display1.setCursor(30, 40);
  display1.print("IP Address:");
  display1.setCursor(27, 50);
  display1.print(telnet.getIP());
  display1.display();
  LogToSD(AWhite + GetASCIITime() + ABrightBlue + " +++ user re-connected@" + telnet.getIP() + nl);
}

void OnTelnetConnectionAttempt(String ip) {
  DrawBanner();
  display1.setCursor(50, 15);
  display1.print("Telnet:");
  display1.setCursor(15, 25);
  display1.print("rejected");
  display1.setCursor(30, 40);
  display1.print("IP Address:");
  display1.setCursor(27, 50);
  display1.print(telnet.getIP());
  display1.display();
  LogToSD(AWhite + GetASCIITime() + ABrightRed + " --- user rejected@" + telnet.getIP() + nl);
}

void SetupTelnet() {
  // passing on functions for various telnet events
  telnet.onConnect(OnTelnetConnect);
  telnet.onConnectionAttempt(OnTelnetConnectionAttempt);
  telnet.onReconnect(OnTelnetReconnect);
  telnet.onDisconnect(OnTelnetDisconnect);
  // passing a lambda function
  telnet.onInputReceived([](String str) {
    // checks for a command entered by user at terminal.
    while (SYSTEM_BUSY) {}
    ParseCommand(str);
  });
  DrawBanner();
  display1.setCursor(45, 15);
  display1.print("Telnet:");
  if (telnet.begin()) {
    display1.setCursor(20, 25);
    display1.print("service running");
    display1.display();
  } else {
    errorMsg("Will reboot...");
  }
}


#endif
