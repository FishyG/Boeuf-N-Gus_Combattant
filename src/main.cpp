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

#define pin_rouge 49
#define pin_vert 48

void setup() {
    // Balls XD
    BoardInit();
    // Start mon pti timer and shit
    movementsInit();
    // Set le servo droit
    pinceIdle();

    pinMode(pin_rouge, INPUT);
    pinMode(pin_vert, INPUT);

    delay(10);

    while (1) {
        Serial.println("Waiting...");
        while ((!ROBUS_IsBumper(FRONT)) && (!ROBUS_IsBumper(REAR)) && (!ROBUS_IsBumper(LEFT)) && (!ROBUS_IsBumper(RIGHT))) {
            delay(10);
        }
        while (0) {
            Serial.println("Turning");
            Serial.println(ENCODER_Read(RIGHT));
            delay(100);
            movePulses(500, -500);
            delay(1000);

            Serial.println("Waiting...");
            while ((!ROBUS_IsBumper(FRONT)) && (!ROBUS_IsBumper(REAR)) && (!ROBUS_IsBumper(LEFT)) && (!ROBUS_IsBumper(RIGHT))) {
                delay(10);
            }
        }

        Goblet();

        Serial.println("Calling dropage!");
        delay(1000);
        pinceDrop();

        Serial.println("Calling ressettage!");
        delay(2000);
        pinceIdle();
        delay(1000);
    }
}

void loop() {
    delay(10);
    SOFT_TIMER_Update();
}

// Te donne 0 si fuckall 1 si le rouge est a on, 2 si verte, 3 si les deux
int detection_mur() {
    int total = 0;
    if (digitalRead(pin_rouge) == LOW) {
        total = total + 1;
    }
    if (digitalRead(pin_vert) == LOW) {
        total = total + 2;
    }
    return total;
}