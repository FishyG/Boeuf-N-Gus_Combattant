// color.cpp
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include "color.h"

// Constante pour correction
#define BLANC_R 1234.00
#define BLANC_G 1456.00
#define BLANC_B 987.00

#define hue_blanc 231
#define sat_blanc 49
#define hue_rouge 251
#define sat_rouge 42
#define hue_vert 226
#define sat_vert 53
#define hue_bleu 225
#define sat_bleu 61
#define hue_jaune 228
#define sat_jaune 34
#define hue_orange 247
#define sat_orange 36

#define DIST_MAX    4.0   // distance max (hue + sat) pour accepter une carte
#define NB_CONFIRM  21     // lectures identiques de suite avant de changer la réponse // WHATS 9 + 10??

static Adafruit_TCS34725 tcs(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_60X);

bool couleurInit() {
  return tcs.begin();
}


// Appelle ça avec le capteur devant une feuille blanche, a la bonne distance
void calibrerBlanc() {
  /*uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  blancR = max((uint16_t)1, r);
  blancG = max((uint16_t)1, g);
  blancB = max((uint16_t)1, b);*/
  while (1) {
      const int N = 30;
      uint32_t sumR = 0, sumG = 0, sumB = 0;

      for (int i = 0; i < N; i++) {
          uint16_t r, g, b, c;
          tcs.getRawData(&r, &g, &b, &c);
          sumR += r;
          sumG += g;
          sumB += b;
          delay(60);
      }

      Serial.print("#define BLANC_R ");
      Serial.println((float)sumR / N);
      Serial.print("#define BLANC_G ");
      Serial.println((float)sumG / N);
      Serial.print("#define BLANC_B ");
      Serial.println((float)sumB / N);
      Serial.println();
  }
}

bool readFuckingRGB(uint8_t &r, uint8_t &g, uint8_t &b) {
  uint16_t rr, gg, bb, c;
  tcs.getRawData(&rr, &gg, &bb, &c);

  float fr = rr / BLANC_R;
  float fg = gg / BLANC_G;
  float fb = bb / BLANC_B;

  float m = max(fr, max(fg, fb));
  if (m <= 0) return false;

  r = fr / m * 255;
  g = fg / m * 255;
  b = fb / m * 255;
  return true;
}

// Retourne le hue en degrés (0-360), ou -1 si gris (pas de teinte) // GOOGLE SI TU SAIS PAS C'EST QUOI
float rgbToHueBombaclat(uint8_t r, uint8_t g, uint8_t b) {
  uint8_t mx = max(r, max(g, b));
  uint8_t mn = min(r, min(g, b));
  int delta = mx - mn;
  float hue;

  if (delta == 0) {
    return -1;
  }

  if (mx == r) {
    hue = (float)(g - b) / delta;
  } else if (mx == g) {
    hue = 2.0 + (float)(b - r) / delta;
  } else {
    hue = 4.0 + (float)(r - g) / delta;
  }

  hue *= 60.0;
  if (hue < 0) {
    hue += 360.0;
  }
  return hue;
}

float rgbToSaturationPUSSYWAGAN(uint8_t r, uint8_t g, uint8_t b) {
  uint8_t mx = max(r, max(g, b));
  uint8_t mn = min(r, min(g, b));
  if (mx == 0) {
    return 0;
  }
  return (float)(mx - mn) / mx * 100.0;
}

struct RefCouleur { float h; float s; int id; };

static const RefCouleur REFS[] = {
  { hue_blanc,  sat_blanc,  0 },
  { hue_rouge,  sat_rouge,  CARTE_ROUGE },
  { hue_vert,   sat_vert,   CARTE_VERT },
  { hue_bleu,   sat_bleu,   CARTE_BLEU },
  { hue_jaune,  sat_jaune,  CARTE_JAUNE },
  { hue_orange, sat_orange, CARTE_ORANGE },
};

// Appelle ça à chaque tour de loop().
// Retourne 1-5 (carte), ou 0 (blanc / trop loin / rien de clair).
int couleurScan() {
  static int dernier = 0;
  static int compte = 0;
  static int stable = 0;

  int id = 0;
  uint8_t r, g, b;

  if (readFuckingRGB(r, g, b)) {
    float h = rgbToHueBombaclat(r, g, b);
    float s = rgbToSaturationPUSSYWAGAN(r, g, b);

    if (h >= 0) {
      float best = 1e9;
      int bestId = 0;

      for (unsigned i = 0; i < sizeof(REFS) / sizeof(REFS[0]); i++) {
        float dh = fabs(h - REFS[i].h);
        if (dh > 180) dh = 360 - dh;        // wrap-around 0/360
        float ds = s - REFS[i].s;
        float d = sqrt(dh * dh + ds * ds);

        if (d < best) {
          best = d;
          bestId = REFS[i].id;
        }
      }

      if (best <= DIST_MAX) id = bestId;    // si le plus proche est le blanc, id = 0 de toute façon
    }
  }

  // Anti-glitch : il faut NB_CONFIRM lectures identiques de suite
  if (id == dernier) {
    if (compte < NB_CONFIRM) compte++;
  } else {
    dernier = id;
    compte = 1;
  }
  if (compte >= NB_CONFIRM) stable = dernier;

  return stable;
}