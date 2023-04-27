#include <Adafruit_PCD8544.h>

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

int modo = 8;






void setup() {
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
  display.clearDisplay();

  if(digitalRead(modo) ==HIGH){
    display.setCursor(70,35);
    display.print("DC:");
    V1 = ((analogRead(A0)-510)*(-0.04902));
    V2 = ((analogRead(A1)-510)*(-0.04902));
    V3 = ((analogRead(A2)-510)*(-0.04902));
    V4 = ((analogRead(A3)-510)*(-0.04902));
  }
  else{
    display.setCursor(70,35);
    display.print("AC:");
    if(V1_prev < ((analogRead(A0)-510)*(-0.04902))){
      V1_prev = ((analogRead(A0)-510)*(-0.04902));
    }
    if(V2_prev < ((analogRead(A1)-510)*(-0.04902))){
      V2_prev = ((analogRead(A1)-510)*(-0.04902));
    }
    if(V3_prev < ((analogRead(A2)-510)*(-0.04902))){
      V3_prev = ((analogRead(A2)-510)*(-0.04902));
    }
    if(V4_prev < ((analogRead(A3)-510)*(-0.04902))){
      V4_prev = ((analogRead(A3)-510)*(-0.04902));
    }
    V1 = (0.707)*V1_prev;
    V2 = (0.707)*V2_prev;
    V3 = (0.707)*V3_prev;
    V4 = (0.707)*V4_prev;
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
}
