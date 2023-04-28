#include <Adafruit_PCD8544.h>
#include <math.h>

// Create an instance of the PCD8544 library
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

float V1 = 0;
float V2 = 0;
float V3 = 0;
float V4 = 0;

float V1_prev = 0;
float V2_prev = 0;
float V3_prev = 0;
float V4_prev = 0;

float V1_read = 0;
float V2_read = 0;
float V3_read = 0;
float V4_read = 0;

int modo = 8;

int counter = 0;





void setup() {
  Serial.begin ( 9600);
  // Initialize the display
  display.begin();
  // Set the contrast (0 to 127)
  display.setContrast(60);
  // Clear the display
  display.clearDisplay();
  // Set the cursor to the top-left corner
  display.setCursor(0,0);
  
}

void loop() {

  V1_read = ((analogRead(A0)-510)*(-0.04902));
  V2_read = ((analogRead(A1)-510)*(-0.04902));
  V3_read = ((analogRead(A2)-510)*(-0.04902));
  V4_read = ((analogRead(A3)-510)*(-0.04902));
  display.clearDisplay();

  if(digitalRead(modo) == HIGH){
    display.setCursor(70,35);
    display.print("DC");
    V1 = ((analogRead(A0)-510)*(-0.04902));
    V2 = ((analogRead(A1)-510)*(-0.04902));
    V3 = ((analogRead(A2)-510)*(-0.04902));
    V4 = ((analogRead(A3)-510)*(-0.04902));
  }
  else{
    display.setCursor(70,35);
    display.print("AC");
    V1_prev = 0;
    V2_prev = 0;
    V3_prev = 0;
    V4_prev = 0;
    for(int i=0; i<1600; i++){
      V1_read = (analogRead(A0)-510)*(-0.04902);
      V1_prev += pow(V1_read, 2);
      V2_read = (analogRead(A1)-510)*(-0.04902);
      V2_prev += pow(V2_read, 2);
      V3_read = (analogRead(A2)-510)*(-0.04902);
      V3_prev += pow(V3_read, 2);
      V4_read = (analogRead(A3)-510)*(-0.04902);
      V4_prev += pow(V4_read, 2);
      delay(0.1);
    }
    V1_prev = sqrt(V1_prev/1600);
    V2_prev = sqrt(V2_prev/1600);
    V3_prev = sqrt(V3_prev/1600);
    V4_prev = sqrt(V4_prev/1600);



    V1 = V1_prev;
    V2 = V2_prev;
    V3 = V3_prev;
    V4 = V4_prev;
  }


  
  display.setCursor(10,5);
  display.print("V1:");
  display.print(V1);

  display.setCursor(10,15);
  display.print("V2:");
  display.print(V2);

  display.setCursor(10,25);
  display.print("V3:");
  display.print(V3);

  display.setCursor(10,35);
  display.print("V4:");
  display.print(V4);

  display.display(); 
  // Do nothing

  if     (modo == HIGH) Serial.print("DC");
  else   Serial.print("AC");
  Serial.print(";");
  Serial.print(V1);
  Serial.print(";");
  Serial.print(V2);
  Serial.print(";");
  Serial.print(V3);
  Serial.print(";");
  Serial.print(V4);
  Serial.print(";");
  Serial.println(":");
}

