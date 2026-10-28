/*
Projet: main.cpp robo swag
Equipe: P23 - Bozo Squad
Auteurs: Jessy, RAJOUTER VOS NOMS ICI
Description: Code epreuve du parcour
Date: Aujourd'hui
*/

/* ****************************************************************************
Inclure les librairies de functions que vous voulez utiliser
**************************************************************************** */

#include <LibRobus.h> // Essentielle pour utiliser RobUS

#define PULSES_LEFT 1700
#define PULSES_RIGHT 1700

#define PIN_5KHZ      A0       
// ---- Seuils ---- base sur 3,2V min : 3,2 / 5 * 1024 = 655, max : 3,49 / 5 * 1024 = 714
#define SEUIL_SIFFLET 550
#define MAX_SIFFLET   714
#define pin_rouge 49
#define pin_vert 48

#define distance 0.495
#define distance_1m 1

// put function declarations here:
char Va_Nord(char orientation);
char Va_Ouest(char orientation);
char Va_Sud(char orientation);
char Va_Est(char orientation);
void fonction_futur_pos();
int deja_alle();
void tableau_post_tape();
void tableau_post_mur();
void tableau_post_historique();
void ajouter_position_actuelle();
void detection_mur();
void enlever_derniere();
void updatePI();
void printDebug();
void moveDistance(float distance_m);
void turnLeft90();
void turnRight90();
bool sifflet5kHz();

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
  .ki = 0.000006,

  .integral = 0,
  .speed = 0,
  .total_pulses = 0,
  .target_pulses = 0
};

MotorParams motor_right = {
  .kp = 0.0004,
  .ki = 0.000006,

  .integral = 0,
  .speed = 0,
  .total_pulses = 0,
  .target_pulses = 0
};

char ori = 'N';
int pos[4] = {2,1,0,0};
int direction_possible[4];
int futur_pos[4];
int map_deja_alle[30][4];
int nb_deja_alle = 0;
int mur = 0;

void setup() {
  BoardInit();

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

  delay(10);

  Serial.println("Waiting...");
  while ((!ROBUS_IsBumper(REAR)) && !sifflet5kHz()) {
    delay(20);
    SOFT_TIMER_Update();
  }
  Serial.println("Going!");
}

void loop() {
  if (pos[1] < 10) {
    Serial.print("Pos: ");
	Serial.println(pos[1]);
	int deplacement_fait = 0;
	direction_possible[0] = 1;
    direction_possible[1] = 1;
    direction_possible[2] = 1;
    direction_possible[3] = 1;
        
    ajouter_position_actuelle();
    tableau_post_tape();
        
    while (deplacement_fait == 0) {
			
			if (direction_possible[0] == 1) {
        Serial.println("Doing 1");
				ori = Va_Nord(ori);
				detection_mur();
        Serial.print("Mur: ");
        Serial.println(mur);
				tableau_post_mur();
				fonction_futur_pos();
				tableau_post_historique();
        for (int i = 0; i < 4; i++){
        Serial.print(direction_possible[i]);}
        Serial.println("");
			
				if (direction_possible[0] == 1){
          Serial.println("Doing move 1");
					moveDistance(distance_1m);
          Serial.println("Done");
					pos[1]++;
					deplacement_fait++;
					break;
				}
			}
		  if (direction_possible[1] == 1) {
				Serial.println("Doing 2");
        ori = Va_Est(ori);
				detection_mur();
        Serial.print("Mur: ");
        Serial.println(mur);
				tableau_post_mur();
				fonction_futur_pos();
				tableau_post_historique();
        for (int i = 0; i < 4; i++){
        Serial.print(direction_possible[i]);}
        Serial.println("");  

				if (direction_possible[1] == 1){
          Serial.println("Doing move 2");
					moveDistance(distance);
          Serial.println("Done");
					pos[0]++;
					deplacement_fait++;
          break;
        }
			}
			if (direction_possible[2] == 1) {
				Serial.println("Doing 3");
        ori = Va_Ouest(ori);
				detection_mur();
        Serial.print("Mur: ");
        Serial.println(mur);
				tableau_post_mur();
				fonction_futur_pos();
				tableau_post_historique();
        for (int i = 0; i < 4; i++){
        Serial.print(direction_possible[i]);}
			  Serial.println("");
				if (direction_possible[2] == 1){
					Serial.println("Doing move 3");
          moveDistance(distance);
          Serial.println("Done");
					pos[0]--;
					deplacement_fait++;
          break;
				}
			}
			if (direction_possible[3] == 1) {
				Serial.println("Doing 4");
        ori = Va_Sud(ori);
				detection_mur();
        Serial.println(mur);
				tableau_post_mur();
				fonction_futur_pos();
				tableau_post_historique();
        for (int i = 0; i < 4; i++){
        Serial.print(direction_possible[i]);}
			  Serial.println("");
				if (direction_possible[3] == 1){
					Serial.println("Doing move 4");
          moveDistance(distance);
					pos[1]--;
					deplacement_fait++;
          break;
				}
			}
			//else {
      //  Serial.println("Enlever last");
			//	enlever_derniere();
			//}
      SOFT_TIMER_Update();
		}
	}
	else {
  }
}

