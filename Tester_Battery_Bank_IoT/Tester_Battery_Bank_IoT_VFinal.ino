/* Actualzación: final de la primera versión del probador de baterías 18650.
Cuenta con la medición de Voltaje, corriente, temperatura en tiempo real, así como un contador de ciclos
y la representación visual en un dashboard mediante la plataforma Thingspeak, se pueden enviar comandos de
carga y descarga a tráves del monitor series"

Desarrollado por Addi Trejo para el laboratorio de instrumentación espacial Linx
Junio 13 2025
*/


// Inclusión de bibliotecas necesarias
#include <Wire.h>                          // Comunicación I2C
#include <LiquidCrystal_PCF8574.h>         // Control del LCD vía I2C
#include <WiFi.h>                          // Librería para conexión WiFi
#include <HTTPClient.h>                    // Librería para enviar datos HTTP
#include "max6675.h"                       // Librería para sensor de temperatura MAX6675 (termopar tipo K)
#include <Adafruit_INA219.h>               // Librería para sensores de corriente INA219

// Configuración LCD
LiquidCrystal_PCF8574 lcd(0x27);           // LCD I2C en la dirección 0x27

// Configuración WiFi
const char* ssid = "ICN";                  // Nombre de la red WiFi
const char* password = "99999999999999999999999999"; // Contraseña de la red WiFi

// Configuración ThingSpeak
const char* server = "http://api.thingspeak.com";    // Dirección del servidor de ThingSpeak
const char* apiKey = "SC8TJFLKBEKIPCX7";             // API Key para enviar datos

// Dirección I2C del MAX17043 (medidor de voltaje de batería)
#define MAX17043_ADDRESS 0x36

// Pines termopar MAX6675
#define SCK 18                           // Pin de reloj
#define CS 5                             // Pin chip select
#define SO 19                            // Pin de datos seriales
MAX6675 thermocouple(SCK, CS, SO);       // Inicializa el termopar con los pines definidos

// Pines relé
const int releCarga = 25;                // Pin para controlar el relé de carga
const int releDescarga = 26;             // Pin para controlar el relé de descarga

// Estado del sistema
bool enCarga = false;                    // Indica si el sistema está cargando
float voltajeMaximo = 0.0;               // Almacena el voltaje máximo registrado

// Tiempos
unsigned long tiempoUltimaConexion = 0;  // Control del tiempo desde la última reconexión WiFi
unsigned long tiempoUltimoEnvio = 0;     // Tiempo desde el último envío de datos
unsigned long tiempoAnterior = 0;        // Tiempo para cálculo de Ah

// Intervalos
const unsigned long intervaloWiFi = 600000;   // Intervalo para reconectar WiFi (10 minutos)
const unsigned long intervaloEnvio = 15000;   // Intervalo de envío a ThingSpeak (15 segundos)

// Contador de ciclos
int cicloContador = 0;                   // Número de ciclos carga/descarga completados

// Sensores INA219
Adafruit_INA219 ina219_carga(0x41);      // Sensor de corriente en el bus de carga
Adafruit_INA219 ina219_descarga(0x40);   // Sensor de corriente en el bus de descarga

// Acumulador Ah
float ampereHora = 0.0;                  // Acumulador de corriente consumida o entregada

// Variables para modo forzado
bool forzarCarga = false;                // Indicador para forzar carga desde Serial
bool forzarDescarga = false;             // Indicador para forzar descarga desde Serial

void setup() {
    Serial.begin(115200);                // Inicializa comunicación serial a 115200 baudios
    Wire.begin(21, 22);                  // Inicializa el bus I2C en pines 21 (SDA) y 22 (SCL)

    lcd.begin(20, 4);                    // Inicializa LCD 20x4
    lcd.setBacklight(1);                 // Enciende luz de fondo
    lcd.clear();                         // Limpia pantalla
    lcd.setCursor(0, 0);
    lcd.print("Inicializando...");       // Mensaje de inicio

    conectarWiFi();                      // Conexión WiFi

    pinMode(releCarga, OUTPUT);          // Configura pin de relé de carga como salida
    pinMode(releDescarga, OUTPUT);       // Configura pin de relé de descarga como salida
    digitalWrite(releCarga, LOW);        // Relé de carga apagado al inicio
    digitalWrite(releDescarga, LOW);     // Relé de descarga apagado al inicio

    // Inicializa sensores INA219
    if (!ina219_carga.begin()) {
        lcd.setCursor(0, 1);
        lcd.print("INA219 CARGA ERROR");
        while (1);                       // Detiene el programa si falla
    }
    if (!ina219_descarga.begin()) {
        lcd.setCursor(0, 1);
        lcd.print("INA219 DESCARGA ERROR");
        while (1);                       // Detiene el programa si falla
    }

    lcd.clear();                         // Limpia pantalla tras inicialización
    tiempoAnterior = millis();          // Guarda tiempo actual
}

