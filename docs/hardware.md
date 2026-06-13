# 🛠️ Documentación de Hardware: Electrónica y Diseño de PCB

Este documento detalla la arquitectura de hardware de la embarcación, los componentes principales y el diseño de la placa de circuito impreso (PCB) desarrollada para integrar el sistema de control.

---

## 1. Componentes Principales

La electrónica de la embarcación está segmentada en dos módulos principales: el transmisor (mando de control) y el receptor (barco).

### Transmisor (Maestro)
* **Microcontrolador:** ESP32 (Maneja la recepción Bluetooth del control de PS4 y el empaquetado de datos).
* **Módulo de Comunicación:** XBee Pro S1. Actúa como enlace de radiofrecuencia (RF) punto a punto de largo alcance.

### Receptor (Esclavo - Barco)
* **Microcontrolador:** ESP32 (Interpreta la trama de datos y genera las señales PWM).
* **Módulo de Comunicación:** XBee Pro S1.
* **Propulsión Principal:** Motor Brushless acoplado a un ESC (Electronic Speed Controller).
* **Dirección:** Servomotor estándar conectado al timón.

---

## 2. Gestión de Energía e Interfaz de Potencia

La alimentación del sistema en el barco está centralizada para optimizar el peso y simplificar las conexiones:

* **Alimentación Principal:** Batería LiPo de alta descarga conectada directamente al controlador de velocidad (ESC).
* **Alimentación Lógica (BEC):** Se está utilizando el circuito regulador interno (BEC - Battery Eliminator Circuit) de 5V del propio ESC. Este voltaje de 5V se inyecta directamente al pin **VIN** del ESP32, alimentando así la etapa lógica.
* *(Nota de Desarrollo: Actualmente el sistema opera con conexiones directas sin una etapa de filtrado adicional con capacitores. A medida que avancen las pruebas de carga mecánica en el agua, se evaluará si el ruido del motor Brushless requiere implementar un banco de condensadores de desacople).*

---

## 3. Placa de Circuito Impreso (PCB) a Medida

Para pasar del protoboard a una solución más sólida y evitar desconexiones por las vibraciones mecánicas propias de una embarcación, se manufacturó una PCB personalizada en cobre.

**Método de Fabricación:**
La placa fue fabricada mediante un método híbrido de sustracción que combina fabricación digital y ataque químico:
1. **Ablación Láser:** Se aplicó una capa protectora sobre la placa de cobre virgen y se utilizó un láser de corte/grabado para retirar selectivamente esta máscara, dejando expuestas únicamente las áreas correspondientes al aislamiento de las pistas.
2. **Ataque Químico:** Posteriormente, la placa fue sumergida en Cloruro Férrico, el cual reaccionó y disolvió el cobre expuesto, dejando las pistas limpias y perfectamente definidas bajo la pintura restante.

**Características del Diseño:**
* **Planos de Masa (Ground Planes):** Diseñados para maximizar la unificación de tierras de los diferentes componentes y disipar mejor el calor.
* **Conectores Modulares:** Se integraron pines y regletas para montar y desmontar fácilmente el ESP32 y el módulo XBee sin necesidad de soldarlos directamente a la placa, facilitando recambios.

---

## 4. Esquemas y Simulación

Toda la validación de conexiones, lógica de pines y disposición de componentes se diseñó previamente.
*(Se recomienda anexar aquí los archivos o capturas del esquema en Proteus correspondientes al diseño de la PCB actual).*
