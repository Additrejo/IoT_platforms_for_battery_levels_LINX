/* Programa principal*/

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_system.h>

#define MAX17043_ADDRESS 0x36
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

const char* ssid = "SterenC";
const char* password = "unodostrescuatro";
const char* server = "http://api.thingspeak.com";
const char* apiKey = "SC8TJFLKBEKIPCX7";

const int releCarga = 25;
const int releDescarga = 26;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
unsigned long lastRestartTime;

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
    lastRestartTime = millis();
}

void loop() {
    float voltage = readVoltage();
    float percentage = calculatePercentage(voltage);
    
    Serial.printf("Voltaje: %.2f V\tPorcentaje: %.1f%%\n", voltage, percentage);
    displayData(voltage, percentage);
    sendToThingSpeak(voltage, percentage);
    
    if (voltage <= 3.07) {
        digitalWrite(releCarga, HIGH);
        digitalWrite(releDescarga, LOW);
        Serial.println("Activando carga...");
    } else if (voltage >= 4.14) {
        digitalWrite(releCarga, LOW);
        digitalWrite(releDescarga, HIGH);
        Serial.println("Activando descarga...");
    }
    
    if (millis() - lastRestartTime >= 300000) {
        Serial.println("Reiniciando el ESP32...");
        esp_restart();
    }
    
    delay(20000);
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
    if (voltage >= 3.7) return 100.0;
    return (voltage - 3.0) / (3.7 - 3.0) * 100.0;
}

void displayData(float voltage, float percentage) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.printf("Voltaje: %.2f V", voltage);
    display.setCursor(0, 10);
    display.printf("Bateria: %.1f %%", percentage);
    display.display();
}

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
