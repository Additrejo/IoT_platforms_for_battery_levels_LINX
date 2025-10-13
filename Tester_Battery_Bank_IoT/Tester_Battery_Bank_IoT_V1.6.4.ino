//V1.6.4 Se ajustó el factor de calbración, y se agregó la orden de carga y descarga a través de comando en monitor series.

#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "max6675.h"
#include <Adafruit_INA219.h>

// Configuración LCD
LiquidCrystal_PCF8574 lcd(0x27);

// Configuración WiFi
const char* ssid = "SterenC";
const char* password = "unodostrescuatro";

// Configuración ThingSpeak
const char* server = "http://api.thingspeak.com";
const char* apiKey = "SC8TJFLKBEKIPCX7";

// Dirección I2C del MAX17043
#define MAX17043_ADDRESS 0x36

// Pines termopar MAX6675
#define SCK 18
#define CS 5
#define SO 19
MAX6675 thermocouple(SCK, CS, SO);

// Pines relé
const int releCarga = 25;
const int releDescarga = 26;

// Estado del sistema
bool enCarga = false;
float voltajeMaximo = 0.0;

// Tiempos
unsigned long tiempoUltimaConexion = 0;
unsigned long tiempoUltimoEnvio = 0;
unsigned long tiempoAnterior = 0;

// Intervalos
const unsigned long intervaloWiFi = 600000;
const unsigned long intervaloEnvio = 15000;

// Contador de ciclos
int cicloContador = 0;

// Sensores INA219
Adafruit_INA219 ina219_carga(0x41);
Adafruit_INA219 ina219_descarga(0x40);

// Acumulador Ah
float ampereHora = 0.0;

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22);

    lcd.begin(20, 4);
    lcd.setBacklight(1);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Inicializando...");

    conectarWiFi();

    pinMode(releCarga, OUTPUT);
    pinMode(releDescarga, OUTPUT);
    digitalWrite(releCarga, LOW);
    digitalWrite(releDescarga, HIGH);

    if (!ina219_carga.begin()) {
        lcd.setCursor(0, 1);
        lcd.print("INA219 CARGA ERROR");
        while (1);
    }
    if (!ina219_descarga.begin()) {
        lcd.setCursor(0, 1);
        lcd.print("INA219 DESCARGA ERROR");
        while (1);
    }

    lcd.clear();
    tiempoAnterior = millis();
}

void loop() {
    // -------------------- LECTURA DE COMANDOS DESDE MONITOR SERIE --------------------
    if (Serial.available()) {
        String comando = Serial.readStringUntil('\n');
        comando.trim();
        if (comando.equalsIgnoreCase("carga")) {
            digitalWrite(releCarga, HIGH);
            digitalWrite(releDescarga, LOW);
            enCarga = true;
            voltajeMaximo = readVoltage(); // reinicia el punto de referencia
            Serial.println(">> CARGA activada por comando");
        } else if (comando.equalsIgnoreCase("descarga")) {
            digitalWrite(releCarga, LOW);
            digitalWrite(releDescarga, HIGH);
            enCarga = false;
            Serial.println(">> DESCARGA activada por comando");
        } else {
            Serial.println(">> Comando no reconocido. Use 'carga' o 'descarga'");
        }
    }

    // -------------------- WIFI --------------------
    if (millis() - tiempoUltimaConexion > intervaloWiFi) {
        WiFi.disconnect();
        delay(1000);
        conectarWiFi();
        tiempoUltimaConexion = millis();
    }

    // -------------------- MEDICIONES --------------------
    float voltage = readVoltage();
    bool bateriaConectada = (voltage < 4.7);
    float percentage = bateriaConectada ? calculatePercentage(voltage) : 0.0;
    float temperatura = bateriaConectada ? thermocouple.readCelsius() : 0.0;
    float corriente_carga = bateriaConectada ? ina219_carga.getCurrent_mA() / 1000.0 : 0.0;
    float corriente_descarga = bateriaConectada ? ina219_descarga.getCurrent_mA() / 1000.0 : 0.0;
    float potencia = voltage * (corriente_carga + corriente_descarga);

    // Acumulación de Ah
    unsigned long tiempoActual = millis();
    float tiempoHoras = (tiempoActual - tiempoAnterior) / 3600000.0;
    ampereHora += (corriente_carga + corriente_descarga) * tiempoHoras;
    tiempoAnterior = tiempoActual;

    // -------------------- CONTROL AUTOMÁTICO --------------------
    String estadoBateria = bateriaConectada ? (enCarga ? "Cargando" : "Descarga") : "No Battery";
    static bool previoEnCarga = false;

    if (bateriaConectada) {
        if (enCarga) {
            if (voltage > voltajeMaximo) voltajeMaximo = voltage;

            if (voltage <= (voltajeMaximo - 0.02)) {
                digitalWrite(releCarga, LOW);
                digitalWrite(releDescarga, HIGH);
                enCarga = false;
                estadoBateria = "Descarga";
            } else {
                digitalWrite(releCarga, HIGH);
                digitalWrite(releDescarga, LOW);
            }
        } else {
            if (voltage <= 3.0) {
                digitalWrite(releDescarga, LOW);
                digitalWrite(releCarga, HIGH);
                enCarga = true;
                voltajeMaximo = voltage;
                estadoBateria = "Cargando";

                if (!previoEnCarga) {
                    cicloContador++;
                    ampereHora = 0.0;
                }
            } else {
                digitalWrite(releCarga, LOW);
                digitalWrite(releDescarga, HIGH);
            }
        }
        previoEnCarga = enCarga;
    } else {
        digitalWrite(releCarga, LOW);
        digitalWrite(releDescarga, LOW);
    }

    // -------------------- MOSTRAR LCD --------------------
    mostrarDatosLCD(voltage, percentage, temperatura, corriente_carga, corriente_descarga, potencia, ampereHora, estadoBateria, cicloContador);

    // -------------------- ENVIAR A THINGSPEAK --------------------
    if (millis() - tiempoUltimoEnvio >= intervaloEnvio) {
        sendToThingSpeak(voltage, percentage, temperatura, corriente_carga, corriente_descarga, potencia, ampereHora, cicloContador);
        tiempoUltimoEnvio = millis();
    }

    delay(1000);
}

