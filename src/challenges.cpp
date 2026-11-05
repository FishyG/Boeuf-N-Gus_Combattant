#include <Arduino.h>
#include "challenges.h"
#include "movements.h"
#include "color.h"
#include "pince.h"

#define PIN_ROSE    42
#define PIN_BLEU    40
#define PIN_VERT    41
#define PIN_JAUNE   38
#define PIN_ORANGE  39

const uint8_t ADRESSE_ATTENDUE = 0x17;
const uint8_t CMD_START = 0x01;
const uint8_t CMD_SLEEP = 0x02;


void allumerLED(int couleur)
{
    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);
    switch(couleur)
    {
        case CARTE_ROUGE:
            digitalWrite(PIN_ROSE, LOW);
            delay(1000);
            digitalWrite(PIN_ROSE, HIGH);
            break;
        case CARTE_BLEU:
            digitalWrite(PIN_BLEU, LOW);
            delay(1000);
            digitalWrite(PIN_BLEU, HIGH);
            break;
        case CARTE_VERT:
            digitalWrite(PIN_VERT, LOW);
            delay(10000);
            digitalWrite(PIN_VERT, HIGH);
            break;
        case CARTE_JAUNE:
            digitalWrite(PIN_JAUNE, LOW);
            delay(10000);
            digitalWrite(PIN_JAUNE, HIGH);
            break;
        case CARTE_ORANGE:
            digitalWrite(PIN_ORANGE, LOW);
            delay(10000);
            digitalWrite(PIN_ORANGE, HIGH);
            break;
    }
}

// Lit la telecommande une fois. Retourne la commande valide, ou -1 si rien / invalide.
int lireCommandeIR() {
    uint32_t message = REMOTE_read();

    if (message == 0 || message == 0xFFFFFFFF) {
        return -1;
    }

    uint8_t adresse     = (message >> 24) & 0xFF;
    uint8_t adresseInv  = (message >> 16) & 0xFF;
    uint8_t commande    = (message >> 8) & 0xFF;
    uint8_t commandeInv = message & 0xFF;

    // Integrite du message
    if (adresse != (uint8_t)~adresseInv || commande != (uint8_t)~commandeInv) {
        Serial.println(" -> message non-integre");
        return -1;
    }

    // Bonne adresse?
    if (adresse != ADRESSE_ATTENDUE) {
        Serial.println(" -> mauvaise adresse");
        return -1;
    }

    return commande;
}

void slapQuille() {
    movePulsesQuick(-15600, 0);
}

void yoinkGoblet() {
    int32_t rotate_pulses_left = 0;
    int32_t rotate_pulses_right = 0;
    int32_t reverse_pulses_left = 0;
    int32_t reverse_pulses_right = 0;
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

// Va probablement falloir que vous modifier le nombre de pulses, jvais me coucher, nicker vos races 
void passeCoteMur() {
    turnRight90();
    delay(250);
    movePulsesQuick(7000,7000);
    delay(250);
    turnLeft90();
    delay(250);
    movePulsesQuick(10500,10500);
    delay(250);
    turnLeft90();
    delay(250);
    movePulsesQuick(7000,7000);
    delay(250);
    turnRight90();
    delay(250);
}

void faireDodoWakeUpManette(){

    // Arret complet, et le PI repart propre apres
    motor_left.speed = 0;
    motor_right.speed = 0;
    motor_left.target_pulses = 0;
    motor_right.target_pulses = 0;
    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);

    Serial.println("Dodo... en attente de CMD_SLEEP");

    // Attend le signal de reveil
    while (true) {
        int cmd = lireCommandeIR();

        if (cmd == CMD_SLEEP) {
            Serial.println("Awake le bot");
            for (int i = 0; i < 2; i++) {
                AX_BuzzerON(1000, 150);
                delay(300);
            }
            return;
        }

        delay(10);
    }
}

static bool pivoterJusquaLigne(bool gauche, unsigned long ms) {
    MOTOR_SetSpeed(LEFT,  gauche ? 0.0 : 0.4);
    MOTOR_SetSpeed(RIGHT, gauche ? 0.4 : 0.0);

    unsigned long start = millis();
    while (millis() - start < ms) {
        if (analogRead(A2) > 400) {
            MOTOR_SetSpeed(LEFT, 0);
            MOTOR_SetSpeed(RIGHT, 0);
            return true;
        }
    }
    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);
    return false;
}

void traverserCanyon() {
    movePulses(0,50);

    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
    moveDistance(0.95);

    for (int i = 0; i < 200; i++) {
        delay(10);
        SOFT_TIMER_Update();
    }

    // Plus de SOFT_TIMER_Update() a partir d'ici : sinon le PI ecrase les MOTOR_SetSpeed
    motor_left.speed = 0;
    motor_right.speed = 0;
    motor_left.target_pulses = 0;
    motor_right.target_pulses = 0;
    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);

    // Balayage qui s'elargit : droite 100 ms, gauche 200 ms, droite 300 ms, ...
    bool trouve = (analogRead(A2) > 400);
    bool gauche = false;
    unsigned long duree = 100;

    while (!trouve && duree <= 1500) {
        trouve = pivoterJusquaLigne(gauche, duree);
        gauche = !gauche;
        duree += 100;
        delay(50);
    }

    if (!trouve) {
        Serial.println("Canyon: ligne pas retrouvee!");
    }
}

static uint8_t epreuvesFaites = 0;  // bit n = carte n deja faite
#define TOUTES_FAITES ((1 << CARTE_ROUGE) | (1 << CARTE_VERT) | (1 << CARTE_BLEU) | (1 << CARTE_JAUNE) | (1 << CARTE_ORANGE))

bool toutEstFait() {
    return (epreuvesFaites & TOUTES_FAITES) == TOUTES_FAITES;
}

bool challengeRunner(int couleur) {
    if (couleur < CARTE_ROUGE || couleur > CARTE_ORANGE) return false;
    if (epreuvesFaites & (1 << couleur)) return false;  // deja faite, on ignore

    epreuvesFaites |= (1 << couleur);  // marque AVANT de lancer
    
    allumerLED(couleur);

    switch (couleur) {
        case CARTE_ROUGE:
            slapQuille();
            break;
        case CARTE_VERT:
            traverserCanyon();
            break;
        case CARTE_BLEU:
            faireDodoWakeUpManette();
            break;
        case CARTE_JAUNE:
            passeCoteMur();
            break;
        case CARTE_ORANGE:
            yoinkGoblet();
            break;
    }

    if (toutEstFait()) {
        pinceDrop();
        while (1) {
            delay(10);
        }
    }
    return true;
}