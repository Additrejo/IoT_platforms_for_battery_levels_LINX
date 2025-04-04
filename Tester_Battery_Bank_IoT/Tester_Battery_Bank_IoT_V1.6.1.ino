/* 
 * Actualización versión 1.6: Se soldó la dirección 0x41 a un sensor INA219, teniendo ya lecturas de los dos sensores
 * Cuando el sensor está cargando da valores negativos, y cuando descarga parece ser que todo archa bien.
 * Aun está pendiente acomodar graficas en thingspeak
 */

// Inclusión de bibliotecas necesarias
#include <Wire.h>                  // Para comunicación I2C
#include <LiquidCrystal_PCF8574.h> // Para controlar LCD con interfaz PCF8574
#include <WiFi.h>                  // Para conectividad WiFi
#include <HTTPClient.h>            // Para hacer peticiones HTTP
#include "max6675.h"               // Para el termopar MAX6675
#include <Adafruit_INA219.h>       // Para los sensores INA219 (corriente/voltaje)

// Configuración de la pantalla LCD 20x4 con dirección I2C 0x27
LiquidCrystal_PCF8574 lcd(0x27); 

// Configuración WiFi - credenciales de red
const char* ssid = "SterenC";          // Nombre de la red WiFi
const char* password = "unodostrescuatro"; // Contraseña WiFi (¡OJO! Debería ser más segura)

// Configuración ThingSpeak - servidor y clave API
const char* server = "http://api.thingspeak.com"; // URL del servidor
const char* apiKey = "SC8TJFLKBEKIPCX7";         // Clave API para escritura (¡Cuidado con exponer esto!)

// Dirección I2C del sensor MAX17043 (medición de voltaje de batería)
#define MAX17043_ADDRESS 0x36

// Configuración de pines para el termopar MAX6675
#define SCK 18  // Pin de reloj
#define CS 5    // Pin de selección (chip select)
#define SO 19   // Pin de datos (MISO)
MAX6675 thermocouple(SCK, CS, SO); // Objeto termopar

// Pines para control de relés
const int releCarga = 25;    // Rele que controla la carga
const int releDescarga = 26; // Rele que controla la descarga

// Variables de estado del sistema
bool enCarga = false;          // Indica si está en modo carga
float voltajeMaximo = 0.0;     // Almacena el voltaje máximo alcanzado

// Variables para control de tiempos
unsigned long tiempoUltimaConexion = 0; // Último momento de conexión WiFi
unsigned long tiempoUltimaLectura = 0;  // Última lectura de sensores
unsigned long tiempoUltimoEnvio = 0;    // Último envío a ThingSpeak

// Intervalos de tiempo en milisegundos
const unsigned long intervaloWiFi = 600000;  // 10 minutos para reconexión WiFi
const unsigned long intervaloEnvio = 15000;  // 15 segundos para envío a ThingSpeak

int cicloContador = 0; // Contador de ciclos carga/descarga

// Sensores INA219 para medir corriente (direcciones I2C diferentes)
Adafruit_INA219 ina219_carga(0x41);   // Sensor para corriente de carga
Adafruit_INA219 ina219_descarga(0x40); // Sensor para corriente de descarga

// Variables para cálculo de capacidad (Amperios-hora)
float ampereHora = 0.0;          // Acumulador de Amperios-hora
unsigned long tiempoAnterior = 0; // Para cálculo del tiempo transcurrido

void setup() {
    // Inicialización del puerto serial para depuración
    Serial.begin(115200);
    
    // Inicialización del bus I2C con pines personalizados (SDA=21, SCL=22)
    Wire.begin(21, 22);
    
    // Configuración inicial de la pantalla LCD
    lcd.begin(20, 4);       // LCD de 20 columnas x 4 filas
    lcd.setBacklight(1);    // Encender retroiluminación
    lcd.clear();            // Limpiar pantalla
    lcd.setCursor(0, 0);    // Posicionar cursor en inicio
    lcd.print("Inicializando..."); // Mensaje inicial
    
    // Conectar a la red WiFi
    conectarWiFi();

    // Configuración de pines de relés como salidas
    pinMode(releCarga, OUTPUT);
    pinMode(releDescarga, OUTPUT);
    
    // Asegurar que los relés están desactivados al inicio
    digitalWrite(releCarga, LOW);
    digitalWrite(releDescarga, LOW);

    // Inicialización del sensor INA219 para carga
    if (!ina219_carga.begin()) {
        Serial.println("Error al detectar INA219 (carga)");
        lcd.setCursor(0, 1);
        lcd.print("INA219 CARGA ERROR");
        while (1); // Detener ejecución si hay error
    }

    // Inicialización del sensor INA219 para descarga
    if (!ina219_descarga.begin()) {
        Serial.println("Error al detectar INA219 (descarga)");
        lcd.setCursor(0, 1);
        lcd.print("INA219 DESCARGA ERROR");
        while (1); // Detener ejecución si hay error
    }

    // Limpiar LCD después de inicialización
    lcd.clear();
    
    // Inicializar variable de tiempo para cálculos
    tiempoAnterior = millis();
}

