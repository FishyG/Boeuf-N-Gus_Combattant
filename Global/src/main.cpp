// dans le setup: Etat_actuel = Attente_depart
#include <LibRobus.h>



void Allumer_DEL(int couleur) {
    digitalWrite(couleur, HIGH);
    int Etat_DEL = 1;
    int PIN_DEL = couleur;
    int Debut_DEL = millis();
}


void loop(){
	 SOFT_TIMER_Update();

    if (Etat_DEL == 1 && (millis() - Debut_DEL >= Duree_DEL)) {
        digitalWrite(PIN_DEL, LOW);
        Etat_DEL = 0;
    }
    
	switch (Etat_actuel) {	
		
		case Attente_depart:
		
			//fonction attente IR -- Etat_actuel = Suivre_ligne
			
			break;
			
		case Suivre_ligne:
		
			//fonction suivre la ligne -- lire les capteurs de couleurs
			
			if (/*couleur = jaune*/ && jaune == 0) {
				Etat_actuel = Epreuve_jaune;
				Allumer_DEL(PIN_jaune);
			} 
			
			else if (/*couleur = bleu*/ && bleu == 0) {
				Etat_actuel = Epreuve_bleu;
				Allumer_DEL(PIN_bleu);
			}
			
			else if (/*couleur = orange*/ && orange == 0) {
				Etat_actuel = Epreuve_orange;
				Allumer_DEL(PIN_orange);
			}
			
			else if (/*couleur = mauve*/ && mauve == 0) {
				Etat_actuel = Epreuve_mauve;
				Allumer_DEL(PIN_mauve);
			}
			
			else if (/*couleur = vert*/ && vert == 0) {
				Etat_actuel = Epreuve_vert;
				Allumer_DEL(PIN_vert);
			}
			
			break;
			
		case Epreuve_jaune:
			jaune = 1;
			tournerdroite();
			avancer();                 // pour la fonction avancer il faut voir de combien on avance
			tournergauche();
			avancer();
			tournergauche();
			avancer();
			trounerdroite();			
			Etat_actuel = Suivre_ligne;
			
			break;
			
		case Epreuve_bleu:
			bleu = 1;
			if (bouton2 == 1)         // il faut mettre bouton2 a 1 quand on appuie dessus
			{
				bip3();              // fonction qui fais 3 bip
				avancer();
			}
			Etat_actuel = Suivre_ligne;
			
			break;
			
		case Epreuve_orange:
		
			//épreuve orange
			orange = 1;
			Etat_actuel = Suivre_ligne;
			
			break;
			
		case Epreuve_mauve:
			mauve = 1;
			tournerdroite();         // on avance pour etre sur de taper la quille
			avancer();
			tourcomplet();
			demitour();
			avancer();
			Etat_actuel = Suivre_ligne;
			
			break;
			
		case Epreuve_vert:
		
			//épreuve verte
			vert = 1;
			Etat_actuel = Suivre_ligne;
			
			break;
			
		case fin:
		
			//fonction fin (arret la)
			
			break;
	}
}
