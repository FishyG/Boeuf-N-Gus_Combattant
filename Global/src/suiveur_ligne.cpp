#include <LibRobus.h>
#include <Arduino.h>

#define PPT 3200.0
#define PULSES_PAR_MM (3200.0 / (PI * 76.37))
#define LARGEUR_ROBOT (103 + 72 + 6.5) // (epaisseur entre roues - cell) + cell + epaisseur de roue
#define PERIODE 25
#define KP 0.1
#define KI 0.2

#define V_MAX_MOTEUR 1
#define V_MIN_MOTEUR 0.15
#define V_PPS_MAX 10272

#define COMPENSATION_INERTIE 120

#define PIN_5KHZ A0

float vitesse_base = 0.35;

int32_t deplacement_total_droit = 0;
int32_t deplacement_total_gauche = 0;
int32_t deplacement_total_moyen = 0;
int32_t deplacement_a_faire = 0;
int32_t integrale_gauche = 0;
int32_t integrale_droite = 0;
int32_t v_moteur_gauche_pps = 0;
int32_t v_moteur_droit_pps = 0;
int32_t compteur = 0;
int sens_rotation = 0;


double calculer_vitesse_moteur (int32_t vitesse_pps, int32_t v_pps_max)
{
  double vitesse_moteur = (double) vitesse_pps / (double) v_pps_max;
  return vitesse_moteur;
}

int32_t PID(int32_t pps_cible, int32_t nb_pulses, int32_t &integrale)
{
  int32_t erreur = pps_cible - nb_pulses * 1000L / PERIODE;
  integrale += erreur;
  return (pps_cible + KP * erreur + KI * integrale);
}

void blabla()
{
  int32_t pulse_moteur_gauche = ENCODER_ReadReset(LEFT);
  int32_t pulse_moteur_droit = ENCODER_ReadReset(RIGHT);

  int32_t v_pps_cible = V_PPS_MAX * vitesse_base;

  double v_moteur_gauche;
  double v_moteur_droit;

  if (compteur > 4)
  {
    v_moteur_gauche_pps = PID(v_pps_cible, pulse_moteur_gauche, integrale_gauche);
    v_moteur_droit_pps = PID(v_pps_cible, pulse_moteur_droit, integrale_droite);
    v_moteur_gauche = calculer_vitesse_moteur(v_moteur_gauche_pps, V_PPS_MAX);  
    v_moteur_droit = calculer_vitesse_moteur(v_moteur_droit_pps, V_PPS_MAX);  
  }
  else
  {
    v_moteur_gauche = 0.2 + compteur * 0.03;
    v_moteur_droit = 0.2 + compteur * 0.03;
  }

  deplacement_total_gauche += pulse_moteur_gauche;
  deplacement_total_droit += pulse_moteur_droit;

  ++compteur;

  MOTOR_SetSpeed(LEFT, v_moteur_gauche);
  MOTOR_SetSpeed(RIGHT, v_moteur_droit);
}

void blablaRond()
{
  int32_t pulse_moteur_gauche = ENCODER_ReadReset(LEFT);
  int32_t pulse_moteur_droit = ENCODER_ReadReset(RIGHT);

  pulse_moteur_gauche = abs(pulse_moteur_gauche);
  pulse_moteur_droit = abs(pulse_moteur_droit);

  int32_t v_pps_cible = V_PPS_MAX * vitesse_base;

  double v_moteur_gauche;
  double v_moteur_droit;

  if (compteur > 4)
  {
    v_moteur_gauche_pps = PID(v_pps_cible, pulse_moteur_gauche, integrale_gauche);
    v_moteur_droit_pps = PID(v_pps_cible, pulse_moteur_droit, integrale_droite);
    v_moteur_gauche = calculer_vitesse_moteur(v_moteur_gauche_pps, V_PPS_MAX);  
    v_moteur_droit = calculer_vitesse_moteur(v_moteur_droit_pps, V_PPS_MAX);  
  }
  else
  {
    v_moteur_gauche = 0.2 + compteur * 0.03;
    v_moteur_droit = 0.2 + compteur * 0.03;
  }

  deplacement_total_gauche += pulse_moteur_gauche;
  deplacement_total_droit += pulse_moteur_droit;

  ++compteur;

  MOTOR_SetSpeed(LEFT, sens_rotation * v_moteur_gauche);
  MOTOR_SetSpeed(RIGHT, -1 * sens_rotation * v_moteur_droit);

  Serial.print(compteur); Serial.print("\t");
  Serial.print(pulse_moteur_gauche); Serial.print("\t");
  Serial.print(pulse_moteur_droit); Serial.print("\t");
  Serial.print(deplacement_total_gauche); Serial.print("\t");
  Serial.print(deplacement_total_droit); Serial.print("\t");
  Serial.print(v_moteur_gauche); Serial.print("\t");
  Serial.println(v_moteur_droit);
}

