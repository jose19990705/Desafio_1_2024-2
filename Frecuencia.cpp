float Frecuencia(int valorActual){
  float frecuencia = 0.0;
  int umbral= 0;
  int tiempoActual = millis();  
  /*
  umbral = El umbral utilizado para detectar cuando la señal cruza el punto medio entre el valor máximo y el valor mínimo.
  millis() = el tiempo en milisegundos desde que el programa comenzó.
  valorMax y valorMin = Variables globales que guardan los valores máximo y mínimo de la señal.
  prevTime = Variable global que almacena el momento en que ocurrió el último cruce de umbral.
  valorPrevio = variable global Guarda el valor anterior de la señal para detectar cuando esta cruza el umbral desde abajo hacia arriba.
  ultimoTiempoReset = Guarda el momento en que los valores de valorMax y valorMin fueron reiniciados.
  */


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
