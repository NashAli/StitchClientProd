#ifndef _COMMS_H
#define _COMMS_H
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
  3 - 1/8   (not used)
  4 - 1/16  (not used)
  5 - 1/32  (not used)

  Subject to change, without notice!
*/


//  Let's start with some network basic communications... bluetooth, telnet, http, ssh, ftp...
/* ---------------------------- NETWORK ----------------------------------------------------- */
/*
   Tests for an available network to which the SewMachine may connect with. If none is found
   the AP is turned on and the user can connect without the local network.
*/
bool noLocal() {
  return (WiFi.status() == WL_NO_SSID_AVAIL);
}
/*
   Keeps track of the network status.
*/
bool isConnected() {
  return (WiFi.status() == WL_CONNECTED);
}


/*
  This initializes the AP setup. This can be done with or without an available network. Typically
  if no network is found or none available to connect to; the SewMachine AP will be activated for
  communications.
*/
void StartAP() {
  // Setting the ESP as an access point
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssidAP, appwd, 13, 0, 1);
  //wait for SYSTEM_EVENT_AP_START
  delay(200);
  if (!WiFi.softAPConfig(IPAddress(192, 168, 5, 1), IPAddress(192, 168, 5, 1), IPAddress(255, 255, 255, 0))) {
    DrawBanner();
    display1.setCursor(10, 20);
    display1.print("AP Setup Failed!");
    display1.display();
  }
  IPAddress APIP = WiFi.softAPIP();
  // Create http WebServer object on port 80
  httpServer.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SD, "/index.html", "text/html");
  });

  httpServer.serveStatic("/", SD, "/home/").setFilter(ON_AP_FILTER);
  httpServer.begin();
  DrawBanner();
  display1.setCursor(20, 20);
  display1.print("AP IP address:");
  display1.setCursor(30, 35);
  display1.print(APIP);
  ShowTime();
  display1.display();

}

/*
   Bluetooth connect
*/
bool check_wifiUpdate() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    return true;
  }
  //Check if we receive anything from Bluetooth
  else if (bt.available()) {
    interval = 50000;
    buffer_in = bt.readStringUntil('\n'); //Read what we recevive
    delay(20);

    if (buffer_in.startsWith("Connect:")) {
      int sTag = buffer_in.indexOf('Connect:') + 1;
      int seTag = buffer_in.indexOf(',');
      int pTag = buffer_in.indexOf(',') + 1;
      String s_param = buffer_in.substring(sTag, seTag);
      String p_param = buffer_in.substring(pTag, buffer_in.length() - 1);
      bt.println("Station:" + s_param );
      bt.println("Password:" + p_param);
      bt.println( "will be stored in EEPROM...");
      addr = 0;
      for (int i = 0; i < s_param.length(); i++) {
        valu = (byte)(s_param.charAt(i));
        EEPROM.write(addr, valu);
        addr++;
      }
      EEPROM.write(addr, 0x0A);
      addr++;
      for (int i = 0; i < p_param.length(); i++) {
        valu = (byte)(p_param.charAt(i));
        EEPROM.write(addr, valu);
        addr++;
      }
      EEPROM.write(addr, 0x0A);
      addr++;
      EEPROM.commit();
      bt.println("SSID & Password Stored!");
      bt.println("Network setup complete!");
      return true;
    }
    return false;
  } else {
    return false;
  }
}

/*

*/
void ConfigNetwork() {
  ShowBTStart();
  EEPROM.begin(25);
  bt.begin("eMBOS");
  while (!check_wifiUpdate()) {}
  //read eeprom first 25 bytes. later to modify to handle multiple networks.
  for (int i = 0; i < 25; i++) {
    valu = EEPROM.read(i);
    stream += (char)valu;
  }
  int seTag = stream.indexOf(0x0A);
  int peTag = stream.indexOf(0x0A, seTag + 2);
  temp = stream.substring(0, seTag + 1);
  temp2 = stream.substring(seTag + 1, peTag + 1 );
  int i = temp.length();
  int j = temp2.length();
  char ssid[i];
  char passw[j];
  temp.toCharArray(ssid, i);
  temp2.toCharArray(passw, j);
  ShowWifiCreds(ssid, passw);
  delay(5000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, passw);
  //connectToWiFi(ssid,passw);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    ShowMessage("No WiFi available");
    delay(1000);
    StartAP();
  } else {
    timeClient.begin();
    delay(50);
    timeClient.update();
    ShowWifi();
    ShowTime();
    httpServer.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(SD, "/index.html", "text/html");
    });
    httpServer.serveStatic("/", SD, "/www/").setFilter(ON_STA_FILTER);
  }
}

/*
   not used!
*/
bool connectToWiFi(const char* ssid, const char* password, int max_tries = 20, int pause = 500) {
  int i = 0;
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  do {
    delay(pause);
  } while (!isConnected() || i++ < max_tries);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  return isConnected();
}



/*
  SSH Start.
*/

void StartSSH() {
  //libssh_begin();
}
void StopSSH() {

}




#endif
