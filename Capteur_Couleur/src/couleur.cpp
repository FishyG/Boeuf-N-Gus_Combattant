#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_TCS34725.h>


/* ============================================================================
   DEFINES ET VARIABLES
   ============================================================================ */


#define AUCUNE        -1
#define PLANCHER       0
#define NOIR           1
#define JAUNE          2
#define ORANGE         3
#define BLEU           4
#define TURQUOISE      5
#define ROSE           6
#define NB_COULEURS    7

char NOMS[NB_COULEURS][10] = {"PLANCHER", "NOIR", "JAUNE", "ORANGE", "BLEU", "TURQUOISE", "ROSE"};


int PIN_DEL[NB_COULEURS] = {-1, -1, 30, 31, 32, 33, 34};

#define DUREE_DEL          10000     
#define TEMPS_MESURE       24        
#define NB_CONFIRMATIONS   3         
#define NB_MESURES_CALIB   20
#define SEUIL_DISTANCE     0.02     
#define POIDS_LUMINOSITE   0.5
#define EEPROM_MAGIQUE     0x4C33    


Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_4X);

unsigned int lectureC, lectureR, lectureG, lectureB;   


float refR[NB_COULEURS], refG[NB_COULEURS], refB[NB_COULEURS], refC[NB_COULEURS];

float distance;                      
bool dejaFaite[NB_COULEURS];        
unsigned long tempsDel[NB_COULEURS]; 

int candidate = AUCUNE;
int compteur = 0;
unsigned long tempsMesure = 0;

bool couleurCharger();


/* ============================================================================
   FONCTIONS POUR LA COURSE
   ============================================================================ */

void couleurLire()
{
  // read16() ne bloque pas (getRawData() attendrait 24 ms a chaque appel)
  lectureC = tcs.read16(TCS34725_CDATAL);
  lectureR = tcs.read16(TCS34725_RDATAL);
  lectureG = tcs.read16(TCS34725_GDATAL);
  lectureB = tcs.read16(TCS34725_BDATAL);
}


void couleurLireAttendre()
{
  delay(TEMPS_MESURE + 2);
  couleurLire();
}


float couleurDistance(float r, float g, float b, float c, int i)
{
  float somme = r + g + b;
  if (somme == 0)
    somme = 1;
  float dr = r / somme - refR[i];
  float dg = g / somme - refG[i];
  float db = b / somme - refB[i];
  float dl = (c - refC[i]) / (c + refC[i] + 1);
  return dr * dr + dg * dg + db * db + POIDS_LUMINOSITE * dl * dl;
}


int couleurClassifier()
{
  int meilleure = AUCUNE;
  int i;

  distance = 1000;
  for (i = 0; i < NB_COULEURS; i++)
  {
    if (refC[i] == 0)
      continue;
    float d = couleurDistance(lectureR, lectureG, lectureB, lectureC, i);
    if (d < distance)
    {
      distance = d;
      meilleure = i;
    }
  }

  if (distance > SEUIL_DISTANCE)
    return AUCUNE;
  return meilleure;
}


int couleurNouvelle()
{
  int c;

  if (millis() - tempsMesure < TEMPS_MESURE)
    return AUCUNE;
  tempsMesure = millis();

  couleurLire();
  c = couleurClassifier();

  if (c != candidate)
  {
    candidate = c;
    compteur = 0;
  }
  if (compteur < NB_CONFIRMATIONS)
    compteur++;

  if (compteur >= NB_CONFIRMATIONS && c >= JAUNE && !dejaFaite[c])
  {
    dejaFaite[c] = true;
    tempsDel[c] = millis();
    digitalWrite(PIN_DEL[c], HIGH);
    return c;
  }
  return AUCUNE;
}


void couleurNouvelEssai()
{
  int i;
  for (i = 0; i < NB_COULEURS; i++)
    dejaFaite[i] = false;
}


void delMiseAJour()
{
  int i;
  for (i = JAUNE; i < NB_COULEURS; i++)
    if (digitalRead(PIN_DEL[i]) == HIGH && millis() - tempsDel[i] >= DUREE_DEL)
      digitalWrite(PIN_DEL[i], LOW);
}


void attendre(unsigned long ms)
{
  unsigned long debut = millis();
  while (millis() - debut < ms)
    delMiseAJour();
}


