#include <BluetoothSerial.h>
#include "BluetoothSerial.h"

#define LED_BUILTIN 2
char data;

BluetoothSerial SerialBT;

void setup() 
{
  Serial.begin(115200);
  SerialBT.begin("ESP32"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() 
{
  if (SerialBT.available()) 
  {
    data=SerialBT.read();
    Serial.print(data);
    Serial.print("\n");
    if(data=='o')
    digitalWrite(2,HIGH);
    else if(data=='f')
    digitalWrite(2,LOW);
  }
}
        
      
    
  

