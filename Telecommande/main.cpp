/*/*=======================================================================================

Programmation pour la telecommande du projet pour l'epreuve du combattant
Auteur: Thomas Blanchard
Version 0.0.0 
Date: 2026-10-28

=========================================================================================*/
#include <LibRobus.h>
#include <Arduino.h>
#include <stdio.h>

/* Les constantes sont identifie ici */

#define FREQUENCE 38000
#define TEMPBIT1  0.002250
#define TEMPBIT0  0.001125
#define BOUTON1 30
#define BOUTON2 31
#define TRANSISTOR 41
#define ALIMENTATION 51
#define VOYANTCOMMANDE 53


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


  }

  if (digitalRead(BOUTON2))
  {

  }
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}








