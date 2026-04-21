#include <Bluepad32.h>

ControllerPtr myControllers[BP32_MAX_GAMEPADS];
int lastAngulo = -1;
int lastVelocidad = -1;
unsigned long lastSendTime = 0;

const uint8_t HEADER = 0xFF; 

void onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            myControllers[i] = ctl;
            break;
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, 16, 17); 
    BP32.setup(&onConnectedController, nullptr);
}

void loop() {
    BP32.update();
    unsigned long now = millis();

    for (auto ctl : myControllers) {
        if (ctl && ctl->isConnected() && ctl->hasData()) {
            
            // --- DIRECCIÓN (Eje X) ---
            int rawX = ctl->axisX();
            if (abs(rawX) < 40) rawX = 0; // Zona muerta mecánica
            
            int angulo = map(rawX, -508, 512, 0, 180); 
            angulo = constrain(angulo, 0, 180);

            // --- ACELERACIÓN (Gatillos) ---
            int potenciaNeta = ctl->throttle() - ctl->brake(); 
            if (abs(potenciaNeta) < 20) potenciaNeta = 0; // Zona muerta mecánica
            
            int velocidad = map(potenciaNeta, -1020, 1020, 0, 253); 
            velocidad = constrain(velocidad, 0, 253);

            // --- TRANSMISIÓN ---
            if ((angulo != lastAngulo || velocidad != lastVelocidad) || (now - lastSendTime > 150)) {
                uint8_t checksum = (angulo + velocidad) & 0xFE; 

                Serial2.write(HEADER);
                Serial2.write((uint8_t)angulo);
                Serial2.write((uint8_t)velocidad);
                Serial2.write(checksum);
                
                lastAngulo = angulo;
                lastVelocidad = velocidad;
                lastSendTime = now;
            }
        }
    }
}