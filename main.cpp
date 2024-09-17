#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  // Pines para el LCD

const int pinEntrada = A5;  // Pin de entrada para la señal
const int numCrucesParaFrecuencia = 5; // Número de cruces por el valor máximo para calcular la frecuencia
bool recopilacion = false;

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);  // Inicializa el LCD con 16 columnas y 2 filas
    pinMode(6, INPUT);
    pinMode(7, INPUT);
}

float Amplitud() {
    int maxVal = 0;
    int minVal = 1023;

    for (int i = 0; i < 100; i++) {  // Toma 100 muestras
        int lectura = analogRead(pinEntrada);
        if (lectura > maxVal) maxVal = lectura;
        if (lectura < minVal) minVal = lectura;
        delay(10); // Espera 10 ms entre lecturas
    }

    return (maxVal - minVal) * (5.0 / 1023.0); // Convierte a voltios
}

// Funciones para la detección y reconocimiento de la señal
// Función que genera la base para seno
float *base_seno(float frecuencia, int muestras) {
    float *b_seno = new float[muestras];
    float intervalo = (1.0 / frecuencia) / (muestras - 1);

    for (int i = 0; i < muestras; i++) {
        float t = i * intervalo;
        float angulo = 2 * 3.14 * frecuencia * t - (3.14 / 2);  // Ajusta el ángulo para empezar en el máximo
        *(b_seno + i) = sin(angulo); // Calcula el seno que servirá para comparar con la entrada
    }

    return b_seno;
}

// Función que genera una señal cuadrada
float *base_cuadrada(float frecuencia, int muestras) {
    float *b_cuadrada = new float[muestras];
    int mitad = muestras / 2;  // La mitad de las muestras serán altas, y la otra mitad bajas

    for (int i = 0; i < muestras; i++) {
        if (i < mitad) {
            *(b_cuadrada + i) = 1;  // Primer mitad del periodo (señal alta)
        } else {
            *(b_cuadrada + i) = -1; // Segunda mitad del periodo (señal baja)
        }
    }

    return b_cuadrada;
}

// Función que genera una señal triangular
float *base_triangular(float frecuencia, int muestras) {
    float *b_triangular = new float[muestras];
    int mitad = muestras / 2;
    float incremento = 2.0 / mitad;  // Incremento para subir desde -1 hasta 1

    for (int i = 0; i < muestras; i++) {
        if (i < mitad) {
            *(b_triangular + i) = -1 + i * incremento;  // Subida en la primera mitad
        } else {
            *(b_triangular + i) = 1 - (i - mitad) * incremento;  // Bajada en la segunda mitad
        }
    }

    return b_triangular;
}

// Generamos el coeficiente de correlación encargado de encontrar similitud entre señales
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

// Función encargada de la frecuencia del sistema
float Frecuencia() {
    int valorMax = 0;
    int valorMin = 1023;
    int valorPrevio = 0;
    unsigned long prevTime = 0;
    unsigned long ultimoTiempoReset = 0;
    float frecuencia = 0.0;
    int umbral = 0;
    unsigned long tiempoActual = millis();
    int contadorCruces = 0; // Contador para registrar el número de cruces por el umbral

    // Cambia el ciclo de for a un ciclo controlado por tiempo (2 segundos, por ejemplo)
    unsigned long tiempoInicio = millis();
    while (millis() - tiempoInicio < 2000) { // Muestrea por 2 segundos
        int valorActual = analogRead(pinEntrada);

        // Actualización de valor máximo y mínimo
        if (valorActual > valorMax) {
            valorMax = valorActual;
        }
        if (valorActual < valorMin) {
            valorMin = valorActual;
        }

        umbral = (valorMax + valorMin) / 2;

        // Actualizar el tiempo en cada iteración
        tiempoActual = millis();

        // Detectar el cruce de umbral
        if (valorActual > umbral && valorPrevio <= umbral) {
            if (prevTime > 0) {
                // Contabilizar los cruces por el umbral
                contadorCruces++;
            }
            prevTime = tiempoActual;
        }

        valorPrevio = valorActual;

        // Restablecer valorMax y valorMin cada segundo
        if (tiempoActual - ultimoTiempoReset > 1000) {
            valorMax = 0;
            valorMin = 1023;
            ultimoTiempoReset = tiempoActual;
        }

        delay(1); // Reduce el retardo para mayor precisión en señales de baja frecuencia
    }

    // Calcular frecuencia según el número de cruces por el umbral detectados en 2 segundos
    if (contadorCruces > 0) {
        frecuencia = contadorCruces / 2.0; // Divide entre 2 para obtener ciclos completos
    } else {
        frecuencia = 0.0; // Si no hay cruces, la frecuencia es 0
    }

    return frecuencia;
}


