#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include <ESP32httpUpdate.h>
#include <EEPROM.h> // For using EEPROM to store useful data in memory
#include <PubSubClient.h> // Used for MQTT
#include <WiFi.h> // For Wifi related applications
#include <WiFiClient.h>
WiFiClient wifiClient;
#include <WebServer.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>// To save Wifi credentials
WiFiManager wm;
#include "FastLED.h"
#define NUM_LEDS  20    // Enter the total number of LEDs on the strip
#define PIN    23      // The pin connected to Din to control the LEDs  //h7 - 23, h10-32 - coonect wire directly to l_3 pad, h3-33
CRGB leds[NUM_LEDS];
// URL of the firmware binary to download and install
const char* firmwareUrl = "https://raw.githubusercontent.com/Akshaygite11/otatrial/main/otaV1.2.bin";
const char *mqtt_server = "mqtt.aykacontrolsystems.in"; // MQTT Broker
const int mqttPort = 1883;
const char* mqttUsername = "acs";
const char* mqttPassword = "acs@2022";
char devid[32] = ""; // Device id (Example: 11110001)
String un = "";
String maintopic = "/"; // Prefix of the topic. (Eg. Vajra/11110001/cycle)
String session_pub;
int count = 0;
int first_loop = 0;
int count_flag = 0;
int ota_flag=0; // to trigger OTA update
String tmp_str = "";
String s1, ts, data, s2;
char* ss;
char charBuf[100];
char charBuf1[100];
char charBuf2[100];
String dev_id = "";
int chk_flag = 0;
void          getSSID();
void          getPassword();
String ssid, password;
unsigned long sendDataPrevMillis = 0;
//bool shouldSaveConfig = false;
void callback(char *topic, byte *payload, unsigned int length);
String ssid_esp = "AykaControlSys";// Default name of WiFi if ESP is in config mode
int length_topics = 20; // 0 to 17
int rly = 4;
int sw = 5;
int en = 0;

PubSubClient client(wifiClient);

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.print("SSID is");
  Serial.println(ssid);
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //  ticker.attach(0.2, tick);
}
WiFiManagerParameter custom_devid("devid", "Device ID", devid, 32);


void setup() {
  Serial.begin(9600);
  delay(100);
  Serial2.begin(9600);
  delay(100);
  Serial.println("");
  Serial.println("AykaControlSystems WirelessCon 3.2");
  EEPROM.begin(512);
  wm.setAPCallback(configModeCallback);
  Serial.print("SSID is : \t");
  Serial.println(ssid);
  Serial.print("Password is : \t");
  Serial.println(password);
  delay(100);
  Serial.print("Read_string(10, 15) output is \t");
  tmp_str = read_String(10, 5);// Read value of device id from EEPROM address 10 and of length 15
  Serial.println(tmp_str);
  Serial.println("Reading String");
  Serial.print("Value of tmp_str is: \t");
  Serial.println(tmp_str);
  tmp_str.trim();
  Serial.print("Value of tmp_str after trim() is : \t");
  Serial.println(tmp_str);
  Serial.print("Length of tmp_str is: \t");
  Serial.println(tmp_str.length());
  Serial.print("Length of ssid start is: \t");
  Serial.println(ssid.length());
  if (checkk(tmp_str, 4)) { // Check is the string length is greater than 5
    auto_conn_ssid(ssid_esp, tmp_str);
    Serial.print("SSID in check() statement is");
    Serial.println(ssid);
  }
  else { // Else get value from the config mode of Wifi manager
    while (!chk_flag) {
      Serial.println("You entered an Invalid ID");
      Serial.println("Try Connecting again");
      fetch_and_conn();
      tmp_str =  custom_devid.getValue();
      chk_flag = checkk(tmp_str, 4);
      Serial.print("New dev id is: \t");
      Serial.println(tmp_str);
    }
    writeString(10, tmp_str); // Save new device id in EEPROM address 10
  }
  dev_id = tmp_str;
  FastLED.addLeds<WS2812B, PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);    // Set power limit of LED strip to 5V, 1500mA
  FastLED.clear();                                    // Initialize all LEDs to "OFF"
  // Set up OTA updates
  ArduinoOTA.begin();
