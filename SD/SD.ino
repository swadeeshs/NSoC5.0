/*
 * Connect the SD card to the following pins:
 *
 * SD Card | ESP32
 *    D2       -
 *    D3       SS
 *    CMD      MOSI
 *    VSS      GND
 *    VDD      3.3V
 *    CLK      SCK
 *    VSS      GND
 *    D0       MISO
 *    D1       -
 */
#include "FS.h"
#include "SD.h"

int in;
String filename, dirname, temp;

void listDir(fs::FS &fs, const char * dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root)
    {
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory())
    {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file)
    {
        if(file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels)
            {
                listDir(fs, file.name(), levels -1);
            }
        } 
        else 
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void readFile(fs::FS &fs, const char * path)
{
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available())
    {
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message)
{
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message))
    {
        Serial.println("File written");
    }
    else 
    {
        Serial.println("Write failed");
    }
    file.close();
}

void deleteFile(fs::FS &fs, const char * path)
{
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path))
    {
        Serial.println("File deleted");
    }
    else 
    {
        Serial.println("Delete failed");
    }
}

void setup()
{
    Serial.begin(115200);
    if(!SD.begin())
    {
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC)
    {
        Serial.println("MMC");
    }
    else if(cardType == CARD_SD)
    {
        Serial.println("SDSC");
    }
    else if(cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    }
    else 
    {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

}

void loop()
{
    Serial.println("------------------------------------------------------------------------");
    Serial.print("Enter 1 to List contents of this directory\nEnter 2 to Create a file\nEnter 3 to Read from a file\nEnter 4 to Delete a file\nEnter 5 to see SDcard memory usage\n");
    Serial.println("------------------------------------------------------------------------");

    in = 5;//Serial.read();

    switch(in)
    {
      case 1: listDir(SD, "/", 0);
              break;
      case 2: Serial.println("");
              Serial.println("");
              Serial.println("Enter the name of the file");
              filename = Serial.read();
              Serial.println("Enter the content of the file");
              temp = Serial.read();
              writeFile(SD, (char *)filename.c_str(), (char *)temp.c_str());
              break;
      case 3: Serial.println("");
              Serial.println("");
              Serial.println("Enter the name of the file");
              filename = Serial.read();
              readFile(SD, (char *)filename.c_str());
              break;
      case 4: Serial.println("");
              Serial.println("");
              Serial.println("Enter the name of the file");
              filename = Serial.read();
              deleteFile(SD, (char *)filename.c_str());
              break;
      case 5: Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
              Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
              break;
      default:Serial.println("Enter a valid choice");
              break;
    }
    while(1)
    delay(2000);
}