// Captura las muestras de la señal
void Muestras(float frecuencia, int numeroMuestras, float* muestras) {
    int intervalo = 1000000 / (2 * frecuencia);  // Intervalo en microsegundos para cumplir con Nyquist
    float maxSignal = 0.0;
    float currentSignal = 0.0;
    float valorPrev = 0.0;

    // Espera hasta que se detecte un valor máximo de la señal
    while (true) {
        currentSignal = analogRead(A5) * (5.0 / 1023.0);  // Lee la señal actual
        if (currentSignal > maxSignal) {
            maxSignal = currentSignal;  // Actualiza el valor máximo
        }

        valorPrev = currentSignal;
        // Inicia la toma de muestras cuando la señal comience a disminuir después del valor máximo
        if (currentSignal < maxSignal * 0.99) {  // Tolerancia para evitar variaciones pequeñas
            break;
        }
    }

    // Toma las muestras una vez que se ha alcanzado el máximo
    for (int i = 0; i < numeroMuestras; i++) {
        muestras[i] = analogRead(A5) * (5.0 / 1023.0);  // Captura la señal y la convierte a voltios
        delayMicroseconds(intervalo);  // Espera el tiempo calculado en microsegundos
    }
}

void loop() {
    if (digitalRead(6) == HIGH) {
        recopilacion = true;
    }
    if (digitalRead(7) == HIGH) {
        recopilacion = false;
    }

    if (recopilacion) {
        float frecuencia = Frecuencia();
        float amplitud = Amplitud();
        int numeroMuestras = (1000.0 / frecuencia) / (1000.0 / (2 * frecuencia + 200));


        float* muestras = new float[numeroMuestras];

        Muestras(frecuencia, numeroMuestras, muestras);


        float* base_seno_ = base_seno(frecuencia, numeroMuestras);
        float* base_cuadrada_ = base_cuadrada(frecuencia, numeroMuestras);
        float* base_triangular_ = base_triangular(frecuencia, numeroMuestras);

        float correlacion_seno = coeficiente_correlacion(base_seno_, muestras, numeroMuestras);
        float correlacion_cuadrada = coeficiente_correlacion(base_cuadrada_, muestras, numeroMuestras);
        float correlacion_triangular = coeficiente_correlacion(base_triangular_, muestras, numeroMuestras);

        delete[] base_seno_;
        delete[] base_cuadrada_;
        delete[] base_triangular_;
        delete[] muestras;

        // Mostrar en el LCD
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Frecuencia: ");
        lcd.print(frecuencia);
        lcd.setCursor(0, 1);
        lcd.print("Amplitud: ");
        lcd.print(amplitud);

        // Determinar el tipo de señal

        for(int j=0;j<1000;j++){
            if (correlacion_seno > correlacion_cuadrada && correlacion_seno > correlacion_triangular &&correlacion_seno>0) {
                Serial.println("Senoidal");
                //recopilacion=false;
                break;
            } else if (correlacion_cuadrada > correlacion_seno && correlacion_cuadrada > correlacion_triangular && correlacion_cuadrada>0) {
                Serial.println("Cuadrada");
                //recopilacion=false;
                break;
            } else if(correlacion_triangular > correlacion_seno && correlacion_triangular > correlacion_cuadrada && correlacion_triangular>0){
                Serial.println("Triangular");
                //recopilacion=false;
                break;
            }
        }
        if(recopilacion==true){
            Serial.print("no detectada");

        }

        delay(2000); // Muestra los datos durante 2 segundos
    }
}