// -------------------- FUNCIONES AUXILIARES --------------------
void conectarWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi conectado");
}

float readVoltage() {
    Wire.beginTransmission(MAX17043_ADDRESS);
    Wire.write(0x02);
    Wire.endTransmission(false);
    Wire.requestFrom(MAX17043_ADDRESS, 2);
    uint16_t rawVoltage = (Wire.read() << 8) | Wire.read();
    float voltage = (rawVoltage >> 4) * 0.00125;
    voltage += 0.09;
    return voltage;
}

float calculatePercentage(float voltage) {
    if (voltage <= 3.0) return 0.0;
    if (voltage >= 3.7) return 100.0;
    return (voltage - 3.0) / (3.7 - 3.0) * 100.0;
}

void mostrarDatosLCD(float voltage, float percentage, float temperatura, float corriente_carga, float corriente_descarga, float potencia, float ampereHora, String estado, int ciclos) {
    lcd.clear();
    if (voltage > 4.7) {
        lcd.setCursor(0, 0);
        lcd.print("V:0.0     B:0.0%");
        lcd.setCursor(0, 1);
        lcd.print("I:0.0     P:0.0");
        lcd.setCursor(0, 2);
        lcd.print("Ah:0.0    T:0.0");
        lcd.setCursor(0, 3);
        lcd.print("No Battery");
    } else {
        lcd.setCursor(0, 0);
        lcd.printf("V:%.2f B:%.1f%%", voltage, percentage);
        lcd.setCursor(0, 1);
        lcd.printf("I:%.3f  P:%.2f", corriente_carga + corriente_descarga, potencia);
        lcd.setCursor(0, 2);
        lcd.printf("Ah:%.3f T:%.1f", ampereHora, temperatura);
        lcd.setCursor(0, 3);
        lcd.printf("Ciclos:%d %s", ciclos, estado.c_str());
    }
}

void sendToThingSpeak(float voltage, float percentage, float temperatura, float ic, float id, float potencia, float ah, int ciclos) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(server) + "/update?api_key=" + apiKey;
        url += "&field1=" + String(voltage, 2);
        url += "&field2=" + String(percentage, 1);
        url += "&field3=" + String(temperatura, 1);
        url += "&field4=" + String(ic, 3);
        url += "&field5=" + String(id, 3);
        url += "&field6=" + String(potencia, 2);
        url += "&field7=" + String(ah, 4);
        url += "&field8=" + String(ciclos);
        http.begin(url);
        int httpCode = http.GET();
        http.end();
    }
}

