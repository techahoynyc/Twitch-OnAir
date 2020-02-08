/*
 *  TechAhoy - Twitch On Air
 *
 *  Adafruit HUZZAH32 controlling Neopixels via Twitch stream
 *
 *
 *  config.h - Private parameters
 */

#include "config.h"
#include <WiFiClientSecure.h>
#include <Adafruit_NeoPixel.h>

const char* wifi = WIFI;
const char* pass =  PASS;

WiFiClient client;
#define HOST "api.twitch.tv"  // remote server we will connect to
#define PATH "/helix/streams?user_login=techahoynyc"
#define REFRESH 20  // seconds between refresh
#define PIXELS 64 //number of neopixels
#define LED 13
#define PIN 21
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, PIN, NEO_GRB + NEO_KHZ800);  // change to NEO_GRBW + NEO_KHZ800 if using RGB + white strips

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    //delay(wait);
  }
}

void setup() {
  strip.begin();

  Serial.begin(115200);

  enableWifi();
}

void loop(){
  delay(5000);

  checkTwitch();
}

void enableWifi()
{
  WiFi.begin(wifi, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
}

void checkTwitch() //client function to send/receive GET request data.
{
  Serial.print("connecting to ");
  Serial.println(HOST);

  String result ="";
  WiFiClientSecure client;
  const int httpPort = 443;
  if (!client.connect(HOST, httpPort)) {
        Serial.println("connection failed");
        return;
  }
  // We now create a URI for the request
  Serial.print("Requesting URL: ");
  Serial.println(PATH);
  // This will send the request to the server
  client.print(String("GET ") + PATH + " HTTP/1.1\r\n" +
               "Host: " + HOST + "\r\n" +
               "Client-ID: hatgoiwwlupesk2wtgj6fb6neq7azx \r\n" +
               "Connection: close\r\n\r\n");

  int32_t timeout = millis() + 1000;
  while (client.available() == 0) {
    if (timeout - millis() < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  boolean isStreaming = false;
  while (client.connected()) {
    //Serial.print(client.readString());
    if (client.find("\"data\":[{\"id\":")) {
      isStreaming = true;
    }
  }

  Serial.print("Streaming status: "); Serial.println(isStreaming);
  //digitalWrite(LED, isStreaming);
  if (isStreaming) {
    colorWipe(strip.Color(255, 0, 0), 50); // Red
  } else {
    colorWipe(strip.Color(10, 10, 10), 50); // Off
    //colorWipe(strip.Color(128, 10, 128), 50); // purple
  }
  Serial.println("disconnecting from server.");
  client.stop();

  delay(REFRESH*1000);
}