//  ArduinoOTA.setPort(8266);
//  ArduinoOTA.setHostname("esp32");
//  ArduinoOTA.setPassword("admin");
//  ArduinoOTA.onStart([](){
//    Serial.println("Starting OTA update...");
//  });
//  ArduinoOTA.onEnd([](){
//    Serial.println("OTA update finished");
//    Serial.println("OTA updates ready");
//  });
//  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
//    Serial.printf("OTA update progress: %u%%\r", (progress / (total / 100)));
//  });
//  ArduinoOTA.onError([](ota_error_t error) {
//    Serial.printf("OTA update error: %u\n", error);
//    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
//    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
//    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
//    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
//    else if (error == OTA_END_ERROR) Serial.println("End Failed");
//  });
  Serial.println("Exiting Conf");
  WiFi.mode(WIFI_STA);
  Serial.println("STRTING ");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay(50);
  reconnect();
  delay(2000);
}

void loop()
{
  if (Serial.available() > 0)
  {
    char ch;
    s1 = "";
    s1 = Serial.readStringUntil('\n');
    ch = s1.charAt(0);
    Serial.println(F("Ddata Recvd "));
    Serial.println(s1);
    if (ch == '$')
    {
      if (s1.length() > 15)
      {
        Serial.println(F("DInvalid argument "));
      }
      else
      {
        char ch1 = s1.charAt(1);
        Serial.print(ch1);
        Serial.println();
        switch (ch1)            //for testing change cases in new ota update
        {
          case '1': //$1
            Serial.print("Red Fade effect");
            fadeAnimation(255, 0, 0);       // Red
            break;
          case '2': //$2
            Serial.print("Blue Fade effect ");
            fadeAnimation(0, 0, 255);       // Blue
            break;

          case '3' ://$3
            Serial.println("Orange fade effect");
            fadeAnimation(255, 128, 0);     // Orange
            break;
        }
      }
   }
  }

  client.loop();
  delay(10);
  if (!client.connected())
  {
    reconnect();
    delay(500);
  }
  else
  {
    if (millis() - sendDataPrevMillis > 15000) {
      sendDataPrevMillis = millis();
      count++;
      Serial.print("Sending Ping :");
      Serial.println(count);
      client.publish((maintopic + dev_id + "/heartbeat").c_str(), "{\"status\": \"available\"}");
      delay(20);
    }
  }
  // Handle MQTT messages and OTA updates
  ArduinoOTA.handle();
  if (ota_flag==1){
    Serial.println("entered OTA update loop");
    performOTAUpdate();
    delay(100);
    ota_flag=0;
  }
}

void reconnect()
{
  Serial.println("In Reconnect loop");
  if (WiFi.status() != WL_CONNECTED)
  {
    // IF wifi is not connected, blink led for 1 sec interval
    //    ticker.attach(1, tick);
    //heartbeat printing
    Serial.println("CNto");
    Serial.println(ssid);
    int a = 0;
    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
      // TRY reconnecting
      delay(1000);
      digitalWrite(2, LOW);
      delay(1000);
      digitalWrite(2, HIGH);
      if (a == 10)
      {
        //  WiFi.begin(ssid, password);
        a = 0;
      }
      a++;
    }
    digitalWrite(2, HIGH);
    Serial.println("WiFiCNTD");
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    while (!client.connected() && WiFi.status() == WL_CONNECTED)
    {
      // WiFi connected but client not connected
      Serial.println("MQTTCT");
      un = (maintopic + dev_id);
      Serial.println(un);
      //      if (client.connect("MQTT", "vajra-mqtt", "vajra99"))
      if (client.connect(un.c_str(), "acs", "acs@2022"))
      {
        //        ticker.attach(2, tick);
        Serial.println("MQTTCNTD");
        Serial.println((maintopic + dev_id + "/heartbeat"));
        client.publish((maintopic + dev_id + "/heartbeat").c_str(), "Active");
        digitalWrite(2, LOW);
        // Subscribe to all the topics defined at the beginning of the code
        client.subscribe((maintopic + dev_id + "/device").c_str());
        client.subscribe((maintopic + dev_id + "/otaCommand").c_str()); // to receive OTA update commands
        client.subscribe((maintopic + dev_id + "/info").c_str());
        client.subscribe((maintopic + dev_id + "/rst").c_str());
        client.subscribe((maintopic + dev_id + "/drst").c_str());
        client.subscribe((maintopic + dev_id + "/wrst").c_str());
      }
      digitalWrite(2, LOW);
      delay(100);
      digitalWrite(2, HIGH);
    }
  }
}

