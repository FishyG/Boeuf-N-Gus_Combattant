/*
Projet: main.cpp robo swag
Equipe: P2
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

#define PIN_ROSE    42
#define PIN_BLEU    40
#define PIN_VERTE   41
#define PIN_JAUNE   38
#define PIN_ORANGE  39

bool cherche_droit;
bool apres_canyon;

const uint8_t ADRESSE_ATTENDUE = 0x17;
const uint8_t CMD_START = 0x01;
const uint8_t CMD_SLEEP = 0x02;

void setup() {
    BoardInit(); // Balls XD
    couleurInit(); // DEVINE IL FAIT KOI SMART ASS
    movementsInit(); // Start mon pti timer and shit
    pinceIdle(); // Set le servo droit

    pinMode(pin_rouge, INPUT);
    pinMode(pin_vert, INPUT);

    pinMode(PIN_JAUNE, OUTPUT);
    digitalWrite(PIN_JAUNE, HIGH);

    pinMode(PIN_ORANGE, OUTPUT);
    digitalWrite(PIN_ORANGE, HIGH);

    pinMode(PIN_BLEU, OUTPUT);
    digitalWrite(PIN_BLEU, HIGH);

    pinMode(PIN_VERTE, OUTPUT);
    digitalWrite(PIN_VERTE, HIGH);

    pinMode(PIN_ROSE, OUTPUT);
    digitalWrite(PIN_ROSE, HIGH);

    delay(500);

    while (1) {
        Serial.println("Waiting...");
        while ((!ROBUS_IsBumper(FRONT)) && (!ROBUS_IsBumper(REAR)) && (!ROBUS_IsBumper(LEFT)) && (!ROBUS_IsBumper(RIGHT))) {
            delay(10);
        }
        Serial.println("Fuck le mur");
        delay(1000);
        while (1) {
            uint8_t r, g, b;
            readFuckingRGB(r, g, b);
            float h = rgbToHueBombaclat(r, g, b);
            float s = rgbToSaturationPUSSYWAGAN(r, g, b);

            Serial.print("R: ");
            Serial.print(r);
            Serial.print(", G: ");
            Serial.print(g);
            Serial.print(", B: ");
            Serial.print(b);
            Serial.print(", Hue ");
            Serial.print(h);
            Serial.print(", Sat ");
            Serial.println(s);
            delay(10);
        }
    }

        Serial.println("BITCHASDD");
        while (true) {
            int cmd = lireCommandeIR();

            if (cmd == CMD_START) {
                Serial.println("Awake le bot");
                for (int i = 0; i < 3; i++) {
                    AX_BuzzerON(1000, 150);
                    delay(300);
                }
                return;
            }

            delay(10);
        }
    }

void loop() {
    ledMiseAJour();

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