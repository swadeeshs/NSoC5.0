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
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

int in = 0;
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

void getFileName()
{
  Serial.println("");
  Serial.println("");
  //remove redundant chars
  while(in != 0)
  in = SerialBT.read();
  Serial.println("Unwanted characters removed");
  Serial.println("Enter the name of the file");
  //wait for the input
  while((in = SerialBT.read()) == 0);
  //append first character read
  filename = char(in);
  while(SerialBT.available())
  filename += char(SerialBT.read());            
}

void getContent()
{
  Serial.println("");
  //remove redundant chars
  while(in != 0)
  in = SerialBT.read();
  //Serial.println("Unwanted characters removed");
  Serial.println("Enter the content of the file");
  //wait for the input
  while((in = SerialBT.read()) == 0);
  //append first character read
  temp = char(in);
  while(SerialBT.available())
  temp += char(SerialBT.read());            
}

void setup()
{
    Serial.begin(115200);
    SerialBT.begin("ESP32test");
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
    while(in == 0)
    {
    in = SerialBT.read();
    delay(2000);
    }
    filename = "";
    switch(in-0x30)
    {
      case 1: getFileName();
              listDir(SD, (char *)filename.c_str(), 0);
              break;
      case 2: getFileName();
              getContent();
              writeFile(SD, (char *)filename.c_str(), (char *)temp.c_str());
              break;
      case 3: getFileName();
              readFile(SD, (char *)filename.c_str());
              break;
      case 4: getFileName();
              deleteFile(SD, (char *)filename.c_str());
              break;
      case 5: Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
              Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
              break;
      default:Serial.println("Enter a valid choice");
              break;
    }
    //remove redundant chars
    while(in != 0)
    {
    in = SerialBT.read();
    }
    delay(5000);
}