String read_String(int addrr, int readlen)
{
  // Function to read string from memory
  int i = 0;
  char datas[99];
  int len = 0;
  unsigned char k;
  k = EEPROM.read(addrr);
  while (i < readlen)
  {
    k = EEPROM.read(addrr + len);
    datas[len] = k;
    Serial.print("IN read_String loop: \t");
    Serial.println(datas[len]);
    len++;
    i++;
    delay(50);
  }
  datas[len] = '\0';
  Serial.println(len);

  return String(datas);
}
void writeString(int addrw, String data)
{
  // Function to write string to memory
  Serial.println("Entering writeString");
  int _size = data.length();
  int i;
  for (i = 0; i < _size; i++)
  {
    EEPROM.write(addrw + i, data[i]);
    delay(10);
  }
  EEPROM.write(addrw + _size, '\0');
  delay(20);
  EEPROM.commit();
  Serial.println("Exiting writeString");
}

void fetch_and_conn() {
  // Enter config mode by combining device id to prefix
  // Example: AykaControlSys/11110001
  wm.resetSettings();
  wm.setConfigPortalTimeout(60);
  if (first_loop  == 0) {
    wm.addParameter(&custom_devid);
    first_loop = 1;
  }
  Serial.println("Before autoconnect");
  wm.autoConnect((maintopic + dev_id).c_str());
}

void auto_conn_ssid(String ssid1, String deviceid) {
  // Function to set Hotspot name in config mode
  // Example: AykaControlSys/11110001 will be the hotspot name
  Serial.println(ssid1.c_str());
  Serial.println("Entered Auto_conn_ssid function");
  wm.setConfigPortalTimeout(60);
  wm.autoConnect((maintopic + deviceid).c_str());
  // wm.autoConnect((ssid1 + " Air " + deviceid).c_str());
  Serial.println("Exiting auto_conn_ssid function");
}

int checkk(String strtemp, int stringlen) {
  // Checks if the string is greater than the length specified.
  int stlen = strtemp.length() ;
  Serial.print("String length is\t");
  Serial.println(stlen);
  if (stlen > stringlen)
  {
    if ((isAlpha(strtemp.charAt(0))) && (isAlpha(strtemp.charAt(1))) && (isAlpha(strtemp.charAt(2)))  && (isDigit(strtemp.charAt(4))))
    {
      count = 1;
    }
    else
    {
      count = 0;
    }

    if (count == 1)Serial.println("Digit is valid");
    else Serial.println("Digit is not valid");
  }
  else {
    count = 0;
    Serial.println("Not valid");
  }
  return count;
}

