/*
Projet: Beuf-N-GUS Epreuve Combattant 
Equipe: P23
Auteurs: Karolann Mauger
Description: Combattant
Date: Aujourd'hui
*/
#include <LibRobus.h>



#define PULSES_LEFT 1700
#define PULSES_RIGHT 1700
#define pin_rouge 49
#define pin_vert 48
#define distance 0.495
#define distance_1m 0.98


#define MUR_SANS_JAUNE   0        
#define MUR_DETECTE      LOW      
#define MUR_COTE         500      // mm, on s'eloigne de la ligne (apres le virage a droite)
#define MUR_LONGER       700      // mm, on depasse le mur (apres le 1er virage a gauche)
#define MUR_RETOUR       500      // mm, on revient sur la ligne (apres le 2e virage a gauche)
#define CANYON           590      // mm, traversee du canyon
#define VITESSE_CHERCHE  0.2


// CONSTANTES
const uint8_t ADRESSE_ATTENDUE = 0x17;
const uint8_t CMD_START = 0x01;
const uint8_t CMD_SLEEP = 0x02;

bool cherche_droit;
bool apres_canyon;
bool canyon_actif = 0;     
bool mur_actif = 0;        

// ---- Doivent etre pareils que dans color.cpp ----
#define AUCUNE        -1
#define JAUNE          2
#define ORANGE         3
#define BLEU           4
#define VERT           5
#define ROSE           6
#define NB_COULEURS    7

#define ARRET_COULEUR  3000      // ms, temps d'arret sur une couleur (en attendant les epreuves)

// ---- Viennent de color.cpp ----
extern char NOMS[NB_COULEURS][10];

bool couleurInit();
int  couleurNouvelle();          // allume aussi la DEL 10 s et ne donne chaque couleur qu'une fois
void couleurNouvelEssai();
void delMiseAJour();             // eteint les DEL apres 10 s
void attendre(unsigned long ms); // comme delay(), mais les DEL s'eteignent quand meme a 10 s

// ---- Etats du robot ----
#define Attente_depart   0
#define Suivre_ligne     1
#define Epreuve_jaune    2
#define Epreuve_bleu     3
#define Epreuve_orange   4
#define Epreuve_rose     5
#define Epreuve_vert     6
#define fin              7
#define Attente_bleu     8    // arrete sur le bleu, attend la telecommande (CMD_SLEEP)

int Etat_actuel;
int bouton2 = 0;

//. les defs de functions, ils sont ici
bool detection_mur();
void suivreLigne();
void arretCouleur();
void bip();
void avancerJusquaLigne();
void contournerMur();

// ---- Viennent de suiveur_ligne.cpp (deplacements avec PID) ----
void deplacement(int distance_mm); // avance de distance_mm mm
void tourne_droit();              // 90 degres a droite
void tourne_gauche();             // 90 degres a gauche
void demi_tour();
void avancer();                   // avance de 500 mm
void arret();

void setup() {
  BoardInit();

  while (!couleurInit())
    delay(1000);
  couleurNouvelEssai();

  pinMode(pin_rouge, INPUT);
  pinMode(pin_vert, INPUT);

  ENCODER_Reset(LEFT);
  ENCODER_Reset(RIGHT);

  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT, 0);

  cherche_droit = 1;
  apres_canyon = 0;

  delay(10);

  mur_actif = MUR_SANS_JAUNE;
  Etat_actuel = Attente_depart;
  Serial.println("Waiting...");
}


