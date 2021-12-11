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
   ANSI Terminal ESC sequences
*/
String ABlack =  "\u001b[30m";
String ARed =  "\u001b[31m";
String AGreen =  "\u001b[32m";
String AYellow =  "\u001b[33m";
String ABlue =  "\u001b[34m";
String AMagenta =  "\u001b[35m";
String ACyan =  "\u001b[36m";
String AWhite =  "\u001b[37m";
String AReset =  "\u001b[0m";

String ABrightBlack = "\u001b[30;1m";
String ABrightRed = "\u001b[31;1m";
String ABrightGreen = "\u001b[32;1m";
String ABrightYellow = "\u001b[33;1m";
String ABrightBlue = "\u001b[34;1m";
String ABrightMagenta = "\u001b[35;1m";
String ABrightCyan = "\u001b[36;1m";
String ABrightWhite = "\u001b[37;1m";

String ABold = "\u001b[1m";
String ClearScreen = "\u001b[2J";
String HomeCursor = "\u001b[;H";

String ASlowBlink = "\u001b[5m";
String tab = "\u0009";
String crs =  "\u00A9";
/*
   rr - response required if '1'
   rt - 0 "OK", 1 "error", 2 "busy", 3 "Access Denied", 4 "Not Found", 5 "unknown".
   pt - prompt type (symbol) 0 - 4.
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
      telnet.println(AReset);
      break;
    case 1:
      telnet.print(ABrightYellow + "$" + AReset);
      break;
    case 2:
      telnet.print(ABrightCyan + "~" + AReset);
      break;
    case 3:
      telnet.print(ABrightRed + ">" + AReset);
      break;
    case 4:
      telnet.print(ABrightYellow + "-" + AReset);
      break;
    default:
      // if nothing else matches, do this
      telnet.print(ABrightYellow + "$" + AReset);
      break;
  }
}

/*
  needs work !!!
  now if you are connected via telnet via the AP then the "No networks found" might make sense
  but on the other hand, if connected via Router then No Networks found, doesn't get there!!!
*/
void TelNetScan() {
  SYSTEM_BUSY = true;
  ResponsePrompt(0, 0, 3); //system prompt
  telnet.println( AGreen + "Scannning the Local Area Network..." + AReset);
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
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
      if (i < 9){
        telnet.print("0");
        }
      telnet.print(sc);
      telnet.print(ABrightBlue + ":" + AReset);
      if (sn == " " || sn == "") {
        sn = AYellow + "unknown" + AReset;
      }
      telnet.print(ABrightCyan + sn + AReset);
      telnet.print(ABrightYellow + " (" + AReset);
      telnet.print(ss);
      telnet.print(ABrightYellow + ")" + AReset);

      if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) {
        telnet.println(ABrightGreen + " Open");
      }
      if (WiFi.encryptionType(i) == WIFI_AUTH_WEP) {
        telnet.println(ARed + " WEP");
      }
      if (WiFi.encryptionType(i) == WIFI_AUTH_WPA_PSK) {
        telnet.println(ARed + " WPA-PSK");
      }
      if (WiFi.encryptionType(i) == WIFI_AUTH_WPA2_PSK) {
        telnet.println(ABrightRed + " WPA2-PSK");
      }
      if (WiFi.encryptionType(i) == WIFI_AUTH_WPA_WPA2_PSK) {
        telnet.println(ARed + " WPA-WPA2-PSK");
      }
      if (WiFi.encryptionType(i) == WIFI_AUTH_WPA2_ENTERPRISE) {
        telnet.println(ABrightMagenta + " WPA2-ENTERPRISE");
      }
      delay(10);
    }
  }
  telnet.println("");
  ResponsePrompt(1, 0, 1); //OK,system prompt
  SYSTEM_BUSY = false;
}

void TelFileDirectory(File dir, int numTabs) {
  while (true) {
    File entry =  dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      telnet.print(tab);
    }
    String en = String(entry.name());
    telnet.print(ABrightBlue + en);
    if (entry.isDirectory()) {
      telnet.println(AYellow + "/" + AReset);
      TelFileDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      telnet.print(tab + tab);
      String es = String(entry.size());
      telnet.println(AYellow + es + ACyan + " bytes" + AReset);
    }
    entry.close();
  }
}



