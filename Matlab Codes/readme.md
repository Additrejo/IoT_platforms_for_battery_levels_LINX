# MATLAB Codes Tester Battery Bank.

**Breve descripción del proyecto:**  
En esta carpeta se encuentra los códigos para graficar en MATLAB los sensores agregados en el proyecto.  
No es necesario instalar alguna dependencia en particular pero si es importante agregar las API Keys correctas para poder importar los datos.

---

## Índice

1. [CorrienteINA219](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Matlab%20Codes/CorrienteINA219.m)

---

## Descripción

Estos códigos pueden usarse para la graficación de datos en tiepo real en [MATLAB](https://la.mathworks.com/products/matlab.html) o bien en su plataforma IoT [ThingSpeak](https://thingspeak.mathworks.com/):  

---

## Instalación

Algo importante que ocuparemos en nuestros códigos serán las API KEYS que thingspeak nos proporciona y que serán colocadas en diferente.  

[![Apikeys.jpg](https://i.postimg.cc/g2XqTykF/Apikeys.jpg)](https://postimg.cc/gxpZwZZN)

```bash
# Read API Key 
[La Read API Key de tu canal]

# Write API key
[La Write API Key de tu canal]

```
---

## Uso

Abrir MATLAB y pegar el código..

**1. [CorrienteINA219](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Matlab%20Codes/CorrienteINA219.m)**

Este código muestra la corriente obtenida por el microcontrolador a tráves del sensor INA219.

![image](https://github.com/user-attachments/assets/d41ef38d-ee65-4d6b-b26d-5596942c1ef2)


**2. [PotenciaINA219](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Matlab%20Codes/PotenciaINA219.m)**

Este código muestra la potencia obtenida por ley de ohm del sensor INA219.  
![image](https://github.com/user-attachments/assets/9078c96c-05ac-4b8b-a72b-69aff09404bb)

**3. [Taza_de_cambio_voltaje](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Matlab%20Codes/Taza_de_cambio_Voltaje.m)**

Este código muestra Las graficas de taza de cambio en el voltaje y el voltaje obtenido por el sensor MAX17043.  

![image](https://github.com/user-attachments/assets/f7fd4a0c-ebec-4231-b396-08c03ae0cc60)

**4. [Taza_de_cambio_voltaje](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Matlab%20Codes/Taza_de_cambio_Volts.m)**

Este código solo muestra la La taza de cambio en el voltaje obtenido por el sensor MAX17043.  

![image](https://github.com/user-attachments/assets/79f97078-41a8-4135-bceb-2f99652de1d3)


**5. [Muestras de voltaje](https://github.com/LINX-ICN-UNAM/IoT_platforms_for_battery_levels_by_LINX/blob/main/Matlab%20Codes/muestras_voltaje.m)**

Muestra 500 lecturas de medición en el voltaje.

![image](https://github.com/user-attachments/assets/6a8780aa-9b0b-4182-a62c-10409354ffdc)


```bash
# Solicitud GET
curl -X GET https://api.ejemplo.com/endpoint
```

---

## Características

Lista las características principales de tu proyecto:

- 🚀 Función principal 1.
- 🔧 Función secundaria 2.
- 🌟 Función adicional 3.

---

## Contribución

Explica cómo otros pueden contribuir al proyecto.

1. Haz un fork del repositorio.
2. Crea una rama para tu característica o corrección:
   ```bash
   git checkout -b nueva-caracteristica
   ```
3. Haz commit de tus cambios:
   ```bash
   git commit -m "Añadida nueva característica"
   ```
4. Envía tus cambios al repositorio original:
   ```bash
   git push origin nueva-caracteristica
   ```
5. Abre un pull request.

Asegúrate de leer nuestro archivo [CONTRIBUTING.md](CONTRIBUTING.md) para más detalles.

---

## Roadmap

Describe los planes futuros de tu proyecto:

- [ ] Característica 1.
- [ ] Característica 2.
- [ ] Corrección de errores conocidos.

---

## Licencia

Este proyecto está licenciado bajo la licencia [Nombre de la Licencia](LICENSE).  
(Copia el texto de la licencia en un archivo `LICENSE` en el repositorio).

---

## Contacto

**Tu Nombre**  
- 📧 Correo: [tu-email@ejemplo.com](mailto:tu-email@ejemplo.com)  
- 🌐 Portafolio o Sitio Web: [www.tu-sitio.com](https://www.tu-sitio.com)  
- 🐦 Twitter: [@tu_usuario](https://twitter.com/tu_usuario)  
- GitHub: [tu-usuario](https://github.com/tu-usuario)

---
