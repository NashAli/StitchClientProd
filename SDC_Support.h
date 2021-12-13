#ifndef _SDC_S_H
#define _SDC_S_H
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
  ULN2803 buffer driver
  DRV8825 - stepper motor driver PCB

  Subject to change, without notice!
*/

// SD Card
#define SD_CS 5     //GPIO5 is available in this project.

#include "FS.h"     //  file system
#include "SD.h"     //  SD Card


SPIClass spiSD(VSPI);
bool SDCARD_READY = false;

/*
  SDCARD functions - espressif origins
  using oled display
  GPIO23  MOSI
  GPIO19  MISO
  GPIO18  SCLK
  GPIO5   CS
*/


void InitializeSDCard(int cs)
{
  spiSD.begin(18, 19, 23, cs); //SCK,MISO,MOSI,SS //VSPI1
  if (!SD.begin(cs, spiSD)) {
    SDCARD_READY = false;
    display1.clearDisplay();
    display1.drawRect(0, 0, 127, 63, WHITE);
    display1.setCursor(30, 5);
    display1.print(OSNAME);
    display1.setCursor(10, 20);
    display1.print("Card Mount Failed");
    display1.setCursor(5, 40);
    display1.print("Insert a SD Card!");
    display1.display();
    delay(2000);
    return;
  }

  else {
    SDCARD_READY = true;
    display1.clearDisplay();
    display1.drawRect(0, 0, 127, 63, WHITE);
    display1.setCursor(30, 5);
    display1.print(OSNAME);
    display1.setCursor(10, 15);
    display1.print("Card present.");
    display1.setCursor(10, 25);
    // print the type of card
    uint8_t cardType = SD.cardType();
    switch (cardType) {
      case CARD_MMC:
        display1.print("Card type: MMC");
        break;
      case CARD_SD:
        display1.print("Card type: SD");
        break;
      case CARD_SDHC:
        display1.print("Card type: SDHC");
        break;
      default:
        display1.print("Card type: Unknown");
    }
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    uint64_t usedBytes = SD.usedBytes() / (1024 * 1024);
    uint64_t freeSpace = (SD.cardSize() - SD.usedBytes()) / (1024 * 1024);

    display1.setCursor(10, 35);
    display1.print("Size: ");
    display1.print(cardSize);
    display1.print(" MB");
    display1.setCursor(10, 45);
    display1.print("Used: ");
    display1.print(usedBytes);
    display1.print(" MB");
    display1.setCursor(10, 55);
    display1.print("Free: ");
    display1.print(freeSpace);
    display1.print(" MB");
    display1.display();
    delay(1000);
  }
}

/*
 * 
 */
void StopSDCard()
{
  SD.end();
}

/*
   Mount drive if exists
*/
void CheckSDCard(int cs)
{
  display1.clearDisplay();
  display1.setCursor(5, 10);
  display1.print("Looking for SD card...");
  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!SD.begin(cs)) {
    display1.setCursor(5, 20);
    display1.print("No SD card found!");
    display1.setCursor(5, 30);
    display1.print("No FAT16/FAT32 partition.");
    display1.setCursor(5, 40);
    display1.print("Data will not be available");
    display1.setCursor(5, 50);
    display1.print("Are you sure?");
    display1.display();
    delay(5000);
    SDCARD_READY = false;
    return;
  } else {
    SDCARD_READY = true;
    display1.setCursor(5, 20);
    display1.print("A card is present.");

  }
  // print the type of card
  switch (SD.cardType()) {
    case CARD_MMC:
      display1.setCursor(5, 30);
      display1.print("Card type: MMC");
      break;
    case CARD_SD:
      display1.setCursor(5, 30);
      display1.print("Card type: SD");
      break;
    case CARD_SDHC:
      display1.setCursor(5, 30);
      display1.print("Card type: SDHC");
      break;
    default:
      display1.setCursor(5, 30);
      display1.print("Card type: Unknown");
  }

  display1.setCursor(2, 40);
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  display1.print("SD Size: ");
  display1.print(cardSize);
  display1.print("MB");
  display1.display();
  delay(2000);
}

void LogToSD(String dataString) {
  File root;
  root.rewindDirectory();
  root = SD.open("/syslog.txt", FILE_APPEND);
  if (!root) {
    DrawBanner();
    display1.setCursor(20, 30);
    display1.print("no logs found.");
    display1.setCursor(20, 40);
    display1.print("re-creating....");
    display1.display();
    delay(250);

    //create new one here, if necessary
    String header = ABrightMagenta + "eMBOS -" + ABrightRed + " SYSTEM LOG - " + ABrightCyan + "MASTER";
    root.println(header);
    root.println(ABrightYellow + "System Log Started@:" + AWhite + GetASCIITime());
    String user = AWhite + GetASCIITime() + tab + "Telnet User@" + "default user" + tab + ABrightGreen + "logged in sucessfully!";
    root.println(user);
    root.close();
    display1.setCursor(20, 50);
    display1.print("done.");
    display1.display();
    delay(500);
  }
  delay (100);
  // if the file is available, append to it:
  if (root) {
    root.println(dataString);
    root.close();
  }
  // if the file isn't open, pop up an error:
  else {
    DrawBanner();
    display1.setCursor(30, 30);
    display1.print("log file");
    display1.setCursor(30, 40);
    display1.print("not opening.");
    display1.display();
  }
}
void DumpLog() {
  if (SDCARD_READY) {
    display1.print("card ready! writing to port:");
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File root = SD.open("/syslog.txt", FILE_READ);
    // if the file is available, write to it:
    if (root) {
      while (root.available()) {
        Serial.write(root.read());
      }
      root.close();
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening syslog.txt");
    }
  }
}

#endif
