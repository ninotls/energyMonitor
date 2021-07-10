#include "EmonLib.h"
#include <LiquidCrystal_I2C.h>

EnergyMonitor emon;
LiquidCrystal_I2C lcd(0x27, 20, 4);

float w_instantane_in  = 0.0;
float w_instantane_out = 0.0;
float kwh_cumule_out   = 0.0;
float kwh_cumule_in    = 0.0;               
float puissance_reelle = 0.0;
float cosinus_phi      = 0.0;

unsigned long previous_millis = 0;   // 2 création de la variable "previous_millis" qui garde en mémoire le temps qui s'écoule en millièmes de seconde"

//-----------------------INITIALISATION DU PROGRAMME-------------------------------------------------

void setup()
{
  // ### Etalonnage ###
  // Serial.begin(9600);

  emon.voltage(0, 208, 1.7);         // 1 Initialisation du Voltage (Pin A0, Valeur à changer pour etalonnage (+/-357 pour 6v et +/- 190 pour 12v))
  emon.current(2, 27);             // 1 Initialisation du Courant en ampère ( Pin A2, Valeur à changer lors de l'etalonnage)

  lcd.init();                         // 2 initialisation de l'afficheur LCD
  lcd.backlight();

  //1ere ligne
  lcd.setCursor(0, 0);
  lcd.print("Conso:");
  lcd.setCursor(11, 0);
  lcd.print("Wts Cos_");
  lcd.print((char) 236);

  //2eme ligne
  lcd.setCursor(0, 1);
  lcd.print("Rejet:"); 
  lcd.setCursor(11, 1);
  lcd.print("Wts ");

  //3eme ligne
  lcd.setCursor(0, 2);
  lcd.print("Cumul conso:"); 
  lcd.setCursor(17, 2);
  lcd.print("kWh");

  //4eme ligne
  lcd.setCursor(0, 3);
  lcd.print("Cumul rejet:"); 
  lcd.setCursor(17, 3);
  lcd.print("kWh");

  // Init LED Rouge
  pinMode(4, OUTPUT);
  // Init LED Jaune
  pinMode(7, OUTPUT);
  // Init LED Verte
  pinMode(10, OUTPUT);
  // On allume toutes les leds
  digitalWrite(4, HIGH);
  delay(500);
  digitalWrite(7, HIGH);
  delay(500);
  digitalWrite(10, HIGH);
  delay(500);
  // Puis on les éteins
  digitalWrite(4, LOW);
  digitalWrite(7, LOW);
  digitalWrite(10, LOW);  
}
 //----------------------- DEMARRAGE DE LA BOUCLE----------------------------------------------------

void loop()
{
  emon.calcVI(40, 2000);
  puissance_reelle = emon.realPower;
  cosinus_phi = emon.powerFactor;
  
  // ### Etalonnage ###
  // float verif_voltage    = emon.Vrms;        //1 creation de la variable "volts moyen" (mesurable avec un voltmètre pour l'etalonnage)
  // float verif_ampere     = emon.Irms;        //1 creation de la variable "Ampères Moyen" (mesurable avec une pince ampèremétrique pour l'etalonnage))

 //--------------------------Etalonnage des volts et ampères sans LCD--------------------------------------

  // ### Etalonnage ###
  //  Serial.print("Est-ce le bon voltage? ");      // 1 envoyer vers l'ordinateur le texte " Est-ce le bon voltage? "
  //  Serial.print(verif_voltage);                  // 1 envoyer vers l'ordinateur la valeur "verif_voltage (Vrms)"
  //  Serial.print(" V  ");                         // 1 envoyer vers l'ordinateur le caractère "V"
  //  Serial.print(verif_ampere);                   // 1 envoyer vers l'ordinateur la valeur "verif_voltage (Vrms)"                    
  //  Serial.print(" A ");                          // 1 envoyer vers l'ordinateur le caractère "A"
  //  Serial.print("\n");
     
 //----------------POUR AVOIR LES W, Wh et kWh de l'élélectricité qui rentre et de l'électricité qui sort de ma maison------------------
  
  if (puissance_reelle >= 0)                      // 2 Si la puissance reelle est positive, (c'est que je consomme et qu'a priori il n'y a pas de soleil)
  { 
    w_instantane_in  = puissance_reelle;         // 2 alors on dit que la puissance instantanée entrante (in) est egale à la puissance reelle.
    w_instantane_out = 0.0;                     // 2 dans ces conditions de consommation (positive) , la valeur de la surproduction est nulle.

    kwh_cumule_in += (puissance_reelle / 1000) * (millis() - previous_millis) / 3600000;

    digitalWrite(4, LOW);
    digitalWrite(7, LOW);
    digitalWrite(10, LOW);
  }  
  else                                             // 2 SINON (c'est que la puissance_reelle est négative)
  {
    w_instantane_in = 0.0;                       // 2 idem au dessus
    w_instantane_out = abs(puissance_reelle);    // 2 idem au dessus

    if (w_instantane_out < 100) {
        digitalWrite(4, LOW);
        digitalWrite(7, LOW);
        digitalWrite(10, HIGH);
    } else if (w_instantane_out < 300) {
        digitalWrite(4, LOW);
        digitalWrite(7, HIGH);
        digitalWrite(10, LOW);
    } else {
        digitalWrite(4, HIGH);
        digitalWrite(7, LOW);
        digitalWrite(10, LOW);
    }
  
    kwh_cumule_out += (abs(puissance_reelle) / 1000) * (millis() - previous_millis) / 3600000;
  }

  previous_millis = millis();
    
// --------------------ECRIRE SUR LCD ET POUR CHANGER LES VALEUR DuLCD AVEC UN BOUTON POUSSOIR------------------------------------------------
  //1ere ligne
  lcd.setCursor(6, 0);
  lcd.print("     ");
  if (w_instantane_in < 1) {
    lcd.setCursor(9, 0);
  } 
  else {
    lcd.setCursor(10 - log10(w_instantane_in), 0);     
  }
  lcd.print(w_instantane_in, 0);

  //2eme ligne
  lcd.setCursor(6, 1);
  lcd.print("     ");
  if (w_instantane_out < 1) {
    lcd.setCursor(9, 1);
  } 
  else {
    lcd.setCursor(10 - log10(w_instantane_out), 1);
  }
  lcd.print(w_instantane_out, 0);
  lcd.setCursor(15, 1);
  lcd.print("   ");
  lcd.setCursor(15, 1);
  lcd.print(abs(cosinus_phi), 2);

  //3eme ligne
  lcd.setCursor(12, 2);
  lcd.print("     ");
  if (kwh_cumule_in < 10) {
    lcd.setCursor(15, 2);
  }
  else {
    lcd.setCursor(15 - floor(log10(kwh_cumule_in)), 2);
  }    
  lcd.print(kwh_cumule_in, 0);

  //4eme ligne
  lcd.setCursor(12, 3);
  lcd.print("     ");
  if (kwh_cumule_out < 10) {
    lcd.setCursor(15, 3);
  }
  else {
    lcd.setCursor(15 - floor(log10(kwh_cumule_out)), 3);
  }
  lcd.print(kwh_cumule_out, 0);
}
