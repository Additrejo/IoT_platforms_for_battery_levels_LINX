# ESP32, MAX17043 & Thingspeak- Battery Level Gauge  

## Autores ✒️

- **Addi Trejo** - _Trabajo inicial_ - [Addi Trejo](https://github.com/additrejo)
  
#### Last update: Ene 13 2025  
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
13 Ene 2025: Exportar datos de Thingspeak a MATLAB.

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
Para obtener los valores de voltaje en tiempo real a través de internet, usaremos la plataforma Thingspeak.  
Es necesario crear una cuenta en IoT Analytics - https://thingspeak.mathworks.com/  

- Crear un nuevo canal.  
[![Thingspeak1.jpg](https://i.postimg.cc/wjx3rdHj/Thingspeak1.jpg)](https://postimg.cc/w7P6tZ7C)

- Agregar un nombre al canal
- Descripción (opcional)
- Field 1: Voltaje. (será el indicador dónde llegarán los datos del voltaje)
- Field 2: Porcentaje de batería (será el indicador dónde llegaran los datos del porcentaje de la batería.  

[![Thingspeak2.jpg](https://i.postimg.cc/prVgHkdz/Thingspeak2.jpg)](https://postimg.cc/vgjP7rfZ)  

Guarda canal.  
[![Thingspeak3.jpg](https://i.postimg.cc/fTGB7BhC/Thingspeak3.jpg)](https://postimg.cc/3dZXYjN0)  

### Código de conexión ESP32 a Thingspeak  

Cargamos el siguiente código en Arduino IDE.  

```bash
#include <Wire.h>  // Librería para la comunicación I2C
#include <Adafruit_SSD1306.h>  // Librería para controlar la pantalla OLED SSD1306
#include <WiFi.h>  // Librería para conectarse a redes Wi-Fi en ESP32
#include <HTTPClient.h>  // Librería para realizar peticiones HTTP
#include <esp_system.h>  // Biblioteca para reiniciar el ESP32

#define MAX17043_ADDRESS 0x36  // Dirección I2C del MAX17043
#define SCREEN_WIDTH 128      // Ancho de la pantalla OLED
#define SCREEN_HEIGHT 32      // Altura de la pantalla OLED
#define OLED_RESET    -1      // Reset por software
#define SCREEN_ADDRESS 0x3C   // Dirección I2C de la pantalla OLED

// Credenciales WiFi
const char* ssid = "INFINITUM44DC";  // Nombre de la red WiFi
const char* password = "Ca9Ni7Cc3a";  // Contraseña de la red WiFi

// Configuración de ThingSpeak
const char* server = "http://api.thingspeak.com";  // Dirección del servidor de ThingSpeak
const char* apiKey = "SC8TJFLKBEKIPCX7";  // Clave API para ThingSpeak

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  // Objeto para manejar la pantalla OLED

unsigned long startTime;  // Tiempo inicial en milisegundos
unsigned long lastRestartTime;  // Tiempo del último reinicio

void setup() {
  Serial.begin(115200);  // Inicializar comunicación serial a 115200 baudios
  Wire.begin(21, 22); // Pines SDA y SCL para ESP32

  // Inicializar pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {  // Comprobar si la pantalla está conectada
    Serial.println("No se encontró pantalla OLED.");
    while (true);  // Detener el programa si no se encuentra la pantalla
  }
  display.clearDisplay();  // Limpiar la pantalla
  display.display();  // Actualizar la pantalla

  // Conexión a WiFi
  WiFi.begin(ssid, password);  // Iniciar la conexión a WiFi
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {  // Esperar a que se establezca la conexión WiFi
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");

  // Realizar un Quick Start para inicializar el MAX17043
  quickStart();  // Enviar comando de inicio rápido al MAX17043

  Serial.println("MAX17043 inicializado.");
  Serial.println("Voltaje (V)\tPorcentaje (%)");  // Encabezado para el plotter

  // Registrar el tiempo de inicio
  startTime = millis();  // Guardar el tiempo en milisegundos desde que se inició el programa
  lastRestartTime = startTime;  // Inicializar tiempo de reinicio
}

void loop() {
  float voltage = readVoltage();  // Leer el voltaje de la batería
  float percentage = calculatePercentage(voltage);  // Calcular el porcentaje de carga de la batería

  // Mostrar valores en el Monitor Serial
  Serial.print(voltage, 2);  // Mostrar voltaje con 2 decimales
  Serial.print("\t");  // Separador para el plotter
  Serial.println(percentage);  // Mostrar porcentaje con 1 decimal

  // Mostrar valores en la pantalla OLED con el reloj
  displayData(voltage, percentage);  // Mostrar el voltaje, porcentaje y tiempo en la pantalla OLED

  // Enviar datos a ThingSpeak
  sendToThingSpeak(voltage, percentage);  // Enviar los datos de voltaje y porcentaje a ThingSpeak

  // Comprobar si han pasado 5 minutos para reiniciar
  unsigned long currentTime = millis();  // Obtener el tiempo actual en milisegundos
  if (currentTime - lastRestartTime >= 300000) {  // Si han pasado 5 minutos (300,000 ms)
    Serial.println("Reiniciando el ESP32...");
    esp_restart();  // Reiniciar el ESP32
  }

  delay(20000);  // Esperar 20 segundos antes de enviar nuevos datos
}

void quickStart() {
  Wire.beginTransmission(MAX17043_ADDRESS);  // Iniciar comunicación con el MAX17043
  Wire.write(0x06);  // Comando Quick Start
  Wire.endTransmission();  // Finalizar la transmisión
  delay(10);  // Esperar 10 ms para completar
}

float readVoltage() {
  Wire.beginTransmission(MAX17043_ADDRESS);  // Iniciar comunicación con el MAX17043
  Wire.write(0x02);  // Registro de voltaje
  Wire.endTransmission(false);  // Finalizar la transmisión sin liberar el bus

  Wire.requestFrom(MAX17043_ADDRESS, 2);  // Solicitar 2 bytes de datos
  uint16_t rawVoltage = (Wire.read() << 8) | Wire.read();  // Leer los 2 bytes y combinarlos en un solo valor
  return (rawVoltage >> 4) * 0.00125;  // Convertir el valor leído en voltios
}

float calculatePercentage(float voltage) {
  if (voltage <= 3.0) {
    return 0.0;  // Menos de 3.0V es 0%
  } else if (voltage >= 3.7) {
    return 100.0;  // Más de 3.7V es 100%
  } else {
    // Interpolación lineal entre 3.0V (0%) y 3.7V (100%)
    return (voltage - 3.0) / (3.7 - 3.0) * 100.0;
  }
}

void displayData(float voltage, float percentage) {
  // Calcular tiempo transcurrido
  unsigned long elapsedTime = millis() - startTime;  // Calcular el tiempo desde el inicio del programa
  int seconds = (elapsedTime / 1000) % 60;  // Segundos transcurridos
  int minutes = (elapsedTime / 60000) % 60;  // Minutos transcurridos

  display.clearDisplay();  // Limpiar la pantalla
  display.setTextColor(SSD1306_WHITE);  // Establecer color de texto blanco
  display.setTextSize(1);  // Establecer tamaño de texto

  // Mostrar voltaje
  display.setCursor(0, 0);  // Establecer la posición del cursor
  display.printf("Voltaje: %.2f V", voltage);  // Mostrar voltaje

  // Mostrar porcentaje
  display.setCursor(0, 10);  // Establecer la posición del cursor
  display.printf("Bateria: %.1f %%", percentage);  // Mostrar porcentaje

  // Mostrar tiempo transcurrido
  display.setCursor(0, 20);  // Establecer la posición del cursor
  display.printf("Tiempo: %02d:%02d", minutes, seconds);  // Mostrar tiempo en formato mm:ss

  display.display();  // Actualizar la pantalla
}

void sendToThingSpeak(float voltage, float percentage) {
  if (WiFi.status() == WL_CONNECTED) {  // Si está conectado a WiFi
    HTTPClient http;  // Crear un objeto HTTPClient

    String url = String(server) + "/update?api_key=" + apiKey +
                 "&field1=" + String(voltage) +
                 "&field2=" + String(percentage);  // Crear la URL para enviar datos a ThingSpeak

    http.begin(url);  // Iniciar la solicitud HTTP
    int httpResponseCode = http.GET();  // Realizar la petición GET

    if (httpResponseCode > 0) {  // Si la respuesta es positiva
      Serial.println("Datos enviados correctamente a ThingSpeak:");
      Serial.println("Voltaje: " + String(voltage) + " V");
      Serial.println("Porcentaje: " + String(percentage) + " %");
    } else {  // Si hay error al enviar los datos
      Serial.println("Error al enviar los datos.");
    }

    http.end();  // Finalizar la solicitud HTTP
  } else {  // Si no está conectado a WiFi
    Serial.println("WiFi desconectado");
  }
}

```
Al cargar el código el monitor series nos debe mostrar lo siguiente:  

[![Thingspeak4.jpg](https://i.postimg.cc/R05CB5xt/Thingspeak4.jpg)](https://postimg.cc/FYpXy6RF)  

Si todo está configurado correctamente, la ESP32 se conectará vía WIFI.
Aparecerá que los datos se han enviado correctamente a Thingspeak.

[![Thingspeak5.jpg](https://i.postimg.cc/tCxLH1qr/Thingspeak5.jpg)](https://postimg.cc/dLJW2VTd)  

### Visualización de datos con Matlab Analysis.  
Para visualizar los datos obtenidos en Thingspeak en Matlab es necesario tener instalado el paquete **ThingSpeak Support Toolbox.**
En un nuevo script agregar el siguiente código:  

```bash

% Configura los detalles del canal
channelID = 12345;                                        % Identificador único del canal en ThingSpeak donde se almacenan los datos.
readAPIKey = 'YOUR_READ_API_KEY';                            % Clave de lectura del canal (solo necesaria si el canal es privado).

% Leer el historial completo de datos del canal
[data, timestamps] = thingSpeakRead(channelID, ...          % Función para leer datos de ThingSpeak.
    'Fields', 1, ...                                        % Especifica el campo que se desea leer (en este caso, el campo 1).
    'DateRange', [datetime(2023,1,1), datetime('now')], ... % Define el rango de fechas desde el 1 de enero de 2023 hasta el momento actual.
    'ReadKey', readAPIKey);                                 % Incluye la clave de lectura necesaria para canales privados.

% Mostrar los datos en la consola
disp(table(timestamps, data)); % Convierte los datos y marcas de tiempo en una tabla y la muestra en la consola.

                                                       % Graficar los datos históricos
figure;                                                % Crea una nueva ventana para la figura.
plot(timestamps, data, '-o');                          % Grafica los datos frente a sus marcas de tiempo con líneas y marcadores circulares.
title('Historial Completo de Datos de ThingSpeak');    % Título del gráfico.
xlabel('Tiempo');                                      % Etiqueta del eje X.
ylabel('Valor');                                       % Etiqueta del eje Y.
grid on;                                               % Activa la cuadrícula en el gráfico para mejorar la visualización.

```

Ejecutamos el código y veremos gráficados nuestros datos de thingspeak.  
[![Matlab-graph-Volta.jpg](https://i.postimg.cc/k57R3136/Matlab-graph-Volta.jpg)](https://postimg.cc/mP6g7SrB)  

Si comparamos la grafica obtenida como las de la página, son exactamente las mismas.  
[![Matlab-graph-Volta-TS.jpg](https://i.postimg.cc/QMXQjxGq/Matlab-graph-Volta-TS.jpg)](https://postimg.cc/Js2Dq8hy)

----
⌨️ con ❤️ por [Addi Trejo](https://github.com/additrejo)
