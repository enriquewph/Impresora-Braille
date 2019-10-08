#include <A4988.h>
#define SENS_HOJA_PIN 35
#define LED1 24
#define LED2 23
#define HOJA_CARGADA digitalRead(SENS_HOJA_PIN)

A4988 stepper(48, 14, 25);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  stepper.begin(400, 1);
  pinMode(SENS_HOJA_PIN, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  cargarHoja();
  Serial.println("SENS_HOJA: " + String(HOJA_CARGADA));
  digitalWrite(LED1, 0);
  digitalWrite(LED2, 0);
}


void cargarHoja()
{
  digitalWrite(LED2, HIGH);
  int cuenta = 0;
  Serial.println("SENS_HOJA: " + String(HOJA_CARGADA));

  //activar mecanismo
  stepper.move(20);
  delay(10);

  while (HOJA_CARGADA);
  {
    //esperar a cargar la hoja
    cuenta++;
  }
  stepper.stop();

  Serial.println("SENS_HOJA: " + String(HOJA_CARGADA));
  Serial.println("CUENTA: " + String(cuenta));
  digitalWrite(LED2, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available())
  {
    String serialInput = Serial.readStringUntil('\n');
    int pasos = -serialInput.toInt();
    Serial.println("Moviendo: " + String(pasos) + " Pasos");
    stepper.move(pasos);
  }
  delay(500);
  digitalWrite(LED1, 0);
  digitalWrite(LED2, 1);
  delay(500);
  digitalWrite(LED1, 1);
  digitalWrite(LED2, 0);
}
