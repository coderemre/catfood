#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#define USE_SERIAL Serial

#define repo "https://raw.githubusercontent.com/coderemre/catfood/master/firmware.bin"

#ifndef APSSID
#define APSSID "Keenetic 5 GHZ"
#define APPSK  "2646151sbl4654"
#endif

ESP8266WebServer server(80);
ESP8266WiFiMulti WiFiMulti;


void updateFirmware () {
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    WiFiClientSecure client;
    client.setInsecure();
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, repo);
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        USE_SERIAL.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        USE_SERIAL.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        USE_SERIAL.println("HTTP_UPDATE_OK");
        break;
    }
  }
}


void ledOn() {
  digitalWrite(LED_BUILTIN, HIGH);
  server.send(200, "text/plain", "hello from esp8266!");
}

void ledOff() {
  digitalWrite(LED_BUILTIN, LOW);
  server.send(200, "text/plain", "hello from esp8266!");
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


void setup() {
  USE_SERIAL.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  // USE_SERIAL.setDebugOutput(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(APSSID, APPSK);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(APSSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  updateFirmware();

  server.on("/?led=on", ledOn);
  server.on("/?led=off", ledOff);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
