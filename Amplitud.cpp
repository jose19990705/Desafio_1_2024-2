float Amplitud(int* muestras, int numeroMuestras) {
  /*
  calcula la amplitud de una señal a partir de un arreglo de muestras.
  Encuentra el valor máximo y el valor mínimo en las muestras, 
  siendo valorMax y valorMin variables globales 
  se calcula como la diferencia entre el valor máximo y el valor mínimo. 
  Esto da el rango total de la señal.
  ajustando el resultado dividiéndolo por 100.0.
  */
  // Encontrar los valores máximo y mínimo
  for (int i = 0; i < numeroMuestras; i++) {
    if (muestras[i] > valorMax) {
      valorMax = muestras[i];
    }
    if (muestras[i] < valorMin) {
      valorMin = muestras[i];
    }
  }
  
  // Calcular la amplitud
  float amplitud = ((valorMax - valorMin) ) / 100.0; 
  return amplitud;
}
