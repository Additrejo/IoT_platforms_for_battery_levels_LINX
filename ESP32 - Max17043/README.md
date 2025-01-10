# ESP32 & MAX17043 - Battery Level Gauge  

## Autores ✒️

- **Addi Trejo** - _Trabajo inicial_ - [Addi Trejo](https://github.com/additrejo)
  
Last update: Ene 08 2025  
----  

## Descripción
En esta carpeta se encuentran los códigos para obtener el Voltaje (V) y el estado de carga (SOC) usando una ESP32 y un modulo dedicado como lo es el MAX17043.

---- 
### Requisitos 📋

Lista de software y Hardware necesarios:

Sofware
- IDE Ardunino

Hardware
- ESP32
- MAX17043
- Pantalla Oled
- Batería Ionlitio 18650 3.7V
  
----

### Arduino IDE 📋
librerias

```bash
Max1704x
```
----
## Roadmap

Ideas, mejoras planificadas y actualizaciones futuras para el proyecto actual.  

----  

## Actualizaciones  
07 Ene 2025: Conexión, configuración y obtención en el valor del voltaje real.  
08 Ene 2025: Icono de batería en tiempo real.  
09 Ene 2025: Conexión a Thingspeak en tiempo real. - [MAX17043 Voltaje y porcentaje en Thingspeak ](https://github.com/Additrejo/IoT_platforms_for_battery_levels_by_LINX/blob/main/ESP32%20-%20Max17043/ESP32-MAX17043-OLED-TS/ESP32-MAX17043-OLED-TS.ino)  

----

## proyecto 🚀

### Conexión, configuración y obtención en el valor del voltaje real.  .
Se construyó el siguiente circuito:  
[![Esquematico-version-1-0.jpg](https://i.postimg.cc/0jD3Lq89/Esquematico-version-1-0.jpg)](https://postimg.cc/9wXJ7KRk)  
**MAX17043:**  
GND → GND (ESP32)  
VCC → 3.3V (ESP32)  
SCL → GPIO22 (ESP32)  
SDA → GPIO21 (ESP32)  
S+ → Positivo Batería  
S- →  Negativo en batería  

**Pantalla OLED**  
GND → GND (ESP32)  
VCC → 3.3V  
SCL → GPIO22 (ESP32)  
SDA →GPIO21 (ESP32)  

Cargamos el siguiente código en el Arduino IDE:
```bash  
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define MAX17043_ADDRESS 0x36  // Dirección I2C del MAX17043
#define SCREEN_WIDTH 128      // Ancho de la pantalla OLED
#define SCREEN_HEIGHT 32      // Altura de la pantalla OLED
#define OLED_RESET    -1      // Reset por software
#define SCREEN_ADDRESS 0x3C   // Dirección I2C de la pantalla OLED  

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // Pines SDA y SCL para ESP32

  // Inicializar pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("No se encontró pantalla OLED.");
    while (true);
  }
  display.clearDisplay();
  display.display();

  // Realizar un Quick Start para inicializar el MAX17043
  quickStart();

  Serial.println("MAX17043 inicializado.");
  Serial.println("Voltaje (V)\tPorcentaje (%)"); // Encabezado para el plotter
}

void loop() {
  float voltage = readVoltage();
  float percentage = calculatePercentage(voltage);

  // Mostrar valores en el Monitor Serial
  Serial.print(voltage, 2);    // Voltaje con 2 decimales
  Serial.print("\t");          // Separador para el plotter
  Serial.println(percentage);  // Porcentaje con 1 decimal

  // Mostrar valores en la pantalla OLED
  displayData(voltage, percentage);

  delay(500); // Actualizar cada 500ms
}

void quickStart() {
  Wire.beginTransmission(MAX17043_ADDRESS);
  Wire.write(0x06); // Comando Quick Start
  Wire.endTransmission();
  delay(10); // Esperar para completar
}

float readVoltage() {
  Wire.beginTransmission(MAX17043_ADDRESS);
  Wire.write(0x02); // Registro de voltaje
  Wire.endTransmission(false);

  Wire.requestFrom(MAX17043_ADDRESS, 2);
  uint16_t rawVoltage = (Wire.read() << 8) | Wire.read();
  return (rawVoltage >> 4) * 0.00125; // Conversión a voltios
}

float calculatePercentage(float voltage) {
  if (voltage <= 3.0) {
    return 0.0; // Menos de 3.0V es 0%
  } else if (voltage >= 3.7) {
    return 100.0; // Más de 3.7V es 100%
  } else {
    // Interpolación lineal entre 3.0V (0%) y 3.7V (100%)
    return (voltage - 3.0) / (3.7 - 3.0) * 100.0;
  }
}

void displayData(float voltage, float percentage) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // Mostrar voltaje
  display.setCursor(0, 0);
  display.printf("Voltaje: %.2f V", voltage);

  // Mostrar porcentaje
  display.setCursor(0, 16);
  display.printf("Bateria: %.1f %%", percentage);

  display.display();
}  
```
Despues de cargar el pograma podemos ver en el monitor series las siguientes lecturas:  

[![Monitor-seres-Version-1-0.jpg](https://i.postimg.cc/Gtp0TfrR/Monitor-seres-Version-1-0.jpg)](https://postimg.cc/mzKp08tp)  
Del lado Izquierdo podemos ver el tiempo, en la parte de en medio podemos ver el voltaje y en la parte derecha podemos ver el porcentaje de la batería.


### Icono de batería en tiempo real.
En la pantalla oled podemos ver las siguientes lecturas: Voltaje, porcentaje y un icono de porcentaje en tiempo real del nivel de la batería.  

[![Oled-version-1-0.jpg](https://i.postimg.cc/rFCn2jLC/Oled-version-1-0.jpg)](https://postimg.cc/RNhQcchN)  

```bash  
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define MAX17043_ADDRESS 0x36  // Dirección I2C del MAX17043
#define SCREEN_WIDTH 128      // Ancho de la pantalla OLED
#define SCREEN_HEIGHT 32      // Altura de la pantalla OLED
#define OLED_RESET    -1      // Reset por software
#define SCREEN_ADDRESS 0x3C   // Dirección I2C de la pantalla OLED  

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // Pines SDA y SCL para ESP32

  // Inicializar pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("No se encontró pantalla OLED.");
    while (true);
  }
  display.clearDisplay();
  display.display();

  // Realizar un Quick Start para inicializar el MAX17043
  quickStart();

  Serial.println("MAX17043 inicializado.");
  Serial.println("Voltaje (V)\tPorcentaje (%)"); // Encabezado para el plotter
}

void loop() {
  float voltage = readVoltage();
  float percentage = calculatePercentage(voltage);

  // Mostrar valores en el Monitor Serial
  Serial.print(voltage, 2);    // Voltaje con 2 decimales
  Serial.print("\t");          // Separador para el plotter
  Serial.println(percentage);  // Porcentaje con 1 decimal

  // Mostrar valores en la pantalla OLED
  displayData(voltage, percentage);

  delay(500); // Actualizar cada 500ms
}

void quickStart() {
  Wire.beginTransmission(MAX17043_ADDRESS);
  Wire.write(0x06); // Comando Quick Start
  Wire.endTransmission();
  delay(10); // Esperar para completar
}

float readVoltage() {
  Wire.beginTransmission(MAX17043_ADDRESS);
  Wire.write(0x02); // Registro de voltaje
  Wire.endTransmission(false);

  Wire.requestFrom(MAX17043_ADDRESS, 2);
  uint16_t rawVoltage = (Wire.read() << 8) | Wire.read();
  return (rawVoltage >> 4) * 0.00125; // Conversión a voltios
}

float calculatePercentage(float voltage) {
  if (voltage <= 3.0) {
    return 0.0; // Menos de 3.0V es 0%
  } else if (voltage >= 3.7) {
    return 100.0; // Más de 3.7V es 100%
  } else {
    // Interpolación lineal entre 3.0V (0%) y 3.7V (100%)
    return (voltage - 3.0) / (3.7 - 3.0) * 100.0;
  }
}

void displayData(float voltage, float percentage) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // Mostrar voltaje
  display.setCursor(0, 0);
  display.printf("Voltaje: %.2f V", voltage);

  // Mostrar porcentaje
  display.setCursor(0, 16);
  display.printf("Bateria: %.1f %%", percentage);

  display.display();
}

```

### Conexión a Thingspeak en tiempo real.
[Falta por documentar]

----
⌨️ con ❤️ por [Addi Trejo](https://github.com/additrejo)
