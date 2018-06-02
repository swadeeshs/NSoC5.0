#include <WiFi.h>
//#include<WiFiClient.h>
#include "mainPage.h">
#include<WebServer.h>

char *ssid =  "ssid"; 
char *pass =  "password";

WebServer server(80);

void handleVal() {
  String s = mainpage;
  server.send(200, "text/html", s);
}

 
void setup() {
       Serial.begin(9600);
       delay(10);
               
       Serial.print("WiFi Connecting to ");
       Serial.println(ssid);
 
       WiFi.begin(ssid, pass); 
       while(WiFi.status() != WL_CONNECTED) {delay(200);}
       Serial.println("");
       Serial.print("WiFi connected at ");
       Serial.println(WiFi.localIP()); 

       server.on("/", handleVal);
       server.begin();
       delay(20);
       Serial.println("Server has Started");
}
 
void loop() {      
  server.handleClient();
}



