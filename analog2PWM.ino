/*
   hawa 09.02.2022
   Board: Arduino Nano V3.0
*/
const char SysVer[] = "analog2PWM 0.21";                    // System-Version; immer anpassen!!!
/*
   Beschreibung des Programms und der Änderungshistorie:
   Das PC Mainboard MSI X99A TOMAHAWK hat mehrere (5) steuerbare Lüfteranschlüsse aber davon
   nur einen mit 4-Pin PWM Steuerung (CPUFan1). Alle anderen werden nur analog über die
   Spannung gesteuert (3-Pin Lüfteranschluß). Diese sollen über diese Schaltung in eine PWM
   Steuerung gewandelt werden um leisere PWM Lüfter einbauen zu können.
   Für mich relevant sind nur 3 davon; SysFan1, SysFan2, SysFan3.

   Die analoge Meß-Eingangsspannung ist zwischen 0V und +12V und stammt von der 3-Pin Lüfteran-
   schlüssen. Dazu dient ein Spannungsteiler auf 5V analoge Meßspannung 47kΩ + 33kΩ 
   mit 810Ω als Minimallast direkt am Lüfteranschluß.
   WICHTIG: die Bezeichnung der Lüfteranschlüsse im MSI Command Center stimmt nicht mit den
   Bezeichnungen der Anschlüsse auf dem Board und den Bezeichnungen im BIOS überein.
   SysFan2 und SysFan3 sind hier vertauscht.

   V0.12: Erweiterung auf 5 Kanäle im Code vorbereitet aber nicht voll verdrahtet auf dem PCB.
   Theoretisch wären bis zu 6 Kanäle möglich mit den 6 PWM Pins des Arduino Nano.

   V0.20: Als präzisere Referenzspannung dient nun ein LM4040 mit 2,048V an AREF.
   Der Spannungsteiler für die Meßeingänge wird verändert auf 1kΩ + 200Ω und der
   Lastwiderstand (810Ω) entfällt.
   Der Spannungsteiler ist so dimensioniert das bei maximaler Steuerspannung für die Lüfter
   der digitale Meßwert 1023 anliegt; entspricht bei 12,29V dem Verhältnis 10,242V zu 2,048V an
   den beiden Widerständen des Spannungsteilers.
*/

/*
   Definition der Variablen und Namen/Ports für I/O
*/
// I) includes:


// II) Belegung der analogen und digitalen I/O-PINs:
//      hier für Arduino Nano!
// A0 bis A5 könenn auch als Digitale I/O-Pins gentzt werden.
// A6 und A7 des Nano nur als analoge I/O-Pins!
//
const int SysFan1_A = 0;
const int SysFan2_A = 1;
const int SysFan3_A = 2;
//const int PumpFan1_A = 3;
// A4 und A5 bleiben reserviert für mögliche I2C Erweiterungen
// A6 = ?                         //
// const int PumpFan2_A = 6;
// A7 = ?                         //
//
// D0 und D1 bleiben frei für USB-Schnittstelle (Rx/Tx)
// D2 = ?                         //
// D3 = ? (PWM)                   //
//const int PumpFan1_P = 3;
// D4 = ?                         //
// D5 = ? (PWM)                   //
// D6 = ? (PWM)                   //
// const int PumpFan2_P = 6;
// D7 = ?                         //
// D8 = ?                         //
// D9 = ? (PWM)                   //
const int SysFan3_P = 9;
// D10 = ? (PWM)                  //
const int SysFan2_P = 10;
// D11 = ? (PWM)                  //
const int SysFan1_P = 11;
// D12 = ?                        //
// D13 = ?  (Achtung: LED an GND) //
const int LED = 13;

// III) Definition von Variablen und Initialisierung:
int PWM_Val = 0;
bool LED_X = false;
float Voltage_A = 0.0;
float PWM_P = 0.0;

/*
   Los geht's mit code:
*/

void setup() {
  Serial.begin(115200);
  Serial.print(F("## System-Version: "));
  Serial.println(SysVer);
  Serial.println(F("Setup"));


  // initialisiere PINs:
  analogReference(EXTERNAL);
  pinMode (SysFan1_P, OUTPUT);
  pinMode (SysFan2_P, OUTPUT);
  pinMode (SysFan3_P, OUTPUT);
  // pinMode (PumpFan1_P, OUTPUT);
  // pinMode (PumpFan2_P, OUTPUT);
  pinMode (LED, OUTPUT);
  digitalWrite(LED, LED_X);
  digitalWrite(SysFan1_P, true);
  digitalWrite(SysFan2_P, true);
  digitalWrite(SysFan3_P, true);
  // digitalWrite(PumpFan1_P, true);
  // digitalWrite(PumpFan2_P, true);
  
  // am Anfang die Lüfter einfach mal mit maximaler Drehzahl laufen lassen.
  delay(5555);

  Serial.println(F("...done"));
}  // end setup

/*
  HAUPT-ROUTINE
*/

void loop() {
  CalcIt (SysFan1_A, SysFan1_P);
  CalcIt (SysFan2_A, SysFan2_P);
  CalcIt (SysFan3_A, SysFan3_P);
  //CalcIt (PumpFan1_A, PumpFan1_P);
  //CalcIt (PumpFan2_A, PumpFan2_P);
  Serial.println();

  delay(1000);
  LED_X = !LED_X;
  digitalWrite(LED, LED_X);
}  // end loop

/*
   EIGENE ROUTINEN
*/

void CalcIt(int Port_A, int Port_P) {
  PWM_Val = analogRead(Port_A);        // lies Analogeingang
  Voltage_A = float(PWM_Val) / 83.24;  // berechne Analogwert als Spannung (1023/12,29)
  PWM_Val /= 4;                        // wandle analog-read range (0-1023) in PWM range (0-255)
  PWM_P = float(PWM_Val) / 2.56;       // berechne PWM Wert in %

  switch (Port_A) {
    case SysFan1_A:
      Serial.print(F("SysFan1:  "));
      break;
    case SysFan2_A:
      Serial.print(F("SysFan2:  "));
      break;
    case SysFan3_A:
      Serial.print(F("SysFan3:  "));
      break;
      //    case PumpFan1_A:
      //    Serial.print(F("PumpFan1: "));
      //  break;
      //    case PumpFan2_A:
      //    Serial.print(F("PumpFan2: "));
      //  break;
  }
  Serial.print(Voltage_A);
  Serial.print(F("V   "));
  Serial.print(F("PWM:  "));
  Serial.print(PWM_Val);
  Serial.print(F(" = "));
  Serial.print(PWM_P);
  Serial.println(F("%"));

  analogWrite(Port_P, PWM_Val);
}
