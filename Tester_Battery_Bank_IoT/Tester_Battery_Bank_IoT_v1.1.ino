/* Actualización: versión final del probador de baterías 18650.
Cuenta con medición de Voltaje, corriente, temperatura en tiempo real, contador de ciclos
y representación visual en dashboard Thingspeak. Permite comandos de carga/descarga desde Serial.

Desarrollado por Addi Trejo para el laboratorio de instrumentación espacial Linx
Junio 13 2025
*/

// Inclusión de bibliotecas necesarias
#include <Wire.h>                  // Comunicación I2C
#include <LiquidCrystal_PCF8574.h> // Control del LCD vía I2C
#include <WiFi.h>                  // Librería para conexión WiFi
#include <HTTPClient.h>            // Librería para enviar datos HTTP
#include "max6675.h"               // Librería para sensor de temperatura MAX6675 (termopar tipo K)
#include <Adafruit_INA219.h>       // Librería para sensores de corriente INA219

// Configuración LCD
LiquidCrystal_PCF8574 lcd(0x27); // LCD I2C en la dirección 0x27

// Configuración WiFi
const char* ssid = "SterenC";
const char* password = "unodostrescuatro";

// Configuración ThingSpeak
const char* server = "http://api.thingspeak.com";
const char* apiKey = "SC8TJFLKBEKIPCX7";

// Dirección I2C del MAX17043 (medidor de voltaje de batería)
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
const unsigned long intervaloWiFi = 600000;   // 10 min
const unsigned long intervaloEnvio = 15000; // 15 s

// Contador de ciclos
int cicloContador = 0;

// Sensores INA219
Adafruit_INA219 ina219_carga(0x41);
Adafruit_INA219 ina219_descarga(0x40);

// Acumuladores Ah separados para carga y descarga
float ampereHoraCarga = 0.0;
float ampereHoraDescarga = 0.0;

// Variables para modo forzado
bool forzarCarga = false;
bool forzarDescarga = false;

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
    digitalWrite(releDescarga, LOW);

    // Inicializa sensores INA219
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
    leerComandosSerial();

    // Reconexión WiFi
    if (millis() - tiempoUltimaConexion > intervaloWiFi) {
        WiFi.disconnect();
        delay(1000);
        conectarWiFi();
        tiempoUltimaConexion = millis();
    }

    float voltage = readVoltage();
    bool bateriaConectada = (voltage < 4.7);
    int percentage = bateriaConectada ? calculatePercentage(voltage) : 0;
    float temperatura = bateriaConectada ? thermocouple.readCelsius() : 0.0;
    float corriente_carga = bateriaConectada ? ina219_carga.getCurrent_mA() / 1000.0 : 0.0;
    float corriente_descarga = bateriaConectada ? ina219_descarga.getCurrent_mA() / 1000.0 : 0.0;
    float potencia = voltage * (corriente_carga + corriente_descarga);

    // Cálculo de Ah separado para carga y descarga
    unsigned long tiempoActual = millis();
    float tiempoHoras = (tiempoActual - tiempoAnterior) / 3600000.0;
    if (enCarga) {
        ampereHoraCarga += corriente_carga * tiempoHoras;
    } else {
        ampereHoraDescarga += corriente_descarga * tiempoHoras;
    }
    tiempoAnterior = tiempoActual;

    String estadoBateria = bateriaConectada ? (enCarga ? "Cargando" : "Descarga") : "No Battery";
    static bool previoEnCarga = false;

    if (bateriaConectada) {
        if (forzarCarga) {
            digitalWrite(releCarga, HIGH);
            digitalWrite(releDescarga, LOW);
            enCarga = true;
            estadoBateria = "Cargando"; 
        } else if (forzarDescarga) {
            digitalWrite(releCarga, LOW);
            digitalWrite(releDescarga, HIGH);
            enCarga = false;
            estadoBateria = "Descarga"; 
        } else {
            // Lógica automática
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
                    estadoBateria = "Cargando";
                }
            } else { // No está en carga (en descarga)
                if (voltage <= 3.0) {
                    digitalWrite(releDescarga, LOW);
                    digitalWrite(releCarga, HIGH);
                    enCarga = true;
                    voltajeMaximo = voltage;
                    estadoBateria = "Cargando";

                    // Reinicio de contadores al iniciar un nuevo ciclo de carga
                    if (!previoEnCarga) {
                        cicloContador++;
                        ampereHoraCarga = 0.0;
                        ampereHoraDescarga = 0.0;
                    }
                } else {
                    digitalWrite(releCarga, LOW);
                    digitalWrite(releDescarga, HIGH);
                    estadoBateria = "Descarga";
                }
            }
        }
        previoEnCarga = enCarga;
    } else {
        digitalWrite(releCarga, LOW);
        digitalWrite(releDescarga, LOW);
        estadoBateria = "No Battery";
    }

    // Mostrar en LCD
    mostrarDatosLCD(voltage, percentage, temperatura, corriente_carga, corriente_descarga, potencia, ampereHoraCarga, ampereHoraDescarga, estadoBateria, cicloContador);

    // Enviar a ThingSpeak
    if (millis() - tiempoUltimoEnvio >= intervaloEnvio) {
        sendToThingSpeak(voltage, percentage, temperatura, corriente_carga, corriente_descarga, potencia, ampereHoraCarga, ampereHoraDescarga, cicloContador);
        tiempoUltimoEnvio = millis();
    }

    delay(1000);
}

