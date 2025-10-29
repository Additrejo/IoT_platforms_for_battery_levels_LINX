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