# Dashboard en ThingSpeak para el Estudio de Baterías de Iones de Litio 18650.  
<img width="1350" height="640" alt="image" src="https://github.com/user-attachments/assets/b625224d-eba7-45aa-8c19-91ace814a6d9" />

Este repositorio documenta los códigos y la configuración necesarios para el **monitoreo y análisis en la nube** de baterías de ion–litio tipo **18650**.  
La plataforma utilizada para la visualización de datos es **ThingSpeak**, la cual permite graficar en tiempo real y almacenar históricos de las pruebas realizadas.  

---
## Estructura del repositorio.

- [Objetivo.](#objetivo)
- [Aplicaciones.](#aplicaciones)
- [Requisitos.](#requisitos)  
  - [Software.](#Software)  
  - [Hardware.](#hardware)  
- [Configuración ESP32 a Thingspeak para la visualzación en tiempo real.](#configuracion-esp32-a-thingspeak-para-la-visualizacion-en-tiempo-real)  
- [Visibilidad del Dashboard en ThingSpeak.](#visibilidad-del-dashboard-en-thingSpeak)  
- [Funcionamiento de los Códigos.](#funcionamiento-de-los-codigos)
- [Antes de ejectuar pruebas.](#ejecutar-pruebas)
- [Battery Tester IoT.](#battery-tester-iot)


  
---

## Objetivo.  

Desarrollar un sistema de adquisición de datos y visualización que permita:  

- Medir en tiempo real parámetros de la batería (voltaje, corriente, potencia, capacidad, estado de carga, ciclos).  
- Enviar los datos de forma inalámbrica (WiFi) a **ThingSpeak**.  
- Configurar un **dashboard dinámico** para analizar el desempeño de la batería en pruebas de carga y descarga.  

---

##  Aplicaciones.  

- Monitoreo remoto de pruebas de baterías 18650.  
- Estudios de **ciclos de vida y degradación** de celdas.  
- Sistemas de energía para **nanosatélites (EPS)**.  
- Proyectos de **IoT energético** con almacenamiento de datos en la nube.  

---
##  Requisitos.  

### Hardware.  
- ESP32 DEVKIT V1  
- Batería Li-ion 18650  
- Sensores (INA219, MAX17043, MAX6675 u otros según configuración)  
- Pantalla LCD u OLED (opcional)  
- Módulo de carga/descarga (TP4056, relés, resistencias de prueba)

### Software.  
- Arduino IDE o PlatformIO  
- Librerías específicas de sensores (Adafruit INA219, Adafruit SSD1306, etc.)  
- Cuenta en [ThingSpeak](https://thingspeak.com)  
- API Key de tu canal en ThingSpeak  

---

## Configuración ESP32 a Thingspeak para la visualzación en tiempo real.
_Para obtener las distintas gráficas y datos obtenidos de la ESP32 a través de internet a la plataforma Thingspeak.  
Es necesario crear una cuenta en - https://thingspeak.mathworks.com/_  

La arquitectura para el envío de datos es la siguiente.  
<img width="670" height="332" alt="image" src="https://github.com/user-attachments/assets/b6a377d7-fa4b-4c16-974b-5f4225f949da" />


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
## Códigos MatLAB (.m) de conexión ESP32 a Thingspeak.  

Para ejecutar código MATrix LABoratory (.m) es necesario elegir la opción "MATLAB Visualization" en el panel de thingspeak.  
[![mtlab-visual.jpg](https://i.postimg.cc/VL5p1Lqq/mtlab-visual.jpg)](https://postimg.cc/Mv2dmwrH)  

Elegimos la opción _custom (no starter code)_ y create.  
[![mtlab-visual-2.jpg](https://i.postimg.cc/bJsVPmbz/mtlab-visual-2.jpg)](https://postimg.cc/ygCyXXv5)  

Agregamos el código MATLAB (.m) y damos en Save and Run.
[![mtlab-visual-3.jpg](https://i.postimg.cc/fLCW4Tc4/mtlab-visual-3.jpg)](https://postimg.cc/PpLnYHVK)

Si nos dirijimos al dashboard podremos ver nuestro Widged desplegado.  
<img width="484" height="335" alt="image" src="https://github.com/user-attachments/assets/9fae9131-7d46-4e4d-a1ee-4cbcdba4dcd8" />


### Antes de ejecutar pruebas.

_Es necesario conectar el circuito de forma correcta en cada uno de las pruebas antes de subir cualquier código._  

```
1. Ejecutar el código en Arduino IDE.
```
*El código debe estar funcionando correctamente para poder visualizar los datos en Thingspeak.

Asegurate de tener lo siguiente:
```
Agregar las credenciales correctas como el ID Channel, Read y Write API Key. que se encuentran en los settings de Thingspeak.
```

---

## Battery Tester IoT


###  Visibilidad del Dashboard en ThingSpeak.  

El **dashboard en ThingSpeak** muestra:  

- [Voltaje (V)](#voltaje-(v)) → evolución durante la carga/descarga.
- [Porcentaje de la batería SOC (%)](#porcentaje-de-la-batería-SOC-(%)) → Porcentaje de la batería.
- [Temperatura (°C)](#temperatura-(°c)) → seguridad y monitoreo térmico.
- 

- **Corriente (A)** → consumo o entrega de energía.  
- **Potencia (W)** → cálculo en tiempo real.  
- **Estado de carga (SoC, %)** → conversión de voltaje a porcentaje.  
- **Capacidad acumulada (mAh)** → integración de corriente en el tiempo.  
- **Número de ciclos** → conteo de cargas y descargas completas.  

Los gráficos permiten **elegir intervalos de tiempo (horas, días, semanas)** y aplicar operaciones matemáticas como promedio, máximo y mínimo.  

---

 ### [Voltaje (V)](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Thingspeak%20Dashboard/Voltaje-bater%C3%ADa%20.m).  
<img width="997" height="316" alt="image" src="https://github.com/user-attachments/assets/edf03c3b-9648-40cb-82f8-a5e778a549b9" />  
 

Esta gráfica muestra el monitoreo del voltaje de una batería a lo largo de los días.  
Se observa un patrón cíclico y repetitivo de carga y descarga.  
Eje X muestra el tiempo (D,M,H).   
Eje Y se muestra el voltaje.  

---

### [Porcentaje de la batería SOC (%)](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Thingspeak%20Dashboard/Porcentaje-bateria.m).  
<img width="999" height="313" alt="image" src="https://github.com/user-attachments/assets/428679fb-fd6f-43bd-afb8-7e1516df3629" />  
   

Esta visualización presenta los mismos ciclos de carga y descarga que la gráfica de voltaje, pero de una forma más fácil de interpretar para el usuario a través de un porcentaje y zonas de color.

Zonas de Estado: La gráfica está dividida en tres zonas de color que indican el estado de la batería:

🔴 Roja (0-20%): Nivel de carga bajo o crítico.

🟢 Verde (20-80%): Rango de operación normal y óptimo para la salud de la batería.

🟡 Amarilla (80-100%): Nivel de carga alto o casi lleno.

**Límites de Carga:*** Es interesante notar que la batería no se carga al 100%, sino que el ciclo de carga se detiene alrededor del 97-98%. Esta es una estrategia común en los sistemas de gestión de baterías (BMS) para prolongar la vida útil de la celda, ya que mantenerla constantemente al 100% genera más estrés químico.

**Profundidad de Descarga:*** La gráfica revela que la batería experimenta ciclos de descarga muy profundos diariamente, pasando por la zona verde y roja hasta agotarse casi por completo. Aunque el sistema está diseñado para funcionar así, estos ciclos tan profundos son más exigentes para la batería que si se realizaran descargas más parciales (por ejemplo, del 80% al 30%).

---

### [Temperatura (°C)](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Thingspeak%20Dashboard/Temperatura-bater%C3%ADa.m). 
<img width="999" height="309" alt="image" src="https://github.com/user-attachments/assets/112c239f-ca92-4c0e-bfa7-06d9b248ac96" />  

Esta gráfica muestra la temperatura de la batería en grados Celsius (°C) durante el mismo periodo que las gráficas anteriores. El análisis revela una clara conexión entre la temperatura y los ciclos de carga de la batería.  

**Eje Y
Etiqueta: "Temperatura (°C)"**

Este eje representa la temperatura de la batería. Las unidades están en grados Celsius (°C). En la gráfica, el rango visible va aproximadamente de 26°C a 36°C.

**Eje X
Etiqueta: "Fecha y Hora"**

Este eje representa el tiempo. Muestra el avance de los días ("Sep 07", "Sep 08", "Sep 09") y las horas, permitiendo ver cómo cambia la temperatura a lo largo del día y la noche.

---

### [Número de ciclos](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Thingspeak%20Dashboard/ciclos-grafica-bater%C3%ADa.m).
<img width="1000" height="313" alt="image" src="https://github.com/user-attachments/assets/408870eb-f84f-4a28-9e2e-595ab8bcb0e1" />  

Su objetivo es mostrar cómo una variable (el número de ciclos) se ha acumulado y ha cambiado a lo largo del tiempo. No solo muestra el valor final, sino también cuándo ocurrió cada incremento.  

Eje Vertical (Y): Representa la cantidad total de ciclos completados.  

Eje Horizontal (X): Representa el tiempo, mostrando la fecha y la hora.  

La gráfica tiene una forma distintiva de "escalera" o "escalones". Este patrón es típico de un contador acumulativo:  
Las líneas horizontales indican períodos de tiempo en los que el número de ciclos se mantuvo constante.  
Los saltos verticales representan el momento exacto en que se completó un nuevo ciclo y el contador aumentó su valor.  

---

### [Corriente de carga acumulada (A)](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Thingspeak%20Dashboard/Voltaje-bater%C3%ADa%20.m).  
<img width="1000" height="314" alt="image" src="https://github.com/user-attachments/assets/8ca7da7e-c1ce-426f-86c9-3a87d556832a" />  

Es una gráfica de área y de línea a lo largo del tiempo, diseñada para mostrar el comportamiento de la corriente durante ciclos repetitivos.

Eje Vertical (Y): Representa el valor de la Corriente en miliamperios (mA). Un punto más alto en la gráfica indica un mayor flujo de corriente hacia la batería.

Eje Horizontal (X): Representa el Tiempo, mostrando la fecha y la hora en que se registraron los datos.

La corriente se mantiene alta y luego comienza a descender de forma curva. Este comportamiento es típico en los algoritmos de carga de baterías (fase de corriente constante seguida de voltaje constante).

Fin (Caída Vertical): El ciclo termina cuando la corriente cae abruptamente a cero, indicando que la carga se ha detenido.

Líneas Rojas y Anotaciones: Las líneas punteadas rojas en la cima de cada ciclo marcan y muestran el valor máximo de corriente alcanzado en ese ciclo en particular. Esto es muy útil para comparar la consistencia del proceso de carga entre un ciclo y otro.

Área Rellena (Verde): El sombreado debajo de la línea ayuda a visualizar cada ciclo como una unidad distinta y resalta la "cantidad" de corriente suministrada a lo largo del tiempo.

---

### [Potencia (W)](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Thingspeak%20Dashboard/Voltaje-bater%C3%ADa%20.m).  

### [Capacidad acumulada (mAh)](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Thingspeak%20Dashboard/Voltaje-bater%C3%ADa%20.m).  

 

  

---



# Versiones  ⚙️

## **_Versión 1.0.0_**
* [Versión 1.5](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Tester_Battery_Bank_IoT/Tester_Battery_Bank_IoT_V1.5.ino)

![image](https://github.com/user-attachments/assets/5fa98963-a349-4f9b-b87f-95d35bf98dbe)


Para ver la gráfica en MATLAB de este circuito consulte la carpeta **[Matlab codes](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/tree/main/Matlab%20Codes)** 

x..

## Autores ✒️

* **Addi Trejo** - *Desarrollador de proyecto* - [additrejo](https://github.com/additrejo)

---


* ## Contador de ciclos.
[![Contadordeciclos.jpg](https://i.postimg.cc/KzDSKqjt/Contadordeciclos.jpg)](https://postimg.cc/wyMbZkhB)  
Este código [Contador de ciclos](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Tester_Battery_Bank_IoT/Contadordeciclos.m) 
