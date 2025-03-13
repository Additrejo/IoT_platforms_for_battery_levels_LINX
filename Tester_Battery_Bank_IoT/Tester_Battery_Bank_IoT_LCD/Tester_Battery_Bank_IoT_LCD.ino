/*
Laboratorio de instrumentación espacial. LINX ICN UNAM.
Autor: Addi Trejo.
Programa: Medición de nivel de batería 18650 con conexión a Thhingspeak.
Ult. Actualización: 12 mar 2025
Descripcion: se agregó una pantalla LCDMmas grande para visualizar los datos en el dispositivo.
*/
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "max6675.h"

// Configuración de la pantalla LCD 20x4
LiquidCrystal_PCF8574 lcd(0x27); // Dirección I2C del LCD

// Configuración WiFi y ThingSpeak
//Credenciales Home
const char* ssid = "SterenC"; 
const char* password = "unodostrescuatro"; 

//Credenciales ICN
/*const char* ssid = "ICN"; 
const char* password = "99999999999999999999999999"; 
*/
const char* server = "http://api.thingspeak.com";
const char* apiKey = "SC8TJFLKBEKIPCX7";

// Pines del MAX17043 (medición de voltaje)
#define MAX17043_ADDRESS 0x36

// Pines del MAX6675 (termopar)
#define SCK 18
#define CS 5
#define SO 19
MAX6675 thermocouple(SCK, CS, SO);

// Pines de relés
const int releCarga = 25;
const int releDescarga = 26;

bool enCarga = false;
unsigned long tiempoUltimaConexion = 0;
unsigned long tiempoUltimaLectura = 0;
unsigned long tiempoUltimoEnvio = 0;
const unsigned long intervaloWiFi = 600000;  // 10 minutos
const unsigned long intervaloEnvio = 15000;  // 15 segundos
int cicloContador = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22);
    
    // Iniciar LCD
    lcd.begin(20, 4);
    lcd.setBacklight(1);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Inicializando...");
    
    conectarWiFi();

    pinMode(releCarga, OUTPUT);
    pinMode(releDescarga, OUTPUT);
    digitalWrite(releCarga, LOW);
    digitalWrite(releDescarga, LOW);

    quickStart();
    delay(500);
    lcd.clear();
}

void loop() {
    if (millis() - tiempoUltimaConexion > intervaloWiFi) {
        Serial.println("Reiniciando WiFi...");
        WiFi.disconnect();
        delay(1000);
        conectarWiFi();
        tiempoUltimaConexion = millis();
    }

    float voltage = readVoltage();
    float percentage = calculatePercentage(voltage);
    float temperatura = thermocouple.readCelsius();
    String estadoBateria = "";

    static bool previoEnCarga = false;
    if (enCarga) {
        if (voltage >= 4.0) {
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
            if (!previoEnCarga) {
                cicloContador++;
                Serial.printf("Nuevo ciclo registrado: %d\n", cicloContador);
            }
        } else {
            digitalWrite(releCarga, LOW);
            digitalWrite(releDescarga, HIGH);
            estadoBateria = "Descargando";
        }
    }
    previoEnCarga = enCarga;

    Serial.printf("Voltaje: %.2f V | Bateria: %.1f%% | Temp: %.2f°C | Estado: %s | Ciclos: %d\n", 
                  voltage, percentage, temperatura, estadoBateria.c_str(), cicloContador);
    mostrarDatosLCD(voltage, percentage, temperatura, estadoBateria, cicloContador);

    if (millis() - tiempoUltimoEnvio >= intervaloEnvio) {
        sendToThingSpeak(voltage, percentage, temperatura, cicloContador);
        tiempoUltimoEnvio = millis();
    }

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

void mostrarDatosLCD(float voltage, float percentage, float temperatura, String estado, int ciclos) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printf("V: %.2fV | B: %.1f%%", voltage, percentage);
    lcd.setCursor(0, 1);
    lcd.printf("Temp: %.2f C", temperatura);
    lcd.setCursor(0, 2);
    lcd.printf("Estado: %s", estado.c_str());
    lcd.setCursor(0, 3);
    lcd.printf("Ciclos: %d", ciclos);
}

void sendToThingSpeak(float voltage, float percentage, float temperatura, int ciclos) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(server) + "/update?api_key=" + apiKey +
                     "&field1=" + String(voltage) +
                     "&field2=" + String(percentage) +
                     "&field3=" + String(temperatura) +
                     "&field4=" + String(ciclos);
        http.begin(url);
        int httpResponseCode = http.GET();
        if (httpResponseCode > 0) {
            Serial.println("Datos enviados a ThingSpeak");
        } else {
            Serial.printf("Error al enviar datos. Código: %d\n", httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("WiFi desconectado. Reintentando...");
        conectarWiFi();
    }
}
