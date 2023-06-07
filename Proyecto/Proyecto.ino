/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID           "TMPL2D65xrkHX"
#define BLYNK_TEMPLATE_NAME         "Quickstart Template"
#define BLYNK_AUTH_TOKEN            "jOWGzQaYytDom3RsnsBKFzXxn1KRiX6U"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#define LED_PIN 25

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h> 

const byte pinDatosDQ = 36;

OneWire oneWireObjeto(pinDatosDQ);
DallasTemperature sensorDS18B20(&oneWireObjeto);

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Redmi_note";
char pass[] = "ahkshrnnh";

int estado;

BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();
  estado = value;

  
  
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  ;
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  pinMode(25, OUTPUT);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);

  sensorDS18B20.begin();
}

void loop()
{
  Blynk.run();
  timer.run();

  sensorDS18B20.requestTemperatures();
  char button[10];
  sprintf(button,"%d", sensorDS18B20.getTempCByIndex(1));
  Serial.print(button);
  Blynk.virtualWrite(V2, button);

  if(estado == 1){
    digitalWrite(LED_PIN,HIGH);
    
  }
  else{
    digitalWrite(LED_PIN,LOW);
  }
}
