#include <WiFi.h>

const char* ssid     = "swadeesh";
const char* password = "srivatsa";

WiFiServer server(80);

void setup()
{
    Serial.begin(115200);
    pinMode(2, OUTPUT);

    delay(10);

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

}

int value = 0;

void loop(){
 WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");        
    String currentLine = "";              
    while (client.connected()) {          
      if (client.available()) {           
        char c = client.read();          
        Serial.write(c);                 
        if (c == '\n') {                 

          if (currentLine.length() == 0) {

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print("Click <a href=\"/ON\">here</a> to turn the LED on.<br>");
            client.print("Click <a href=\"/OFF\">here</a> to turn the LED off.<br>");

            client.println();

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;    
        }

        
        if (currentLine.endsWith("GET /ON"))
        {
          digitalWrite(2, HIGH);
        }
        if (currentLine.endsWith("GET /OFF"))
        {
          digitalWrite(2, LOW);              
        }
      }
    }
    
    client.stop();
    Serial.println("Client Disconnected");
  }
  
}
