#include <LiquidCrystal.h>
float Amplitud ;
float Frecuencia ;
bool recopilacion;
// Código encargado de detectar las pulsaciones de los botones y empezar a recopilar los datos.
//Este aún tiene el faltante de las funciones encargadas de encontrar la frecuencia y voltaje de la señal.
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  // Pines para el LCD

void setup() {
    lcd.begin(16, 2);
    pinMode(6, INPUT);
    pinMode(7,INPUT);
    Serial.begin(9600);
}

void loop() {

    if(digitalRead(6)==HIGH){
        recopilacion=true;
    }
    if(digitalRead(7)==HIGH){
        recopilacion=false;
    }
    if (recopilacion==true) {
        Amplitud = 1;
        Frecuencia +=0.5;
        lcd.clear();  // Limpiar la pantalla LCD
        lcd.setCursor(0, 0);
        lcd.print("Amplitud: ");
        lcd.print(Amplitud);
        lcd.setCursor(0, 1);
        lcd.print("Frecuencia: ");
        lcd.print(Frecuencia);
    }
    if(recopilacion==false){
        lcd.clear();
    }


    delay(1000); // Esperar 1000 milisegundos (1 segundo)
}
