# * Desarrollo y uso de plataformas IoT para el análisis de baterías Li-Ion.

[![thigspeak-dashboard.jpg](https://i.postimg.cc/3NwT5HV1/thigspeak-dashboard.jpg)](https://postimg.cc/dkgXrgqZ)

## Descripción
_Repositorio de documentación acerca del uso de plataformas IoT para mediciones de baterías Li-Ion en el laboratorio de instrumentación espacial LINX ICN UNAM._  

Este repositorio te permitirá obtener las instrucciones necesarias para propósitos de desarrollo y pruebas de hardware así como el uso de plataformas IoT en el estudio de baterías Li-Ion.


## Pre-requisitos 📋
### Hardware necesario.  
* PC
* ESP32
* Componentes de electrónica según pruebas.

### Software necesario.
* [Arduino IDE](https://www.arduino.cc/en/software) - Programación de microcontrolador.
* [Matlab](https://la.mathworks.com/products/matlab.html) - Analisis de datos.

### Plataformas.
*  [Thingspeak](https://thingspeak.mathworks.com/) - Plataforma de visualización de datos IoT.

  _*Para el caso de Thingspeak y matlab se necesita solo una cuenta en mathworks para poder ser usadas_


## Menu de Carpetas
Aquí se encuentra el menu de carpetas que contienen los debugs, pruebas de sensores, así como plataformas.

*   [Códigos ESP32/ESP32_INA219.](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/tree/main/C%C3%B3digos%20ESP32/ESP32_INA219)  
    Esta carpeta contiene las pruebas, así como el funcionamiento del sensor INA219 que mide la corriente y el voltaje de una batería 18650.
*   [ESP32 - Max17043.](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/tree/main/ESP32%20-%20Max17043)  
    Esta carpeta contiene las pruebas, así como el funcionamiento del sensor Max17043 que mide el voltaje, así como del SOC de una batería 18650.  
*   [ESP32 - Relevadores.](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/tree/main/ESP32-Relevadores)  
    Contiene las pruebas de relevadores para conmutar Carga y descarga.
*   [Esp32_test_LCD](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/tree/main/Esp32_test_LCD)  
    Contiene las pruebas de la pantalla LCD.
*   [Tester_Battery_Bank_IoT (Versiones) ](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/tree/main/Tester_Battery_Bank_IoT)  
    Códigos de programación para el hardware.   
*   [Thingspeak Dashboard](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/tree/main/Thingspeak%20Dashboard)  
    Códigos de visualización y sus versiones del dashboard en Thingspeak.  
    


<!--
### Pre-requisitos 📋

_Software necesario_
```  
Arduino IDE
Visual Studio Code
Bibliotecas que será indicadas en cada subproyecto
Matlab

```
--->

### Ejecutar pruebas 🔧

_Es necesario conectar el circuito de forma correcta en cada uno de las pruebas antes de subir cualquier código._  

```
Ejecutar el código en Arduino IDE.
```

_En caso de graficar datos en matlab, asegurarse de que el programa esté ejecutando de forma correcta en Arduino IDE._

```
NOTA IMPORTANTE: Agregar las credenciales correctas como el ID Channel, Read y Write API Key. que se encuentran en los settings de Thingspeak.
```

<!-- 
## Ejecutando las pruebas ⚙️

_Explica como ejecutar las pruebas automatizadas para este sistema_

### Analice las pruebas end-to-end 🔩

_Explica que verifican estas pruebas y por qué_

```
Da un ejemplo
```

### Y las pruebas de estilo de codificación ⌨️

_Explica que verifican estas pruebas y por qué_

```
Da un ejemplo
```

## Despliegue 📦

_Agrega notas adicionales sobre como hacer deploy_

## Construido con 🛠️

_Menciona las herramientas que utilizaste para crear tu proyecto_

* [Dropwizard](http://www.dropwizard.io/1.0.2/docs/) - El framework web usado
* [Maven](https://maven.apache.org/) - Manejador de dependencias
* [ROME](https://rometools.github.io/rome/) - Usado para generar RSS

## Contribuyendo 🖇️

Por favor lee el [CONTRIBUTING.md](https://gist.github.com/villanuevand/xxxxxx) para detalles de nuestro código de conducta, y el proceso para enviarnos pull requests.

## Wiki 📖

Puedes encontrar mucho más de cómo utilizar este proyecto en nuestra [Wiki](https://github.com/tu/proyecto/wiki)

## Versionado 📌

Usamos [SemVer](http://semver.org/) para el versionado. Para todas las versiones disponibles, mira los [tags en este repositorio](https://github.com/tu/proyecto/tags).

## Licencia 📄

Este proyecto está bajo la Licencia (Tu Licencia) - mira el archivo [LICENSE.md](LICENSE.md) para detalles
--->
## Autores ✒️

* **Addi Trejo** - *Desarrollador de proyecto* - [additrejo](https://github.com/additrejo)

