#include <DHT.h>

#define DHTPIN 0

DHT dht(DHTPIN, DHT11); //Initializing the DHT sensor type with this code

void setup()
{
  Serial.begin(9600);
  delay(10);
}

void loop()
{
  float h = dht.readHumidity(); //Reading Humidity values from DHT11
  float t = dht.readTemperature(); //Reading Temperature values from DHT11
  Serial.println("Temperature:");
  Serial.println(t);
  Serial.print("Humidity:");
  Serial.println(h);
  delay(2000);
}
