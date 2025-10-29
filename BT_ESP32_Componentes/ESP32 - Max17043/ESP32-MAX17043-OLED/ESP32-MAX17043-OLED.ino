#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define MAX17043_ADDRESS 0x36  // Dirección I2C del MAX17043
#define SCREEN_WIDTH 128      // Ancho de la pantalla OLED
#define SCREEN_HEIGHT 32      // Altura de la pantalla OLED
#define OLED_RESET    -1      // Reset por software
#define SCREEN_ADDRESS 0x3C   // Dirección I2C de la pantalla OLED

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // Pines SDA y SCL para ESP32

  // Inicializar pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("No se encontró pantalla OLED.");
    while (true);
  }
  display.clearDisplay();
  display.display();

  // Realizar un Quick Start para inicializar el MAX17043
  quickStart();

  Serial.println("MAX17043 inicializado.");
  Serial.println("Voltaje (V)\tPorcentaje (%)"); // Encabezado para el plotter
}

void loop() {
  float voltage = readVoltage();
  float percentage = calculatePercentage(voltage);

  // Mostrar valores en el Monitor Serial
  Serial.print(voltage, 2);    // Voltaje con 2 decimales
  Serial.print("\t");          // Separador para el plotter
  Serial.println(percentage);  // Porcentaje con 1 decimal

  // Mostrar valores en la pantalla OLED
  displayData(voltage, percentage);

  delay(500); // Actualizar cada 500ms
}

void quickStart() {
  Wire.beginTransmission(MAX17043_ADDRESS);
  Wire.write(0x06); // Comando Quick Start
  Wire.endTransmission();
  delay(10); // Esperar para completar
}

float readVoltage() {
  Wire.beginTransmission(MAX17043_ADDRESS);
  Wire.write(0x02); // Registro de voltaje
  Wire.endTransmission(false);

  Wire.requestFrom(MAX17043_ADDRESS, 2);
  uint16_t rawVoltage = (Wire.read() << 8) | Wire.read();
  return (rawVoltage >> 4) * 0.00125; // Conversión a voltios
}

float calculatePercentage(float voltage) {
  if (voltage <= 3.0) {
    return 0.0; // Menos de 3.0V es 0%
  } else if (voltage >= 3.7) {
    return 100.0; // Más de 3.7V es 100%
  } else {
    // Interpolación lineal entre 3.0V (0%) y 3.7V (100%)
    return (voltage - 3.0) / (3.7 - 3.0) * 100.0;
  }
}

void displayData(float voltage, float percentage) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // Mostrar voltaje
  display.setCursor(0, 0);
  display.printf("Voltaje: %.2f V", voltage);

  // Mostrar porcentaje
  display.setCursor(0, 16);
  display.printf("Bateria: %.1f %%", percentage);

  display.display();
}

