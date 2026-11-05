#include "challenges.h"
#include "movements.h"

void slapQuille() {
    movePulsesQuick(-15600, 0);
}

void yoinkGoblet() {
    int32_t rotate_pulses_left = 0;
    int32_t rotate_pulses_right = 0;
    int32_t reverse_pulses_left = 0;
    int32_t reverse_pulses_right = 0;
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
        delay(20);
        distance = analogRead(A7);
    }
    Serial.println("FOUND IT!");

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
        delay(20);
        distance = analogRead(A7);
    }
    Serial.println("FOUND IT!");

    Serial.print("Distance: ");
    Serial.println(distance);

    // Stop le tournage du robot
    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);
    delay(250);
    // Get le nombre de pulses depuis le start (The missile knows where it is at all times. It knows this because it knows where it isn't.)
    rotate_pulses_left = ENCODER_Read(LEFT);
    rotate_pulses_right = ENCODER_Read(RIGHT);
    // Les pulses d'en haut c le nombre qui va falloir faire dans l'autre sens. :O
    Serial.print("Left pulses: ");
    Serial.println(rotate_pulses_left);
    Serial.print("Right pulses: ");
    Serial.println(rotate_pulses_right);

    Serial.println("Rapprochage du verre!");
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
    MOTOR_SetSpeed(LEFT, -0.1);
    MOTOR_SetSpeed(RIGHT, -0.1);

    distance = 0;
    // On se rapproche du verre, pis quand tu pognes le 600, c que t'es proche en tabarnak (8-10 cm)
    while (distance < 600) {
        delay(10);
        distance = analogRead(A7);
    }
    Serial.println("On a pogner le peak!"); 
    Serial.print("Distance: ");
    Serial.println(distance);

    // On se COLLE sur le verre, pis quand tu pognes en bas de 400, c que t'es collee sur le verre (<5 cm)
    while (distance > 400) {
        distance = analogRead(A7);
    }
    Serial.println("Coller dessus!");

    Serial.print("Distance: ");
    Serial.println(distance);

    // Avec le reculage bonus, t pretty fucking sure que tu lui a foncer dedans
    Serial.println("Reculage bonus!");
    delay(250);

    // Stop le reculage
    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);
    delay(250);
    // Get le nombre de pulses depuis le start du avancage (The missile knows where it is at all times. It knows this because it knows where it isn't.)
    reverse_pulses_left = ENCODER_Read(LEFT);
    reverse_pulses_right = ENCODER_Read(RIGHT);
    Serial.print("Left pulses: ");
    Serial.println(reverse_pulses_left);
    Serial.print("Right pulses: ");
    Serial.println(reverse_pulses_right);
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);

    /* Yoink le verre*/
    pinceYoink();
    
    /* Retournage au centre */
    Serial.println("Retournage au centre!");
    movePulses(reverse_pulses_left * -1, reverse_pulses_right * -1);
    delay(100);

    //Serial.println("Retournage sur lui-meme!");
    movePulses(rotate_pulses_left * -1, rotate_pulses_right * -1);

    Serial.println("Stopage!");
    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);
}

void passeCoteMur() {
    turnLeft90();
    delay(250);
    movePulsesQuick(2000,2000);
    delay(250);
    turnRight90();
    delay(250);
    movePulsesQuick(4000,4000);
    delay(250);
    turnRight90();
    delay(250);
    movePulsesQuick(2000,2000);
    delay(250);
    turnLeft90();
    delay(250);
}