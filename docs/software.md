# 💻 Documentación de Software: Lógica y Control (Versión Brushless)

El sistema operativo del barco a control remoto está dividido en una arquitectura de **Maestro (Transmisor)** y **Esclavo (Receptor)**, ejecutados en microcontroladores ESP32 y programados bajo el entorno de Arduino IDE.

Este documento detalla la estructura y el flujo de los scripts `MAESTRO_BRUSHLESS.ino` y `ESCLAVO_BRUSHLESS.ino`.

---

## 1. Arquitectura de Comunicación y Topología Dual

El sistema operativo del barco requiere una configuración asimétrica dividida en dos firmwares independientes cargados en dos microcontroladores ESP32 separados:

1.  **Nodo Maestro (Transmisor):** Dedicado exclusivamente a procesar las interrupciones del stack Bluetooth del control de PS4, mapear las zonas muertas y transmitir los comandos por hardware serial.
2.  **Nodo Esclavo (Receptor):** Dedicado a la escucha continua del puerto serial, filtrado matemático de datos, ejecución de la secuencia de armado y generación de señales PWM de potencia.

### Capa de Enlace en Modo Broadcast (Difusión)
[cite_start]La comunicación por el puerto `Serial2` (pines 16 y 17) opera de manera unidireccional a una tasa de transferencia alta de 115200 baudios[cite: 3, 16, 30, 51]. Al utilizar el método de **Broadcast**, el Maestro inyecta la trama directamente al espectro sin esperar confirmaciones de recepción (ACK) ni establecer handshakes previos. 

Esto garantiza que el retardo entre el movimiento del joystick en tierra y la respuesta del timón o motor en el agua sea prácticamente nulo, optimizando la maniobrabilidad de la embarcación bajo condiciones de estrés operativo.

---

## 2. Arquitectura de Comunicación (Capa de Enlace)

Para garantizar que el barco no interprete ruido electromagnético o paquetes a medias como comandos, se diseñó un protocolo de envío rígido utilizando los módulos XBee por el puerto Serial2 a 115200 baudios.

### Trama de Datos de 4 Bytes
Cada instrucción enviada por el Maestro consta de 4 bytes estrictos:

1.  **Cabecera (Header): `0xFF` (255):** Indica el inicio absoluto de una instrucción.
2.  **Ángulo de Dirección:** Valor entre `0` y `180` grados (Eje X del joystick).
3.  **Aceleración (Potencia):** Valor mapeado entre `0` y `253`. (El límite es 253 para asegurar que un dato nunca coincida accidentalmente con la cabecera `0xFF`).
4.  **Checksum (Validación):** Una suma matemática básica enmascarada `(angulo + velocidad) & 0xFE`.

> **Filtro de Descarte:** Si el Esclavo recibe un Checksum que no coincide matemáticamente con los datos de ángulo y aceleración, ignora el paquete completo, asumiéndolo como corrupto por ruido RF.

---

## 3. Controlador Maestro (Transmisor)

El script `MAESTRO_BRUSHLESS.ino` actúa como el puente entre el usuario (mediante el control de PS4) y el módulo transmisor XBee.

### Flujo de Trabajo y Buenas Prácticas:
* **Integración Bluetooth:** Utiliza la librería `Bluepad32` para emparejar y recibir datos en tiempo real de un control de PlayStation 4.
* **Zonas Muertas (Deadzones):** Se aplica un filtro mecánico en la lectura del joystick (dirección) y de los gatillos (aceleración). Si los valores brutos son menores a `40` o `20` respectivamente, se fuerzan a `0`. Esto evita que el barco se mueva solo por el desgaste físico normal de los resortes del control (drift).
* **Ahorro de Ancho de Banda (Keep-Alive):** El ESP32 solo transmite por RF si detecta un cambio en la posición de los mandos. Sin embargo, para que el receptor sepa que sigue conectado, envía un paquete de "latido" (Keep-Alive) cada 150 milisegundos si no hay actividad.

---

## 4. Controlador Esclavo (Receptor)

El script `ESCLAVO_BRUSHLESS.ino` es el cerebro del barco. Su objetivo principal es interpretar de forma segura las instrucciones del XBee, transformándolas en señales PWM y cuidando los componentes electrónicos.

### Subsistemas Clave:

* **Secuencia de Armado ESC (Electronic Speed Controller):** Al iniciar, el programa envía de forma sostenida un pulso de `1000 microsegundos` durante 3 segundos al ESC. Esto es un requisito de seguridad estándar para que el controlador Brushless inicialice sin disparar el motor.
* **Asignación de Timers (Anti-Vibración):** El ESP32 sufre de colisión de temporizadores de hardware cuando se usan PWMs de forma nativa junto con la librería Servo. Se resuelve forzando el Timer 0 para el Servo (Timón) y el Timer 1 para el ESC (Motor Principal).
* **Filtro EMA (Exponential Moving Average):** Las lecturas válidas de velocidad y dirección no se aplican directamente a los motores. Pasan por una fórmula de suavizado matemático: `Valor = (0.7 * Nuevo_Dato) + (0.3 * Valor_Anterior)`. Esto previene picos de corriente catastróficos que podrían quemar el ESC o reiniciar el ESP32 si el usuario mueve el mando bruscamente de 0 a 100%.
* **Watchdog (Fail-Safe Tolerante):** Si el ESP32 receptor no detecta un paquete válido del maestro en un lapso de **400 ms** (asumiendo pérdida de señal, interferencia severa o control remoto apagado), el barco entra en modo seguro: el timón se bloquea en 90° (centro) y el motor Brushless se apaga de inmediato bajando a 1000 microsegundos.
* **Guillotinas Matemáticas:** Como filtro extra final, se fuerza al servo del timón al centro exacto (90°) si los valores oscilan cerca del punto neutro (85° a 95°), garantizando trayectorias completamente rectas sin oscilaciones del servo.

---

## 5. Dependencias Requeridas
* **Bluepad32:** Para la gestión del control Bluetooth de PS4 (Lado Maestro).
* **ESP32Servo:** Para la correcta manipulación de señales de control en microsegundos y evitar problemas de hardware con los temporizadores nativos (Lado Esclavo).
