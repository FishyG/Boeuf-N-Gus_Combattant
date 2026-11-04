/*
Projet: main.cpp robo swag
Equipe: P23
Auteurs: Jessy, RAJOUTER VOS CNOMS ICI
Description: Code epreuve du combattant
Date: Aujourd'hui lmao
*/

/* ****************************************************************************
Inclure les librairies de functions que vous voulez utiliser
**************************************************************************** */

#include <LibRobus.h> // Pour utiliser RobUS

#define PULSES_LEFT 1700
#define PULSES_RIGHT 1700

#define pin_rouge 49
#define pin_vert 48

#define distance 0.495
#define distance_1m 1

#define PULSES_PER_M 13367

void updatePI();
void moveDistance(float distance_m);
void turnLeft90();
void turnRight90();
int detection_mur();
void slapQuille();

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

// Si vous avez a modifier les valeur de PI(No D), elles vont ici
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

void setup() {
  BoardInit();

  // Servomotor and shit (sont disable pour pas qui pete toute)
  SERVO_Disable(0); // C'est le servo qui squeeze
  SERVO_Disable(1); // C'est le servo qui tilt
  SERVO_SetAngle(0,0); // c valeur par defaut pour pas qui arrache toute
  // Use le full range, 0 et 180 deg
  SERVO_SetAngle(1, 20); // c valeur par defaut pour pas qui arrache toute
  // DO NOT USE O-180!!!! Seulement 0-20, thx zoubi >w<

  pinMode(pin_rouge, INPUT);
  pinMode(pin_vert, INPUT);

  // Motor(s) and shit
  ENCODER_Reset(LEFT);
  ENCODER_Reset(RIGHT);

  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT, 0);

  SOFT_TIMER_SetCallback(TIMER_ID_PID, updatePI);
  SOFT_TIMER_SetDelay(TIMER_ID_PID, 100);
  SOFT_TIMER_SetRepetition(TIMER_ID_PID, -1);
  SOFT_TIMER_Enable(TIMER_ID_PID);
  
  // SOFT_TIMER_SetCallback(TIMER_ID_DEBUG, printDebug);
  // SOFT_TIMER_SetDelay(TIMER_ID_DEBUG, 500);
  // SOFT_TIMER_SetRepetition(TIMER_ID_DEBUG, -1);
  // SOFT_TIMER_Enable(TIMER_ID_DEBUG);

  delay(10);

  // Set le servo droit
  SERVO_Enable(1);
  SERVO_SetAngle(1, 20);
  SERVO_Enable(0);
  SERVO_SetAngle(0,0);

  /*while (1) {
    Serial.println("Waiting...");
    while (!ROBUS_IsBumper(REAR)) {
      delay(10);
    }
    Serial.println("Slapping");
    delay(1000);
    slapQuille();
  }*/
  // Met le servo tilt more or less droit
  SERVO_Enable(1);
  SERVO_SetAngle(1, 20);
  SERVO_Enable(0);
  SERVO_SetAngle(0,0);

  Serial.println("Waiting...");
  while (!ROBUS_IsBumper(REAR)) {
    delay(10);
  }
  Serial.println("Pognage!");
  delay(1000);
  SERVO_SetAngle(0,178);
  delay(750);
  SERVO_SetAngle(1,0);

  Serial.println("Waiting...");
  while (!ROBUS_IsBumper(REAR)) {
    delay(10);
  }
  Serial.println("Dropage!");
  delay(1000);
  SERVO_SetAngle(1, 23);
  delay(500);
  SERVO_SetAngle(0,0);
  Serial.println("Done!");
  while (1)
  {
    delay(10);
  }
  SOFT_TIMER_Update();
  Serial.println("Going!");
}

void loop() {
  delay(10);
  SOFT_TIMER_Update();
}

// Te donne 0 si fuckall 1 si le rouge est a on, 2 si verte, 3 si les deux
int detection_mur(){

  int total = 0;
  if (digitalRead(pin_rouge) == LOW){
		total = total + 1;
	}
  if (digitalRead(pin_vert) == LOW){
		total = total + 2;
  }
  return total;
}

void slapQuille() {
  int32_t left_pulses = 0;
  ENCODER_Reset(LEFT);
  MOTOR_SetSpeed(LEFT, 0.3); 
  while (left_pulses < 10000) {
    left_pulses = ENCODER_Read(LEFT);
  }
  MOTOR_SetSpeed(LEFT, 0.2);
  while (left_pulses < 15670) {
    left_pulses = ENCODER_Read(LEFT);
  }
  MOTOR_SetSpeed(LEFT, 0); 
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
