#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

// Configuración de la pantalla LCD 20x4
LiquidCrystal_PCF8574 lcd(0x27);

// Pines de los botones
const int boton1 = 32;
const int boton2 = 33;
const int boton3 = 34;

// Variables de estado
int menu = 0;

void setup() {
  // Inicializar pantalla LCD
  lcd.begin(20, 4);
  lcd.setBacklight(1);

  // Configurar pines de botones
  pinMode(boton1, INPUT_PULLUP);
  pinMode(boton2, INPUT_PULLUP);
  pinMode(boton3, INPUT_PULLUP);

  // Mostrar pantalla inicial
  mostrarMenuPrincipal();
}

void loop() {
  if (digitalRead(boton1) == LOW) {
    delay(200);  // Antirrebote
    if (menu == 0) {
      menu = 1;
      mostrarMenuCargar();
    } else if (menu == 1) {
      menu = 2;
      mostrarOpciones();
    } else if (menu == 2) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cargar bateria");
      delay(1000);
      mostrarMenuCargar();
    }
  } else if (digitalRead(boton2) == LOW && menu == 2) {
    delay(200);  // Antirrebote
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ciclos");
    delay(1000);
    mostrarMenuCargar();
  } else if (digitalRead(boton3) == LOW) {
    delay(200);  // Antirrebote
    if (menu == 2) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cargar bateria");
      delay(1000);
      menu = 0;
      mostrarMenuPrincipal();
    }
  }
}

void mostrarMenuPrincipal() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Linx Battery Tester V1.4");
  lcd.setCursor(0, 1);
  lcd.print("Bateria: Li-On");
  lcd.setCursor(0, 2);
  lcd.print("V: 3.85V B: 80.5%");
  lcd.setCursor(0, 3);
  lcd.print("->Entrar");
}

void mostrarMenuCargar() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Opciones");
  lcd.setCursor(0, 1);
  lcd.print("-> 1 Cargar bateria");
  lcd.setCursor(0, 2);
  lcd.print("-> 2 Ciclos de carga");
  lcd.setCursor(0, 3);
  lcd.print("-> 3 Salir");
}

void mostrarOpciones() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Opciones:");
  lcd.setCursor(0, 1);
  lcd.print("-> Cargar bateria");
  lcd.setCursor(0, 2);
  lcd.print("-> Iniciar Ciclos de carga");
  lcd.setCursor(0, 3);
  lcd.print("-> Atras");
}

