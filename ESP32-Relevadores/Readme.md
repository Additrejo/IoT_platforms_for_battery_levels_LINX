# Relevadores - Battery Level Gauge  

## Autores ✒️

- **Addi Trejo** - _Trabajo inicial_ - [Addi Trejo](https://github.com/additrejo)
  
#### Last update: 
22 Ene 2025: Conexión, configuración para encender 2 leds.  
----  

## Descripción
En esta carpeta se encuentran los códigos para la experimentación en los relevadores de los ciclos de carga.

---- 
### Requisitos 📋

Lista de software y Hardware necesarios:

Sofware
- IDE Ardunino

Hardware
- ESP32
- Relevadores
----

### Arduino IDE 📋
librerias:

```bash
No utiliza una librería en especifico para su funcionameinto 
```
----

## proyecto 🚀

### Conexión 2 leds controlados por dos relevadores  .
Se construyó el siguiente circuito:  
[![Relevador-con-2-salidas-led.jpg](https://i.postimg.cc/t4jFppHf/Relevador-con-2-salidas-led.jpg)](https://postimg.cc/dL5h9M3m)  

## Conexiones:
#Módulo relé 1:

Conecta el pin IN del relé 1 al GPIO 23 de la ESP32.  
Alimenta el módulo relé con VCC (3.3V o 5V según el módulo) y GND de la ESP32.  
Conecta el LED 1 al canal del relé 1:  
Conecta el terminal común (COM) del relé 1 a una fuente de energía (por ejemplo, +5V).  
Conecta el terminal NO (Normally Open) del relé 1 al ánodo del LED 1.  
Conecta el cátodo del LED 1 a una resistencia (220-330 Ω) y luego a GND.  

#Módulo relé 2:  

Conecta el pin IN del relé 2 al GPIO 22 de la ESP32.  
Alimenta el módulo relé con VCC y GND de la ESP32.  
Conecta el LED 2 al canal del relé 2 de manera similar al LED 1:  
COM del relé 2 a +5V.  
NO del relé 2 al ánodo del LED 2.  
Cátodo del LED 2 a una resistencia y luego a GND.   

```bash

/*
Autor: Addi Trejo
Programa: 2 leds controlados por 2 reles
Last Update: Ene 22 2024
Descripcion: Este programa controla el encendido de 2 leds con el uso de dos reles. 
*/

// Definimos el pin del relé
const int relePin = 25;

void setup() {
  // Configuramos el pin del relé como salida
  pinMode(relePin, OUTPUT);

  // Inicializamos el relé apagado
  digitalWrite(relePin, LOW);
}

void loop() {
  // Encender el relé
  digitalWrite(relePin, HIGH);
  delay(2000); // Mantener encendido por 20 segundos

  // Apagar el relé
  digitalWrite(relePin, LOW);
  delay(2000); // Mantener apagado por 2 segundos
}

```
