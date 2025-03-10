# Configuración para el dashboard para el análisis de baterías Li-Ion en plataforma Thingspeak y MATLAB.
_Repositorio de documentación para la configuración de la plataforma thingspeak para la medición de baterías Li-Ion en el laboratorio de instrumentación espacial LINX ICN UNAM._

[![thigspeak-dashboard.jpg](https://i.postimg.cc/3NwT5HV1/thigspeak-dashboard.jpg)](https://postimg.cc/dkgXrgqZ)

## Start🚀

_Estas instrucciones te permitirá obtener la información necesaria para la configuración y pruebas en el estudio de baterías Li-Ion._

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

  _*Para el caso de Thingspeak y matlab se necesita solo una cuenta para poder ser usadas_  
  
---
## Configuración ESP32 a Thingspeak en tiempo real.
_Para obtener las distintas gráficas y datos obtenidos de la ESP32 a través de internet a la plataforma Thingspeak.  
Es necesario crear una cuenta en - https://thingspeak.mathworks.com/_  

A continuación se muestran los pasos para crear un nuevo canal.

###  * Crear un nuevo canal en thingspeak.  
[![Thingspeak1.jpg](https://i.postimg.cc/wjx3rdHj/Thingspeak1.jpg)](https://postimg.cc/w7P6tZ7C)

- Agregar un nombre al canal
- Descripción (opcional)
- Field 1: Voltaje. (será el indicador dónde llegarán los datos del voltaje)
- Field 2: Porcentaje de batería (será el indicador dónde llegaran los datos del porcentaje de la batería.
- *Así creamos Field n para cualquier sensor o dato que se requiera mostrar en el dashboard de thingspeak

[![Thingspeak2.jpg](https://i.postimg.cc/prVgHkdz/Thingspeak2.jpg)](https://postimg.cc/vgjP7rfZ)  

### * Guardar canal.  
[![Thingspeak3.jpg](https://i.postimg.cc/fTGB7BhC/Thingspeak3.jpg)](https://postimg.cc/3dZXYjN0)  

### * API KEYS  

Algo importante que ocuparemos en nuestros códigos serán las API KEYS que thingspeak nos proporciona y que serán colocadas en diferente.  

[![Apikeys.jpg](https://i.postimg.cc/g2XqTykF/Apikeys.jpg)](https://postimg.cc/gxpZwZZN)
---
## Códigos de conexión ESP32 a Thingspeak  
*  ### Voltaje y porcentaje de la batería.
[![Thingspeak5.jpg](https://i.postimg.cc/tCxLH1qr/Thingspeak5.jpg)](https://postimg.cc/dLJW2VTd)  
Para obtener estas gráficas, ya se encuentran documentadas en la carpeta [ESP32 - Max17043
](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/tree/main/ESP32%20-%20Max17043)


* ## Contador de ciclos
[![Contadordeciclos.jpg](https://i.postimg.cc/KzDSKqjt/Contadordeciclos.jpg)](https://postimg.cc/wyMbZkhB)  
Este código [Contador de ciclos](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Tester_Battery_Bank_IoT/Contadordeciclos.m) se ejecuta en Matlab o desde la opció "MATLAB Visualization" en el panel de thingspeak.  
[![mtlab-visual.jpg](https://i.postimg.cc/VL5p1Lqq/mtlab-visual.jpg)](https://postimg.cc/Mv2dmwrH)  
Elegimos la opción _custom (no starter code)_ y create.  
[![mtlab-visual-2.jpg](https://i.postimg.cc/bJsVPmbz/mtlab-visual-2.jpg)](https://postimg.cc/ygCyXXv5)  
Pegamos el código y damos en Save and Run.

[![mtlab-visual-3.jpg](https://i.postimg.cc/fLCW4Tc4/mtlab-visual-3.jpg)](https://postimg.cc/PpLnYHVK)

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
Agregar las credenciales correctas como el ID Channel, Read y Write API Key. que se encuentran en los settings de Thingspeak.
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

<!--- También puedes mirar la lista de todos los [contribuyentes](https://github.com/your/project/contributors) quíenes han participado en este proyecto. --->



---
Hecho con ❤️ por [Addi Trejo](https://github.com/Additrejo)
