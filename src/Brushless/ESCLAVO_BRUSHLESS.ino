#include <Arduino.h>
#include <ESP32Servo.h>

Servo miServo; 
Servo miESC;   

const int pinServo = 18;
const int pinESC = 13;   

const uint8_t HEADER = 0xFF;

float anguloActual = 90.0;     
float velocidadActual = 126.0; 
const float alpha = 0.7;       

unsigned long ultimaVezRecibido = 0; 

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, 16, 17);
    
    // Asignación estricta de Timers para evitar desconexiones
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    
    // Forzamos a que el servo use el Timer 0 y el ESC el Timer 1
    miServo.setPeriodHertz(50);
    miServo.attach(pinServo, 500, 2400);
    
    miESC.setPeriodHertz(50);
    miESC.attach(pinESC, 1000, 2000); 

    // --- SECUENCIA DE ARMADO (UNIDIRECCIONAL) ---
    Serial.println("Armando ESC (1000us)...");
    miESC.writeMicroseconds(1000); // 1000 = Freno/Apagado
    delay(3000);                   
    Serial.println("¡ESC Listo!");
}

void loop() {
    // --- 1. RECEPCIÓN SEGURA ---
    while (Serial2.available() >= 4) {
        if (Serial2.read() == HEADER) {
            uint8_t angRecibido = Serial2.read();
            uint8_t velRecibida = Serial2.read();
            uint8_t chkRecibido = Serial2.read();

            if (chkRecibido == ((angRecibido + velRecibida) & 0xFE)) {
                anguloActual = (alpha * angRecibido) + ((1.0 - alpha) * anguloActual);
                velocidadActual = (alpha * velRecibida) + ((1.0 - alpha) * velocidadActual);
                ultimaVezRecibido = millis(); 
            }
        }
    }

    // --- 2. FAIL-SAFE TOLERANTE ---
    // Aumentado a 400ms para evitar parpadeos si el XBee pierde un paquete
    if (millis() - ultimaVezRecibido > 400) {
        anguloActual = 90.0;     
        velocidadActual = 126.0; 
    }

    int anguloFinal = round(anguloActual);
    int velFinal = round(velocidadActual);

    // --- 3. GUILLOTINAS MATEMÁTICAS (Eliminador de Fantasmas) ---
    
    // A. Filtro estricto para el Servo (Rango muerto entre 85° y 95°)
    if (anguloFinal > 85 && anguloFinal < 95) {
        miServo.write(90); // Forzar el centro exacto
    } else {
        miServo.write(anguloFinal);
    }

    // B. Lógica Unidireccional para el ESC
    int microsegundosESC = 1000; // Por defecto: Apagado

    // El centro del gatillo es 126. Requerimos al menos 140 para que se digne a arrancar
    if (velFinal > 140) { 
        // Acelerar (R2): Mapeamos hacia adelante
        microsegundosESC = map(velFinal, 140, 253, 1050, 2000); 
    } 
    else {
        // Freno (L2) o Gatillos sueltos (<140): Apagado estricto
        microsegundosESC = 1000;
    }

    miESC.writeMicroseconds(microsegundosESC);

    delay(5); 
}