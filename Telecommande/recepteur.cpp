/*
Projet: Boeuf-N-Gus_Combattant_Recepteur
Equipe: P23
Auteurs:  
          Thomas Blanchard
          Xavier Dupuis  
          Christian Tamba Tene
Description: Code du recepteur NEC. Code incomplet. Il doit etre ajoute dans le code main
Date: 2026-10-30
*/

// LIBRAIRIES
#include <LibRobus.h>
#include <Arduino.h>

// CONSTANTES
const uint8_t ADRESSE_ATTENDUE = 0x17;
const uint8_t CMD_START = 0x01;

void setup()
{
  BoardInit();
}

void loop() 
{
  uint32_t message = REMOTE_read();

  if (message == 0 || message == 0xFFFFFFFF) {
    return;
  }

  Serial.print(message);

  uint8_t adresse = (message >> 24) & 0xFF;
  uint8_t adresseInv = (message >> 16) & 0xFF;
  uint8_t commande = (message >> 8) & 0xFF;
  uint8_t commandeInv = message & 0xFF;


  // integrite message
  if (adresse == (uint8_t)~adresseInv && commande == (uint8_t)~commandeInv) 
  {
    // Verif adresse
    if (adresse == ADRESSE_ATTENDUE) {
      // On met les choix de commandes ici
      Serial.print(" -> Commande: ");
      Serial.print(commande);
      Serial.println(" reçue");
      // 3 buzz pour le start
      // TODO: ajouter un switch pour les 2 commandes que le robot peut recevoir
      if (commande == CMD_START) {
        for (int i = 0; i < 3; i++) {
          AX_BuzzerON(1000, 150);
          delay(300);
        }
      }
    }
    else
    {
      Serial.println(" -> mauvaise adresse");
    }
  }
  else
  {
    Serial.println(" -> message non-integre");
  }
}
