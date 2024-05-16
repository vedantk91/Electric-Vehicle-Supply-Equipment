#include <EEPROM.h> // For using EEPROM to store useful data in memory
#include <PubSubClient.h> // Used for MQTT
#include <WiFi.h> // For Wifi related applications
//#include <DNSServer.h> // For Wifi related applications
#include <WiFiClient.h>
WiFiClient wifiClient;
#include <WebServer.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>// To save Wifi credentials
WiFiManager wm;
#include <Ticker.h> // To perform events periodically 
//String URL_fw_Bin; // Used to store URL pointing the binary file for OTA updates
//String fprt_default = "70 94 DE DD E6 C4 69 48 3A 92 70 A1 48 56 78 2D 18 64 E0 B7";
char devid[32] = ""; // Device id (Example: 11110001)
//const char *mqtt_server = "indrawater.com"; // MQTT Broker
const char *mqtt_server = "mqtt.aykacontrolsystems.in"; // MQTT Broker
//const char *mqtt_server = "mqtt.validchk.com"; // MQTT Broker
Ticker ticker, btn;
String un = "";
String maintopic = "/"; // Prefix of the topic. (Eg. Vajra/11110001/cycle)
String session_pub;
int count = 0;
int first_loop = 0;
int count_flag = 0;
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

//void tick()
//{
//  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));     // set pin to the opposite state
//}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.print("SSID is");
  Serial.println(ssid);
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //  ticker.attach(0.2, tick);
}

//WiFiClient wifiClient;
PubSubClient client(wifiClient);
WiFiManagerParameter custom_devid("devid", "Device ID", devid, 32);

void setup()
{

  pinMode(rly, OUTPUT);
  digitalWrite(rly, HIGH);
  delay(1000);
  digitalWrite(rly, LOW);

  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  delay(100);
  Serial.begin(9600);
  delay(100);
  Serial2.begin(9600);
  delay(100);


  //  char json[] =
  //    "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

  // Root of the object tree.
  //
  // It's a reference to the JsonObject, the actual bytes are inside the
  // JsonBuffer with all the other nodes of the object tree.
  // Memory is freed when jsonBuffer goes out of scope.
  //  JsonObject& root = jsonBuffer.parseObject(json);
  //
  //  // Test if parsing succeeds.
  //  if (!root.success()) {
  //    Serial.println("parseObject() failed");
  //    return;
  //  }

  // Fetch values.
  //
  // Most of the time, you can rely on the implicit casts.
  // In other case, you can do root["time"].as<long>();
  //  const char* sensor = root["sensor"];
  //  long time = root["time"];
  //  double latitude = root["data"][0];
  //  double longitude = root["data"][1];

  //StaticJsonDocument<200> doc;

  // char json[]=
  //   "{"sockets": {"socket_1": 1},"add_info": "Thank you for charging with Ayka","sessionId": "6327107f8ecb3c70c7e13b8c","duration": 60}";
  //  DeserializationError error = deserializeJson(doc, input);
  //
  //  if (error) {
  //    Serial.print("deserializeJson() failed: ");
  //    Serial.println(error.f_str());
  //    return;
  //  }

  //  int sockets_socket_1 = doc["sockets"]["socket_1"]; // 1
  //
  //  const char* add_info = doc["add_info"]; // "Thank you for charging with Ayka"
  //  const char* sessionId = doc["sessionId"]; // "6327107f8ecb3c70c7e13b8c"
  //  int duration = doc["duration"]; // 60

  Serial.println("");
  Serial.println("AykaControlSystems WirelessCon 3.2");
  digitalWrite(2, LOW);
  delay(300);
  digitalWrite(2, HIGH);
  delay(300);
  digitalWrite(2, LOW);
  delay(300);
  digitalWrite(2, HIGH);
  delay(300);
  EEPROM.begin(512);
  // Stream& input;


  //  ticker.attach(1, tick);
  //  ssid = wm.getSSID();
  //  password = wm.getPassword();
  //  ssid = wm.getSSID(); // Get wifi parameters
  //  password = wm.getPassword();
  // wm.setSaveConfigCallback(saveConfigCallback);
  //  wm.setheartbeatOutput(false);
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
  Serial.println("Exiting Conf");
  WiFi.mode(WIFI_STA);
  //  ssid = wm.getSSID();
  //  password = wm.getPassword();
  //  ssid = wm.getSSID();
  //  password = wm.getPassword();
  //  Serial.println(ssid);
  //  Serial.println(password);

  Serial.println("STRTING ");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay(50);
  reconnect();
  delay(2000);
  // Stream& input;

  //    StaticJsonDocument<0> filter;
  //    filter.set(true);



}

