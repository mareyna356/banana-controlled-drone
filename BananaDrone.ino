#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

Adafruit_MPR121 cap = Adafruit_MPR121();

// Recuerda los pins tocados para saber cuando se sueltan
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

byte throttlePin = 11; //Arriba-Abajo PWM
byte yawPin = 10; //Gira izquierda-derecha PWM
byte rollPin = 9; //Izquiera-derecha PWM
byte pitchPin = 3; //Adelante-atrás PWM

bool truco1 = false; //zig-zag horizontal
bool truco1dir = false; //dirección del zig-zag
                        //f = izquierda, t = derecha
bool truco2 = false; //zig-zag vertical
bool truco2dir = false; //dirección del zig-zag
                        //f = abajo, t = arriba
bool truco3 = false; //Vuelo en 8
byte truco3pasos = 0;
bool truco4 = false; //Vuelo en círculo
byte truco4pasos = 0;

//Constantes de señales analógicas
//(simula movimientos del joystick)
byte alto = 150; //Joystick en alto
byte medioThrottle = 65; //Joysticks en medio
byte medioYaw = 80;
byte medioRoll = 85;
byte medioPitch = 70;
byte bajo = 22; //Joystick en bajo

unsigned long tiempo;

void setup() {
  Serial.begin(9600);

  while (!Serial) {
    delay(10);
  }

  pinMode(throttlePin, OUTPUT);
  pinMode(yawPin, OUTPUT);
  pinMode(rollPin, OUTPUT);
  pinMode(pitchPin, OUTPUT);

  analogWrite(throttlePin, medioThrottle);
  analogWrite(yawPin, medioYaw);
  analogWrite(rollPin, medioRoll);
  analogWrite(pitchPin, medioPitch);

  // Dirección I2C por defecto del MPR121 es 0x5A
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 no detectado\n");
    while (1);
  }
  Serial.println("Start");
}

