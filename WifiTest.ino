
#include <Arduino.h>

#include <ESP8266WiFi.h>
//#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>
#include "Page.h"

#ifndef DEBUGGING
#define DEBUGGING(...)
#endif

#ifndef DEBUGGING_L
#define DEBUGGING_L(...)
#endif

const char* ssid = "SSID";
const char* password = "YourPassWord";
const char* host = "esp8266WebSocket";
unsigned long lastTimeHost = 0;
String g_strIP;
const char* SON = "GS:true";
const char* SOFF = "GS:false";


bool gb_onoffswitch;

//ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);



void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

  switch (type) {
    case WStype_DISCONNECTED:
      {
 //       Serial.printf("[%u] Disconnected!\n", num);
      }
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        //Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // send message to client
        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_TEXT:
      {
 //       Serial.printf("[%u] get Text: %s\n", num, payload);
        String Command = String((char *) &payload[0]);
        
        // if GetState Command arrives
            if(Command == "GS")
            {
            // send message to client          
            webSocket.sendTXT(num, gb_onoffswitch ? SON:SOFF);
            }
            if(Command.startsWith("WS:"))
            {
              gb_onoffswitch = Command.substring(3) == "true";
              digitalWrite(LED_BUILTIN,gb_onoffswitch);                          
              webSocket.broadcastTXT(gb_onoffswitch ? SON:SOFF);
            }
        
      }
      break;
      // if GetState Command arrives
      //      if(Command == "GS")
      //      {
      //      // send message to client
      //
      //      webSocket.sendTXT(num, "GS");
      //      }

      //        case WStype_BIN:
      //            Serial.printf("[%u] get binary lenght: %u\n", num, lenght);
      //            hexdump(payload, lenght);
      //
      //            // send message to client
      //            // webSocket.sendBIN(num, payload, lenght);
      //            break;
  }

}

void handleRoot() {
  
  //String htmlmessage = "";
  server.send(200, "text/html", htmlmessage_1 + g_strIP + htmlmessage_2);
}

void handleNotFound() {

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);

}

void WifiConnect() {

  // WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    IPAddress l_ip = WiFi.localIP();
    String ipStr = String(l_ip[0]) + '.' + String(l_ip[1]) + '.' + String(l_ip[2]) + '.' + String(l_ip[3]);
    g_strIP = ipStr;
  }
  DEBUGGING("Connected");
  DEBUGGING(WiFi.localIP());

}

void WebSocketConnect() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void MDNSConnect() {
   if(MDNS.begin("esp8266")) {
//        Serial.println("MDNS responder started");
    }
 
}

void HTTPUpdateConnect() {

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
//  httpUpdater.setup(&server);
  server.begin();
  DEBUGGING_L("HTTPUpdateServer ready! Open http://");
  DEBUGGING_L(host);
  DEBUGGING(".local/update in your browser\n");
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("BOOT Time");
  Serial.flush();
  
  gb_onoffswitch = false;  

  
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);

  //WiFiMulti.addAP(ssid, password);
  
  WifiConnect();

//while(WiFiMulti.run() != WL_CONNECTED) {
//        delay(100);
//    }
// IPAddress l_ip = WiFi.localIP();
//    String ipStr = String(l_ip[0]) + '.' + String(l_ip[1]) + '.' + String(l_ip[2]) + '.' + String(l_ip[3]);
//    g_strIP = ipStr;

  WebSocketConnect();

  MDNSConnect();

  HTTPUpdateConnect();
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", 81);
    
  digitalWrite(LED_BUILTIN,LOW);
}



void loop() {
    webSocket.loop();    
    server.handleClient();
}

