#include "challenges.h"

void slapQuille() {
    int32_t left_pulses = 0;
    ENCODER_Reset(LEFT);
    MOTOR_SetSpeed(LEFT, 0.3);
    while (left_pulses < 10000) {
        left_pulses = ENCODER_Read(LEFT);
    }
    MOTOR_SetSpeed(LEFT, 0.2);
    while (left_pulses < 15670) {
        left_pulses = ENCODER_Read(LEFT);
    }
    MOTOR_SetSpeed(LEFT, 0);
}

void Goblet() {
    int Pulse_tourne_LEFT = 0;
    int Pulse_tourne_RIGHT = 0;
    int Pulse_recule_LEFT = 0;
    int Pulse_recule_RIGHT = 0;
    int Pulse_retourne_LEFT = 0;
    int Pulse_retourne_RIGHT = 0;
    int Pulse_avance_LEFT = 0;
    int Pulse_avance_RIGHT = 0;
    int distance = 0;

    Serial.println("Cherchage!");
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
    MOTOR_SetSpeed(LEFT, -0.1);
    MOTOR_SetSpeed(RIGHT, 0.1);

    while (distance < 300) {
        Pulse_tourne_LEFT = ENCODER_Read(LEFT);
        Pulse_tourne_RIGHT = ENCODER_Read(RIGHT);
        distance = analogRead(A7);
        delay(20);
    }
    Serial.print("Distance: ");
    Serial.println(distance);

    Serial.println("Stopage!");
    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);
    delay(250);

    Serial.println("Remettage sur l'alignage!");
    distance = 0;
    MOTOR_SetSpeed(LEFT, 0.05);
    MOTOR_SetSpeed(RIGHT, -0.05);
    while (distance < 300) {
        Pulse_tourne_LEFT = ENCODER_Read(LEFT);
        Pulse_tourne_RIGHT = ENCODER_Read(RIGHT);
        distance = analogRead(A7);
        delay(20);
    }
    Serial.print("Distance: ");
    Serial.println(distance);

    Serial.println("Reculage pour setupage!");
    distance = 0;
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
    MOTOR_SetSpeed(LEFT, -0.1);
    MOTOR_SetSpeed(RIGHT, -0.1);

    while (distance < 600) {
        Pulse_recule_LEFT = ENCODER_Read(LEFT);
        Pulse_recule_RIGHT = ENCODER_Read(RIGHT);
        distance = analogRead(A7);
    }
    Serial.print("Distance: ");
    Serial.println(distance);

    while (distance > 540) {
        Pulse_recule_LEFT = ENCODER_Read(LEFT);
        Pulse_recule_RIGHT = ENCODER_Read(RIGHT);
        distance = analogRead(A7);
    }
    Serial.print("Distance: ");
    Serial.println(distance);

    /*recule_difference*/
    Serial.println("Reculage diff!");
    MOTOR_SetSpeed(LEFT, -0.1);
    MOTOR_SetSpeed(RIGHT, -0.1);
    delay(250);
    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);

    /*fermer_Pince*/
    Serial.println("Calling pognage!");
    delay(1000);
    pinceYoink();

    /*avance_difference*/
    Serial.println("Avancage diff!");
    MOTOR_SetSpeed(LEFT, 0.1);
    MOTOR_SetSpeed(RIGHT, 0.1);
    delay(500);
    MOTOR_SetSpeed(LEFT, 0.2);
    MOTOR_SetSpeed(RIGHT, 0.2);
    delay(750);

    Serial.println("Retoutnage!");
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
    MOTOR_SetSpeed(LEFT, 0.1);
    MOTOR_SetSpeed(RIGHT, 0.1);

    Serial.println("Idk dawg!");
    while (Pulse_avance_LEFT < Pulse_recule_LEFT && Pulse_avance_RIGHT < Pulse_recule_RIGHT) {
        Pulse_avance_LEFT = ENCODER_Read(LEFT);
        Pulse_avance_RIGHT = ENCODER_Read(RIGHT);
    }

    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
    MOTOR_SetSpeed(LEFT, -0.2);
    MOTOR_SetSpeed(RIGHT, 0.2);

    while (Pulse_retourne_LEFT < Pulse_tourne_LEFT && Pulse_retourne_RIGHT < Pulse_tourne_RIGHT) {
        Pulse_retourne_LEFT = ENCODER_Read(LEFT);
        Pulse_retourne_RIGHT = ENCODER_Read(RIGHT);
    }

    Serial.println("Stopage!");
    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);
}