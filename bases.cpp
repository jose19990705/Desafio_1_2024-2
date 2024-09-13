   //Con esta función, generamos la base para seno
float *base_seno(float frecuencia, int muestras) {
    float b_seno = new float[muestras];
    float intervalo = (1.0 / frecuencia) / (muestras - 1);

    for (int i = 0; i < muestras; i++) {
        float t = i * intervalo;
        float angulo = 2 * 3.14 * frecuencia * t;
        *(b_seno+i) = sin(angulo); // Calcula el seno que servirá para comparar con la entrada
    }

    return b_seno;
}
// señal cuadrada.
float* base_cuadrada(int muestras) {
    float *b_cuadrada = new float[muestras];

    for (int i = 0; i < muestras; i++) {
        if (i < (muestras / 2)) {
            *(b_cuadrada+i) = 1;  // Primer mitad del periodo
        } else {
            *(b_cuadrada+i) = -1; // Segunda mitad del periodo
        }
    }

    return b_cuadrada;
}

// Función que genera una señal triangular
float *base_triangular( int muestras) {
    float *b_triangular = new float[muestras];  // Pendiente de la onda triangular
    float valor = 0;  // Valor inicial

    for (int i = 0; i < muestras; i++) {
        *(b_triangular+i) = valor;
        if (i < muestras / 2) {
            valor +=  (2.0 / (muestras / 2));  // Subida
        } else {
            valor -=  (2.0 / (muestras / 2));  // Bajada
        }
    }

    return b_triangular;
}
//Generamos el coeficiente de correlación encargado de encontrar similitud entre señales.
float coeficiente_correlacion(float* x, float* y, int n) {
    float suma_xy = 0, suma_x2 = 0, suma_y2 = 0;

    // Calcular la correlación cruzada y las autocorrelaciones
    for (int i = 0; i < n; i++) {
        suma_xy += x[i] * y[i];   // Correlación cruzada Rxy(0)
        suma_x2 += x[i] * x[i];   // Autocorrelación Rxx(0)
        suma_y2 += y[i] * y[i];   // Autocorrelación Ryy(0)
    }

    // Coeficiente de correlación normalizado
    return suma_xy / (sqrt(suma_x2) * sqrt(suma_y2));
}