void callback(char *topic, byte * payload, unsigned int length)
{
  /*  0 = "1",
      1 = "cycle",
      2 = "aqit",
      3 = "rint",
      4 = "ct",
      5 = "sernum",
      6 = "vbus",
      7 = "wrst",
      8 = "stat1",
      9 = "stat2",
      10 = "stat3",
      11 = "stat4",
      12 = "stat5",
      13 = "devrst"
  */
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(2, HIGH);
  delay(200);
  String msg = "";
  for (int i = 0; i < length; i++)
  {
    msg += ((char)payload[i]);
  }
  // uncomment below code to upload data to mqtt server in json format
//    JsonObject& root1 = jsonBuffer.createObject();// for sending data to mqtt server in json
//    root1["sessionId"] = sessionId;
//    root1.printTo(Serial);
//    String root_1;
//    root1.printTo(root_1);
//    client.publish((maintopic + dev_id + "/session").c_str(), root_1.c_str());
//    s2.toCharArray(charBuf, s2.length() + 1);
//    client.publish((maintopic + dev_id + "/heartbeat").c_str(), charBuf);

  // IF the client publishes to any of the subscribed topics, do these tasks
  if (strcmp(topic, (maintopic + dev_id + "/otaCommand").c_str()) == 0)
  {
    StaticJsonBuffer<256> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(payload);
    if (!root.success()) {
      Serial.println(F("Parsing failed!"));
      return;
    }
    // Decode JSON/Extract values
    int otaCommand = 0;
    String otaCommand_str = String(root["otaCommand"].as<char*>());
    otaCommand = otaCommand_str.toInt();
    Serial.print("otaCommand is: ");
    Serial.println(otaCommand);
    // Start the OTA update
    if (otaCommand == 1)
    {
      client.publish((maintopic + dev_id + "/otaCommand").c_str(), "OTA command 1 received");
      ota_flag=1;  // Triggers OTA update
    }
    else
    {
      Serial.println("Invalid OTA command");
      ota_flag=0;
      client.publish((maintopic + dev_id + "/session").c_str(), "Invalid OTA command");
    }
    s2.toCharArray(charBuf, s2.length() + 1);
    client.publish((maintopic + dev_id + "/heartbeat").c_str(), charBuf);
  }
  else if (strcmp(topic, (maintopic + dev_id + "/wrst").c_str()) == 0)
  { // WRST
    Serial.println("Device Wifi RST Recvd");
    client.publish((maintopic + dev_id + "/heartbeat").c_str(), "Reset Rcvd ");
    delay(500);
    wm.resetSettings();
    delay(500);
    ESP.restart();
  }
  else if (strcmp(topic, (maintopic + dev_id + "/info").c_str()) == 0)
  {
    Serial.println("Device Info Recvd");
    client.publish((maintopic + dev_id + "/heartbeat").c_str(), "Info Req Rcvd ");
    //    String ip = "D" + getIp();
    //    Serial.println(ip);
    //    client.publish((maintopic + dev_id + "/heartbeat").c_str(), ip.c_str());
    //    client.publish((maintopic + dev_id + "/heartbeat").c_str(), (ssid + " " + password).c_str());
    //    client.publish((maintopic + dev_id + "/heartbeat").c_str(), WiFi.macAddress().c_str());
    delay(500);
  }
  else if (strcmp(topic, (maintopic + dev_id + "/rst").c_str()) == 0)
  {
    Serial.println("Device Restart Recvd");
    client.publish((maintopic + dev_id + "/heartbeat").c_str(), "Device Restart Recvd");
    delay(500);
    ESP.restart();
  }
  else if (strcmp(topic, (maintopic + dev_id + "/drst").c_str()) == 0)
  { // Dev rst
    Serial.println("Device RST Recvd");
    client.publish((maintopic + dev_id + "/heartbeat").c_str(), "Device RST Recvd");
    writeString(10, "FALSEVAL");
    delay(500);
    ESP.restart();
  }
  else
  {
    Serial.println("INV Req Rcvd");
    client.publish((maintopic + dev_id + "/heartbeat").c_str(), "INV Req Rcvd");
  }
}

// OTA function
void performOTAUpdate() {
  Serial.println("OTA update started");
  // Start OTA update
  t_httpUpdate_return ret = ESPhttpUpdate.update(firmwareUrl);

  if (ret == HTTP_UPDATE_OK) {
    Serial.println("OTA update successful");
    client.publish((maintopic + dev_id + "/otaTopic").c_str(), "OTA update successful");
  } else {
    Serial.printf("OTA update failed with error code %d\n", ret);
    client.publish((maintopic + dev_id + "/otaTopic").c_str(), "OTA update failed");
  }
}

void fadeAnimation(int red, int green, int blue){
  float r, g, b;

  // FADE IN
  for(int i = 0; i <= 255; i++) {
    r = (i/256.0)*red;
    g = (i/256.0)*green;
    b = (i/256.0)*blue;
    fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
    FastLED.show();
    delay(15);
  }

  // FADE OUT
  for(int i = 255; i >= 0; i--) {
    r = (i/256.0)*red;
    g = (i/256.0)*green;
    b = (i/256.0)*blue;
    fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
    FastLED.show();
    delay(10);
  }
}

// mqtt command for OTA update
//Topic : /deviceId/otaCommand
//Data: JSON
//For start
//{
//  "otaCommand": 1
//}