void TelSysStat() {
  SYSTEM_BUSY = true;
  ResponsePrompt(0, 0, 2); //subsystem prompt
  telnet.println( ACyan + "Status:" + AReset);
  telnet.println(AYellow + "Current Local Time : " + ABrightWhite + GetASCIITime());
  telnet.print(ABrightBlue + "Local IP: " + ABrightYellow);
  telnet.println(WiFi.localIP().toString());
  telnet.println(AGreen + "Connected to IP Address: " + ABrightRed + telnet.getIP());
  telnet.println(AReset + "Access Point Address: 192.168.5.1");
  ResponsePrompt(1, 0, 1); //OK,system prompt
  SYSTEM_BUSY = false;
}
void TelCalibrate() {
  telnet.println(ABrightCyan + "Calibrate Machine.." + AReset);
  SYSTEM_BUSY = true;
  ResponsePrompt(1, 0, 2);  //OK,subsystem prompt
  telnet.println(ABrightRed + "Running Calibration Sequence!! - Standby..." + AReset);
  ShowPosition();
  delay(3000);
  ResponsePrompt(1, 0, 2);  //OK,subsystem prompt
  telnet.println(ABrightRed + "Calibration Done!" + AReset);
  ResponsePrompt(1, 0, 1);  //OK,system prompt
  SYSTEM_BUSY = false;
}
/*

*/
void RunLimitsTest() {
  SYSTEM_BUSY = true;
  ShowLimits();
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println(ABrightYellow + "See OLED," + ABrightRed + " done!" + AReset);
  ResponsePrompt(1, 0, 1); //system prompt
  SYSTEM_BUSY = false;
}
/*
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
   X Motor Test DRV8825
*/
void RunXTest() {
  SYSTEM_BUSY = true;
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
}
/*
   Y Motor Test DRV8825
*/
void RunYTest() {
  SYSTEM_BUSY = true;
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
}
/*
   I2C Bus scan
*/
void RunI2CBusScanTest() {
  SYSTEM_BUSY = true;
  ResponsePrompt(1, 0, 2); //OK, subsystem prompt only
  telnet.println(ABrightYellow + "Running i2c bus scan!.. Standby." + AReset);
  ResponsePrompt(1, 3, 1); //Access Denied!, system prompt
  SYSTEM_BUSY = false;
}
/*
  System Diagnostics - motor test, remote test,...
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
    RunLimitsTest();
  }
  if (p1 == "M" || p1 == "m") {
    telnet.println(ABrightYellow + "SelfTest " + ACyan + "set to test motor: " + ABrightBlue + p2 + AReset);
    if (p2 == "-X" || p2 == "-x") {
      RunXTest();
    }
    if (p2 == "Y" || p2 == "y") {
      RunYTest();
    }
    if (p2 == "B" || p2 == "b") {
      RunXTest();
      RunYTest();
    }
  }
  if (p1 == "I" || p1 == "i") {
    RunI2CBusScanTest();
  }
  if (p1 == "N" || p1 == "n") {
    telnet.println(ABrightYellow + "SelfTest " + ACyan + "- set to test needle motor: " + ABrightRed + "CAUTION!"  + AReset);
    if (p2 == "o" || p2 == "O") {
      telnet.println(AYellow + "One-Shot/Single Cycle"  + AReset);
      RunNeedleTest(0);
    }
    if (p2 == "c" || p2 == "C") {
      telnet.println(AYellow + "1 second continous run"  + AReset);
      RunNeedleTest(1);
    }
  }
  if (p1 == "C" || p1 == "c") {
    TestCalibration();
  }
}
/*

*/
void TelSysLogs() {
  telnet.println(ACyan + "Logs..." + AReset);
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println("none");
  ResponsePrompt(1, 0, 1); //system prompt
}

void TelSysFiles() {
  telnet.println(ABrightGreen + "File Listings..." + AReset);
  SD.begin();
  File root = SD.open("/");
  TelFileDirectory(root, 0);
  ResponsePrompt(1, 0, 1); //OK, system prompt
}