void detection_mur(){
	int led_rouge;
	int led_verte;
  
  for (int i = 0; i < 300; i++)
  {
    delay(2);
    SOFT_TIMER_Update();
  }

  led_rouge = digitalRead(pin_rouge);
  led_verte = digitalRead(pin_vert);
  Serial.print("Mur: ");
	if (led_rouge == LOW && led_verte == LOW){
		mur = 1;
    Serial.println("yes");
	} else {
    mur = 0;
    Serial.println("no");
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

void turnLeft90() {
  Serial.println("Running turn left");
  int32_t target_turn_pulses = PULSES_LEFT;

  motor_left.total_pulses = 0;
  motor_right.total_pulses = 0;

  motor_left.integral = 0;
  motor_right.integral = 0;

  while (-motor_left.total_pulses < target_turn_pulses || motor_right.total_pulses < target_turn_pulses) {
    int32_t avg_pulses = (-motor_left.total_pulses + motor_right.total_pulses) / 2;
    int32_t remaining = target_turn_pulses - avg_pulses;

    if (remaining > 500) {
      motor_left.target_pulses = -200;
      motor_right.target_pulses = 200;
    }
    else if (remaining > 200) {
      motor_left.target_pulses = -120;
      motor_right.target_pulses = 120;
    }
    else {
      motor_left.target_pulses = -60;
      motor_right.target_pulses = 60;
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

  Serial.println("done left");
}

void turnRight90() {
  Serial.println("Running turn right");
  int32_t target_turn_pulses = PULSES_RIGHT;

  motor_left.total_pulses = 0;
  motor_right.total_pulses = 0;

  motor_left.integral = 0;
  motor_right.integral = 0;

  while ( motor_left.total_pulses < target_turn_pulses || -motor_right.total_pulses < target_turn_pulses) {
    int32_t avg_pulses = (motor_left.total_pulses - motor_right.total_pulses) / 2;
    int32_t remaining = target_turn_pulses - avg_pulses;

    if (remaining > 500) {
      motor_left.target_pulses = 200;
      motor_right.target_pulses = -200;
    }
    else if (remaining > 200) {
      motor_left.target_pulses = 120;
      motor_right.target_pulses = -120;
    }
    else {
      motor_left.target_pulses = 60;
      motor_right.target_pulses = -60;
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
  Serial.println("done right");
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
      motor_left.target_pulses = 600;
      motor_right.target_pulses = 600;
    }
    else if (remaining > 1000) {
      motor_left.target_pulses = 400;
      motor_right.target_pulses = 400;
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
