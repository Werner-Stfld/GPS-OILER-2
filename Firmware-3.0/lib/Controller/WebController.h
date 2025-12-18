#pragma once

#include <Arduino.h>                  //
#include <WiFi.h>              // Für WIFI
#include <WebServer.h>         // Für WIFI
#include <LittleFs.h>                 // LittleFS library
#include <ArduinoJson.h>              // Json Bibliothek

#include "globals.h"
#include "uservar.h"

class JsonEndpoint {
  public: 
  const char *uri;
  void (*handle) (JsonDocument &);
  JsonEndpoint(const char *u, void (*h) (JsonDocument &)): uri(u), handle(h) {}
};

class WebController: VarContainer {
  const char *defaultSSID = "GPS-OILER";
  const char *defaultPassword = "12345678";
  // WiFi
  const byte my_WiFi_Mode = 2;              // WIFI_STA = 1 = Workstation  WIFI_AP = 2  = Accesspoint
  IPAddress local_ip = IPAddress(192, 168, 4, 1); // Die Festgelegte IP Adresse des AP
  unsigned long TimeAPoutmillis;             // Variable für die Abschaltung des AP
  boolean activ = true;                      // Variable wird zurückgesetzt wenn Timeout für Access Point erreicht.
  WebServer *server = new WebServer(80);

  static String getContentType(String filename) {
    if (filename.endsWith(".htm")) return "text/html";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    return "text/plain";
  }

  // web file server prvides the web files contained in the data folder to the browser
  void ServeFile(String path)
  {
    File file = LittleFS.open(path, "r");
    server->streamFile(file, getContentType(path));
    file.close();
  }

  void ServeFile(String path, String contentType)
  {
    File file = LittleFS.open(path, "r");
    server->streamFile(file, contentType);
    file.close();
  }

  bool HandleFileRead(String path) 
  { 
    if (path.endsWith("/")) path += "index.html";
    Serial.println("handleFileRead: " + path);
    
    if (LittleFS.exists(path)) 
    {
      ServeFile(path);
      return true;
    }

    Serial.println("file not found: " + path);
    return false;
  }

  void handleNotFound() {
    const char *notFound = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <link rel="icon" href="data:,">
    <title>404 - Page Not Found</title>
    <style>
        h1 {color: #ff4040;}
    </style>
</head>
<body>
    <h1>404</h1>
    <p>Oops! The page you are looking for could not be found.</p>
</body>
</html>
)=====";
    server->send(404, "text/plain", notFound);
  }

  void RetriggerAPTimeout() {
    TimeAPoutmillis = millis() + TimeAPout.get() * 1000 * 60; 
  }

  void putHandler (void (*put) (JsonDocument & doc)) {
    RetriggerAPTimeout();
    String json = server->arg("plain");
    Serial.println(json.c_str());

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
      Serial.println("Error reading json format: ");
      Serial.println(error.c_str());
      server->send(400);
      return;
    }
    put(doc);
    server->send(204);
  }

  void getHandler(void (*get) (JsonDocument & doc)) {
    RetriggerAPTimeout();
    String json;
    JsonDocument doc;
    get(doc);
    serializeJson(doc, json);
    server->send(200, "application/json", json);
  }

  class HandlerContext {
    WebController *controller;
    void (*handler) (JsonDocument & doc);
    public:
    HandlerContext(WebController *ctrl, void (*h) (JsonDocument & doc)): controller(ctrl), handler(h) {}
    void get () {
      controller->getHandler(handler);
    };
    void put () {
      controller->putHandler(handler);
    }
  };

  void SetupWebServer(JsonEndpoint *getEndpoints, JsonEndpoint *putEndpoints) {

    for (int i = 0; getEndpoints[i].handle != nullptr;i++) {
      HandlerContext *ctx = new HandlerContext(this, getEndpoints[i].handle);
      server->on(getEndpoints[i].uri, HTTP_GET, [ctx] () { ctx->get(); });
    }

    for (int i = 0; putEndpoints[i].handle != nullptr;i++) {
      HandlerContext *ctx = new HandlerContext(this, putEndpoints[i].handle);
      server->on(putEndpoints[i].uri, HTTP_PUT, [ctx] () { ctx->put(); });
    }
    if (!LittleFS.begin()) {
      Serial.println("LittleFS konnte nicht gestartet werden");
    }
    
    server->onNotFound([&]() {
      if (!HandleFileRead(server->uri()))
        handleNotFound();
    });

    server->begin();
  }

  void StartOwnAccessPoint()
  {
    Serial.println("Starting Access Point");
    WiFi.mode(WIFI_AP); // Accesspoint

    WiFi.softAP(ssid_ap.get(), password_ap.get());
    Serial.println("");
    Serial.print("Started AP:\t");
    Serial.println(WiFi.softAPSSID());
    Serial.print("IP address:\t");
    Serial.println(WiFi.softAPIP());
  }

  void ConnectToExistingAccessPoint()
  {
    WiFi.mode(WIFI_STA); // Access foreign Accesspoint
    WiFi.begin("network", "network-key");
    while(WiFi.status() != WL_CONNECTED)
    {
      delay(200);
      Serial.println(".");
    }
    Serial.println("");
    Serial.print("Connected to:\t");
    Serial.println(WiFi.SSID());
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());
  }

  static int checkBoundsTimeAPout(int v) {
    if (v < 1) 
      v = 1;
    return v;
  }
public:

  StringVar ssid_ap = StringVar("SSID", defaultSSID, PrefKeys::ssid_ap);                          // Die SSID
  StringVar password_ap = StringVar("Password", defaultPassword, PrefKeys::password_ap);                       // alternativ :  = "12345678";
  IntVar TimeAPout = IntVar(String("AP Timeout"), 5, PrefKeys::TimeAPout, checkBoundsTimeAPout);  // Zeit in Minuten bis sich der AP wieder abschaltet

  WebController() {
    add(&ssid_ap);
    add(&password_ap);
    add(&TimeAPout);
  }

  void flush() {
    ssid_ap.flush();
    password_ap.flush();
    TimeAPout.flush();
  }

  void setup(JsonEndpoint *getEp, JsonEndpoint *putEp)
  {
    ssid_ap.restore();
    password_ap.restore(); 
    TimeAPout.restore();
    StartOwnAccessPoint();
    SetupWebServer(getEp, putEp);
    RetriggerAPTimeout();
  };

  void loop()
  {
    if (activ) // do nothing if activ == false
    {
      if (millis() >= TimeAPoutmillis)
      {
        // WiFi.mode(WIFI_OFF);
        // activ = false;
      }
      server->handleClient();
    }
#ifdef HW_PINS_DEFINED
    if (digitalRead(WLAN_RESET_PIN) == LOW)
    {
      ssid_ap.set(defaultSSID, SetMode::flush);
      password_ap.set("", SetMode::flush);
    }
#endif
  };
};

extern WebController webController;
