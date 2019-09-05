
/*
  Stepper Motor Control - one revolution

  This program drives a unipolar or bipolar stepper motor.
  The motor is attached to digital pins 8 - 11 of the Arduino.

  The motor should revolve one revolution in one direction, then
  one revolution in the other direction.
| ||||||||||||||||||||||||||||||| 

  Created 11 Mar. 2007
  Modified 30 Nov. 2009
  by Tom Igoe

*/

#include <Stepper.h>

const int stepsPerRevolution = 48;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 9, 10 , 11);

bool estadoAnterior = 0;
bool continuo = 0;

void setup() {
  // set the speed at 60 rpm:
  Serial.begin(9600);
  myStepper.setSpeed(600);
}

bool andando = false;
int velocidadActual = 600;


void serialEvent() {
  char reading = Serial.read();
  switch (reading)
  {
    case 'A':
      myStepper.step(-stepsPerRevolution * 5);
      break;
    case 'S':
      myStepper.step(stepsPerRevolution * 5);
      break;

    case 'C':
      continuo = !continuo;
      break;
    case '1':
      velocidadActual -= 100;
      Serial.print("Vel actual: ");
      Serial.println(velocidadActual);
      break;
    case '2':
      velocidadActual -= 10;
      Serial.print("Vel actual: ");
      Serial.println(velocidadActual);
      break;
    case '3':
      velocidadActual += 10;
      Serial.print("Vel actual: ");
      Serial.println(velocidadActual);
      break;
    case '4':
      velocidadActual += 100;
      Serial.print("Vel actual: ");
      Serial.println(velocidadActual);
      break;
  }

  myStepper.setSpeed(velocidadActual);
}

void loop()
{
  /*
  bool estadoActual = digitalRead(A0);
  if (estadoAnterior != estadoActual)
  {
    if (estadoActual)
    {
      Serial.println("Hoja en impresora");
    }
    else
    {
      Serial.println("Hoja terminada");

      if (andando)
      {
        if (continuo)
        {
          myStepper.step(stepsPerRevolution * 10);
          myStepper.step(-stepsPerRevolution * 5);
          
        }
      }
    }

    estadoAnterior = estadoActual;
  }

  if (andando)
  {
    myStepper.step(stepsPerRevolution);
  }
  else
  {
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
  }
 */
  
}