void loop() {
    leerComandosSerial();  // Revisa si hay comandos por el monitor serial

    // Reintenta conexión WiFi si ha pasado el intervalo
    if (millis() - tiempoUltimaConexion > intervaloWiFi) {
        WiFi.disconnect();
        delay(1000);
        conectarWiFi();
        tiempoUltimaConexion = millis();
    }

    float voltage = readVoltage();   // Lectura de voltaje de batería
    bool bateriaConectada = (voltage < 4.7); // Si el voltaje es mayor, asume batería desconectada
    float percentage = bateriaConectada ? calculatePercentage(voltage) : 0.0;
    float temperatura = bateriaConectada ? thermocouple.readCelsius() : 0.0;
    float corriente_carga = bateriaConectada ? ina219_carga.getCurrent_mA() / 1000.0 : 0.0;
    float corriente_descarga = bateriaConectada ? ina219_descarga.getCurrent_mA() / 1000.0 : 0.0;
    float potencia = voltage * (corriente_carga + corriente_descarga);

    // Cálculo de Ah acumulados
    unsigned long tiempoActual = millis();
    float tiempoHoras = (tiempoActual - tiempoAnterior) / 3600000.0;
    ampereHora += (corriente_carga + corriente_descarga) * tiempoHoras;
    tiempoAnterior = tiempoActual;

    String estadoBateria = bateriaConectada ? (enCarga ? "Cargando" : "Descarga") : "No Battery";
    static bool previoEnCarga = false;

    if (bateriaConectada) {
        if (forzarCarga) {
            digitalWrite(releCarga, HIGH);
            digitalWrite(releDescarga, LOW);
            enCarga = true;
            estadoBateria = "Carga";
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
            } else {
                if (voltage <= 3.0) {
                    digitalWrite(releDescarga, LOW);
                    digitalWrite(releCarga, HIGH);
                    enCarga = true;
                    voltajeMaximo = voltage;
                    estadoBateria = "Cargando";

                    if (!previoEnCarga) {
                        cicloContador++;   // Cuenta un ciclo nuevo
                        ampereHora = 0.0;  // Reinicia acumulador de Ah
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

    // Mostrar datos en LCD
    mostrarDatosLCD(voltage, percentage, temperatura, corriente_carga, corriente_descarga, potencia, ampereHora, estadoBateria, cicloContador);

    // Enviar datos a ThingSpeak cada cierto tiempo
    if (millis() - tiempoUltimoEnvio >= intervaloEnvio) {
        sendToThingSpeak(voltage, percentage, temperatura, corriente_carga, corriente_descarga, potencia, ampereHora, cicloContador);
        tiempoUltimoEnvio = millis();
    }

    delay(1000); // Espera de 1 segundo
}

// Lee comandos del usuario por el monitor serial
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

// Conexión a red WiFi
void conectarWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi conectado");
}

// Lectura de voltaje desde el MAX17043
float readVoltage() {
    Wire.beginTransmission(MAX17043_ADDRESS);
    Wire.write(0x02); // Dirección del registro de voltaje
    Wire.endTransmission(false);
    Wire.requestFrom(MAX17043_ADDRESS, 2);
    uint16_t rawVoltage = (Wire.read() << 8) | Wire.read();
    float voltage = (rawVoltage >> 4) * 0.00125; // Conversión a voltaje real
    return voltage;
}

// Cálculo de porcentaje de batería basado en voltaje
float calculatePercentage(float voltage) {
    if (voltage <= 3.0) return 0.0;
    if (voltage >= 4.7) return 100.0;
    return (voltage - 3.0) / (4.7 - 3.0) * 100.0;
}

// Mostrar datos en pantalla LCD
void mostrarDatosLCD(float voltage, float percentage, float temperatura, float corriente_carga, float corriente_descarga, float potencia, float ampereHora, String estado, int ciclos) {
    lcd.clear();

    if (voltage > 4.7) {
        lcd.setCursor(0, 0); lcd.print("V:0.0     B:0.0%");
        lcd.setCursor(0, 1); lcd.print("I:0.0     P:0.0");
        lcd.setCursor(0, 2); lcd.print("Ah:0.0    T:0.0");
        lcd.setCursor(0, 3); lcd.print("No Battery");
    } else {
        lcd.setCursor(0, 0);
        lcd.printf("V:%.2f B:%.1f%%", voltage, percentage);
        lcd.setCursor(0, 1);
        lcd.printf("I:%.4f  P:%.2f", corriente_carga + corriente_descarga, potencia);
        lcd.setCursor(0, 2);
        lcd.printf("Ah:%.4f T:%.1f", ampereHora, temperatura);
        lcd.setCursor(0, 3);
        lcd.printf("Ciclos:%d %s", ciclos, estado.c_str());
    }
}

// Envía datos a ThingSpeak vía HTTP GET
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
        int httpCode = http.GET(); // Realiza petición GET
        http.end();                // Termina la conexión
    }
}
