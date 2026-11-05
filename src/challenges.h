#pragma once

#include <LibRobus.h> // Pour utiliser RobUS
#include "pince.h"

void slapQuille();
void yoinkGoblet();
void passeCoteMur();
void faireDodoWakeUpManette();
int lireCommandeIR();
bool challengeRunner(int couleur);
void allumerLED(int couleur);
void ledMiseAJour();   // eteint les DEL apres 10 s