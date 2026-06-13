<p align="center">
  <img src="/images/Placa.jpeg" width="300" alt="Placa PCB BarcoRC">
</p>

<h1 align="center">🚤 Proyecto BarcoRC</h1>

<p align="center">
  <strong>Sistema integral de telemetría, potencia y control para embarcación a escala.</strong><br>
  <i>Desarrollado en el marco de las actividades del Club de Mecatrónica - ESPOL.</i>
</p>

---

## 📖 Descripción del Proyecto

**BarcoRC** es un proyecto de ingeniería mecatrónica enfocado en el diseño, construcción y validación de una embarcación a control remoto de alto rendimiento. El sistema resuelve problemas críticos de control inalámbrico y ruido electromagnético mediante el uso de microcontroladores duales, enlaces de radiofrecuencia industriales y placas de circuito impreso (PCB) fabricadas a medida.

### ✨ Características Principales
* **Telemetría de Larga Distancia:** Enlace robusto en modo *Broadcast* utilizando módulos **XBee Pro S1** (IEEE 802.15.4).
* **Control Inteligente:** Interfaz de usuario intuitiva mediante emparejamiento Bluetooth nativo con un control de **PlayStation 4 (DualShock 4)**.
* **Procesamiento Dual:** Arquitectura Maestro/Esclavo impulsada por dos microcontroladores **ESP32**.
* **Potencia y Propulsión:** Motor Brushless gestionado por un ESC (Electronic Speed Controller) con rutinas de filtrado matemático (EMA) para evitar picos de corriente.
* **Hardware Customizado:** PCB manufacturada mediante ablación láser y ataque químico, con planos de masa para mitigar interferencias electromagnéticas (EMI).

---

## 📂 Estructura del Repositorio

El proyecto está organizado de la siguiente manera para facilitar su lectura y escalabilidad:

* 📁 **`/docs`**: Documentación técnica detallada.
  * `hardware.md`: Detalles de la electrónica, PCB y esquema de alimentación.
  * `software.md`: Lógica de control, trama de datos XBee y algoritmos del ESP32.
* 📁 **`/src`**: Código fuente principal (Firmware Arduino).
  * `MAESTRO_BRUSHLESS.ino`: Código del mando transmisor (PS4 a XBee).
  * `ESCLAVO_BRUSHLESS.ino`: Código del barco receptor (XBee a PWM/Motores).
* 📁 **`/hardware`**: Archivos de diseño electrónico.
  * `/proteus`: Esquemáticos y simulaciones del circuito.
* 📁 **`/images`**: Registro fotográfico de la placa, pruebas y ensamblaje.

---

## ⚙️ Requisitos y Dependencias (Software)

Para compilar y cargar el código en los microcontroladores ESP32, es necesario instalar las siguientes librerías en el Arduino IDE:

1. **[Bluepad32](https://github.com/ricardoquesada/bluepad32):** Para la conexión Bluetooth con el mando de PS4 (Solo Maestro).
2. **[ESP32Servo](https://github.com/madhephaestus/ESP32Servo):** Para la generación de señales PWM precisas usando los Timers de hardware del ESP32 (Solo Esclavo).

---

## 🚀 Guía de Inicio Rápido

1. **Clonar el repositorio:**
   ```bash
   git clone [https://github.com/ClubMecatronica-ESPOL/BarcoRC.git](https://github.com/ClubMecatronica-ESPOL/BarcoRC.git)
