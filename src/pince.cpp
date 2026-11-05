#include "pince.h"

// Ca marche zoubi >w< <3
void pinceYoink() {
  Serial.println("Pognage!");
  SERVO_Enable(0);
  SERVO_SetAngle(0,178); // Ferme la pince
  delay(750);
  SERVO_Enable(1);
  SERVO_SetAngle(1,0); // Leve la réguine
  delay(200);
}

void pinceDrop() {
  Serial.println("Dropage!");
  SERVO_Enable(1);
  SERVO_SetAngle(1, 20); // Baisse la réguine
  delay(500);
  SERVO_Enable(0);
  SERVO_SetAngle(0, 35); // Ouvre la pince
  delay(100);
}

void pinceIdle() {
  Serial.println("Idleage!");
  SERVO_Enable(1);
  SERVO_SetAngle(1, 20); // Baisse la réguine
  SERVO_Enable(0);
  SERVO_SetAngle(0, 35); // Ouvre la pince
  delay(250);
}