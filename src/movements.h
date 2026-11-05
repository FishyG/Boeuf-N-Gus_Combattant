#pragma once
#include <LibRobus.h> // Pour utiliser RobUS

// Les structs c trop swag XD
typedef struct {
  float kp;
  float ki;
  float integral;
  float speed;
  int32_t total_pulses;
  int32_t target_pulses;
} MotorParams;

// Les IDs des timers je les mets ici
enum TimerIDs {
    TIMER_ID_PID,
    TIMER_ID_DEBUG,
};

extern MotorParams motor_left;
extern MotorParams motor_right;

void movementsInit(); // C lui qui start le SOFT timer update
void updatePI();
void moveDistance(float distance_m);
void movePulses(int32_t left_target, int32_t right_target);
void movePulsesQuick(int32_t left_target, int32_t right_target);
void turnLeft90();
void turnRight90();

