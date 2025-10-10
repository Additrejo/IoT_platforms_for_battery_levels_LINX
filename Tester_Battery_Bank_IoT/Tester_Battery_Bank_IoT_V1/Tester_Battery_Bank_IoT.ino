/* Programa de carga y descarga controlada por voltaje */

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_system.h"  // Para esp_restart()

#define MAX17043_ADDRESS 0x36
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

const char* ssid = "SterenC";
const char* password = "unodostrescuatro";
const char* server = "http://api.thingspeak.com";
const char* apiKey = "SC8TJFLKBEKIPCX7";

const int releCarga = 25;     // Relevador para cargar la batería (TP4050)
const int releDescarga = 26;  // Relevador para descargar la batería (Resistencias)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool enCarga = false;  // Estado de carga o descarga
unsigned long tiempoInicio = 0;  
int ciclosCompletados = 0;  // Contador de ciclos

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22);

    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("No se encontró pantalla OLED.");
        while (true);
    }
    display.clearDisplay();
    display.display();

    WiFi.begin(ssid, password);
    Serial.print("Conectando a WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi conectado");

    pinMode(releCarga, OUTPUT);
    pinMode(releDescarga, OUTPUT);
    digitalWrite(releCarga, LOW);
    digitalWrite(releDescarga, LOW);

    quickStart();
    Serial.println("MAX17043 inicializado.");

    tiempoInicio = millis();  // Guardar el tiempo de inicio
}

void loop() {
    float voltage = readVoltage();
    float percentage = calculatePercentage(voltage);
    String estadoBateria = "";

    if (enCarga) {  
        if (voltage >= 4.15) {  // Cuando llegue al voltaje máximo, cambiar a descarga
            digitalWrite(releCarga, LOW);    
            digitalWrite(releDescarga, HIGH);  
            enCarga = false;
            ciclosCompletados++;
            estadoBateria = "Descargando";
        } else {
            digitalWrite(releCarga, HIGH);
            digitalWrite(releDescarga, LOW);
            estadoBateria = "Cargando";
        }
    } else {  
        if (voltage <= 3.50) {  // Cuando llegue al voltaje mínimo, cambiar a carga
            digitalWrite(releDescarga, LOW);
            digitalWrite(releCarga, HIGH);
            enCarga = true;
            estadoBateria = "Cargando";
        } else {
            digitalWrite(releCarga, LOW);
            digitalWrite(releDescarga, HIGH);
            estadoBateria = "Descargando";
        }
    }

    Serial.printf("Voltaje: %.2f V\tPorcentaje: %.1f%%\tEstado: %s\tCiclos: %d\n", 
                  voltage, percentage, estadoBateria.c_str(), ciclosCompletados);

    displayData(voltage, percentage, estadoBateria);
    sendToThingSpeak(voltage, percentage);

    if (millis() - tiempoInicio >= 600000) { 
        Serial.println("Reiniciando ESP32...");
        delay(1000);
        esp_restart();
    }

    delay(1000);
}

// Reinicio rápido del MAX17043
void quickStart() {
    Wire.beginTransmission(MAX17043_ADDRESS);
    Wire.write(0x06);
    Wire.endTransmission();
    delay(10);
}

// Lee el voltaje desde el sensor MAX17043
float readVoltage() {
    Wire.beginTransmission(MAX17043_ADDRESS);
    Wire.write(0x02);
    Wire.endTransmission(false);
    Wire.requestFrom(MAX17043_ADDRESS, 2);
    uint16_t rawVoltage = (Wire.read() << 8) | Wire.read();
    return (rawVoltage >> 4) * 0.00125;
}

// Calcula el porcentaje de batería basado en el voltaje
float calculatePercentage(float voltage) {
    if (voltage <= 3.0) return 0.0;
    if (voltage >= 4.7) return 100.0;
    return (voltage - 3.0) / (4.7 - 3.0) * 100.0;
}

// Muestra los datos en la pantalla OLED
void displayData(float voltage, float percentage, String estado) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.printf("Voltaje: %.2f V", voltage);
    display.setCursor(0, 10);
    display.printf("Bateria: %.1f %%", percentage);
    display.setCursor(0, 20);
    display.printf("Estado: %s", estado.c_str()); // Muestra "Cargando" o "Descargando"
    display.display();
}

// Envía los datos de voltaje y porcentaje a ThingSpeak
void sendToThingSpeak(float voltage, float percentage) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(server) + "/update?api_key=" + apiKey + "&field1=" + String(voltage) + "&field2=" + String(percentage);
        http.begin(url);
        int httpResponseCode = http.GET();
        if (httpResponseCode > 0) {
            Serial.println("Datos enviados correctamente a ThingSpeak");
        } else {
            Serial.println("Error al enviar los datos");
        }
        http.end();
    } else {
        Serial.println("WiFi desconectado");
    }
}
