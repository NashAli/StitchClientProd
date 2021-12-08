#ifndef OLED_SUPPORT_H
#define OLED_SUPPORT_H
/*
  Created:      19:55 2021.12.06
  Last Updated: 00:23 2021.12.07
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

#include <Adafruit_GFX.h>
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
    sets up the display
*/
void InitDisplay() {
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
 * 
 */
void DrawBanner() {
  display1.clearDisplay();
  display1.drawRect(0, 0, 127, 63, WHITE);
  display1.setCursor(30, 5);
  display1.print(OSNAME);
  display1.display();
}

/*
 * 
 */
void ShowPosition() {
  DrawBanner();
  display1.setCursor(20, 15);
  display1.print("X:");
  display1.setCursor(30, 15);
  display1.print(XPOS);
  display1.setCursor(20, 25);
  display1.print("Y:");
  display1.setCursor(30, 25);
  display1.print(YPOS);
  display1.setCursor(20, 35);
  display1.print("HEAD: UP");
  display1.display();
}

/*
    Displays the current wifi setup
*/
void ShowWifi() {
  DrawBanner();
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
  delay(3000);
}



/*
  Diagnostics use - do not use

void ShowAccel() {
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
