#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  // Pines de conexión del LCD
float prevTime = 0;
int valorPrevio = 0;
int valorMax = 0;
int valorMin = 0;
bool finMuestra = false;
float frecuenciaPrevia = 0;
bool frecuenciaEncontrada = false;

unsigned long ultimoTiempoReset = 0;

float Frecuencia(int valorActual){
  float frecuencia = 0.0;
  int umbral= 0;
  int tiempoActual = millis();  
   
  if (valorActual > valorMax) {
    valorMax = valorActual;
    }
  if (valorActual < valorMin) {
    valorMin = valorActual;
      }

  umbral = (valorMax + valorMin) / 2;

  // Detectar el cruce de umbral
  if (valorActual > umbral && valorPrevio <= umbral) {
    if (prevTime > 0) {
      // Calcular la frecuencia
      frecuencia = 1000.0 / (tiempoActual - prevTime);  
      
      // Mostrar la frecuencia en el LCD y en el monitor serial
    }
    prevTime = tiempoActual;  
  }

  valorPrevio = valorActual;  
  
  if (tiempoActual - ultimoTiempoReset > 1000) {  
    valorMax = 0;
    valorMin = 1023;
    ultimoTiempoReset = tiempoActual;  
  }
  return frecuencia;
}

int* Muestras(int valorActual, float frecuenciaPrevia) {
  static int muestras[10];  
  int intervalo = (1000.0 / (2 * frecuenciaPrevia + 40)); 
  for (int i = 0; i < 10; i++) {
    muestras[i] = analogRead(A0);  
    delay(intervalo);
  }

  return muestras;  
}

void setup() {
  lcd.begin(16, 2);  // Inicializar el LCD
  Serial.begin(9600); 
  pinMode(A0, INPUT);
}

void loop() {
  int valorActual = analogRead(A0);
  float frecuencia = Frecuencia(valorActual);
  
  if(frecuencia > 0.0 && frecuenciaEncontrada == false && frecuencia < 90.0){
    frecuenciaPrevia = frecuencia;
    lcd.setCursor(0, 0);
    lcd.print("Frecuencia: ");	
  	lcd.setCursor(0, 1);
    lcd.print(frecuencia);
    lcd.print(" Hz     ");
    frecuenciaEncontrada = true;
  }

  if(frecuenciaPrevia != 0 && finMuestra == false ){
    int *muestras = Muestras(valorActual, frecuenciaPrevia);
    finMuestra = true; 
 
  }
  
}
