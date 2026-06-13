# 🛠️ Documentación de Hardware: Electrónica y Diseño de PCB

Este documento detalla la arquitectura de hardware de la embarcación, los componentes principales y el diseño de la placa de circuito impreso (PCB) desarrollada para garantizar la estabilidad del sistema frente al ruido electromagnético.

---

## 1. Componentes Principales

La electrónica de la embarcación está segmentada en dos módulos principales: el transmisor (mando de control) y el receptor (barco).

### Transmisor (Maestro)
* **Microcontrolador:** ESP32 (Maneja la recepción Bluetooth del control de PS4 y el empaquetado de datos).
* **Módulo de Comunicación:** XBee Pro S1. Actúa como enlace de radiofrecuencia (RF) punto a punto de largo alcance y alta fiabilidad.

### Receptor (Esclavo - Barco)
* **Microcontrolador:** ESP32 (Interpreta la trama de datos y genera las señales PWM).
* **Módulo de Comunicación:** XBee Pro S1.
* **Propulsión Principal:** Motor Brushless acoplado a un ESC (Electronic Speed Controller).
* **Dirección:** Servomotor estándar conectado al timón.

---

## 2. Gestión de Energía e Interferencias

Uno de los mayores retos en la ingeniería de vehículos RC con motores Brushless es el ruido electromagnético (EMI) y los picos de consumo de corriente. Si el ESC y el motor comparten la misma línea de alimentación sin filtrado con el microcontrolador, los picos de arranque del motor pueden causar caídas de tensión (brownouts) que reinician el ESP32 o corrompen los datos del XBee.

**Solución Implementada:**
* **Aislamiento de Lógica y Potencia:** Se separó físicamente la circuitería lógica (que opera a 3.3V para el ESP32 y el XBee) de las líneas de alta corriente que van hacia el ESC y el motor Brushless.
* **Filtrado:** Uso de capacitores de desacople estratégicamente ubicados para absorber picos de voltaje y estabilizar las señales.

---

## 3. Placa de Circuito Impreso (PCB) a Medida

Para pasar del protoboard a una solución robusta y resistente a las vibraciones mecánicas propias de una embarcación, se manufacturó una PCB personalizada mediante fresado (aislamiento de pistas). 

**Características del Diseño de la PCB:**
* **Planos de Masa (Ground Planes):** Maximizan la dispersión de calor y actúan como blindaje electromagnético contra el ruido generado por la conmutación del motor Brushless.
* **Trazos de Señal Aislados:** Las pistas de comunicación serial (TX/RX) entre el ESP32 y el XBee están ruteadas evitando paralelismos innecesarios con las líneas de alimentación.
* **Conectores Modulares:** Se integraron regletas y pines (headers) hembra/macho para permitir el fácil reemplazo del ESP32 o el XBee sin necesidad de desoldar componentes.

*(Nota: En la carpeta `/images` de este repositorio se puede visualizar la placa fresada en cobre).*

---

## 4. Esquemas y Simulación en Proteus

Toda la validación de conexiones, lógica de pines y disposición de componentes se simuló y esquematizó previamente en software.

Los archivos fuente del circuito se encuentran en la carpeta `/hardware/proteus` de este repositorio. Para visualizar o editar el circuito:
1. Asegúrate de tener instalado Proteus Design Suite.
2. Abre el archivo principal `.pdsprj`.
3. Revisa la correcta asignación de las librerías del ESP32 y XBee en tu entorno local antes de ejecutar la simulación.