void arret(){
  v_moteur_gauche_pps = 0;
  v_moteur_droit_pps = 0;
  deplacement_total_droit = 0;
  deplacement_total_gauche = 0;
  deplacement_total_moyen = 0;
  integrale_droite = 0;
  integrale_gauche = 0;
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
  ENCODER_Reset(LEFT);
  ENCODER_Reset(RIGHT);
}

void deplacement(int distance)
{
  deplacement_a_faire = PULSES_PAR_MM * distance;
  deplacement_total_droit = 0;
  deplacement_total_gauche = 0;
  integrale_gauche = 0;
  integrale_droite = 0;
  compteur = 0;

  SOFT_TIMER_SetCallback(0, blabla);
  SOFT_TIMER_SetDelay(0, PERIODE);

  SOFT_TIMER_Enable(0);
  while (deplacement_total_gauche < deplacement_a_faire || deplacement_total_droit < deplacement_a_faire)
  {
    SOFT_TIMER_Update();
  };

  SOFT_TIMER_Disable(0);
  arret();
}

void rotation(int angle)
{
  deplacement_a_faire = LARGEUR_ROBOT * PI * angle / 360 * PULSES_PAR_MM - COMPENSATION_INERTIE;
  deplacement_total_droit = 0;
  deplacement_total_gauche = 0;
  integrale_gauche = 0;
  integrale_droite = 0;
  compteur = 0;

  Serial.print("--- Rotation "); Serial.print(angle);
  Serial.print(" deg | cible = "); Serial.println(deplacement_a_faire);

  SOFT_TIMER_SetCallback(0, blablaRond);
  SOFT_TIMER_SetDelay(0, PERIODE);
  SOFT_TIMER_Enable(0);
  while (deplacement_total_gauche < deplacement_a_faire || deplacement_total_droit < deplacement_a_faire)
  {
    SOFT_TIMER_Update();
  }
  SOFT_TIMER_Disable(0);

  int32_t g = deplacement_total_gauche;
  int32_t d = deplacement_total_droit;
  arret();

  delay(300); // laisse le robot s'immobiliser
  int32_t glisse_g = abs(ENCODER_Read(LEFT));
  int32_t glisse_d = abs(ENCODER_Read(RIGHT));

  Serial.print("Fin   G="); Serial.print(g); Serial.print(" D="); Serial.println(d);
  Serial.print("Inertie G="); Serial.print(glisse_g); Serial.print(" D="); Serial.println(glisse_d);
  Serial.print("Total G="); Serial.print(g + glisse_g); Serial.print(" D="); Serial.println(d + glisse_d);
  Serial.print("Erreur G="); Serial.print(g + glisse_g - deplacement_a_faire);
  Serial.print(" D="); Serial.println(d + glisse_d - deplacement_a_faire);
}

void avancer()
{
  ENCODER_Reset(RIGHT);
  ENCODER_Reset(LEFT);
  deplacement(500);
  ++increment_historique;
  historique[increment_historique] = historique[increment_historique - 1] + orientation;
  adjacence[historique[increment_historique]][historique[increment_historique - 1]] = 0;
}

void tourne_droit()
{
  ENCODER_Reset(RIGHT);
  ENCODER_Reset(LEFT);
  sens_rotation = 1;
  rotation(90);
}

void demi_tour()
{
  ENCODER_Reset(RIGHT);
  ENCODER_Reset(LEFT);
  sens_rotation = 1;
  rotation(182);
}

void tourne_gauche(){
  ENCODER_Reset(RIGHT);
  ENCODER_Reset(LEFT);
  sens_rotation = -1;
  rotation(90);
}

void suivreLigne() {
  int u1 = analogRead(A1);
  int u2 = analogRead(A2);
  int u3 = analogRead(A3);

	if (u2 > 400) {
		MOTOR_SetSpeed(LEFT, 0.2);
		MOTOR_SetSpeed(RIGHT, 0.2);
		apres_canyon = 0;
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