void loop() {
  // Consigue los pins que están siendo tocados
  currtouched = cap.touched();

  // El MPR121 tiene 12 pins
  for (uint8_t i = 0; i < 12; i++) {
    // Si está siendo tocado y antes no lo estaba, if es verdad
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      Serial.print("Presionaste: ");
      Serial.println(i);
      
      //Desactiva todos los trucos cuando se presiona otra banana
      truco1 = false;
      truco1dir = false;
      truco2 = false;
      truco2dir = false;
      truco3 = false;
      truco3pasos = 0;
      truco4 = false;
      truco4pasos = 0;
      
      switch (i) {
        case 0:
          analogWrite(throttlePin, alto); //Arriba
          break;
        case 1:
          analogWrite(throttlePin, bajo); //Abajo
          break;
        case 2:
          analogWrite(yawPin, alto); //Gira derecha
          break;
        case 3:
          analogWrite(yawPin, bajo); //Gira izquierda
          break;
        case 4:
          analogWrite(rollPin, alto); //Derecha
          break;
        case 5:
          analogWrite(rollPin, bajo); //Izquierda
          break;
        case 6:
          analogWrite(pitchPin, alto); //Adelante
          break;
        case 7:
          analogWrite(pitchPin, bajo); //Atras
          break;
        case 8:
          truco1 = true;
          analogWrite(rollPin, alto); //Derecha
          tiempo = millis();
          break;
        case 9:
          truco2 = true;
          analogWrite(throttlePin, alto); //Arriba
          tiempo = millis();
          break;
        case 10:
          truco3 = true;
          analogWrite(rollPin, alto); //Derecha
          tiempo = millis()-150;
          break;
        case 11:
          truco4 = true;
          analogWrite(rollPin, alto); //Derecha
          tiempo = millis()-150;
          break;
      }
    }
    // Si estaba siendo tocado pero ahora no lo está, if es verdad
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      Serial.print("Soltaste: ");
      Serial.println(i);
      switch (i) {
        case 0:
        case 1:
          analogWrite(throttlePin, medioThrottle);
          break;
        case 2:
        case 3:
          analogWrite(yawPin, medioYaw);
          break;
        case 4:
        case 5:
          analogWrite(rollPin, medioRoll);
          break;
        case 6:
        case 7:
          analogWrite(pitchPin, medioPitch);
          break;
        case 8:
          truco1 = false;
          truco1dir = false;
          analogWrite(rollPin, medioRoll);
          break;
        case 9:
          truco2 = false;
          truco2dir = false;
          analogWrite(throttlePin, medioThrottle);
          break;
        case 10:
          truco3 = false;
          truco3pasos = 0;
          analogWrite(throttlePin, medioThrottle);
          analogWrite(rollPin, medioRoll);
          break;
        case 11:
          truco4 = false;
          truco4pasos = 0;
          analogWrite(throttlePin, medioThrottle);
          analogWrite(rollPin, medioRoll);
          break;
      }
    }
  }

  if (truco1 && millis()-tiempo > 800) {
    if (truco1dir) {
      analogWrite(rollPin, alto); //Derecha
      truco1dir = false;
    } else {
      analogWrite(rollPin, bajo); //Izquierda
      truco1dir = true;
    }
    tiempo = millis();
  }

  if (truco2 && millis()-tiempo > 1000) {
    if (truco2dir) {
      analogWrite(throttlePin, alto); //Arriba
      truco2dir = false;
    } else {
      analogWrite(throttlePin, bajo); //Abajo
      truco2dir = true;
    }
    tiempo = millis();
  }

  if (truco3) {
    switch (truco3pasos) {
      case 0:
        if (millis()-tiempo > 300) {
          analogWrite(throttlePin, alto);
          tiempo = millis();
          truco3pasos++;
        }
        break;
      case 1:
        if (millis()-tiempo > 300) {
          analogWrite(rollPin, medioRoll);
          tiempo = millis();
          truco3pasos++;
        }
        break;
      case 2:
        if (millis()-tiempo > 300) {
          analogWrite(rollPin, bajo);
          tiempo = millis();
          truco3pasos++;
        }
        break;
      case 3:
        if (millis()-tiempo > 600) {
          analogWrite(rollPin, medioRoll);
          tiempo = millis();
          truco3pasos++;
        }
        break;
      case 4:
        if (millis()-tiempo > 300) {
          analogWrite(rollPin, alto);
          tiempo = millis();
          truco3pasos++;
        }
        break;
      case 5:
        if (millis()-tiempo > 300) {
          analogWrite(throttlePin, medioThrottle);
          tiempo = millis();
          truco3pasos++;
        }
        break;
      case 6:
        if (millis()-tiempo > 300) {
          analogWrite(throttlePin, bajo);
          tiempo = millis();
          truco3pasos++;
        }
        break;
      case 7:
        if (millis()-tiempo > 300) {
          analogWrite(rollPin, medioRoll);
          tiempo = millis();
          truco3pasos++;
        }
        break;
      case 8:
        if (millis()-tiempo > 300) {
          analogWrite(rollPin, bajo);
          tiempo = millis();
          truco3pasos++;
        }
        break;
      case 9:
        if (millis()-tiempo > 600) {
          analogWrite(rollPin, medioRoll);
          tiempo = millis();
          truco3pasos++;
        }
        break;
      case 10:
        if (millis()-tiempo > 300) {
          analogWrite(rollPin, alto);
          tiempo = millis();
          truco3pasos++;
        }
        break;
      case 11:
        if (millis()-tiempo > 300) {
          analogWrite(throttlePin, medioThrottle);
          tiempo = millis();
          truco3pasos = 0;
        }
        break;
    }
  }

  if (truco4) {
    switch (truco4pasos) {
      case 0:
        if (millis()-tiempo > 300) {
          analogWrite(throttlePin, alto); //Arriba
          tiempo = millis();
          truco4pasos++;
        }
        break;
      case 1:
        if (millis()-tiempo > 300) {
          analogWrite(rollPin, medioRoll); //Stick estático
          tiempo = millis();
          truco4pasos++;
        }
        break;
      case 2:
        if (millis()-tiempo > 300) {
          analogWrite(rollPin, bajo); //Izquierda
          tiempo = millis();
          truco4pasos++;
        }
        break;
      case 3:
        if (millis()-tiempo > 300) {
          analogWrite(throttlePin, medioThrottle); //Stick estático
          tiempo = millis();
          truco4pasos++;
        }
        break;
      case 4:
        if (millis()-tiempo > 300) {
          analogWrite(throttlePin, bajo); //Abajo
          tiempo = millis();
          truco4pasos++;
        }
        break;
      case 5:
        if (millis()-tiempo > 300) {
          analogWrite(rollPin, medioRoll); //Stick estático
          tiempo = millis();
          truco4pasos++;
        }
        break;
      case 6:
        if (millis()-tiempo > 300) {
          analogWrite(rollPin, alto); //Derecha
          tiempo = millis();
          truco4pasos++;
        }
        break;
      case 7:
        if (millis()-tiempo > 300) {
          analogWrite(throttlePin, medioThrottle); //Stick estático
          tiempo = millis();
          truco4pasos = 0;
        }
        break;
    }
  }

  // Guarda el estado
  lasttouched = currtouched;
}
