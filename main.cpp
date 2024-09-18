#include <LiquidCrystal.h>

// Inicializa la librería LiquidCrystal con los pines que controlan el LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int pinEntrada = A5;  // Pin analógico para la lectura de la señal
const int numCrucesParaFrecuencia = 5; // Número de cruces de umbral para calcular la frecuencia
bool recopilacion = false;  // Variable para iniciar o detener la toma de datos

void setup() {
    Serial.begin(9600); // Inicia la comunicación serial
    lcd.begin(16, 2);   // Inicializa el LCD con 16 columnas y 2 filas
    pinMode(6, INPUT);  // Configura el pin 6 como entrada (control para empezar a recopilar datos)
    pinMode(7, INPUT);  // Configura el pin 7 como entrada (control para detener la recopilación)
}

// Función para medir la amplitud de la señal
float Amplitud() {
    int maxVal = 0;    // Valor máximo de la señal
    int minVal = 1023; // Valor mínimo de la señal

    // Realiza 100 lecturas de la señal analógica
    for (int i = 0; i < 100; i++) {
        int lectura = analogRead(pinEntrada);  // Lee el valor de la señal
        if (lectura > maxVal) maxVal = lectura;  // Actualiza el valor máximo
        if (lectura < minVal) minVal = lectura;  // Actualiza el valor mínimo
        delay(10); // Espera 10 ms entre lecturas
    }

    // Devuelve la amplitud de la señal convertida a voltios
    return (maxVal - minVal) * (5.0 / 1023.0);
}

// Función que genera una señal base senoidal
float *base_seno(float frecuencia, int muestras) {
    float *b_seno = new float[muestras];  // Reserva memoria para la señal senoidal
    float intervalo = (1.0 / frecuencia) / (muestras - 1);  // Intervalo de tiempo entre muestras

    // Calcula los valores de la señal senoidal
    for (int i = 0; i < muestras; i++) {
        float t = i * intervalo;  // Tiempo en cada muestra
        float angulo = 2 * 3.14 * frecuencia * t - (3.14 / 2);  // Ajusta el ángulo para que empiece en el máximo
        *(b_seno + i) = sin(angulo);  // Asigna el valor del seno a cada muestra
    }

    return b_seno;  // Devuelve la señal senoidal
}

// Función que genera una señal cuadrada
float *base_cuadrada(float frecuencia, int muestras) {
    float *b_cuadrada = new float[muestras];  // Reserva memoria para la señal cuadrada
    int mitad = muestras / 2;  // La mitad de las muestras serán altas, la otra mitad bajas

    // Asigna los valores de la señal cuadrada
    for (int i = 0; i < muestras; i++) {
        if (i < mitad) {
            *(b_cuadrada + i) = 1;  // Primer mitad del periodo (señal alta)
        } else {
            *(b_cuadrada + i) = -1;  // Segunda mitad del periodo (señal baja)
        }
    }

    return b_cuadrada;  // Devuelve la señal cuadrada
}

// Función que genera una señal triangular
float *base_triangular(float frecuencia, int muestras) {
    float *b_triangular = new float[muestras];  // Reserva memoria para la señal triangular
    int mitad = muestras / 2;  // La mitad de las muestras será subida y la otra mitad bajada
    float incremento = 2.0 / mitad;  // Incremento para subir de -1 a 1

    // Asigna los valores de la señal triangular
    for (int i = 0; i < muestras; i++) {
        if (i < mitad) {
            *(b_triangular + i) = -1 + i * incremento;  // Subida en la primera mitad
        } else {
            *(b_triangular + i) = 1 - (i - mitad) * incremento;  // Bajada en la segunda mitad
        }
    }

    return b_triangular;  // Devuelve la señal triangular
}

// Función que calcula el coeficiente de correlación entre dos señales
float coeficiente_correlacion(float* x, float* y, int n) {
    float suma_xy = 0, suma_x2 = 0, suma_y2 = 0;

    // Calcular la correlación cruzada y las autocorrelaciones
    for (int i = 0; i < n; i++) {
        suma_xy += x[i] * y[i];   // Producto cruzado de ambas señales
        suma_x2 += x[i] * x[i];   // Autocorrelación de la primera señal
        suma_y2 += y[i] * y[i];   // Autocorrelación de la segunda señal
    }

    // Devuelve el coeficiente de correlación normalizado
    return suma_xy / (sqrt(suma_x2) * sqrt(suma_y2));
}

