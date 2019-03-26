/* This Arduino sketch implements an Open Pixel Control server
 * for controlling FastLEDs from an ESP32.
 */

#include <WiFi.h>
#include <FastLED.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>    

/* Edit this to configure the number of pixels you have, and your SSID and password */
#define PIN 23       // pin that FastLEDs are connected to
#define N_PIXELS 60  // number of FastLEDs on your strip
#define WIFI_SSID   "XXXX"
#define WIFI_PASSWD "XXXX"

WiFiManager wifiManager;

WiFiServer server(7890);

// Define the array of leds
CRGB leds[N_PIXELS];


void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  wifiManager.autoConnect("ESP32-OPC-SERVER", "AP-PASSWORD");

  LEDS.addLeds<WS2812,PIN,GRB>(leds,N_PIXELS);
  LEDS.setBrightness(84);
  FastLED.show();
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  while (WiFi.status() != WL_CONNECTED) {
    //wait till wifi is connected
  }

  //start wifi server listening for connection on the 7890
  server.begin();

  // Port defaults to 3232
  ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    
    ArduinoOTA.begin();

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();
  WiFiClient client = server.available();
  if (client) {
    while (client.connected()) {
      readFrame(client);
      yield();
    }
    client.stop();
  }
}

int readFully (WiFiClient &client, uint8_t *buf, size_t len) {
  size_t i;

  for (i = 0; i < len; i++) {
    int b;

    if ((b = blockingRead(client)) < 0)
      return -1;

    buf[i] = (uint8_t) b;
  }

  return 0;
}

int blockingRead (WiFiClient &client) {
  int b;
  while ((b = client.read()) < 0) {
    yield();
    if (! client.connected())
      return -1;
  }
  return b;
}

void readFrame(WiFiClient &client) {
  uint8_t buf4[4];
  uint8_t cmd;
  size_t payload_length, leds_in_payload, i;

  // read channel number (ignored), command, and length
  if (readFully (client, buf4, sizeof (buf4)) < 0)
    return;

  cmd = buf4[1];
  payload_length = (((size_t) buf4[2]) << 8) + (size_t) buf4[3];
  leds_in_payload = payload_length / 3;
  if (leds_in_payload > N_PIXELS)
    leds_in_payload = N_PIXELS;
  if (cmd != 0)                 // we only support command 0, set pixel colors
    leds_in_payload = 0;

  // read pixel data; 3 bytes per pixel
  for (i = 0; i < leds_in_payload; i++) {
    if (readFully (client, buf4, 3) < 0)
      return;
    leds[i] = CRGB(buf4[0], buf4[1], buf4[2]);
  }
  FastLED.show();

  // discard any remaining data (e. g. if they sent us more pixels than we have)
  payload_length -= leds_in_payload * 3;

  for (; payload_length != 0; payload_length--) {
    if (blockingRead(client) < 0)
      return;
  }
}