void loop() {
    // Verificar si es necesario reiniciar la conexión WiFi
    if (millis() - tiempoUltimaConexion > intervaloWiFi) {
        Serial.println("Reiniciando WiFi...");
        WiFi.disconnect(); // Desconectar WiFi
        delay(1000);       // Esperar 1 segundo
        conectarWiFi();    // Reconectar
        tiempoUltimaConexion = millis(); // Actualizar tiempo de última conexión
    }

    // Lectura de todos los sensores
    float voltage = readVoltage(); // Leer voltaje de batería
    float percentage = calculatePercentage(voltage); // Calcular porcentaje de carga
    float temperatura = thermocouple.readCelsius(); // Leer temperatura
    
    // Leer corrientes (convertir mA a A)
    float corriente_carga = ina219_carga.getCurrent_mA() / 1000.0; 
    float corriente_descarga = ina219_descarga.getCurrent_mA() / 1000.0; 
    
    // Calcular potencia (P = V * I)
    float potencia = voltage * (corriente_carga + corriente_descarga);

    // Cálculo de Amperios-hora (integración de corriente sobre tiempo)
    unsigned long tiempoActual = millis();
    float tiempoHoras = (tiempoActual - tiempoAnterior) / 3600000.0; // Convertir ms a horas
    ampereHora += (corriente_carga + corriente_descarga) * tiempoHoras; // Integrar corriente
    tiempoAnterior = tiempoActual; // Actualizar tiempo anterior

    // Variables para control de estado
    String estadoBateria = "";         // Estado actual como texto
    static bool previoEnCarga = false; // Estado anterior (static mantiene valor entre llamadas)

    // Lógica de control de carga/descarga
    if (enCarga) {
        // En modo carga
        if (voltage > voltajeMaximo) {
            voltajeMaximo = voltage; // Actualizar voltaje máximo si es necesario
        }

        // Verificar si el voltaje bajó 0.02V del máximo (fin de carga)
        if (voltage <= (voltajeMaximo - 0.02)) {  
            digitalWrite(releCarga, LOW);    // Desactivar carga
            digitalWrite(releDescarga, HIGH); // Activar descarga
            enCarga = false; // Cambiar a modo descarga
            estadoBateria = "Descarga";
            Serial.println("Cambio a descarga por caída de voltaje.");
        } else {
            // Continuar cargando
            digitalWrite(releCarga, HIGH);  // Activar carga
            digitalWrite(releDescarga, LOW); // Desactivar descarga
            estadoBateria = "Cargando";
        }
    } else {
        // En modo descarga
        // Verificar si el voltaje bajó a 3.0V (iniciar carga)
        if (voltage <= 3.0) {  
            digitalWrite(releDescarga, LOW); // Desactivar descarga
            digitalWrite(releCarga, HIGH);   // Activar carga
            enCarga = true; // Cambiar a modo carga
            voltajeMaximo = voltage; // Establecer nuevo voltaje máximo
            estadoBateria = "Cargando";
            
            // Si es un nuevo ciclo (no estaba en carga antes)
            if (!previoEnCarga) {
                cicloContador++; // Incrementar contador de ciclos
                Serial.printf("Nuevo ciclo registrado: %d\n", cicloContador);
                ampereHora = 0.0; // Resetear acumulador de Amperios-hora
            }
        } else {
            // Continuar descargando
            digitalWrite(releCarga, LOW);    // Desactivar carga
            digitalWrite(releDescarga, HIGH); // Activar descarga
            estadoBateria = "Descarga";
        }
    }
    previoEnCarga = enCarga; // Guardar estado actual para siguiente iteración

    // Mostrar todos los datos por el puerto serial
    Serial.printf("V: %.2f V | B: %.1f%% | IC: %.3f A | ID: %.3f A | P: %.3f W | Ah: %.4f | T: %.2f°C | Estado: %s | Ciclos: %d\n", 
                  voltage, percentage, corriente_carga, corriente_descarga, potencia, ampereHora, temperatura, estadoBateria.c_str(), cicloContador);
    
    // Mostrar datos en la pantalla LCD
    mostrarDatosLCD(voltage, percentage, temperatura, corriente_carga, corriente_descarga, potencia, ampereHora, estadoBateria, cicloContador);

    // Enviar datos a ThingSpeak si ha pasado el intervalo definido
    if (millis() - tiempoUltimoEnvio >= intervaloEnvio) {
        sendToThingSpeak(voltage, percentage, temperatura, corriente_carga, corriente_descarga, potencia, ampereHora, cicloContador);
        tiempoUltimoEnvio = millis(); // Actualizar tiempo de último envío
    }

    delay(1000); // Esperar 1 segundo antes de repetir el loop
}

