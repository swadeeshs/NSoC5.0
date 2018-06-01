#include <DHT.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
int input = 32;

BluetoothSerial SerialBT;
DHT dht(input, DHT11); //Initializing the DHT sensor type with this code

void BT_Print(String x)
{
  for(int i = 0; i < x.length(); i++)
  {
    SerialBT.write(x[i]);
    delay(20);
  }
}

void setup() 
{
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() 
{
  float h = dht.readHumidity(); //Reading Humidity values from DHT11
  float t = dht.readTemperature(); //Reading Temperature values from DHT11
  BT_Print("Temparature : ");
  BT_Print(String(t));
  BT_Print("\n");
  BT_Print("Humidity : ");
  BT_Print(String(h));
  BT_Print("\n");
  BT_Print("\n");
  delay(2000);
}
