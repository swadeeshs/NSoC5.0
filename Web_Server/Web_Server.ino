#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <SPI.h>
#include <SD.h>

#define DBG_OUTPUT_PORT Serial

const char* ssid = "mi4";
const char* password = "12344321";
const char* host = "smarthome";

WebServer server(80);

bool loggedIn = false;

bool checkSD()
{
  if (!SD.begin())
    {
      DBG_OUTPUT_PORT.println("Card Mount Failed");
      return false;
    }
    return true;
}

void handleNotFound()
{
  String message = "\nPage not found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  DBG_OUTPUT_PORT.print(message);
}

void serve()
{
  if(!checkSD())
   return;
  String path = server.uri();
  String dataType = "text/html";
  String response = "";

  if(path.endsWith("/")) path += "index.htm";

  if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if(path.endsWith(".htm")) dataType = "text/html";
  else if(path.endsWith(".css")) dataType = "text/css";
  else if(path.endsWith(".js")) dataType = "application/javascript";
  else if(path.endsWith(".png")) dataType = "image/png";
  else if(path.endsWith(".gif")) dataType = "image/gif";
  else if(path.endsWith(".jpg")) dataType = "image/jpeg";
  else if(path.endsWith(".ico")) dataType = "image/x-icon";
  else if(path.endsWith(".xml")) dataType = "text/xml";
  else if(path.endsWith(".pdf")) dataType = "application/pdf";
  else if(path.endsWith(".zip")) dataType = "application/zip";

  if(path == "/index.htm" && !loggedIn)
  {
    path = "/login.htm";
  }
  File dataFile = SD.open(path);
  if (!dataFile)
  {
    DBG_OUTPUT_PORT.println("Failed to open file for reading");
    handleNotFound();
    return;
  }
  DBG_OUTPUT_PORT.println("Reading file" + path);
  
  while (dataFile.available())  
  {
    char temp = char(dataFile.read());
    response += temp;//DBG_OUTPUT_PORT.print(temp);
  }

  for(int i=0; i<path.length(); i++)
  {
    if(path[i] == '/')
     dataFile.rewindDirectory();
  }

  dataFile.close();

  DBG_OUTPUT_PORT.println("Served " + dataType);
  server.send(200, dataType, response);
}


void handleLogin()
{
  String Email = "", Password = "";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    if(server.argName(i) == "email")
      Email = server.arg(i);
    if(server.argName(i) == "password")
      Password = server.arg(i);
     DBG_OUTPUT_PORT.println(" NAME:" + server.argName(i) + " VALUE:" + server.arg(i));
  }
  if(Email == "test@test.in" && Password == "12345")
  {
    loggedIn = true;
    serve();
  }
  else
  {
    server.send(410, "text/plain", "access denied");
  }
}

void setup()
{
  DBG_OUTPUT_PORT.begin(115200);
  DBG_OUTPUT_PORT.setDebu`gOutput(true);
  DBG_OUTPUT_PORT.print("\n");
  if(!checkSD())
   return;
  
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    DBG_OUTPUT_PORT.println("No SD card attached");
    return;
  }

  DBG_OUTPUT_PORT.print("SD Card Type: ");
  if (cardType == CARD_MMC)
  {
    DBG_OUTPUT_PORT.println("MMC");
  }
  else if (cardType == CARD_SD)
  {
    DBG_OUTPUT_PORT.println("SDSC");
  }
  else if (cardType == CARD_SDHC)
  {
    DBG_OUTPUT_PORT.println("SDHC");
  }
  else
  {
    DBG_OUTPUT_PORT.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  DBG_OUTPUT_PORT.printf("SD Card Size: %lluMB\n", cardSize);


  WiFi.begin(ssid, password);
  DBG_OUTPUT_PORT.print("Connecting to ");
  DBG_OUTPUT_PORT.println(ssid);

  // Wait for connection
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20)
  {
    //wait 10 seconds
    delay(500);
  }
  if (i == 21)
  {
    DBG_OUTPUT_PORT.print("Could not connect to ");
    DBG_OUTPUT_PORT.println(ssid);
    while (1) delay(500);
  }
  DBG_OUTPUT_PORT.print("Connected! IP address: ");
  DBG_OUTPUT_PORT.println(WiFi.localIP());

  if (MDNS.begin(host))
  {
    MDNS.addService("http", "tcp", 80);
    DBG_OUTPUT_PORT.println("MDNS responder started");
    DBG_OUTPUT_PORT.print("You can now connect to http://");
    DBG_OUTPUT_PORT.print(host);
    DBG_OUTPUT_PORT.println(".local");
  }
  
  server.on("/", HTTP_GET, serve);
  server.on("/css/style.css", HTTP_GET, serve);
  server.on("/script.js", HTTP_GET, serve);
  server.on("/", HTTP_POST, handleLogin);
  server.onNotFound(handleNotFound);

  server.begin();
  DBG_OUTPUT_PORT.println("HTTP server started");

}

void loop()
{
  server.handleClient();
}
