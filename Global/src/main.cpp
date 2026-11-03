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

bool cherche_droit;
bool apres_canyon;

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

int Etat_actuel;
int bouton2 = 0;

//. les defs de functions, ils sont ici
void detection_mur();
void updatePI();
void printDebug();
void moveDistance(float distance_m);
void suivreLigne();
void arretCouleur();

// Les IDs des timers je les mets ici
enum TimerIDs {
  TIMER_ID_PID,
  TIMER_ID_DEBUG,
};

typedef struct {
  float kp;
  float ki;
  float integral;
  float speed;
  int32_t total_pulses;
  int32_t target_pulses;
} MotorParams;

// Si vous avez a modifier les valeur de PID, elles vont ici
MotorParams motor_left = {
  .kp = 0.0004,
  .ki = 0.000002,

  .integral = 0,
  .speed = 0,
  .total_pulses = 0,
  .target_pulses = 0
};

MotorParams motor_right = {
  .kp = 0.0004,
  .ki = 0.000002,

  .integral = 0,
  .speed = 0,
  .total_pulses = 0,
  .target_pulses = 0
};

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

  SOFT_TIMER_SetCallback(TIMER_ID_PID, updatePI);
  SOFT_TIMER_SetDelay(TIMER_ID_PID, 100);
  SOFT_TIMER_SetRepetition(TIMER_ID_PID, -1);
  SOFT_TIMER_Enable(TIMER_ID_PID);

  SOFT_TIMER_SetCallback(TIMER_ID_DEBUG, printDebug);
  SOFT_TIMER_SetDelay(TIMER_ID_DEBUG, 500);
  SOFT_TIMER_SetRepetition(TIMER_ID_DEBUG, -1);
  //SOFT_TIMER_Enable(TIMER_ID_DEBUG);
  cherche_droit = 1;
  apres_canyon = 0;

  delay(10);

  Etat_actuel = Attente_depart;
  Serial.println("Waiting...");
}


void loop() {
	int couleur;

	delMiseAJour();

	switch (Etat_actuel) {

		case Attente_depart:

			//fonction attente IR -- Etat_actuel = Suivre_ligne
			if (ROBUS_IsBumper(REAR)) {      // TEMPORAIRE : bumper arriere en attendant l'IR
				Serial.println("Going!");
				Etat_actuel = Suivre_ligne;
			}

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
			else
				suivreLigne();

			break;

		// Pour le moment les epreuves sont en commentaire : le robot s'arrete 3 s et repart
		case Epreuve_jaune:
			arretCouleur();
			//tournerdroite();
			//avancer();                 // pour la fonction avancer il faut voir de combien on avance
			//tournergauche();
			//avancer();
			//tournergauche();
			//avancer();
			//tournerdroite();
			Etat_actuel = Suivre_ligne;

			break;

		case Epreuve_bleu:
			arretCouleur();
			//if (bouton2 == 1)
			//{
			//	bip3();
			//	avancer();
			//}
			Etat_actuel = Suivre_ligne;

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
			arretCouleur();
			//épreuve verte
			Etat_actuel = Suivre_ligne;

			break;

		case fin:

			//fonction fin (arret la)
			MOTOR_SetSpeed(LEFT, 0);
			MOTOR_SetSpeed(RIGHT, 0);

			break;
	}
}


// En attendant les epreuves : arret, bip, 3 s, puis on repart
// Utiliser attendre() et non delay() dans les epreuves pour que les DEL s'eteignent a 10 s
void arretCouleur() {
  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT, 0);

  AX_BuzzerON();
  attendre(200);
  AX_BuzzerOFF();

  attendre(ARRET_COULEUR);
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
		if (u1 > 400 && u3 > 400) {
			ENCODER_Reset(LEFT);
			ENCODER_Reset(RIGHT);
			moveDistance(0.59);
			for (int i = 0; i < 200; i++)
			{
				delay(10);
				SOFT_TIMER_Update();
			}
			MOTOR_SetSpeed(LEFT, 0);
			MOTOR_SetSpeed(RIGHT, 0);
			apres_canyon = 1;
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


// PI(pas de D) pour les moteurs
void updatePI() {
  int32_t left_pulses = ENCODER_ReadReset(LEFT);
  int32_t right_pulses = ENCODER_ReadReset(RIGHT);

  float left_error = motor_left.target_pulses - left_pulses;
  float right_error = motor_right.target_pulses - right_pulses;

  motor_left.total_pulses += left_pulses;
  motor_right.total_pulses += right_pulses;

  motor_left.integral += left_error;
  motor_right.integral += right_error;

  motor_left.speed += motor_left.kp * left_error + motor_left.ki * motor_left.integral;
  motor_right.speed += motor_right.kp * right_error + motor_right.ki * motor_right.integral;

  if (motor_left.speed > 1.0)
    motor_left.speed = 1.0;
  if (motor_left.speed < -1.0)
    motor_left.speed = -1.0;

  if (motor_right.speed > 1.0)
    motor_right.speed = 1.0;
  if (motor_right.speed < -1.0)
    motor_right.speed = -1.0;

  MOTOR_SetSpeed(LEFT, motor_left.speed);
  MOTOR_SetSpeed(RIGHT, motor_right.speed);
}

void printDebug() {
  Serial.print("L total: ");
  Serial.print(motor_left.total_pulses);

  Serial.print(" | L integral: ");
  Serial.print(motor_left.integral);

  Serial.print(" | L speed: ");
  Serial.print(motor_left.speed);

  Serial.print(" | R total: ");
  Serial.print(motor_right.total_pulses);

  Serial.print(" | R integral: ");
  Serial.print(motor_right.integral);

  Serial.print(" | R speed: ");
  Serial.println(motor_right.speed);
}

void moveDistance(float distance_m) {
  int32_t distance_target = distance_m * 13367;

  motor_left.total_pulses = 0;
  motor_right.total_pulses = 0;

  motor_left.integral = 0;
  motor_right.integral = 0;

  while (motor_left.total_pulses < distance_target || motor_right.total_pulses < distance_target) {
    int32_t remaining = distance_target - (motor_left.total_pulses + motor_right.total_pulses) / 2;
    if (remaining > 2000) {
      motor_left.target_pulses = 400;
      motor_right.target_pulses = 400;
    }
    else if (remaining > 1000) {
      motor_left.target_pulses = 200;
      motor_right.target_pulses = 200;
    } else {
      motor_left.target_pulses = 150;
      motor_right.target_pulses = 150;
    }

    SOFT_TIMER_Update();
  }

  motor_left.target_pulses = 0;
  motor_right.target_pulses = 0;

  for (int i = 0; i < 250; i++)
  {
    delay(2);
    SOFT_TIMER_Update();
  }

  SOFT_TIMER_Update();
}
