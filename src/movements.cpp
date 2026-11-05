// movements.cpp
#include <LibRobus.h>
#include "movements.h"
#include "challenges.h"   // ledMiseAJour()

#define PULSES_LEFT 2000
#define PULSES_RIGHT 2000
#define PULSES_PER_M 13367

#define DISTANCE 0.495
#define DISTANCE_1M 1


MotorParams motor_left = { .kp = 0.0004, .ki = 0.000006, .integral = 0, .speed = 0, .total_pulses = 0, .target_pulses = 0 };
MotorParams motor_right = { .kp = 0.0004, .ki = 0.000006, .integral = 0, .speed = 0, .total_pulses = 0, .target_pulses = 0 };

void movementsInit() {
    // Motor(s) and shit
    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);

    SOFT_TIMER_SetCallback(TIMER_ID_PID, updatePI);
    SOFT_TIMER_SetDelay(TIMER_ID_PID, 100);
    SOFT_TIMER_SetRepetition(TIMER_ID_PID, -1);
    SOFT_TIMER_Enable(TIMER_ID_PID);
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
    movePulses(-PULSES_LEFT, PULSES_RIGHT);
    Serial.println("done left");
}

void turnRight90() {
    Serial.println("Running turn right");
    movePulses(PULSES_LEFT, -PULSES_RIGHT);
    Serial.println("done right");
}

void movePulses(int32_t left_target, int32_t right_target) {
    // Remise à zéro de tout (speed aussi, sinon il part avec l'ancienne vitesse)
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
    motor_left.total_pulses = 0;
    motor_right.total_pulses = 0;
    motor_left.integral = 0;
    motor_right.integral = 0;
    motor_left.speed = 0;
    motor_right.speed = 0;

    int dirL = (left_target >= 0) ? 1 : -1;
    int dirR = (right_target >= 0) ? 1 : -1;
    int32_t absL = abs(left_target);
    int32_t absR = abs(right_target);

    while (abs(motor_left.total_pulses) < abs(left_target) || abs(motor_right.total_pulses) < absR) {
        int32_t remL = absL - abs(motor_left.total_pulses);
        int32_t remR = absR - abs(motor_right.total_pulses);

        // Chaque roue a son propre profil, et s'arrête quand elle a fini
        motor_left.target_pulses = (remL <= 0) ? 0 : dirL * ((remL > 2000) ? 300 : (remL > 1000) ? 150 : (remL > 300) ? 67 : 15); // 67 xD
        motor_right.target_pulses = (remR <= 0) ? 0 : dirR * ((remR > 2000) ? 300 : (remR > 1000) ? 150 : (remR > 300) ? 67 : 15);

        SOFT_TIMER_Update();
        ledMiseAJour();
    }

    motor_left.target_pulses = 0;
    motor_right.target_pulses = 0;
    for (int i = 0; i < 250; i++) {
        delay(2);
        SOFT_TIMER_Update();
        ledMiseAJour();
    }
    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);
}

void movePulsesQuick(int32_t left_target, int32_t right_target) {
    const int32_t TOL = 15;            // tolérance en pulses
    const uint32_t TIMEOUT_MS = 8000;  // sécurité si ça converge pas

    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
    motor_left.total_pulses = 0;
    motor_right.total_pulses = 0;
    motor_left.integral = 0;
    motor_right.integral = 0;
    motor_left.speed = 0;
    motor_right.speed = 0;

    auto profile = [](int32_t rem) -> int32_t {
        int32_t a = abs(rem);
        int32_t v = (a > 2000) ? 500 : (a > 1000) ? 200 : (a > 300) ? 67 : 30;
        return (rem >= 0) ? v : -v;
    };

    uint32_t start = millis();
    int settled = 0;

    while (settled < 3 && millis() - start < TIMEOUT_MS) {
        int32_t remL = left_target - motor_left.total_pulses;
        int32_t remR = right_target - motor_right.total_pulses;

        motor_left.target_pulses = (abs(remL) <= TOL) ? 0 : profile(remL);
        motor_right.target_pulses = (abs(remR) <= TOL) ? 0 : profile(remR);

        // Il faut être dans la tolérance pendant quelques itérations de suite
        if (abs(remL) <= TOL && abs(remR) <= TOL)
            settled++;
        else
            settled = 0;

        SOFT_TIMER_Update();
        delay(10);
    }

    motor_left.target_pulses = 0;
    motor_right.target_pulses = 0;
    for (int i = 0; i < 250; i++) {
        delay(2);
        SOFT_TIMER_Update();
        ledMiseAJour();
    }
    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);
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
        } else if (remaining > 1000) {
            motor_left.target_pulses = 400;
            motor_right.target_pulses = 400;
        } else {
            motor_left.target_pulses = 150;
            motor_right.target_pulses = 150;
        }

        SOFT_TIMER_Update();
        ledMiseAJour();
    }

    motor_left.target_pulses = 0;
    motor_right.target_pulses = 0;

    for (int i = 0; i < 250; i++) {
        delay(2);
        SOFT_TIMER_Update();
        ledMiseAJour();
    }

    SOFT_TIMER_Update();
}