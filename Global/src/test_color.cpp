#include <LibRobus.h>

// ---- Doivent etre pareils que dans color.cpp ----
#define AUCUNE          -1
#define JAUNE            2
#define NB_COULEURS      7
#define SEUIL_DISTANCE   0.02
#define DEL_ALLUMEE      LOW
#define DEL_ETEINTE      HIGH

// ---- Viennent de color.cpp ----
extern unsigned int lectureC;
extern char NOMS[NB_COULEURS][10];
extern int PIN_DEL[NB_COULEURS];

void  couleurLireAttendre();
int   couleurClassifier();
int   couleurNouvelle();
void  couleurNouvelEssai();
void  couleurAfficherLecture();
float couleurDistanceEntre(int i, int j);
int   couleurNbCalibrees();
int   couleurId();
void  attendre(unsigned long ms);
void  delMiseAJour();


void attendreEntree()
{
  while (Serial.available() > 0)
    Serial.read();
  while (Serial.available() == 0)
    delMiseAJour();
  delay(20);
  while (Serial.available() > 0)
    Serial.read();
}

// v : le capteur repond, donne des valeurs correctes
void testVerifier()
{
  int id = couleurId();

  Serial.print("ID du capteur : 0x");
  Serial.print(id, HEX);
  if (id == 0x44 || id == 0x4D)
    Serial.println("  OK");
  else
    Serial.println("  ECHEC (fil I2C ?)");

  couleurLireAttendre();
  Serial.print("Lecture : C=");
  Serial.print(lectureC);
  if (lectureC == 0)
    Serial.println("  ECHEC (tout a 0 : DEL eteinte ? interrupteur du capteur ?)");
  else if (lectureC >= 10200)
    Serial.println("  ECHEC (sature : baisser le gain)");
  else
    Serial.println("  OK");

  Serial.print("Couleurs calibrees : ");
  Serial.print(couleurNbCalibrees());
  Serial.print("/");
  Serial.println(NB_COULEURS);
}

// b : bruit, 50 mesures sur la meme surface. Les valeurs doivent bouger tres peu.
void testBruit()
{
  unsigned int minC = 65535, maxC = 0;
  float somme = 0;
  int i;

  Serial.println("Garder le capteur immobile sur une couleur...");
  for (i = 0; i < 50; i++)
  {
    couleurLireAttendre();
    somme += lectureC;
    if (lectureC < minC) minC = lectureC;
    if (lectureC > maxC) maxC = lectureC;
  }
  Serial.print("C moyen = ");   Serial.print(somme / 50, 0);
  Serial.print("  min = ");     Serial.print(minC);
  Serial.print("  max = ");     Serial.print(maxC);
  Serial.print("  variation = ");
  Serial.print(100.0 * (maxC - minC) / (somme / 50 + 1), 1);
  Serial.println(" % (moins de 5 % = bon)");
}

// m : 2 couleurs plus proches que 2 x SEUIL_DISTANCE risquent d'etre confondues
void testMatrice()
{
  int i, j;
  bool probleme = false;

  for (i = 0; i < NB_COULEURS; i++)
    for (j = i + 1; j < NB_COULEURS; j++)
    {
      float d = couleurDistanceEntre(i, j);
      if (d >= 0 && d < 2 * SEUIL_DISTANCE)
      {
        Serial.print("TROP PROCHES : ");
        Serial.print(NOMS[i]);  Serial.print(" / ");  Serial.print(NOMS[j]);
        Serial.print("  distance ");
        Serial.println(d, 4);
        probleme = true;
      }
    }
  if (!probleme)
    Serial.println("OK : toutes les couleurs calibrees sont bien separees");
}

// r : on pose chaque couleur et on verifie que 8 mesures sur 10 sont bonnes
void testReconnaissance()
{
  int i, k;
  int reussis = 0;

  for (i = 0; i < NB_COULEURS; i++)
  {
    Serial.print("Poser le capteur sur ");
    Serial.print(NOMS[i]);
    Serial.println(" puis Entree");
    attendreEntree();

    int bonnes = 0;
    for (k = 0; k < 10; k++)
    {
      couleurLireAttendre();
      if (couleurClassifier() == i)
        bonnes++;
    }
    couleurAfficherLecture();
    if (bonnes >= 8)
    {
      Serial.println("  OK");
      reussis++;
    }
    else
    {
      Serial.print("  ECHEC (");
      Serial.print(bonnes);
      Serial.println("/10 bonnes)");
    }
  }
  Serial.print("Resultat : ");
  Serial.print(reussis);
  Serial.print("/");
  Serial.println(NB_COULEURS);
}

// e : allume chaque DEL 1 s et fait un bip
void testDel()
{
  int i;
  for (i = JAUNE; i < NB_COULEURS; i++)
  {
    Serial.print("DEL ");
    Serial.println(NOMS[i]);
    if (PIN_DEL[i] < 0)
    {
      Serial.println("  pas de broche");
      continue;
    }
    digitalWrite(PIN_DEL[i], DEL_ALLUMEE);
    delay(1000);
    digitalWrite(PIN_DEL[i], DEL_ETEINTE);
  }
  AX_BuzzerON();
  delay(200);
  AX_BuzzerOFF();
}

// d : comme en course. A appeler a chaque tour de loop() quand le mode est actif.
void testDetection()
{
  int couleur = couleurNouvelle();
  if (couleur != AUCUNE)
  {
    Serial.print("Carton : ");
    Serial.println(NOMS[couleur]);
    AX_BuzzerON();
    attendre(100);
    AX_BuzzerOFF();
  }
}