void wait(uint8_t commande_attendue) {
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
      if(commande == commande_attendue){
        switch (commande)
        {
        case CMD_START:
        for(int i = 0; i < 3; i++) {
        AX_BuzzerON(1000, 150);
        delay(300);
      }
        break;
        case CMD_SLEEP:
          for(int i = 0; i < 2; i++){
        AX_BuzzerON(1000, 150);
        delay(300);
      }
          break;
        default:
          break;
        }
        Etat_actuel = Suivre_ligne;
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



void loop() {
	int couleur;

	delMiseAJour();

	switch (Etat_actuel) {

		case Attente_depart:
			//fonction attente IR -- Etat_actuel = Suivre_ligne
			wait(CMD_START);
			break;

		case Suivre_ligne:

			couleur = couleurNouvelle();   // allume aussi la DEL de la couleur
			if (couleur != AUCUNE) {
				Serial.print("Carton : ");
				Serial.println(NOMS[couleur]);
			}

			if (couleur == JAUNE)
				Etat_actuel = Epreuve_jaune;
			else if (couleur == BLEU)
				Etat_actuel = Epreuve_bleu;
			else if (couleur == ORANGE)
				Etat_actuel = Epreuve_orange;
			else if (couleur == ROSE)
				Etat_actuel = Epreuve_rose;
			else if (couleur == VERT)
				Etat_actuel = Epreuve_vert;
			else if (mur_actif && (digitalRead(pin_vert) == MUR_DETECTE || digitalRead(pin_rouge) == MUR_DETECTE) && detection_mur())
				contournerMur();
			else
				suivreLigne();

			break;

		case Epreuve_jaune:
			bip();
			mur_actif = 1;
			Etat_actuel = Suivre_ligne;

			break;

		case Epreuve_bleu:
			bip();
			Etat_actuel = Attente_bleu;
			break;

		case Attente_bleu:
			wait(CMD_SLEEP);
			break;

		case Epreuve_orange:
			arretCouleur();
			//épreuve orange
			Etat_actuel = Suivre_ligne;

			break;

		case Epreuve_rose:
			arretCouleur();
			//tournerdroite();
			//avancer();
			//tourcomplet();
			//demitour();
			//avancer();
			Etat_actuel = Suivre_ligne;

			break;

		case Epreuve_vert:
			// Canyon : on continue a suivre la ligne jusqu'aux 3 capteurs sur le noir (voir suivreLigne)
			bip();
			canyon_actif = 1;
			Etat_actuel = Suivre_ligne;

			break;

		case fin:

			//fonction fin (arret la)
			MOTOR_SetSpeed(LEFT, 0);
			MOTOR_SetSpeed(RIGHT, 0);

			break;
	}
}

void arretCouleur() {
  bip();
  attendre(ARRET_COULEUR);
}

// Arret et bip de 0.2 s
void bip() {
  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT, 0);

  AX_BuzzerON();
  attendre(200);
  AX_BuzzerOFF();
}


void suivreLigne() {
    int u1 = analogRead(A1);
    int u2 = analogRead(A2);
    int u3 = analogRead(A3);
//	Serial.print("U1: ");
//    Serial.println(u1);
//	Serial.print("U2: ");
//    Serial.println(u2);
//	Serial.print("U3: ");
//    Serial.println(u3);
//	Serial.println("");

	if (u2 > 400) {
		MOTOR_SetSpeed(LEFT, 0.2);
		MOTOR_SetSpeed(RIGHT, 0.2);	/* code */
		apres_canyon = 0;
		if (canyon_actif && u1 > 400 && u3 > 400) {    // canyon seulement apres le carton vert
			ENCODER_Reset(LEFT);
			ENCODER_Reset(RIGHT);
			deplacement(CANYON);
			attendre(2000);
			MOTOR_SetSpeed(LEFT, 0);
			MOTOR_SetSpeed(RIGHT, 0);
			apres_canyon = 1;
			canyon_actif = 0;
		}
	} else if (u1 > 400) {
		MOTOR_SetSpeed(LEFT, 0.1);
		MOTOR_SetSpeed(RIGHT, 0.4);
		apres_canyon = 0;
	} else if (u3 > 400) {
		MOTOR_SetSpeed(LEFT, 0.4);
		MOTOR_SetSpeed(RIGHT, 0.1);
		apres_canyon = 0;
	} else {
		if (cherche_droit && apres_canyon) {
			MOTOR_SetSpeed(LEFT, 0.4);
			MOTOR_SetSpeed(RIGHT, 0.0);
			delay(100);
			cherche_droit = 0;
		} else if (!cherche_droit && apres_canyon) {
			MOTOR_SetSpeed(LEFT, 0.0);
			MOTOR_SetSpeed(RIGHT, 0.4);
			delay(200);
		}

	}
}


bool detection_mur()
{
  
  int obstacle = 0;

  for (int i = 0; i < 7; i++)
  {
    
    int vert = digitalRead(pin_vert);
    int rouge = digitalRead(pin_rouge);
    
    if (vert == MUR_DETECTE && rouge == MUR_DETECTE)
    {
      obstacle ++;
    }
    if (vert == MUR_DETECTE || rouge == MUR_DETECTE)
    {
      obstacle ++;
    }
    
    if (obstacle >= 4)
    {
      return true;
    }
    attendre(7);
  }
  // Pas de mur
  return false;
}


void avancerJusquaLigne() {
  MOTOR_SetSpeed(LEFT, VITESSE_CHERCHE);
  MOTOR_SetSpeed(RIGHT, VITESSE_CHERCHE);
  while (analogRead(A1) <= 400 && analogRead(A2) <= 400 && analogRead(A3) <= 400)
    delMiseAJour();
  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT, 0);
}

void contournerMur() {
  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT, 0);
  attendre(200);

  tourne_droit();
  deplacement(MUR_COTE);
  tourne_gauche();
  deplacement(MUR_LONGER);
  tourne_gauche();
  deplacement(MUR_RETOUR);

  tourne_droit();           // de retour sur la ligne : on se remet dans le sens du parcours

  mur_actif = 0;
}