void loop()
{
  //  httpsClient.setFingerprint(fingerprint);
  //  httpsClient.setTimeout(15000); // 15 Seconds

  //  if (Serial.available() > 0)
  //  {
  //    s1 = Serial.readStringUntil('\n');
  //    delay(10);
  //    Serial.println(s1);
  //    StaticJsonBuffer<256> jsonBuffer;
  //    char json[] =
  //      "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
  //    s1.toCharArray(charBuf2, s1.length() + 1);
  //    s1 = "";
  //char *json = s1;
  //    JsonObject& root = jsonBuffer.parseObject(charBuf2);
  //    int sockets_socket_1 = root["sockets"]["socket_1"]; // 1
  //
  //    // const char* add_info = root["add_info"]; // "Thank you for charging with Ayka"
  //    const char* sessionId = root["sessionId"]; // "6327107f8ecb3c70c7e13b8c"
  //    int duration = root["duration"]; // 60
  //
  //    Serial.print("socket_1 :"); Serial.println(sockets_socket_1);
  //    Serial.print("session id :"); Serial.println(sessionId);
  //    Serial.print("duration :"); Serial.println(duration);
  if (Serial.available() > 0)
  {
    s1 = Serial.readStringUntil('\n');
    delay(10);
    Serial.println(s1);
    s1.toCharArray(charBuf2, s1.length() + 1);
    s1 = "";
    StaticJsonBuffer<256> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(charBuf2);
    if (!root.success()) {
      Serial.println(F("Parsing failed!"));
      return;
    }
    // Decode JSON/Extract values
    int duration = 0;
    Serial.println(F("Response:"));
    String duration_str = String(root["duration"].as<char*>());
    Serial.print("Duration is: ");
    duration = duration_str.toInt();
    Serial.println(duration);
    Serial2.println((String)"$1tbased" + duration);

    String sessionId = String(root["sessionId"].as<char*>());
    Serial.print("sessionId is: ");
    Serial.println(sessionId);

    String socket_str = String(root["sockets"]["socket_1"].as<char*>());
    int socket_int = socket_str.toInt();
    Serial.print("Socket is: ");
    Serial.println(socket_int);

    //    StaticJsonBuffer<256> jsonBuffer;
    //    JsonObject& root = jsonBuffer.parseObject(charBuf2);
    //    if (!root.success()) {
    //      Serial.println(F("Parsing failed!"));
    //      return;
    //    }
    // Decode JSON/Extract values
    String Status = String(root["status"].as<char*>());
    Serial.print("Status is: ");
    Serial.println(Status);
    // client.publish((maintopic + dev_id + "/remote").c_str(), "status received");



    //    Serial.print("socket_1 :"); Serial.println(socket_int);
    //    Serial.print("session id :"); Serial.println(sessionId);
    //    Serial.print("duration :"); Serial.println(duration);
    //Serialization
    //  StaticJsonBuffer<96> jsonBuffer;
    //
    //  JsonObject& root = jsonBuffer.parseObject(json);
    //
    //  doc["sessionId"] = sessionId;
    //  doc["timestamp"] = millis();
    //
    //  serializeJson(doc, output);
    //  client.publish("/dev_id/session", output);
  }

  client.loop();
  send1();
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
      String c1 = (String)count;
      //    client.publish((maintopic + dev_id + "/heartbeat").c_str(), c1.c_str());
      client.publish((maintopic + dev_id + "/heartbeat").c_str(), "{\"status\": \"available\"}");
      delay(20);
    }
  }

  //  StaticJsonBuffer<256> jsonBuffer;
  //    char json[] =
  //      "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
  //    JsonObject& root = jsonBuffer.parseObject(json);
  //
  //    if (!root.success()) {
  //      Serial.println("parseObject() failed");
  //      return;
  //    }
  //    const char* sensor = root["sensor"];
  //    long time = root["time"];
  //    double latitude = root["data"][0];
  //    double longitude = root["data"][1];
  //    Serial.print("time :"); Serial.println(time);
  //    Serial.print("latitude:"); Serial.println(latitude);
  //    Serial.print("longitude :"); Serial.println(longitude);
  //

  //  char json[] = "{\"sockets\":\ {\"socket_1\":1},\"sessionId\":"6327107f8ecb3c70c7e13b8c",\"duration\": 60}";
  //  JsonObject& root = jsonBuffer.parseObject(json);
  //
  //  // Test if parsing succeeds.
  //  if (!root.success()) {
  //    Serial.println("parseObject() failed");
  //    return;
  //  }
  //  int sockets_socket_1 = root["sockets"]["socket_1"]; // 1
  //
  // // const char* add_info = root["add_info"]; // "Thank you for charging with Ayka"
  //  const char* sessionId = root["sessionId"]; // "6327107f8ecb3c70c7e13b8c"
  //  int duration = root["duration"]; // 60

  //  Serial.print("socket_1 :"); Serial.println(sockets_socket_1);
  //  Serial.print("session id :"); Serial.println(sessionId);
  //  Serial.print("duration :"); Ser4ial.println(duration);
  //Serialization
  //  StaticJsonBuffer<96> jsonBuffer;
  //
  //  JsonObject& root = jsonBuffer.parseObject(json);
  //
  //  doc["sessionId"] = sessionId;
  //  doc["timestamp"] = millis();
  //
  //  serializeJson(doc, output);
  //  client.publish("/dev_id/session", output);
}
void send1()
{
  // Used to call functions from Serial monitor

  if (Serial.available() > 0)
  {
    s1 = Serial.readStringUntil('\n');
    s2 = s1.substring(1);
    char a = s1.charAt(0);
    switch (a)
    {
      case 'D':
        s2.toCharArray(charBuf, s2.length() + 1);
        client.publish((maintopic + dev_id + "/heartbeat").c_str(), charBuf);
        break;
      case 'W': {
          Serial.println("RST RCVD");
          wm.resetSettings();
          delay(500);
          ESP.restart();
          break;
        }
      case 'R': {
          Serial.println("Device RST Recvd");
          client.publish((maintopic + dev_id + "/heartbeat").c_str(), "Device RST Recvd");
          writeString(10, "FALSEVAL");
          delay(500);
          ESP.restart();
        }
      default:
        break;
    }
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

        //        // Memory location 25 has the length of the URL saved in it.
        //        String otaurlsize = read_String(25, 2);
        //        int urlsize = otaurlsize.toInt();
        //
        //        // Version is available after the URL
        //        // Sample Destination: http://test.taypro.in/firmware/V1.00.bin
        //        // URL is: http://test.taypro.in/firmware
        //        // "/V" is hardcoded
        //        // So, otaversion location will be base address (30+urlsize) + (2)"/V"
        //        String otaversions = read_String(32 + urlsize, 4);
        //        if (otaversions.charAt(1) != '.')
        //        {
        //          // OTA version is valid only if the decimal point is available at index 1, for eg 1.00
        //          otaversions = "0.00";
        //          // Since invalid version, restore it to defaul 0.00
        //          writeString(30 + urlsize, otaversions);
        //          Serial.println("OTA version was not valid");
        //          Serial.println("Changed OTA version to 0.00");
        //        }
        //        else
        //        {
        //          Serial.print("OTA Version is:\t");
        //          Serial.println(otaversions);
        //          client.publish((maintopic + dev_id + "/heartbeat").c_str(), (otaversions).c_str());
        //        }
        digitalWrite(2, LOW);
        //        for (int i = 0 ; i <= length_topics ; i++) {
        // Subscribe to all the topics defined at the beginning of the code
        client.subscribe((maintopic + dev_id + "/device").c_str());
        client.subscribe((maintopic + dev_id + "/remote").c_str());
        //        client.subscribe((maintopic + dev_id + "/heartbeat").c_str());
        client.subscribe((maintopic + dev_id + "/info").c_str());
        client.subscribe((maintopic + dev_id + "/rst").c_str());
        client.subscribe((maintopic + dev_id + "/drst").c_str());
        client.subscribe((maintopic + dev_id + "/wrst").c_str());
        //                  Serial.print("Subscribed to: \t");
        //        }
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
    count = 1;
    //    for (int i = 0; i < stlen; i++)
    //    {
    //      if (isDigit(strtemp.charAt(i)))
    //      {
    //        count = 1;
    //        Serial.print("isDigit() Count is\t");
    //        Serial.println(strtemp.charAt(i));
    //      }
    //      else
    //      {
    //        count = 0;
    //        break;
    //      }
    //    }
    //    if ((isAlpha(strtemp.charAt(0))) && (isAlpha(strtemp.charAt(1))) && (isAlpha(strtemp.charAt(2)))  && (isDigit(strtemp.charAt(4))))
    //    {
    //      count = 1;
    //      //      Serial.print("isDigit() Count is\t");
    //      //      Serial.println(strtemp.charAt(i));
    //    }
    //    else
    //    {
    //      count = 0;
    //      //      break;
    //    }

    //    if (count == 1)Serial.println("Digit is valid");
    //    else Serial.println("Digit is not valid");
  }
  else
  {
    count = 0;
    Serial.println("Not valid");
  }
  return count;
}
//
//String getIp()
//{
//  // Fetches dynamic ip using ipify api
//  Serial.println("Entered Getip");
//  HttpClient http();
//  //http.begin(wifiClient, url);
//  http.begin(http://api.ipify.org);
//
//  int httpCode = http.GET();
//  String json = http.getString();
//  return json;
//}

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

  // IF the client publishes to any of the subscribed topics, do these tasks
  // Topic example: Vajra/11110001/cycle
  //  if (strcmp(topic, (maintopic + dev_id + "/remote").c_str()) == 0)
  //  {
  //    StaticJsonBuffer<256> jsonBuffer;
  //    JsonObject& root3 = jsonBuffer.parseObject(payload);
  //    if (!root3.success()) {
  //      Serial.println(F("Parsing failed!"));
  //      return;
  //    }
  //    // Decode JSON/Extract values
  //
  //    String command = String(root3["command"].as<char*>());
  //    Serial.print("Status is: ");
  //    Serial.println(command);
  //    client.publish((maintopic + dev_id + "/session").c_str(), "status received");
  //  }
  if (strcmp(topic, (maintopic + dev_id + "/device").c_str()) == 0)
  {
    // Serial.println("1s Relay ON");
    StaticJsonBuffer<512> jsonBuffer;

    JsonObject& root = jsonBuffer.parseObject(payload);
    if (!root.success()) {
      Serial.println(F("Parsing failed!"));
      return;
    }
    // Decode JSON/Extract values
    int duration = 0;
    Serial.println(F("Response:"));
    String duration_str = String(root["duration"].as<char*>());
    Serial.print("Duration is: ");
    duration = duration_str.toInt();
    Serial.println(duration);
    Serial2.println((String)"$1tbased" + duration);

    String sessionId = String(root["sessionId"].as<char*>());
    Serial.print("sessionId is: ");
    Serial.println(sessionId);

    String socket_str = String(root["sockets"]["socket_1"].as<char*>());
    int socket_int = socket_str.toInt();
    Serial.print("Socket is: ");
    Serial.println(socket_int);
    //    digitalWrite(rly, HIGH);
    //    btn.attach(1, button);
    client.publish((maintopic + dev_id + "/session").c_str(), "Access Granted ");
    JsonObject& root1 = jsonBuffer.createObject();
    root1["sessionId"] = sessionId;
    root1.printTo(Serial);
    String root_1;
    root1.printTo(root_1);
    //Serial.print(root_1);
    client.publish((maintopic + dev_id + "/session").c_str(), root_1.c_str());
    s2.toCharArray(charBuf, s2.length() + 1);
    client.publish((maintopic + dev_id + "/heartbeat").c_str(), charBuf);
  }
  else if (strcmp(topic, (maintopic + dev_id + "/remote").c_str()) == 0)
  {
    StaticJsonBuffer<256> jsonBuffer;
    JsonObject& root3 = jsonBuffer.parseObject(payload);
    if (!root3.success()) {
      Serial.println(F("Parsing failed!"));
      return;
    }
    // Decode JSON/Extract values
    int command = 0;
    String command_str = String(root3["command"].as<char*>());
    command = command_str.toInt();
    Serial.print("Status is: ");
    Serial.println(command);
    client.publish((maintopic + dev_id + "/session").c_str(), "status received");
    if (command == 1)
    {
      Serial2.println((String)"$2start");
      client.publish((maintopic + dev_id + "/session").c_str(), "Remotely charging started");
    }
    else if (command == 0)
    {
      Serial2.println((String)"$3stop");
      client.publish((maintopic + dev_id + "/session").c_str(), "Remotely charging stopped");
    }
    else
    {
      Serial.println("Invalid remote command");
      client.publish((maintopic + dev_id + "/session").c_str(), "Invalid remote command");
      //      client.publish((maintopic + dev_id + "/session").c_str(), ("command is :" + command).c_str());
      //      JsonObject& root2 = jsonBuffer.createObject();
      //      root2["command"] = command;
      //      root2.printTo(Serial);
      //      String root_2;
      //      root1.printTo(root_2);
      //      client.publish((maintopic + dev_id + "/session").c_str(), root_2.c_str());
    }
    //client.publish((maintopic + dev_id + "/remote").c_str(), "status received");
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

//void button()
//{
//  Serial.println("In relay close");
//  digitalWrite(rly, LOW);
//  en = 0;
//  btn.detach();
//}
