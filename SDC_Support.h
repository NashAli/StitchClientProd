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

  I2C Device Listing
  0x20 - MCP23017 Port Expander - Stepper Motor Drivers
  0x21 - MCP23017 - Aux Controller
  0x3C - OLED - used
  0x68 - MPU6050 - Head motion detect.

  DRV8825 - stepper motor driver PCB


*/

// SD Card
#define SD_CS 5     //GPIO5 is available in this project.

#include "FS.h"
#include "SD.h"                  //  SD Card

File root;
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
    delay(5000);
  }
  SD.end();
}

void printDirectory(File dir, int numTabs)
{
  // Begin at the start of the directory
  dir.rewindDirectory();
  uint8_t ypos = 80;
  while (true) {
    File entry =  dir.openNextFile();
    ypos = ypos + 12;
    if (! entry) {
      // no more files
      break;
    }
    // print the 8.3 name to oled
    display1.setCursor(20, ypos);
    display1.print(entry.name());
    // Recurse for directories, otherwise print the file size
    if (entry.isDirectory()) {
      String ename = entry.name();
      display1.setCursor(ename.length() + 65, ypos);
      display1.print("/");
      ypos = ypos + 12;
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      display1.setCursor(180, ypos);
      display1.print(entry.size());
    }
    entry.close();
  }
  delay(1000);
}


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
  delay(4000);
}

void LogToSD(String dataString) {

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    display1.print("error opening datalog.txt");
  }

}
void DumpLog() {
  if (SDCARD_READY) {
    display1.print("card ready! writing to port:");
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open("datalog.txt");
    // if the file is available, write to it:
    if (dataFile) {
      while (dataFile.available()) {
        Serial.write(dataFile.read());
      }
      dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
      display1.print("error opening datalog.txt");
    }
  }
}

// List the SD card (DON'T MODIFY ANY OF THESE FOLLOWING FUNCTIONS)
void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char * path) {
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char * path) {
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path)) {
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}


// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
// rename File (DON'T MODIFY THIS FUNCTION)
void renameFile(fs::FS &fs, const char * path1, const char * path2) {
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}
// Delete File (DON'T MODIFY THIS FUNCTION)
void deleteFile(fs::FS &fs, const char * path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

void testFileIO(fs::FS &fs, const char * path) {
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file) {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }


  file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++) {
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}


#endif
