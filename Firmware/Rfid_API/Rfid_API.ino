
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 5
#define RST_PIN 13
byte arr[15];
byte nuidPICC[4];
String CSN;

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

const char* ssid = "FYP";
const char* password = "2416@ashay";

const char* host = "https://kind-red-abalone-hem.cyclic.app/api/";
String endpoint ;
const String ABB="5A924C18" ;
String ashay_web = host + ABB;


void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);   // Connect to WiFi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

}
void loop() {

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  //  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  //  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    //    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  //  if (rfid.uid.uidByte[0] != nuidPICC[0] ||
  //      rfid.uid.uidByte[1] != nuidPICC[1] ||
  //      rfid.uid.uidByte[2] != nuidPICC[2] ||
  //      rfid.uid.uidByte[3] != nuidPICC[3] ) {
  //    Serial.println(F("A new card has been detected."));
  //
  //    // Store NUID into nuidPICC array
  for (byte i = 0; i < 1; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
    //    }

    //    Serial.println(F("The NUID tag is:"));
    //    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    //    Serial.println();
  }
  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

  // wait for WiFi connection
  if ( ! rfid.PICC_IsNewCardPresent())
    //    if((WiFi.status() != WL_CONNECTED))
  {

    HTTPClient http;

//    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    //http.begin("https://www.howsmyssl.com/a/check", ca); //HTTPS
//    Serial.print(ashay_web);
//    Serial.print(endpoint);
    http.begin(ashay_web); //HTTP


//    Serial.print("[HTTP] GET...\n");                              // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {                                         // HTTP header has been send and Server response header has been handled
//                Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
        Serial.println("DOne");
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }

  delay(5000);
}

void printHex(byte * buffer, byte bufferSize) {
  char temp;
  byte hexx;
  for (byte i = 0; i < bufferSize; i++) {
    hexx = buffer[i];
    if (hexx < 0x0A)
    {
      temp = hexx + 0x30;
      //        Serial.write(temp);
    }
    else if ((hexx > 0x09) && (hexx < 0x10))
    {
      temp = hexx + 0x37;
      //        Serial.write(temp);
    }
    CSN += String(buffer[i], HEX);
  }
  CSN.toUpperCase();
  //Serial.println(CSN);
  CSN.trim();
//  Serial.println(CSN);
  endpoint = CSN;
}
