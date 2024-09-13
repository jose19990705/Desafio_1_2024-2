#ifndef BASES_H
#define BASES_H


float *base_seno(float frecuencia, int muestras);
float* base_cuadrada(int muestras);
float *base_triangular( int muestras);
float coeficiente_correlacion(float* x, float* y, int n);
#endif // BASES_H
