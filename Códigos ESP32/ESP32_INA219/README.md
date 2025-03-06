# Configuración INA219.
_Esta corpeta contiene todo lo necesario para la prueba del sensor de corriente INA219._

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

CONEXIONES
MAX17043:
GND → GND (ESP32)
VCC → 3.3V (ESP32)
SCL → GPIO22 (ESP32)
SDA → GPIO21 (ESP32)
S+ → Positivo Batería
S- → Negativo en batería

Pantalla OLED
GND → GND (ESP32)
VCC → 3.3V
SCL → GPIO22 (ESP32)
SDA →GPIO21 (ESP32)

## Desarrollo del proyecto

Para iniciar debemos armar el siguiente circuito.  
[![INA219.jpg](https://i.postimg.cc/QCtg4zcz/INA219.jpg)](https://postimg.cc/G8Ws9q7P)