// Función para conectar a WiFi
void conectarWiFi() {
    WiFi.begin(ssid, password); // Iniciar conexión
    Serial.print("Conectando a WiFi");
    
    // Esperar hasta que se establezca la conexión
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("."); // Mostrar progreso
    }
    Serial.println("\nWiFi conectado"); // Confirmar conexión exitosa
}

// Función para leer voltaje del MAX17043
float readVoltage() {
    Wire.beginTransmission(MAX17043_ADDRESS); // Iniciar comunicación con el sensor
    Wire.write(0x02); // Registrar para leer voltaje
    Wire.endTransmission(false); // Mantener conexión I2C activa
    
    Wire.requestFrom(MAX17043_ADDRESS, 2); // Solicitar 2 bytes de datos
    // Combinar los dos bytes y ajustar según hoja de datos
    uint16_t rawVoltage = (Wire.read() << 8) | Wire.read();
    return (rawVoltage >> 4) * 0.00125; // Convertir a voltaje (ver hoja de datos)
}

// Función para calcular porcentaje de carga basado en voltaje
float calculatePercentage(float voltage) {
    if (voltage <= 3.0) return 0.0;    // 3.0V = 0%
    if (voltage >= 4.7) return 100.0;  // 4.7V = 100%
    // Interpolación lineal entre 3.0V y 4.7V
    return (voltage - 3.0) / (4.7 - 3.0) * 100.0;
}

// Función para mostrar datos en el LCD
void mostrarDatosLCD(float voltage, float percentage, float temperatura, float corriente_carga, float corriente_descarga, float potencia, float ampereHora, String estado, int ciclos) {
    lcd.clear(); // Limpiar pantalla
    
    // Línea 1: Voltaje y porcentaje de carga
    lcd.setCursor(0, 0);
    lcd.printf("V: %.2fV  B: %.1f%%", voltage, percentage);
    
    // Línea 2: Corriente y potencia (dependiendo del estado)
    lcd.setCursor(0, 1);
    if (estado == "Cargando") {
        lcd.printf("IC: %.3fA  P: %.3fW", corriente_carga, potencia);
    } else {
        lcd.printf("ID: %.3fA  P: %.3fW", corriente_descarga, potencia);
    }
    
    // Línea 3: Amperios-hora y temperatura
    lcd.setCursor(0, 2);
    lcd.printf("Ah: %.4f  T: %.1fC", ampereHora, temperatura);
    
    // Línea 4: Contador de ciclos y estado
    lcd.setCursor(0, 3);
    lcd.printf("Ciclos: %d  %s", ciclos, estado.c_str());
}

// Función para enviar datos a ThingSpeak
void sendToThingSpeak(float voltage, float percentage, float temperatura, float corriente_carga, float corriente_descarga, float potencia, float ampereHora, int ciclos) {
    // Solo enviar si hay conexión WiFi
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http; // Objeto para peticiones HTTP
        
        // Construir URL con todos los parámetros
        String url = String(server) + "/update?api_key=" + apiKey +
                     "&field1=" + String(voltage) +                  // Campo 1: Voltaje
                     "&field2=" + String(percentage) +               // Campo 2: Porcentaje
                     "&field3=" + String(temperatura) +              // Campo 3: Temperatura
                     "&field4=" + String(ciclos) +                   // Campo 4: Ciclos
                     "&field5=" + String(corriente_carga) +          // Campo 5: Corriente carga
                     "&field6=" + String(corriente_descarga) +      // Campo 6: Corriente descarga
                     "&field7=" + String(potencia) +                 // Campo 7: Potencia
                     "&field8=" + String(ampereHora);               // Campo 8: Amperios-hora
        
        http.begin(url); // Iniciar conexión
        http.GET();      // Enviar petición GET
        http.end();      // Cerrar conexión
    }
}