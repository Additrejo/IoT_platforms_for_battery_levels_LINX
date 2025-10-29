// Pines de los relés
const int rele1Pin = 25;
const int rele2Pin = 26;

void setup() {
  // Configuramos los pines de los relés como salidas
  pinMode(rele1Pin, OUTPUT);
  pinMode(rele2Pin, OUTPUT);

  // Inicializamos los relés apagados
  digitalWrite(rele1Pin, LOW);
  digitalWrite(rele2Pin, LOW);
}

void loop() {
  // Encender el relé 1 y apagar el relé 2
  digitalWrite(rele1Pin, HIGH);
  digitalWrite(rele2Pin, LOW);
  delay(20000); // Mantener por 20 segundos

  // Apagar el relé 1 y encender el relé 2
  digitalWrite(rele1Pin, LOW);
  digitalWrite(rele2Pin, HIGH);
  delay(20000); // Mantener por 20 segundos
}
