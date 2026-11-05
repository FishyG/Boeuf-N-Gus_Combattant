#pragma once

#include <Arduino.h> // Include Arduino framework so types like "String" are known
#include <LibRobus.h> // Pour utiliser RobUS


// 1. Function Prototypes (Declarations)
void pinceIdle();
void pinceYoink();
void pinceDrop();

// 2. Shared Global Variables (if any)
// Use 'extern' so they can be defined in one file but used everywhere
extern int sensorSampleCount; 
