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
