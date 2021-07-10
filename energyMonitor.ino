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

unsigned long previous_millis = 0;

//-----------------------INITIALISATION DU PROGRAMME-------------------------------------------------

void setup()
{
  // ### Etalonnage ###
  // Serial.begin(9600);

  emon.voltage(0, 208, 1.7);
  emon.current(2, 27); 

  lcd.init();
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
  // float verif_voltage    = emon.Vrms; 
  // float verif_ampere     = emon.Irms; 

 //--------------------------Etalonnage des volts et ampères sans LCD--------------------------------------

  // ### Etalonnage ###
  //  Serial.print("Est-ce le bon voltage? ");
  //  Serial.print(verif_voltage); 
  //  Serial.print(" V  ");
  //  Serial.print(verif_ampere);                  
  //  Serial.print(" A ");
  //  Serial.print("\n");
     
 //----------------POUR AVOIR LES W, Wh et kWh de l'élélectricité qui rentre et de l'électricité qui sort de ma maison------------------
  
  if (puissance_reelle >= 0) 
  {
    w_instantane_in  = puissance_reelle;
    w_instantane_out = 0.0;

    kwh_cumule_in += (puissance_reelle / 1000) * (millis() - previous_millis) / 3600000;

    digitalWrite(4, LOW);
    digitalWrite(7, LOW);
    digitalWrite(10, LOW);
  }  
  else
  {
    w_instantane_in = 0.0;
    w_instantane_out = abs(puissance_reelle);

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
