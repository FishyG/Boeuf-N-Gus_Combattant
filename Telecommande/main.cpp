/*/*=======================================================================================

Programmation pour la telecommande du projet pour l'epreuve du combattant
Auteur: Thomas Blanchard
Version 0.0.0 
Date: 2026-10-28

=========================================================================================*/
#include <LibRobus.h>
#include <Arduino.h>
#include <stdio.h>
#include <IRremote.hpp>


/* Les constantes sont identifie ici */

#define FREQUENCE 38000
#define TEMPBIT1  0.002250
#define TEMPBIT0  0.001125
#define BOUTON1 30
#define BOUTON2 31
#define TRANSISTOR 41
#define ALIMENTATION 51
#define VOYANTCOMMANDE 53
#define COMAND1 0x34
#define COMAND2 0x35
#define ADDRESS 0x00

// put function declarations here:
int myFunction(int, int);

void setup()
{
  BoardInit();

  pinMode(BOUTON1, INPUT);
  pinMode(BOUTON2, INPUT);
  pinMode(TRANSISTOR, OUTPUT); 
  pinMode(ALIMENTATION, OUTPUT);
  pinMode(VOYANTCOMMANDE , OUTPUT);

}

void loop() 
{
  // put your main code here, to run repeatedly:;
  digitalWrite(ALIMENTATION, 1);
  if (digitalRead(BOUTON1))
  { 

    IrSender.begin(TRANSISTOR, LED_BUILTIN); // initialiser l'envoi IR avec le pin du transistor et la LED de feedback
    IrSender.sendNEC(ADDRESS, COMAND1, 0); // envoyer le signal de commande
    digitalWrite(VOYANTCOMMANDE, 1); 
    delay(1000); 
  }

  if (digitalRead(BOUTON2))
  {
    IrSender.begin(TRANSISTOR, LED_BUILTIN); // initialiser l'envoi IR avec le pin du transistor et la LED de feedback
    IrSender.sendNEC(ADDRESS, COMAND2, 0); // envoyer le signal de commande
    digitalWrite(VOYANTCOMMANDE, 1); 
    delay(1000); 
  }
  digitalWrite(VOYANTCOMMANDE, 0); 
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}


/* digitalWrite(TRANSISTOR, 1) pour le temps de debut;
delay de x seconde
digitalWrite(TRANSISTOR, 0);
delay de x seconde
ensuite des pulses pour envoyer le signal

une fois le signal envoyé :
 if digitalRead(Bouton1)
 ecrire la loop de commande
 fin */





