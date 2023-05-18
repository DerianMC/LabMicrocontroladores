#include "ThingSpeak.h"
#include "WiFi.h"

#define LED_PIN 25

const char* ssid = "Redmi_note";
const char* password = "ahkshrnnh";

unsigned long channelID = 2154048;                //ID de vuestro canal.
const char* WriteAPIKey = "1L1RJPVINEXT4B47";     //Write API Key de vuestro canal.

WiFiClient cliente;

int prueba = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  WiFi.begin(ssid, password);
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi conectado!");
  Serial.println("-----------------------");

  ThingSpeak.begin(cliente);
}
void loop() {
  digitalWrite(LED_PIN,HIGH);
  delay(16);
  ThingSpeak.setField(1, prueba);
  digitalWrite(LED_PIN,LOW);
  delay(16);
  prueba = prueba+1;
  ThingSpeak.writeFields(channelID,WriteAPIKey);
  Serial.println("Datos enviados a ThingSpeak!");
}