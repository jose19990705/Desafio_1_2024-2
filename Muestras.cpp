void Muestras(float frecuenciaPrevia, int numeroMuestras, int* muestras) {
  /*
  Calcula el tiempo entre muestras (intervalo) en milisegundos, basado en la frecuencia de la señal capturada previamente.
  Lee la señal analógica desde el pin A0 usando analogRead() y almacena el valor en el arreglo muestras[].
  Este proceso se repite numeroMuestras veces, tomando una lectura en cada ciclo.
  Usa delay(intervalo) para esperar el tiempo calculado entre cada lectura.
  Una vez completada la toma de todas las muestras, establece finMuestra en true para indicar que el muestreo ha terminado.
  */
  int intervalo = (1000.0 / (2 * frecuenciaPrevia + 40));  // Calcula el intervalo entre muestras
  for (int i = 0; i < numeroMuestras; i++) {
    muestras[i] = analogRead(A0);  // Captura la señal analógica en el pin A0
    delay(intervalo);  // Espera el tiempo calculado antes de tomar la siguiente muestra
  }
  finMuestra = true;  // Marca el final del proceso de muestreo
}
