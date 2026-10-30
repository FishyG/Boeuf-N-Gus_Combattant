/*
Projet: Boeuf-N-Gus_Combattant_Telecommande
Equipe: P23
Auteurs:  
          Thomas Blanchard
          Xavier Dupuis  
          Christian Tamba Tene
Description: Code de la telecommande NEC pour activer le robot
Date: 2026-10-30
*/

#define SEND_NEC

#include <LibRobus.h>
#include <Arduino.h>


const int LED_ROUGE = 22;
const int LED_VERTE = 24;
const int BOUTON_START = 30;
const int BOUTON_SLEEP = 31;

IRsend irsend;
int etat_precedent;

uint32_t messageNEC(uint8_t adresse, uint8_t commande)
{
  return ((uint32_t)adresse << 24)
       | ((uint32_t)(uint8_t)~adresse << 16)
       | ((uint32_t)commande << 8)
       | (uint8_t)~commande;
}

void envoyer_start()
{
  uint32_t message = messageNEC(0x17, 0x01);
  digitalWrite(LED_VERTE, LOW);
  irsend.sendNEC(message, 32);
  digitalWrite(LED_VERTE, HIGH);
  Serial.print("Envoyé : 0x");
  Serial.println(message, HEX);
  delay(500);
}


void setup()
{
  BoardInit();

  digitalWrite(LED_VERTE, HIGH);
  pinMode(LED_VERTE, OUTPUT);

  pinMode(LED_ROUGE, OUTPUT);
  digitalWrite(LED_ROUGE, LOW);

  pinMode(BOUTON_START, INPUT);
  etat_precedent = HIGH;
}

void loop() 
{
  // TODO: Ajuster le code pour prendre en compte les 2 boutons
  int etat_actuel = digitalRead(BOUTON_START);

  if (etat_actuel == LOW && etat_precedent == HIGH) {
    envoyer_start();
  }

  etat_precedent = etat_actuel;
}