bool couleurInit()
{
  int i;
  for (i = JAUNE; i < NB_COULEURS; i++)
  {
    pinMode(PIN_DEL[i], OUTPUT);
    digitalWrite(PIN_DEL[i], LOW);
  }

  if (!tcs.begin())
  {
    Serial.println("Capteur de couleur introuvable (verifier le fil I2C)");
    return false;
  }
  if (!couleurCharger())
    Serial.println("Pas de calibration dans l'EEPROM");
  return true;
}


/* ============================================================================
   FONCTIONS POUR LA CALIBRATION
   ============================================================================ */

void couleurSauvegarder()
{
  int adresse = 512;
  EEPROM.put(adresse, (int)EEPROM_MAGIQUE);  adresse += sizeof(int);
  EEPROM.put(adresse, refR);                 adresse += sizeof(refR);
  EEPROM.put(adresse, refG);                 adresse += sizeof(refG);
  EEPROM.put(adresse, refB);                 adresse += sizeof(refB);
  EEPROM.put(adresse, refC);
  Serial.println("Sauvegarde");
}

bool couleurCharger()
{
  int adresse = 512;
  int magique;
  EEPROM.get(adresse, magique);              adresse += sizeof(int);
  if (magique != EEPROM_MAGIQUE)
    return false;
  EEPROM.get(adresse, refR);                 adresse += sizeof(refR);
  EEPROM.get(adresse, refG);                 adresse += sizeof(refG);
  EEPROM.get(adresse, refB);                 adresse += sizeof(refB);
  EEPROM.get(adresse, refC);
  return true;
}


void couleurEffacer()
{
  int i;
  for (i = 0; i < NB_COULEURS; i++)
  {
    refR[i] = 0;
    refG[i] = 0;
    refB[i] = 0;
    refC[i] = 0;
  }
  couleurSauvegarder();
  Serial.println("Calibration effacee");
}


void couleurCalibrer(int couleur)
{
  float sommeR = 0, sommeG = 0, sommeB = 0, sommeC = 0, total;
  int i;

  if (couleur < 0 || couleur >= NB_COULEURS)
    return;

  for (i = 0; i < NB_MESURES_CALIB; i++)
  {
    couleurLireAttendre();
    sommeR += lectureR;
    sommeG += lectureG;
    sommeB += lectureB;
    sommeC += lectureC;
  }
  total = sommeR + sommeG + sommeB + 1;
  refR[couleur] = sommeR / total;
  refG[couleur] = sommeG / total;
  refB[couleur] = sommeB / total;
  refC[couleur] = sommeC / NB_MESURES_CALIB + 1;

  Serial.print(NOMS[couleur]);
  Serial.println(" calibre (envoyer s pour sauvegarder)");
  if (sommeC / NB_MESURES_CALIB >= 10200)
    Serial.println("ATTENTION : sature, baisser le gain");
}

void couleurAfficherCalibration()
{
  int i;
  for (i = 0; i < NB_COULEURS; i++)
  {
    Serial.print(i);  Serial.print(" ");  Serial.print(NOMS[i]);  Serial.print(" : ");
    Serial.print(refR[i], 3);  Serial.print(" ");
    Serial.print(refG[i], 3);  Serial.print(" ");
    Serial.print(refB[i], 3);  Serial.print(" ");
    Serial.println(refC[i], 0);
  }
}


float couleurDistanceEntre(int i, int j)
{
  if (refC[i] == 0 || refC[j] == 0)
    return -1;
  return couleurDistance(refR[i], refG[i], refB[i], refC[i], j);
}

int couleurNbCalibrees()
{
  int i, nb = 0;
  for (i = 0; i < NB_COULEURS; i++)
    if (refC[i] != 0)
      nb++;
  return nb;
}


int couleurId()
{
  return tcs.read8(TCS34725_ID);
}

void couleurAfficherNoms()
{
  int i;
  for (i = 0; i < NB_COULEURS; i++)
  {
    Serial.print("  ");  Serial.print(i);  Serial.print(" = calibrer ");  Serial.println(NOMS[i]);
  }
}

void couleurAfficherLecture()
{
  int c = couleurClassifier();
  Serial.print("C=");   Serial.print(lectureC);
  Serial.print(" R=");  Serial.print(lectureR);
  Serial.print(" G=");  Serial.print(lectureG);
  Serial.print(" B=");  Serial.print(lectureB);
  Serial.print(" => ");
  if (c == AUCUNE)
    Serial.print("AUCUNE");
  else
    Serial.print(NOMS[c]);
  Serial.print(" (distance ");
  Serial.print(distance, 4);
  Serial.println(")");
}
