#pragma once

#include <Arduino.h>                  //
#include <WiFi.h>              // Für WIFI
#include <WebServer.h>         // Für WIFI
#include <LittleFs.h>                 // LittleFS library
#include <ArduinoJson.h>              // Json Bibliothek
#include <esp_system.h>

#include "globals.h"
#include "uservar.h"

class JsonEndpoint {
  public: 
  const char *uri;
  http_method method;
  void (*handle) (JsonDocument &);
  JsonEndpoint(const char *u, http_method m, void (*h) (JsonDocument &)): uri(u), method(m), handle(h) {}
};

class WebController: public VarContainer {
  const char *defaultSSID = "GPS-OILER";
  const char *defaultPassword = "12345678";
  IPAddress local_ip = IPAddress(192, 168, 4, 1); // Die Festgelegte IP Adresse des AP
  boolean activ = true;                      // Variable wird zurückgesetzt wenn Timeout für Access Point erreicht.
  bool filesystemExists = false;
  WebServer *server = new WebServer(80);

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
    <h1>404 - Page Not Found</h1>
    <p>The page you are looking for could not be found.</p>
    <p>The file system may be missing on the device.</p>
</body>
</html>
)=====";
    server->send(404, "text/html", notFound);
  }

  void putHandler (void (*put) (JsonDocument & doc)) {
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
    String json;
    JsonDocument doc;
    get(doc);
    serializeJson(doc, json);
    server->send(200, "application/json", json);
  }

  class HandlerContext {
    void (*handler) (JsonDocument & doc);
    public:
    WebController *controller;
    HandlerContext(WebController *ctrl, void (*h) (JsonDocument & doc)): controller(ctrl), handler(h) {}
    void get () {
      controller->getHandler(handler);
    };
    void put () {
      controller->putHandler(handler);
    }
  };

  void SetupWebServer(JsonEndpoint *endpoints) {

    for (int i = 0; endpoints[i].handle != nullptr;i++) {
      HandlerContext *ctx = new HandlerContext(this, endpoints[i].handle);
      if (endpoints[i].method == HTTP_GET) {
        server->on(endpoints[i].uri, endpoints[i].method, [ctx] () { ctx->get(); });
      }
      if (endpoints[i].method == HTTP_PUT) {
        server->on(endpoints[i].uri, endpoints[i].method, [ctx] () { ctx->put(); });
      }
    }

    if (LittleFS.begin()) {
      server->serveStatic("/", LittleFS, "/");
    } else {
      Serial.println("LittleFS: Unable to start file system.");
    }

    server->onNotFound([&]() {
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
    Serial.print("Started WIFI access point:\t");
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
  bool isWiFiStarted = false;
  JsonEndpoint *_endpoints;
public:
  void startWiFi() {
    Serial.println("starting wifi");
    if (activ) {
      Serial.println("WiFi already started");
      return;
    }
    StartOwnAccessPoint();
    SetupWebServer(_endpoints);
    activ = true;
  }

  StringVar ssid_ap = StringVar(defaultSSID, PrefKeys::ssid_ap);
  StringVar password_ap = StringVar(defaultPassword, PrefKeys::password_ap);

  WebController(JsonEndpoint *ep) {
    _endpoints = ep;
    add(&ssid_ap);
    add(&password_ap);
  }

  void resetWiFi() {
    ssid_ap.set(defaultSSID, SetMode::flush);
    password_ap.set(defaultPassword, SetMode::flush);
    esp_restart();
  }

  void setup()
  {
    restore();
    activ = false;
  };

  void loop()
  {
    if (activ)
    {
      server->handleClient();
    }
  };
};

extern WebController webController;
