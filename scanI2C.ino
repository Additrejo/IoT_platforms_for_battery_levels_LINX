#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // Pines SDA y SCL
  Serial.println("Escaneando bus I2C...");

  for (byte i = 8; i < 120; i++) {
    Wire.beginTransmission(i);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("Se encontró un dispositivo I2C en la dirección: 0x");
      Serial.println(i, HEX);
    }
  }
}

void loop() {
  // No es necesario hacer nada aquí
}