// Comandos Serial
void leerComandosSerial() {
    if (Serial.available()) {
        String comando = Serial.readStringUntil('\n');
        comando.trim();

        if (comando.equalsIgnoreCase("carga")) {
            forzarCarga = true;
            forzarDescarga = false;
            Serial.println("Comando recibido: FORZAR CARGA");
        } else if (comando.equalsIgnoreCase("descarga")) {
            forzarCarga = false;
            forzarDescarga = true;
            Serial.println("Comando recibido: FORZAR DESCARGA");
        } else if (comando.equalsIgnoreCase("auto")) {
            forzarCarga = false;
            forzarDescarga = false;
            Serial.println("Comando recibido: MODO AUTOMÁTICO");
        }
    }
}

// WiFi
void conectarWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi conectado");
}

// Lectura voltaje MAX17043
float readVoltage() {
    Wire.beginTransmission(MAX17043_ADDRESS);
    Wire.write(0x02);
    Wire.endTransmission(false);
    Wire.requestFrom(MAX17043_ADDRESS, 2);
    uint16_t rawVoltage = (Wire.read() << 8) | Wire.read();
    return (rawVoltage >> 4) * 0.00125;
}

// Porcentaje batería
int calculatePercentage(float voltage) {
    if (voltage <= 3.0) return 0;
    if (voltage >= 4.20) return 100;
    return (int)((voltage - 3.0) / (4.20 - 3.0) * 100.0);
}


// Mostrar LCD
void mostrarDatosLCD(float voltage, int percentage, float temperatura, float corriente_carga, float corriente_descarga, float potencia, float ahC, float ahD, String estado, int ciclos) {
    lcd.clear();

    unsigned long tiempoSegundos = millis() / 1000;
    int horas = tiempoSegundos / 3600;
    int minutos = (tiempoSegundos % 3600) / 60;

    char tiempoStr[6];
    sprintf(tiempoStr, "%02d:%02d", horas, minutos);

    if (voltage > 4.7) {
        lcd.setCursor(0, 0); lcd.print("V:0.0   T:00:00 B:0%");
        lcd.setCursor(0, 1); lcd.print("I:0.0   P:0.0");
        lcd.setCursor(0, 2); lcd.print("Ah:0.0  T:0.0");
        lcd.setCursor(0, 3); lcd.print("No Battery");
    } else {
        lcd.setCursor(0, 0);
        lcd.printf("V:%.2f T:%s B:%d%%", voltage, tiempoStr, percentage);
        lcd.setCursor(0, 1);
        lcd.printf("I:%.3f  P:%.2f", corriente_carga + corriente_descarga, potencia);
        
        lcd.setCursor(0, 2);
        if (estado == "Cargando") {
            lcd.printf("AhC:%.3f T:%.1f", ahC, temperatura);
        } else { 
            lcd.printf("AhD:%.3f T:%.1f", ahD, temperatura);
        }
        
        lcd.setCursor(0, 3);
        lcd.printf("Ciclos:%d %s", ciclos, estado.c_str());
    }
}


// ThingSpeak
void sendToThingSpeak(float voltage, int percentage, float temperatura, float ic, float id, float potencia, float ahC, float ahD, int ciclos) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(server) + "/update?api_key=" + apiKey;
        url += "&field1=" + String(voltage, 2);
        url += "&field2=" + String(percentage);
        url += "&field3=" + String(temperatura, 1);
        // MODIFICACIÓN: Se envía la capacidad de carga (AhC) al campo 4
        url += "&field4=" + String(ahC, 4); 
        url += "&field5=" + String(ahD, 4); // Se mantiene el envío de AhD al campo 5
        url += "&field6=" + String(potencia, 2);
        // MODIFICACIÓN: Se envía la corriente de carga instantánea (ic) al campo 7
        url += "&field7=" + String(ic, 3); 
        url += "&field8=" + String(ciclos);
        http.begin(url);
        int httpCode = http.GET();
        http.end();
    }
}