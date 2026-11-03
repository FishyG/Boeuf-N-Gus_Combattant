//coucou
// Les moteur_v et moteur_h doivent etre des INDEX de moteur (0 ou 1)

// Activation des moteurs
void activation_servo(moteur_v, moteur_h) 
{
    int moteur_h, moteur_v; // L'index des moteurs (0 ou 1)
  // Fonction de la librairie ROBUS
    SERVO_Enable(moteur_1);
    SERVO_Enable(moteur_2);
}

// Fonction pour la fermeture de la pince
void fermeture_pince(moteur_h) 
{
  int moteur_h; // L'index du moteur horizontal (0 ou 1)
  // Angle lorsque la pince est ouverte
  int angle = 180;
  //Fermeture graduelle de la pince
  for (int i = 180; i >= 0; i -= 10) {
    SERVO_SetAngle(moteur_h, i);
    delay(30);
  }
}

// Fonction pour l'ouverture de la pince
void ouverture_pince(moteur_h) 
{
  int moteur_h; // L'index du moteur horizontal (0 ou 1)
  // Angle lorsque la pince est fermée
  int angle = 0;
  //Ouverture graduelle de la pince
  for (int i = 0; i <= 180; i += 10) {
    SERVO_SetAngle(moteur_h, i);
    delay(30);
  }
}

// Fonction pour le mouvement vertical du bras ( vers le haut )
void mouvement_vertical_haut(moteur_v) 
{
  // Angle lorsque le bras est en bas
  int angle = 0;
  // Mouvement graduelle du bras vers le haut
  for (int i = 0; i <= 180; i += 10) {
    SERVO_SetAngle(moteur_v, i);
    delay(30);
  }
}

// Fonction pour le mouvement vertical du bras ( vers le bas )
void mouvement_vertical_bas(moteur_v) 
{
  // Angle lorsque le bras est en haut
  int angle = 180;
  // Mouvement graduelle du bras vers le bas
  for (int i = 180; i >= 0; i -= 10) {
    SERVO_SetAngle(moteur_v, i);
    delay(30);
  }
}

//Fonction pour désactiver les servomoteurs
void desactivation_servo(moteur_v, moteur_h) 
{
  // Fonction de la librairie ROBUS
    SERVO_Disable(moteur_v);
    SERVO_Disable(moteur_h);
}