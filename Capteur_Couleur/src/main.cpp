

#include <LibRobus.h>

// ---- Viennent de couleur.cpp ----
bool couleurInit();
void couleurLire();
void couleurCalibrer(int couleur);
void couleurSauvegarder();
void couleurEffacer();
void couleurAfficherCalibration();
void couleurAfficherNoms();
void couleurAfficherLecture();
void couleurNouvelEssai();
void delMiseAJour();
void attendre(unsigned long ms);

// ---- Viennent de test.cpp ----
void testVerifier();
void testBruit();
void testMatrice();
void testReconnaissance();
void testDel();
void testDetection();

bool lectureContinue = false;
bool modeDetection = false;

void afficherMenu()
{
  Serial.println();
  Serial.println("=== CAPTEUR DE COULEUR (le robot ne bouge pas) ===");
  Serial.println("Calibration :");
  couleurAfficherNoms();
  Serial.println("  s = sauvegarder   a = afficher la calibration   x = tout effacer");
  Serial.println("Tests :");
  Serial.println("  v = verifier le capteur      b = bruit (capteur immobile)");
  Serial.println("  r = reconnaissance de chaque couleur");
  Serial.println("  m = couleurs trop proches ?  e = DEL + buzzer");
  Serial.println("  l = lecture continue ON/OFF  d = detection comme en course ON/OFF");
  Serial.println("  h = ce menu");
}

void setup()
{
  BoardInit();
  while (!couleurInit())
    delay(1000);
  afficherMenu();
}

void loop()
{
  char commande;

  delMiseAJour();

  if (Serial.available() > 0)
  {
    commande = Serial.read();
    if (commande >= '0' && commande <= '9')
      couleurCalibrer(commande - '0');
    else if (commande == 's')
      couleurSauvegarder();
    else if (commande == 'a')
      couleurAfficherCalibration();
    else if (commande == 'x')
      couleurEffacer();
    else if (commande == 'v')
      testVerifier();
    else if (commande == 'b')
      testBruit();
    else if (commande == 'r')
      testReconnaissance();
    else if (commande == 'm')
      testMatrice();
    else if (commande == 'e')
      testDel();
    else if (commande == 'l')
      lectureContinue = !lectureContinue;
    else if (commande == 'd')
    {
      modeDetection = !modeDetection;
      couleurNouvelEssai();
    }
    else if (commande == 'h')
      afficherMenu();
  }

  if (modeDetection)
    testDetection();

  if (lectureContinue)
  {
    couleurLire();
    couleurAfficherLecture();
    attendre(200);
  }
}
