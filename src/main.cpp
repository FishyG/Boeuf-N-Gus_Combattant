/*
Projet: main.cpp robo swag
Equipe: P23
Auteurs: Jessy, RAJOUTER VOS CNOMS ICI
Description: Code epreuve du combattant
Date: Aujourd'hui lmao
*/

/* ****************************************************************************
Inclure les librairies de functions que vous voulez utiliser
**************************************************************************** */

#include <LibRobus.h>  // Pour utiliser RobUS

#include "challenges.h"
#include "pince.h"
#include "movements.h"
#include "color.h"

#define pin_rouge 49
#define pin_vert 48

bool cherche_droit;
bool apres_canyon;

void setup() {
    BoardInit(); // Balls XD
    couleurInit(); // DEVINE IL FAIT KOI SMART ASS
    movementsInit(); // Start mon pti timer and shit
    pinceIdle(); // Set le servo droit

    pinMode(pin_rouge, INPUT);
    pinMode(pin_vert, INPUT);

    delay(500);

    while (((!ROBUS_IsBumper(FRONT)) && (!ROBUS_IsBumper(REAR)) && (!ROBUS_IsBumper(LEFT)) && (!ROBUS_IsBumper(RIGHT)))) {
        delay(10);
    }
}

void loop() {
    int u1 = analogRead(A1);
    int u2 = analogRead(A2);
    int u3 = analogRead(A3);

    int couleur = couleurScan();
    if (couleur) {
        challengeRunner(couleur);
    }

    if (u2 > 400) {
        MOTOR_SetSpeed(LEFT, 0.2);
        MOTOR_SetSpeed(RIGHT, 0.2);
    } else if (u1 > 400) {
        MOTOR_SetSpeed(LEFT, 0.1);
        MOTOR_SetSpeed(RIGHT, 0.4);
    } else if (u3 > 400) {
        MOTOR_SetSpeed(LEFT, 0.4);
        MOTOR_SetSpeed(RIGHT, 0.1);
    }
}