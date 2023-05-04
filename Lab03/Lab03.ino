#include <Adafruit_PCD8544.h>
#include <math.h>

//Se crea instancia usando Adafruit_PCD8544
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);


//Tensión a mostrar en display
float V1 = 0;
float V2 = 0;
float V3 = 0;
float V4 = 0;

//Variables utilizadas para conseguir Vrms
float V1_prev = 0;
float V2_prev = 0;
float V3_prev = 0;
float V4_prev = 0;

//Tensión medida
float V1_read = 0;
float V2_read = 0;
float V3_read = 0;
float V4_read = 0;

//Variable contenedora de modo de operación
int modo = 8;
int serial = 9;



void setup() {
  pinMode(2, OUTPUT);
  Serial.begin ( 9600);
  //Inicializa display
  display.begin();
  //Establece contraste
  display.setContrast(60);
  //Limpia display
  display.clearDisplay();
  //Posiciona cursor en (0,0)
  display.setCursor(0,0);
  
}

void loop() {

  display.clearDisplay();

  //modo HIGH es DC
  if(digitalRead(modo) == HIGH){
    //Imprime modo de operación 
    display.setCursor(70,35);
    display.print("DC");
    //Mide la tensión para cada línea y la escala al valor original
    V1 = ((analogRead(A0)-510)*(-0.04902));
    V2 = ((analogRead(A1)-510)*(-0.04902));
    V3 = ((analogRead(A2)-510)*(-0.04902));
    V4 = ((analogRead(A3)-510)*(-0.04902));
  }
  //Modo AC
  else{
    display.setCursor(70,35);
    display.print("AC");
    //Reinicia valores de V1_prev
    V1_prev = 0;
    V2_prev = 0;
    V3_prev = 0;
    V4_prev = 0;
    //Inicia medición de 1600 valores para sacar Vrms
    for(int i=0; i<1600; i++){
      //Lee pin analógico
      V1_read = (analogRead(A0)-510)*(-0.04902);
      //Suma al total de valor el valor leído
      V1_prev += pow(V1_read, 2);
      V2_read = (analogRead(A1)-510)*(-0.04902);
      V2_prev += pow(V2_read, 2);
      V3_read = (analogRead(A2)-510)*(-0.04902);
      V3_prev += pow(V3_read, 2);
      V4_read = (analogRead(A3)-510)*(-0.04902);
      V4_prev += pow(V4_read, 2);
      delay(0.1);
    }
    //Saca raíz cuadra de V*_prev/1600 obteniendo Vrms
    V1_prev = sqrt(V1_prev/1600);
    V2_prev = sqrt(V2_prev/1600);
    V3_prev = sqrt(V3_prev/1600);
    V4_prev = sqrt(V4_prev/1600);


    //ASigna a V* los valores de Vrms
    V1 = V1_prev;
    V2 = V2_prev;
    V3 = V3_prev;
    V4 = V4_prev;
  }
  
  //Cuando cualquiera de los valores de V* es mayor a 20 enciende LED de advertencia
  if(abs(V1) > 20 || abs(V2) > 20 || abs(V3) > 20 || abs(V4) > 20){
    digitalWrite(2, HIGH);
  }
  else{
    digitalWrite(2, LOW);
  }

  //Muestra en display las tensiones de cada V*
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


  //Si switch serial en alto empieza a enviar datos por puerto serial
  //Envía modo de operación, así como cada una de las tensiones
  if(digitalRead(serial) == HIGH){
    if(digitalRead(modo) == HIGH) Serial.print("DC");
    else Serial.print("AC");
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
}

