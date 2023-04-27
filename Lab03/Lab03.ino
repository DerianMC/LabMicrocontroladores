#include <Adafruit_PCD8544.h>

// Create an instance of the PCD8544 library
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

int val = 0;

void setup() {
  // Initialize the display
  display.begin();
  // Set the contrast (0 to 127)
  display.setContrast(60);
  // Clear the display
  display.clearDisplay();
  // Set the cursor to the top-left corner
  display.setCursor(0,0);
  // Print "Hello World!"
  display.println("V1:");
  display.println("V2");
  display.println("V3");
  display.println("V4");

  // Display the contents of the buffer
  display.display();
}

void loop() {

  int val = analogRead(A0);

  display.clearDisplay();
  display.setCursor(0,10);
  display.print("Var value ");
  display.print(val);
  display.setCursor(0,20);
  display.print("Var value 2 ");
  display.print(val);
  display.display();
  // Do nothing
}
