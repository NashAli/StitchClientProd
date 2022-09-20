#ifndef OLED_SUPPORT_H
#define OLED_SUPPORT_H
/*
  Created:      19:55 2021.12.06
  Last Updated: 00:23 2021.12.07
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

//#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//***********************************************************************
//  DEFINES
//***********************************************************************
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -4// Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_ADDR   0x3C


/* --------------------------- OLED ------------------------------------------------------ */

Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/*
   Title:       InitDisplay
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Sets up the display
   Input:       nothing
   Returns:     nothing
*/
void initializeDisplay() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display1.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) { // Address 0x3C for 128x64

    for (;;); // Don't proceed, loop here forever!!
  }
  display1.setTextSize(1);
  display1.setTextColor(WHITE);
  display1.setRotation(0);
  display1.clearDisplay();
  display1.display();
  display1.clearDisplay();
  display1.drawRect(0, 0, 127, 63, WHITE);
  display1.setCursor(30, 5);
  display1.print(OSNAME);
  display1.display();
  delay(1000); // Pause for 1 seconds
}

/*
   Title:       DrawBanner
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Provides a new screen for UI
   Input:       nothing
   Returns:     nothing
*/
void drawBanner() {
  display1.clearDisplay();
  display1.drawRect(0, 0, 127, 63, WHITE);
  display1.setCursor(30, 5);
  display1.print(OSNAME);
  display1.display();
}
/*
   Title:       errorMsg
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Handler for a fatal error, one that requires that the system be possibly restarted
   Input:       String message, restart<true:false>
   Returns:     nothing
*/
void errorMsg(String error, bool restart = true) {
  if (restart) {
    drawBanner();
    display1.setCursor(5, 15);
    display1.print(error);
    display1.display();
    delay(2000);
    ESP.restart();
    delay(2000);
  }
}

/*
   Title:       ShowMessage
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Show a single line of info
   Input:       message
   Returns:
*/
void showMessage(String message) {
  drawBanner();
  display1.setCursor(10, 50);
  display1.print(message);
  display1.display();
  delay(1000);
}
/*
   Title:       ShowBigMessage
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description: Show a two line message
   Input:       message, mess1
   Returns:
*/
void showBigMessage(String message, String mess1) {
  drawBanner();
  display1.setCursor(10, 20);
  display1.print(message);
  display1.setCursor(10, 30);
  display1.print(mess1);
  display1.display();
  display1.display();
  delay(1000);
}
/*
   Title:       ShowLimits
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
*/
void showLimits() {
  drawBanner();
  display1.setCursor(30, 20);
  display1.print("X Y X Y N N");
  display1.setCursor(30, 30);
  display1.print("H H M M U D");
  display1.setCursor(30, 40);
  //read the limits and display results.
  display1.print("x x x x x x");
  display1.display();
}
/*
   Title:       ShowBTStart
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
*/
void showBTStart() {
  drawBanner();
  display1.setCursor(10, 20);
  display1.print("Bluetooth active!");
  display1.setCursor(20, 35);
  display1.print("for 30 seconds");
  display1.setCursor(5, 50);
  display1.print("Enter your creds now");
  display1.display();
  delay(2500);
  drawBanner();
  display1.setCursor(10, 20);
  display1.print("Bluetooth active!");
  display1.setCursor(20, 35);
  display1.print("If completed.");
  display1.setCursor(8, 50);
  display1.print("Wait for auto-login.");
  display1.display();
}
/*
   Title:       ShowWifiCreds
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
*/
void showWifiCreds(String ssid, String passw) {
  drawBanner();
  display1.setCursor(10, 20);
  display1.print("Connecting to..");
  display1.setCursor(5, 40);
  display1.print("Station:");
  display1.println(ssid);
  display1.setCursor(5, 50);
  display1.print("Password:");
  display1.println(passw);
  display1.display();
  delay(1000);
}
/*
   Title:       ShowPosition
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
*/
void showPosition() {
  drawBanner();
  display1.setCursor(20, 20);
  display1.print("X:");
  display1.setCursor(30, 20);
  display1.print(XCPOS);
  display1.setCursor(20, 30);
  display1.print("Y:");
  display1.setCursor(30, 30);
  display1.print(YCPOS);
  display1.setCursor(20, 40);
  display1.print("HEAD: UP");
  display1.display();
}
/*
   Title:       ShowWifi
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
*/
void showWifi() {
  drawBanner();
  display1.setCursor(7, 15);
  display1.print("Wifi connected to");
  display1.setCursor(30, 25);
  display1.print("IP Address:");
  display1.setCursor(27, 35);
  display1.print(WiFi.localIP().toString());
  display1.setCursor(35, 50);
  display1.print("RSSI: ");
  display1.println(WiFi.RSSI());
  display1.display();
  delay(1000);
}
/*
   Title:       ShowTime
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
*/
void showTime() {
  configTime(utcOffsetInSeconds, 3600, ntpServer);
  display1.setCursor(5, 50);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    display1.print("Failed to obtain time");
    display1.display();
    return;
  }
  display1.print(&timeinfo, "%a,%b,%d,%H:%M:%S");
  display1.display();
}
/*
   Title:       ShowFreeMemory
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:
*/
void showFreeMemory() {
  uint32_t ram = ESP.getFreeHeap();
  drawBanner();
  display1.setCursor(22, 22);
  display1.print("Available RAM:");
  display1.setCursor(28, 37);
  display1.print(ram);
  display1.println(" Bytes");
  display1.display();
  showTime();
}
/*
   Title:       ShowAccel
   Author:      zna
   Date:        01-22-22
   Version:     1.0.0
   Description:
   Input:
   Returns:

  Diagnostics use - do not use

  void showAccel()
  {
  sensors_event_t a, g;
  mpu.getEvent(&a, &g);
  display1.println("Accelerometer - m/s^2");
  display1.print(a.acceleration.x, 1);
  display1.print(", ");
  display1.print(a.acceleration.y, 1);
  display1.print(", ");
  display1.print(a.acceleration.z, 1);
  display1.println("");
  display1.println("Gyroscope - rps");
  display1.print(g.gyro.x, 1);
  display1.print(", ");
  display1.print(g.gyro.y, 1);
  display1.print(", ");
  display1.print(g.gyro.z, 1);
  display1.println("");
  display1.display();
  }

*/

#endif
