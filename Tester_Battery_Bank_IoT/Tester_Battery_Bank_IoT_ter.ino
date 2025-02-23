/* Monitoreo de batería y temperatura con MAX6675, MAX17043 y OLED */

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "max6675.h"

// 🖥️ Configuración de pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 📡 Configuración WiFi y ThingSpeak
const char* ssid = "SterenC";
const char* password = "unodostrescuatro";
const char* server = "http://api.thingspeak.com";
const char* apiKey = "SC8TJFLKBEKIPCX7";

// ⚡ Pines del MAX17043 (medición de voltaje)
#define MAX17043_ADDRESS 0x36

// 🌡️ Pines del MAX6675 (termopar)
#define SCK 18
#define CS 5
#define SO 19
MAX6675 thermocouple(SCK, CS, SO);

// 🔌 Pines de relés
const int releCarga = 25;
const int releDescarga = 26;

bool enCarga = false;
unsigned long tiempoUltimaConexion = 0;
unsigned long tiempoUltimaLectura = 0;
const unsigned long intervaloWiFi = 600000; // 10 minutos

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22);

    // 🖥️ Iniciar pantalla OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("No se encontró pantalla OLED.");
        while (true);
    }
    display.clearDisplay();
    display.display();

    conectarWiFi();

    // 🔌 Configurar pines de relés
    pinMode(releCarga, OUTPUT);
    pinMode(releDescarga, OUTPUT);
    digitalWrite(releCarga, LOW);
    digitalWrite(releDescarga, LOW);

    // ⚡ Inicializar MAX17043
    quickStart();

    // ⏳ Esperar estabilización del MAX6675
    delay(500);
}

void loop() {
    // Verificar si es tiempo de reiniciar WiFi
    if (millis() - tiempoUltimaConexion > intervaloWiFi) {
        Serial.println("Reiniciando WiFi...");
        WiFi.disconnect();
        delay(1000);
        conectarWiFi();
        tiempoUltimaConexion = millis();
    }

    // 🔋 Leer voltaje y temperatura
    float voltage = readVoltage();
    float percentage = calculatePercentage(voltage);
    float temperatura = thermocouple.readCelsius();
    String estadoBateria = "";

    // 🔄 Control de carga/descarga
    if (enCarga) {
        if (voltage >= 4.2) {
            digitalWrite(releCarga, LOW);
            digitalWrite(releDescarga, HIGH);
            enCarga = false;
            estadoBateria = "Descargando";
        } else {
            digitalWrite(releCarga, HIGH);
            digitalWrite(releDescarga, LOW);
            estadoBateria = "Cargando";
        }
    } else {
        if (voltage <= 3.0) {
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

    // 📟 Mostrar datos
    Serial.printf("Voltaje: %.2f V | Batería: %.1f%% | Temp: %.2f°C | Estado: %s\n", voltage, percentage, temperatura, estadoBateria.c_str());

    displayData(voltage, percentage, temperatura, estadoBateria);
    sendToThingSpeak(voltage, percentage, temperatura);

    delay(1000);
}

void conectarWiFi() {
    WiFi.begin(ssid, password);
    Serial.print("Conectando a WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi conectado");
}

void quickStart() {
    Wire.beginTransmission(MAX17043_ADDRESS);
    Wire.write(0x06);
    Wire.endTransmission();
    delay(10);
}

float readVoltage() {
    Wire.beginTransmission(MAX17043_ADDRESS);
    Wire.write(0x02);
    Wire.endTransmission(false);
    Wire.requestFrom(MAX17043_ADDRESS, 2);
    uint16_t rawVoltage = (Wire.read() << 8) | Wire.read();
    return (rawVoltage >> 4) * 0.00125;
}

float calculatePercentage(float voltage) {
    if (voltage <= 3.0) return 0.0;
    if (voltage >= 4.7) return 100.0;
    return (voltage - 3.0) / (4.7 - 3.0) * 100.0;
}

void displayData(float voltage, float percentage, float temperatura, String estado) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.printf("V: %.2f V | B: %.1f%%", voltage, percentage);
    display.setCursor(0, 10);
    display.printf("Temp: %.2f C", temperatura);
    display.setCursor(0, 20);
    display.printf("Estado: %s", estado.c_str());
    display.display();
}

void sendToThingSpeak(float voltage, float percentage, float temperatura) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(server) + "/update?api_key=" + apiKey +
                     "&field1=" + String(voltage) +
                     "&field2=" + String(percentage) +
                     "&field3=" + String(temperatura);
        http.begin(url);
        int httpResponseCode = http.GET();
        if (httpResponseCode > 0) {
            Serial.println("Datos enviados a ThingSpeak");
        } else {
            Serial.printf("Error al enviar datos. Código: %d\n", httpResponseCode);
        }
        http.end(); // ✅ Cierra la conexión
    } else {
        Serial.println("WiFi desconectado. Reintentando...");
        conectarWiFi();
    }
}

