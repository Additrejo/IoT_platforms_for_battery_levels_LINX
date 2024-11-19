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
unsigned long interval = 100;
float shuntvoltage = 0;
float busvoltage = 0;
float current_mA = 0;
float current_A = 0;
float loadvoltage = 0;
float power_mW = 0;
float energy = 0;
float correctionFactor = 3.05;  // Ajuste para obtener el valor real de voltaje



void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // Configura SDA y SCL para ESP32
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  
  if (!ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(5000); }
  }

  Serial.println("Measuring voltage and current with INA219 ...");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ina219values();
    displaydata();
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

void displaydata() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(loadvoltage);  // Voltaje corregido
  display.setCursor(31, 0);
  display.print("V");
  display.setCursor(75, 0);
  display.print(current_A);  // Corriente en A
  display.setCursor(110, 0);
  display.print("A");
  display.setCursor(0, 13);
  display.print(loadvoltage * current_A * 1000);  // Potencia en mW
  display.setCursor(57, 13);
  display.print("mW");
  display.setCursor(0, 23);
  display.print(energy);
  display.setCursor(57, 23);
  display.print("mWh");
  display.display();
}