// Función que mide la frecuencia de la señal
float Frecuencia() {
    int valorMax = 0;
    int valorMin = 1023;
    int valorPrevio = 0;
    unsigned long prevTime = 0;
    unsigned long ultimoTiempoReset = 0;
    float frecuencia = 0.0;
    int umbral = 0;
    unsigned long tiempoActual = millis();
    int contadorCruces = 0;

    // Muestrea por 2 segundos
    unsigned long tiempoInicio = millis();
    while (millis() - tiempoInicio < 2000) {
        int valorActual = analogRead(pinEntrada);

        // Actualiza los valores máximos y mínimos
        if (valorActual > valorMax) {
            valorMax = valorActual;
        }
        if (valorActual < valorMin) {
            valorMin = valorActual;
        }

        umbral = (valorMax + valorMin) / 2;  // Calcula el umbral para detectar cruces

        // Detecta el cruce del umbral
        if (valorActual > umbral && valorPrevio <= umbral) {
            if (prevTime > 0) {
                contadorCruces++;  // Incrementa el contador de cruces
            }
            prevTime = tiempoActual;
        }

        valorPrevio = valorActual;

        // Restablece los valores máximos y mínimos cada segundo
        if (tiempoActual - ultimoTiempoReset > 1000) {
            valorMax = 0;
            valorMin = 1023;
            ultimoTiempoReset = tiempoActual;
        }

        delay(1);  // Retardo para aumentar la precisión
    }

    // Calcula la frecuencia en base a los cruces de umbral detectados
    if (contadorCruces > 0) {
        frecuencia = contadorCruces / 2.0;  // Cada 2 cruces es un ciclo completo
    } else {
        frecuencia = 0.0;  // Si no hay cruces, la frecuencia es 0
    }

    return frecuencia;  // Devuelve la frecuencia calculada
}

// Función que toma las muestras de la señal
void Muestras(float frecuencia, int numeroMuestras, float* muestras) {
    int intervalo = 1000000 / (2 * frecuencia);  // Calcula el intervalo entre muestras para cumplir con Nyquist
    float maxSignal = 0.0;
    float currentSignal = 0.0;
    float valorPrev = 0.0;

    // Espera a detectar un valor máximo de la señal
    while (true) {
        currentSignal = analogRead(A5) * (5.0 / 1023.0);  // Lee la señal y la convierte a voltios
        if (currentSignal > maxSignal) {
            maxSignal = currentSignal;
        }

        valorPrev = currentSignal;

        // Inicia la toma de muestras cuando la señal empieza a disminuir después del máximo
        if (currentSignal < maxSignal * 0.99) {
            break;
        }
    }

    // Toma las muestras de la señal
    for (int i = 0; i < numeroMuestras; i++) {
        muestras[i] = analogRead(A5) * (5.0 / 1023.0);  // Captura la señal y la convierte a voltios
        delayMicroseconds(intervalo);  // Espera el intervalo calculado en microsegundos
    }
}

// Función principal
void loop() {
    if (digitalRead(6) == HIGH) {  // Inicia la recopilación de datos si el botón en el pin 6 está activado
        recopilacion = true;
    }
    if (digitalRead(7) == HIGH) {  // Detiene la recopilación si el botón en el pin 7 está activado
        recopilacion = false;
    }

    if (recopilacion) {  // Si está activada la recopilación de datos
        float frecuencia = Frecuencia();  // Calcula la frecuencia de la señal
        float amplitud = Amplitud();      // Calcula la amplitud de la señal
        int numeroMuestras = (1000.0 / frecuencia) / (1000.0 / (2 * frecuencia + 200));  // Calcula el número de muestras necesarias

        float* muestras = new float[numeroMuestras];  // Reserva memoria para las muestras
        Muestras(frecuencia, numeroMuestras, muestras);  // Captura las muestras de la señal

        // Genera las señales base (senoidal, cuadrada, triangular)
        float* base_seno_ = base_seno(frecuencia, numeroMuestras);
        float* base_cuadrada_ = base_cuadrada(frecuencia, numeroMuestras);
        float* base_triangular_ = base_triangular(frecuencia, numeroMuestras);

        // Calcula los coeficientes de correlación con las señales base
        float correlacion_seno = coeficiente_correlacion(base_seno_, muestras, numeroMuestras);
        float correlacion_cuadrada = coeficiente_correlacion(base_cuadrada_, muestras, numeroMuestras);
        float correlacion_triangular = coeficiente_correlacion(base_triangular_, muestras, numeroMuestras);

        // Libera la memoria utilizada
        delete[] base_seno_;
        delete[] base_cuadrada_;
        delete[] base_triangular_;
        delete[] muestras;

        // Mostrar los resultados en el LCD
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Frecuencia: ");
        lcd.print(frecuencia);
        lcd.setCursor(0, 1);
        lcd.print("Amplitud: ");
        lcd.print(amplitud);

        // Determina el tipo de señal basado en el coeficiente de correlación
        for(int j = 0; j < 1000; j++) {
            if (correlacion_seno > correlacion_cuadrada && correlacion_seno > correlacion_triangular && correlacion_seno > 0) {
                Serial.println("Senoidal");  // Si la correlación con la señal senoidal es mayor
                break;
            } else if (correlacion_cuadrada > correlacion_seno && correlacion_cuadrada > correlacion_triangular && correlacion_cuadrada > 0) {
                Serial.println("Cuadrada");  // Si la correlación con la señal cuadrada es mayor
                break;
            } else if (correlacion_triangular > correlacion_seno && correlacion_triangular > correlacion_cuadrada && correlacion_triangular > 0) {
                Serial.println("Triangular");  // Si la correlación con la señal triangular es mayor
                break;
            }
        }

        if (recopilacion == true) {
            Serial.print("no detectada");  // Si no se detecta ninguna señal
        }

        delay(2000);  // Muestra los datos durante 2 segundos antes de tomar nuevos
    }
}
