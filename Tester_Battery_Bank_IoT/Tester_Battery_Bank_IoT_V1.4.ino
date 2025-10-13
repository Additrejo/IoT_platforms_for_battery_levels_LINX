#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "max6675.h"
#include <Adafruit_INA219.h> // Librería para el INA219

// Configuración de la pantalla LCD 20x4
LiquidCrystal_PCF8574 lcd(0x27); // Dirección I2C del LCD

// Configuración WiFi y ThingSpeak
const char* ssid = "SterenC"; 
const char* password = "unodostrescuatro"; 
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
float voltajeMaximo = 0.0;  // Se usará para detectar la caída de 0.02V
unsigned long tiempoUltimaConexion = 0;
unsigned long tiempoUltimaLectura = 0;
unsigned long tiempoUltimoEnvio = 0;
const unsigned long intervaloWiFi = 600000;  // 10 minutos
const unsigned long intervaloEnvio = 15000;  // 15 segundos
int cicloContador = 0;

// Sensor INA219
Adafruit_INA219 ina219;

// Variables para el cálculo de Amperios-hora
float ampereHora = 0.0;
unsigned long tiempoAnterior = 0;

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
    
    // Inicializar INA219
    if (!ina219.begin()) {
        Serial.println("Error al detectar INA219");
        lcd.setCursor(0, 1);
        lcd.print("INA219 ERROR");
        while (1);
    }

    lcd.clear();
    tiempoAnterior = millis();
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
    float corriente = ina219.getCurrent_mA() / 1000.0; // Conversión a Amperios
    float potencia = voltage * corriente;

    // Cálculo de Amperios-hora
    unsigned long tiempoActual = millis();
    float tiempoHoras = (tiempoActual - tiempoAnterior) / 3600000.0;  // Convertir ms a horas
    ampereHora += corriente * tiempoHoras;
    tiempoAnterior = tiempoActual;

    String estadoBateria = "";

    static bool previoEnCarga = false;

    if (enCarga) {
        if (voltage > voltajeMaximo) {
            voltajeMaximo = voltage;  // Actualiza el voltaje máximo durante la carga
        }

        if (voltage <= (voltajeMaximo - 0.02)) {  // Si cae 0.02V respecto al máximo, inicia descarga
            digitalWrite(releCarga, LOW);
            digitalWrite(releDescarga, HIGH);
            enCarga = false;
            estadoBateria = "Descarga";
            Serial.println("Cambio a descarga por caída de voltaje.");
        } else {
            digitalWrite(releCarga, HIGH);
            digitalWrite(releDescarga, LOW);
            estadoBateria = "Cargando";
        }
    } else {
        if (voltage <= 3.0) {  // Si la batería está descargada, inicia carga
            digitalWrite(releDescarga, LOW);
            digitalWrite(releCarga, HIGH);
            enCarga = true;
            voltajeMaximo = voltage;  // Reinicia el voltaje máximo al iniciar la carga
            estadoBateria = "Cargando";
            if (!previoEnCarga) {
                cicloContador++;
                Serial.printf("Nuevo ciclo registrado: %d\n", cicloContador);
                ampereHora = 0.0; // Reiniciar Ah cuando empieza nueva carga
            }
        } else {
            digitalWrite(releCarga, LOW);
            digitalWrite(releDescarga, HIGH);
            estadoBateria = "Descarga";
        }
    }
    previoEnCarga = enCarga;

    Serial.printf("V: %.2f V | B: %.1f%% | I: %.3f A | P: %.3f W | Ah: %.4f | T: %.2f°C | Estado: %s | Ciclos: %d\n", 
                  voltage, percentage, corriente, potencia, ampereHora, temperatura, estadoBateria.c_str(), cicloContador);
    
    mostrarDatosLCD(voltage, percentage, temperatura, corriente, potencia, ampereHora, estadoBateria, cicloContador);

    if (millis() - tiempoUltimoEnvio >= intervaloEnvio) {
        sendToThingSpeak(voltage, percentage, temperatura, corriente, potencia, ampereHora, cicloContador);
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

void mostrarDatosLCD(float voltage, float percentage, float temperatura, float corriente, float potencia, float ampereHora, String estado, int ciclos) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printf("V: %.2fV  B: %.1f%%", voltage, percentage);
    lcd.setCursor(0, 1);
    lcd.printf("I: %.3fA  P: %.3fW", corriente, potencia);
    lcd.setCursor(0, 2);
    lcd.printf("Ah: %.4f  T: %.1fC", ampereHora, temperatura);
    lcd.setCursor(0, 3);
    lcd.printf("Ciclos: %d  %s", ciclos, estado.c_str());
}

void sendToThingSpeak(float voltage, float percentage, float temperatura, float corriente, float potencia, float ampereHora, int ciclos) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(server) + "/update?api_key=" + apiKey +
                     "&field1=" + String(voltage) +
                     "&field2=" + String(percentage) +
                     "&field3=" + String(temperatura) +
                     "&field4=" + String(ciclos) +
                     "&field5=" + String(corriente) +
                     "&field6=" + String(potencia) +
                     "&field7=" + String(ampereHora);
        http.begin(url);
        http.GET();
        http.end();
    } else {
        Serial.println("WiFi desconectado. Reintentando...");
        conectarWiFi();
    }
}

