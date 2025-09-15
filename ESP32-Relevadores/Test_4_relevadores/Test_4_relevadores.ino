#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*
Autor: Addi Trejo
Programa: Rutina con 4 relevadores y pantalla OLED
Last Update: Abr 11 2025
*/

// Configuración de la pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pines de los relevadores
const int in1 = 25;
const int in2 = 26;
const int in3 = 27;
const int in4 = 14;

void setup() {
  Serial.begin(115200);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("No se encontró la pantalla OLED"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(2);  // Tamaño grande
  display.setTextColor(SSD1306_WHITE);
  mostrarEstado("Iniciando");

  // Apagar todos los relevadores
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void loop() {
  // Encender todos
  activarRele(in1, "Rele 1");
  activarRele(in2, "Rele 2");
  activarRele(in3, "Rele 3");
  activarRele(in4, "Rele 4");
  delay(5000);

  // Apagar todos
  apagarTodos();
  mostrarEstado("Todos OFF");
  delay(1000);

  // Encender uno por uno
  activarRele(in1, "Rele 1");
  delay(5000);
  activarRele(in2, "Rele 2");
  delay(5000);
  activarRele(in3, "Rele 3");
  delay(5000);
  activarRele(in4, "Rele 4");
  delay(5000);

  // Encender todos otra vez
  activarRele(in1, "Rele 1");
  activarRele(in2, "Rele 2");
  activarRele(in3, "Rele 3");
  activarRele(in4, "Rele 4");
  delay(5000);

  // Apagar uno por uno
  apagarRele(in1, "Rele 1");
  delay(5000);
  apagarRele(in2, "Rele 2");
  delay(5000);
  apagarRele(in3, "Rele 3");
  delay(5000);
  apagarRele(in4, "Rele 4");
  delay(5000);

  mostrarEstado("Reiniciando");
  delay(2000);
}

void mostrarEstado(String texto) {
  display.clearDisplay();
  display.setCursor(0, 48);  // Posición abajo
  display.print(texto);
  display.display();
}

void activarRele(int pin, String nombre) {
  digitalWrite(pin, HIGH);
  mostrarEstado(nombre + " OFF");
}

void apagarRele(int pin, String nombre) {
  digitalWrite(pin, LOW);
  mostrarEstado(nombre + " ON");
}

void apagarTodos() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
