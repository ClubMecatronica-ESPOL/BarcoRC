#include <Arduino.h>
#include <ESP32Servo.h>

Servo miServo;
// Definición de pines
const int pinServo = 18;
const int pinENA = 14; 
const int pinIN1 = 26; 
const int pinIN2 = 27;

const uint8_t HEADER = 0xFF;

// --- VARIABLES PARA FILTRO DE SUAVIZADO (EMA) ---
float anguloActual = 90.0;     // Centro inicial del servo
float velocidadActual = 126.0; // Centro neutral del motor (entre 0 y 253)
const float alpha = 0.7;       // Factor de respuesta: 0.1 (suave/lento) a 1.0 (instantáneo/brusco)

// Variable para el temporizador de seguridad (Watchdog)
unsigned long ultimaVezRecibido = 0; 

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, 16, 17);
    
    // PREVENCIÓN DE COLISIÓN DE TIMERS (Evita vibración en el servo al usar PWM del motor)
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    
    miServo.setPeriodHertz(50); // Frecuencia estándar para servos
    miServo.attach(pinServo, 500, 2350); // Rango de pulso estándar
    
    pinMode(pinIN1, OUTPUT);
    pinMode(pinIN2, OUTPUT);
    
    // Configuración del PWM para el motor (5000Hz es óptimo para el L298N)
    ledcAttach(pinENA, 5000, 8); 
}

void loop() {
    // --- 1. LECTURA Y VALIDACIÓN ---
    while (Serial2.available() >= 4) {
        if (Serial2.read() == HEADER) {
            uint8_t angRecibido = Serial2.read();
            uint8_t velRecibida = Serial2.read();
            uint8_t chkRecibido = Serial2.read();

            uint8_t chkCalculado = (angRecibido + velRecibida) & 0xFE;

            // Si el paquete está corrupto por ruido, se ignora por completo
            if (chkRecibido == chkCalculado) {
                // Filtro Pasa Bajas (suaviza los picos bruscos de corriente)
                anguloActual = (alpha * angRecibido) + ((1.0 - alpha) * anguloActual);
                velocidadActual = (alpha * velRecibida) + ((1.0 - alpha) * velocidadActual);
                
                // Reiniciamos el cronómetro de seguridad porque recibimos un dato válido
                ultimaVezRecibido = millis(); 
            }
        }
    }

    // --- 2. FAIL-SAFE (SEGURIDAD ANTE PÉRDIDA DE SEÑAL) ---
    // Si pasan más de 250ms sin señal del maestro, el sistema entra en modo seguro
    if (millis() - ultimaVezRecibido > 250) {
        anguloActual = 90.0;     // Timón al centro
        velocidadActual = 126.0; // Motores apagados
    }

    // --- 3. EJECUCIÓN FÍSICA ---
    int anguloFinal = round(anguloActual);
    int velFinal = round(velocidadActual);

    // CONTROL DEL SERVO (Con zona muerta de centro absoluto)
    if (anguloFinal > 87 && anguloFinal < 93) {
        miServo.write(90);
    } else {
        miServo.write(anguloFinal);
    }

    // CONTROL DEL MOTOR DC (Con zona muerta amplia de apagado)
    if (velFinal > 137) { // AVANCE
        // Mapeamos el PWM desde 80 (para vencer la inercia del motor) hasta 255
        int pwm = map(velFinal, 137, 253, 80, 255); 
        digitalWrite(pinIN1, HIGH);
        digitalWrite(pinIN2, LOW);
        ledcWrite(pinENA, pwm);
    } 
    else if (velFinal < 117) { // REVERSA
        // Mapeamos el PWM en reversa
        int pwm = map(velFinal, 117, 0, 80, 255);
        digitalWrite(pinIN1, LOW);
        digitalWrite(pinIN2, HIGH);
        ledcWrite(pinENA, pwm);
    } 
    else { // STOP ABSOLUTO
        digitalWrite(pinIN1, LOW);
        digitalWrite(pinIN2, LOW);
        ledcWrite(pinENA, 0); // Cortar voltaje al motor
    }
    
    // Pequeña pausa para permitir que los procesos en segundo plano del ESP32 respiren
    delay(2); 
}