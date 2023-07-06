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

#define POWER_PIN 0

#define WATER_PIN 16

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const byte pinDatosDQ = 36;
const int oneWireBus = 4; 

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Redmi_note";
char pass[] = "ahkshrnnh";


//Variables necesarias en codigo
int START;
float TIEMPO = 0.5;
int tiempo_restante;

//Estados de maquina de estados 
enum STATES {
  ESPERA,
  CARGA,
  HERVIDO,
  FIN
};

//Definicion de estado actual como uno e STATES
STATES currentState;

BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();
  START = value;
}

BLYNK_WRITE(V1)
{
  // Set incoming value from pin V0 to a variable
  float value = param.asFloat();
  TIEMPO = value;
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
  pinMode(POWER_PIN, OUTPUT);
  pinMode(WATER_PIN, OUTPUT);
  // Start the DS18B20 sensor
  sensors.begin();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
  Blynk.virtualWrite(V1, 0.5);
  currentState = ESPERA;
  
}

void loop()
{
  

  Blynk.run();
  timer.run();
  //Se obtiene temperatura en celcius
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");
  Serial.println(currentState);
  delay(10);
  //Se escribe temperatura en Blynk
  Blynk.virtualWrite(V2, temperatureC);


  //Inicia maquina de estados 
  switch(currentState)
  {
    //Se mantiene en ESPERA hasta sennal de inicio
    case ESPERA:
      tiempo_restante = TIEMPO*80;
      digitalWrite(POWER_PIN, LOW);
      digitalWrite(WATER_PIN, LOW);
      if(START == 1){
        currentState = CARGA;
      }
    break;
    //Mantiene bomba encendida durante tiempo_restante segundos
    case CARGA:
      if(START == 1){
        digitalWrite(WATER_PIN, HIGH);
        tiempo_restante--;
        delay(990);
        if(tiempo_restante<5){
          currentState = HERVIDO;
          digitalWrite(WATER_PIN, LOW);
        }
        }
      else{
        digitalWrite(WATER_PIN, LOW);
      }
    break;

    //Pasa a fin cuando T>92
    case HERVIDO:
      if(START == 1){
        if(temperatureC < 92){
          digitalWrite(POWER_PIN, HIGH);
          digitalWrite(WATER_PIN, LOW);
        }
        else{
          currentState = FIN;
        }
        }
      else{
        digitalWrite(POWER_PIN, LOW);
      }
    break;
    //Mantiene agua dentro de un rango de valores, 92 y 95
    case FIN:
      if(START == 1){
        if(temperatureC < 92){
          digitalWrite(POWER_PIN, HIGH);
          digitalWrite(WATER_PIN, LOW);
        }
        else if(temperatureC > 95){
          digitalWrite(POWER_PIN, LOW);
        }
      }
      else{
        digitalWrite(POWER_PIN, LOW);
      }
      
    break; 
  }

}
