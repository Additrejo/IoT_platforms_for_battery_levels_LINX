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

###  * Despliegue - Crear un nuevo canal en thingspeak.  📦
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


### Ejecutar pruebas 🔧

_Es necesario conectar el circuito de forma correcta en cada uno de las pruebas antes de subir cualquier código._  

```
Ejecutar el código en Arduino IDE.
```

_En caso de graficar datos en matlab, asegurarse de que el programa esté ejecutando de forma correcta en Arduino IDE._

```
Agregar las credenciales correctas como el ID Channel, Read y Write API Key. que se encuentran en los settings de Thingspeak.
```


# Versiones  ⚙️

## **_Versión 1.4_**
* [Versión 1.4](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Tester_Battery_Bank_IoT/Tester_Battery_Bank_IoT_V1.4.ino)

![image](https://github.com/user-attachments/assets/2caca5f5-c604-44eb-8fd5-2efa8101ad4a)  

Esta versión es capaz de obtener la corriente de descarga (entre la resistencia y la batería 18650).  

### Gráfica en Matlab
Para ver la gráfica en MATLAB de este circuito consulte la carpeta **[Matlab codes](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/tree/main/Matlab%20Codes)** --> [INA219](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Matlab%20Codes/CorrienteINA219.m)  

![image](https://github.com/user-attachments/assets/08c86828-4559-44b0-96b9-866fff83cc64)  


## **_Versión 1.1_**
* [Versión 1.1](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Tester_Battery_Bank_IoT/Tester_Battery_Bank_IoT_V1.5.ino)

![image](https://github.com/user-attachments/assets/5fa98963-a349-4f9b-b87f-95d35bf98dbe)

Al agregar un segundo sensor de corriente, ahora podemos obtener la corriente de descarga (entre la resistencia y la batería 18650) y la corriente de carga (entre el BMS y la batería) 

## **_Versión 1.2_**
* [Versión 1.2](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Tester_Battery_Bank_IoT/Tester_Battery_Bank_IoT_V1.5.ino)

<img width="1084" height="652" alt="image" src="https://github.com/user-attachments/assets/1f9e72db-6adf-46f3-9cde-c953ca1b47cf" />


---

### Gráfica en Matlab
Para ver la gráfica en MATLAB de este circuito consulte la carpeta **[Matlab codes](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/tree/main/Matlab%20Codes)** 

x..

**


## Wiki 📖

Puedes encontrar mucho más de cómo utilizar este proyecto en nuestra [Wiki](https://github.com/tu/proyecto/wiki)



## Licencia 📄

Este proyecto está bajo la Licencia (LINXlicence) - mira el archivo [LICENSE.md](LICENSE.md) para detalles
--->
## Autores ✒️

* **Addi Trejo** - *Desarrollador de proyecto* - [additrejo](https://github.com/additrejo)

---