void TelSysJobs() {
  telnet.println( AGreen + "Jobs..." + AReset);
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println("no jobs in list");
  ResponsePrompt(1, 0, 1); //system prompt
}
void TelSysRun(String command) {
  telnet.println(ABrightGreen + "Run Job..." + AReset);
  ResponsePrompt(0, 1, 2); //system prompt only
  telnet.println("none to select");
  ResponsePrompt(1, 0, 1); //system prompt
}
void TelSysCancel() {
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println("nothing to cancel");
  ResponsePrompt(1, 0, 1); //system prompt
}
void TelSysHelp() {
  telnet.println(ABrightYellow + "              Help system!" + AReset);
  telnet.println();
  telnet.println(AMagenta + "    Commands:");
  telnet.println();
  telnet.println(AGreen + " netscan" + AWhite + "    - returns scan of network.");
  telnet.println(AGreen + " status" + AWhite + "     - returns complete status on system.");
  telnet.println(AGreen + " logs" + AWhite + "       - returns system logs.");
  telnet.println(AGreen + " run" + AWhite + "        - runs a job on the system.");
  telnet.println(AGreen + " files" + AWhite + "      - returns file system on machine.");
  telnet.println(AGreen + " jobs" + AWhite + "       - show available jobs.");
  telnet.println(AGreen + " cancel" + AWhite + "     - cancels job if one is running.");
  telnet.println(AGreen + " selftest" + AWhite + "   - selftest diagnostics on system.");
  telnet.println(AGreen + " calibrate" + AWhite + "  - runs calibration on system.");
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
  if (command.startsWith("selftest")) {
    TelSelfTest(command);
    ResponsePrompt(1, 0, 1);
  }
  if (command == "netscan" || command == "net" || command == "nets" || command == "netsc") {
    TelNetScan();
  }
  if (command == "calibrate" || command == "cal" || command == "cali" || command == "calib") {
    TelCalibrate();
  }
  if (command == "status" || command == "sta" || command == "stat" || command == "statu") {
    TelSysStat();
  }
  if (command.startsWith("run")) {
    TelSysRun(command);
  }
  if (command == "logs") {
    TelSysLogs();
  }
  if (command == "files") {
    TelSysFiles();
  }
  if (command == "jobs") {
    TelSysJobs();
  }
  if (command == "cancel") {
    TelSysCancel();
  }
  if (command == "help" || command == "?" || command == "help?" || command == "Help" || command == "HELP" || command == "HELP!") {
    TelSysHelp();
  }
  if (command.startsWith("#")) {
    ResponsePrompt(1, 0, 1); //system prompt
  }
  if (command.startsWith("W>")) {
    int se = command.length();
    int xTag = command.indexOf('X:');
    int yTag = command.indexOf('Y:', xTag + 1);
    int zTag = command.indexOf(' ', yTag + 1);
    String x_param = ABrightGreen + "X= " + command.substring(xTag + 1, yTag - 1);
    XDEST = command.substring(xTag + 1, yTag - 1).toInt();
    String y_param = ABrightGreen + "Y= " + command.substring(yTag + 1, zTag);
    YDEST = command.substring(yTag + 1, zTag).toInt();
    GotoPoint(XDEST, YDEST);
    telnet.println(x_param);
    telnet.println(y_param);
    telnet.print(AReset);
    ResponsePrompt(1, 0, 1);

  }
  if (command.startsWith("G>")) {
    int se = command.length();
    int xTag = command.indexOf('X:');
    int yTag = command.indexOf('Y:', xTag + 1);
    int endTag = command.indexOf(' ', yTag + 1);
    String x_param = ABrightMagenta + "X= " + command.substring(xTag + 1, yTag - 1);
    XDEST = command.substring(xTag + 1, yTag - 1).toInt();
    String y_param = ABrightMagenta + "Y= " + command.substring(yTag + 1, endTag);
    YDEST = command.substring(yTag + 1, endTag).toInt();
    GotoPoint(XDEST, YDEST);
    telnet.println(x_param);
    telnet.println(y_param);
    telnet.print(AReset);
    ResponsePrompt(1, 0, 1);
  }

  if (command.startsWith("M>")) {
    int Index1 = command.indexOf(' ');
    ResponsePrompt(1, 0, 1);
    int se = command.length();
    String m_param = command.substring(Index1, se);
    telnet.println(ABrightYellow + "MacroName: " + ABrightCyan + m_param + ".msf" + AReset);
    ResponsePrompt(1, 0, 1);
  }

  if (command.startsWith("T>")) {
    int Index1 = command.indexOf(' ');
    ResponsePrompt(1, 0, 1);
    int se = command.length();
    String t_param = command.substring(Index1, se);
    telnet.println(ABrightRed + "Thread Change - " + ABrightWhite + t_param + AReset);
    ResponsePrompt(1, 0, 1);
  }
  if (command.startsWith("C>")) {
    MachineRun = true;
    telnet.println(ABrightRed + "Job Paused" + AReset);
    ResponsePrompt(1, 0, 1);
  }
  if (command.startsWith("V>")) {
    int idx = command.indexOf(' ');
    int se = command.length();
    String blablabla = command.substring(idx + 1, idx + 2);
    if (blablabla == "0" ) {
      Verbosity = false;
    } else {
      Verbosity = true;
    }
    ResponsePrompt(1, 0, 1);  //OK, system prompt - normal completion.
  }

}
/*
 * *************************************************************************************************
*/
// callback functions for telnet events
void OnTelnetConnect(String ip) {
  telnet.println(ClearScreen);
  telnet.println(HomeCursor);
  telnet.println(ASlowBlink + ABrightRed + ABold + " eMBOS V1.1 " + AReset);
  telnet.println(" " + ABrightYellow + crs + "ETM Studios" + ABrightGreen + " All Rights Reserved 2021   " + AReset);
  telnet.print(ABrightCyan + " System Local Time: ");
  String nt = String(GetASCIITime());
  telnet.println(ABrightBlue + nt );
  telnet.println(ABrightMagenta +  ABold + " Telnet Service:");
  telnet.println(ACyan + " Welcome user -> " + AYellow + telnet.getIP());
  telnet.println(AGreen + " Type '?' or 'help' for commands" + AReset);
  ResponsePrompt(1, 0, 1);
  DrawBanner();
  display1.setCursor(45, 15);
  display1.print("Telnet:");
  display1.setCursor(25, 25);
  display1.print("connected to");
  display1.setCursor(30, 40);
  display1.print("IP Address:");
  display1.setCursor(27, 50);
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
    display1.setCursor(45, 25);
    display1.print("system will reboot");
    display1.display();
    delay(2000);
    errorMsg("Will reboot...");
  }
}


#endif
