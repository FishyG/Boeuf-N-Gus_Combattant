// color.h
#pragma once
#include <Arduino.h>

#define CARTE_ROUGE  1
#define CARTE_VERT   2
#define CARTE_BLEU   3
#define CARTE_JAUNE  4
#define CARTE_ORANGE 5

bool couleurInit();
void calibrerBlanc();
bool readFuckingRGB(uint8_t &r, uint8_t &g, uint8_t &b);
float rgbToHueBombaclat(uint8_t r, uint8_t g, uint8_t b);
float rgbToSaturationPUSSYWAGAN(uint8_t r, uint8_t g, uint8_t b);
int couleurScan();