# Configuración INA219.
_Esta corpeta contiene todo lo necesario para la prueba del sensor de corriente INA219._
[![INA219-modulo.jpg](https://i.postimg.cc/sXS4GLpd/INA219-modulo.jpg)](https://postimg.cc/vgYVp3q3)

## Start🚀

_* Nota, siga al pie de la letra las instrucciones, una mala conexión puede ocacionar un corto circuito o sobrecalentamiento que pueden dañar los componentes ._


### Hardware necesario.  
* PC
* ESP32
* sensor INA 219
* Batería 18650
* Pantalla OLED (Opcional)
* Cables de conexión.

### Documentación Importante.
* [INA 219 Datasheet](https://www.ti.com/product/INA219?utm_source=google&utm_medium=cpc&utm_campaign=asc-null-null-GPN_EN-cpc-pf-google-wwe&utm_content=INA219&ds_k=INA219+Datasheet&DCM=yes&gad_source=1&gclid=CjwKCAiArKW-BhAzEiwAZhWsIMS3zVEjz2ye64ynyMb54fHC5mWYcVLQw__rI5zdF_kPCK49rcOv5RoCnbIQAvD_BwE&gclsrc=aw.ds) - Datasheet del integrado.

### Librerías.
*  [Librería Adafruit_INA 219](https://github.com/adafruit/Adafruit_INA219) - Librería Sensor de corriente INA219 para el IDE Arduino en Github.
*  [Librería Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306) - Librería para pantalla Oled para el IDE Arduino en Github.

## Menu de programas
Aquí se encuentra el menu de carpetas que contienen los debugs, pruebas de sensores, así como plataformas.

[1. ESP32_INA219.ino.](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/C%C3%B3digos%20ESP32/ESP32_INA219/ESP32_INA219.ino)   
Contiene el programa de prueba del sensor con una ESP32.  


## Desarrollo del proyecto

Para iniciar debemos armar el siguiente circuito.  
[![INA219.jpg](https://i.postimg.cc/QCtg4zcz/INA219.jpg)](https://postimg.cc/G8Ws9q7P)   
---
### CONEXIONES:   
#### INA219:  
GND → GND (ESP32)  
VCC → 3.3V (ESP32)  
SCL → GPIO22 (ESP32)  
SDA → GPIO21 (ESP32)  
VIN- → Catodo Led -  
VIN+ → Positivo Batería  

#### LED:  
Catodo → Negativo Batería  
Ánodo → VIN-  

#### Pantalla OLED  
GND → GND (ESP32)  
VCC → 3.3V  
SCL → GPIO22 (ESP32)  
SDA →GPIO21 (ESP32)  

---
### CÓDIGO DE PRUEBA:

*Descargar las librerías mencionadas anteriormente en Arduino IDE antes de cargar el programa.

```bash  
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels
#define OLED_RESET    -1  // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  // OLED I2C address for 128x32 display

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_INA219 ina219;

unsigned long previousMillis = 0;
unsigned long interval = 30000;  // Intervalo de 30 segundos
unsigned long startMillis = 0;  // Tiempo inicial de simulación

float shuntvoltage = 0;
float busvoltage = 0;
float current_mA = 0;
float current_A = 0;
float loadvoltage = 0;
float power_mW = 0;
float energy = 0;
float correctionFactor = 1.0;  // Inicialmente 1.0 para calibración

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // Configura SDA y SCL para ESP32
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();

  if (!ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(5000); }
  }

  startMillis = millis();

  Serial.println("Calibración: Ingresa el factor de corrección por el monitor serial.");
  Serial.println("Voltaje en el Serial Plotter:");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    correctionFactor = input.toFloat();
    Serial.print("Nuevo factor de corrección: ");
    Serial.println(correctionFactor);
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ina219values();
    displaydata(currentMillis - startMillis);  // Pasar tiempo transcurrido
    plotBusVoltage();  // Graficar el Bus Voltage
  }
}

void ina219values() {
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  
  current_A = current_mA / 1000.0;
  loadvoltage = (busvoltage + (shuntvoltage / 1000)) * correctionFactor;
  energy += loadvoltage * current_A / 3600;

  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V (corregido)");
  Serial.print("Current:       "); Serial.print(current_A); Serial.println(" A");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");
}

void displaydata(unsigned long elapsedMillis) {
  unsigned long seconds = elapsedMillis / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;

  seconds %= 60;
  minutes %= 60;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.printf("Time: %02lu:%02lu:%02lu", hours, minutes, seconds);

  display.setCursor(0, 8);
  display.printf("V: %.2fV  A: %.3fA", loadvoltage, current_A);

  display.setCursor(0, 16);
  display.printf("P: %.2fmW", loadvoltage * current_A * 1000);

  display.setCursor(0, 24);
  display.printf("E: %.2fmWh", energy);

  display.display();
}

void plotBusVoltage() {
  // Imprimir solo el Bus Voltage para el Serial Plotter
  Serial.println(busvoltage);
}
```
Ejecutando el circuito obtendremos el valor del voltaje y la corriente que consume el LED.  
[![INA219-Conexi-n.jpg](https://i.postimg.cc/7ZH4s869/INA219-Conexi-n.jpg)](https://postimg.cc/4mSjy0pH)


## Dirección I2C   
Si se utiliza más de un sensor INA219, a cada placa se le debe asignar una dirección única. Esto se realiza mediante los puentes de dirección en el borde derecho de la placa. La dirección base I2C de cada placa es 0x40. La dirección binaria que se programa con los puentes de dirección se suma a la dirección base I2C.

Para programar el desplazamiento de la dirección, utilice una gota de soldadura para puentear el puente de dirección correspondiente a cada "1" binario de la dirección.  

![image](https://github.com/user-attachments/assets/664d99bb-81d7-456b-93ed-3d5f8b868990)  
_Imagen: Adafruit_

Se pueden conectar hasta 4 placas. El direccionamiento es el siguiente:  
Placa 0 : Dirección = 0x40 Desplazamiento = binario 00000 (no se requieren puentes)  
Placa 1 : Dirección = 0x41 Desplazamiento = binario 00001 (puente A0 como en la imagen superior)  
Placa 2 : Dirección = 0x44 Desplazamiento = binario 00100 (puente A1)  
Placa 3 : Dirección = 0x45 Desplazamiento = binario 00101 (puentes A0 y A1)  
