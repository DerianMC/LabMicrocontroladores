// Se toma como guia el ejemplo de "Get Started With Machine Learning on Arduino"
// Se realizan las modificaciones necesarias para la captura correcta de los datos
// Derechos reservados para arduino.cc


/*
  IMU Capture
  This example uses the on-board IMU to start reading acceleration and gyroscope
  data from on-board IMU and prints it to the Serial Monitor for one second
  when the significant motion is detected.
  You can also use the Serial Plotter to graph the data.
  The circuit:
  - Arduino Nano 33 BLE or Arduino Nano 33 BLE Sense board.
  Created by Don Coleman, Sandeep Mistry
  Modified by Dominic Pajak, Sandeep Mistry
  This example code is in the public domain.
*/

// Se incluye el sensor integrado en Arduino nano ble
#include <Arduino_LSM9DS1.h>

//Se definen constantes
const float accelerationThreshold = 2.5; // threshold of significant in G's
const int numSamples = 119;

int samplesRead = numSamples;

// Conexion serial e inicializacion de IMU
void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}


void loop() {
  // Definicion de los valores a obtener del acelerometro
  float aX, aY, aZ, gX, gY, gZ;

  // wait for significant motion
  // Espera a detectar algun cambio en los sensores
  // Cuando se da el cambia realiza la lectura del acelerometro 
  while (samplesRead == numSamples) {
    if (IMU.accelerationAvailable()) {
      // read the acceleration data
      IMU.readAcceleration(aX, aY, aZ);

      // sum up the absolutes
      float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

      // check if it's above the threshold
      if (aSum >= accelerationThreshold) {
        // reset the sample read count
        samplesRead = 0;
        break;
      }
    }
  }

  // check if the all the required samples have been read since
  // the last time the significant motion was detected
  while (samplesRead < numSamples) {
    // check if both new acceleration and gyroscope data is
    // available
    // Realiza la lectura del acelerometro y el giroscopio 
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      // read the acceleration and gyroscope data
      IMU.readAcceleration(aX, aY, aZ);
      IMU.readGyroscope(gX, gY, gZ);

      // Aumenta el numero de muestras leidas 
      samplesRead++;

      // print the data in CSV format
      // Se imprime en este formato para que pueda ser capturado de manera correcta por el script
      Serial.print(aX, 3);
      Serial.print(";");
      Serial.print(aY, 3);
      Serial.print(";");
      Serial.print(aZ, 3);
      Serial.print(";");
      Serial.print(gX, 3);
      Serial.print(";");
      Serial.print(gY, 3);
      Serial.print(";");
      Serial.print(gZ, 3);
      Serial.print(";");
      Serial.print(":");
      Serial.println();

      if (samplesRead == numSamples) {
        // add an empty line if it's the last sample
        Serial.println();
      }
    }
  }
}