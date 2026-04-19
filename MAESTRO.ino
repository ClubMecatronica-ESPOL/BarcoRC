#include <Bluepad32.h>

ControllerPtr myControllers[BP32_MAX_GAMEPADS];
int lastAngulo = -1;
int lastVelocidad = -1;
unsigned long lastSendTime = 0;

// Usamos 0xFF (255) como cabecera estricta, asegurando que los datos de control nunca lleguen a 255.
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
    // Configuración del XBee (Asegúrate de que los baudios coincidan con tu XBee)
    Serial2.begin(115200, SERIAL_8N1, 16, 17); 
    BP32.setup(&onConnectedController, nullptr);
}

void loop() {
    BP32.update();
    unsigned long now = millis();

    for (auto ctl : myControllers) {
        if (ctl && ctl->isConnected() && ctl->hasData()) {
            
            // --- 1. SERVO (Joystick Izquierdo X) ---
            int rawX = ctl->axisX();
            // ZONA MUERTA: Ignorar el "drift" o desgaste mecánico del joystick en el centro
            if (abs(rawX) < 40) {
                rawX = 0; 
            }
            // Mapeo normal y límite estricto
            int angulo = map(rawX, -508, 512, 0, 180);
            angulo = constrain(angulo, 0, 180);

            // --- 2. MOTOR (Gatillos R2 - L2) ---
            int potenciaNeta = ctl->throttle() - ctl->brake(); 
            // ZONA MUERTA: Ignorar si los gatillos no están presionados firmemente
            if (abs(potenciaNeta) < 20) {
                potenciaNeta = 0;
            }
            // Mapeamos a un máximo de 253 para no confundir el valor con la cabecera (255)
            int velocidad = map(potenciaNeta, -1020, 1020, 0, 253); 
            velocidad = constrain(velocidad, 0, 253);

            // --- 3. ENVÍO DE DATOS ---
            // Se envía si hay ALGÚN cambio o si han pasado 150ms (señal de vida / Keep-Alive)
            if ((angulo != lastAngulo || velocidad != lastVelocidad) || (now - lastSendTime > 150)) {
                
                // Checksum simple sumando y enmascarando
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