#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_INA219 ina219;

unsigned long previousMillis = 0;
unsigned long interval = 100;
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;
  float energy = 0;

void setup(void) 
{
  Serial.begin(115200);
  while (!Serial) {
      // will pause Zero, Leonardo, etc until serial console opens
      delay(1);
  }

  uint32_t currentFrequency;
    
  Serial.println("Hello!");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  
  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(1000); }
  }
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();

  Serial.println("Measuring voltage and current with INA219 ...");
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    ina219values();
    displaydata();
  }
}


void ina219values()  {

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  energy = energy + loadvoltage * current_mA / 3600;
  
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");
}
void displaydata() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(loadvoltage);
  display.setCursor(31, 0);
  display.println("V");
  display.setCursor(62, 0);  
  display.setCursor(75, 0);
  display.println(current_mA);
  display.setCursor(110, 0);
  display.println("mA");
  display.setCursor(0, 6);
  display.println("--------------------"); 



    
  display.setCursor(0, 13);
  display.println(loadvoltage * current_mA);
  display.setCursor(57, 13);
  display.println("mW");
  display.setCursor(0, 23);
  display.println(energy);
  display.setCursor(57, 23);
  display.println("mWh");
  display.display();
}