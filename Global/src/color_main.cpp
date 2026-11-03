#include <LibRobus.h>


// ---- Doivent etre pareils que dans color.cpp ----
#define AUCUNE        -1
#define JAUNE          2
#define ORANGE         3
#define BLEU           4
#define VERT           5
#define ROSE           6
#define NB_COULEURS    7

// ---- Viennent de color.cpp ----
extern char NOMS[NB_COULEURS][10];

bool couleurInit();
int  couleurNouvelle();
void couleurNouvelEssai();
void delMiseAJour();
void attendre(unsigned long ms);


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


// ---- A coder (vides pour que ca compile) ----
// Dans ces fonctions, utiliser attendre() au lieu de delay() pour que les DEL s'eteignent a 10 s
void tournerdroite() {}
void tournergauche() {}
void avancer() {}
void tourcomplet() {}
void demitour() {}
void bip3() {}


void setup() {
	BoardInit();
	while (!couleurInit())
		delay(1000);
	couleurNouvelEssai();
	Etat_actuel = Attente_depart;
}


void loop() {
	int couleur;
	SOFT_TIMER_Update();
	delMiseAJour();
	switch (Etat_actuel) {

		case Attente_depart:

			//fonction attente IR -- Etat_actuel = Suivre_ligne
			Etat_actuel = Suivre_ligne;   // TEMPORAIRE : part tout de suite en attendant l'IR

			break;

		case Suivre_ligne:

			//fonction suivre la ligne

			couleur = couleurNouvelle();
			if (couleur != AUCUNE)
			{
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

			break;

		case Epreuve_jaune:
			tournerdroite();
			avancer();                 // pour la fonction avancer il faut voir de combien on avance
			tournergauche();
			avancer();
			tournergauche();
			avancer();
			tournerdroite();
			Etat_actuel = Suivre_ligne;

			break;

		case Epreuve_bleu:
			if (bouton2 == 1)
			{
				bip3();
				avancer();
			}
			Etat_actuel = Suivre_ligne;

			break;

		case Epreuve_orange:

			//épreuve orange
			Etat_actuel = Suivre_ligne;

			break;

		case Epreuve_rose:
			tournerdroite();
			avancer();
			tourcomplet();
			demitour();
			avancer();
			Etat_actuel = Suivre_ligne;

			break;

		case Epreuve_vert:

			//épreuve verte
			Etat_actuel = Suivre_ligne;

			break;

		case fin:

			//fonction fin (arret la)

			break;
	}
}
