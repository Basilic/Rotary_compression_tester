#include <EEPROM.h>

  // Configuration du montage par defaut
  double ZeroBar=0.51; // Tension a pression Atmosphérique
  double AnalogueRef=4.2; //Tension de Référence
  double Facteur_conversion=50; // Facteur tension pression
  byte AnalogPin=2; // pin de lecture de la sonde

  // Variable utilisé dans les routine
  String analogtype="EXTERNAL";
  String inputString = "";
  int AnalogValeur=0;
  double  Valeur=0;
  double  Valeur2=0;
  byte Value;
  unsigned long StartTime=0;
  
  
void setup() {
 
  Value = EEPROM.read(0); // lecture du bit de sauvegarde
  if(Value==1){ // si bit a 1 alors des donnée ont été sauvegardé on lit l'ensemble des données
    ZeroBar =EEPROM_readDouble(10);
    AnalogueRef = EEPROM_readDouble(20);
    Facteur_conversion =EEPROM_readDouble(30);
    AnalogPin = EEPROM.read(40);
    Value = EEPROM.read(2);
    if (Value==0){
      analogtype="EXTERNAL";
    }else{
      analogtype="INTERNAL";
    }
  }
  // initialize both serial ports:
  Serial.begin(115200,SERIAL_8E1);
  analogReference(EXTERNAL);
  if (analogtype=="INTERNAL"){
    analogReference(DEFAULT);
  }
  }

void loop() {
  
  // read from port 0, send to port 1:
    AnalogValeur= analogRead(AnalogPin);
    StartTime=micros();
    Valeur=abs((((AnalogValeur*AnalogueRef)/1023.0))-ZeroBar)*Facteur_conversion; 
    Serial.println(String(Valeur,0)+";"+String(StartTime));
    delay(1);    
}

void serialEvent() {
// réceptionne les trames jusqu'a un caractére de fin de ligne
  while (Serial.available()) {
    // arrivé d'une octet
    char inChar = (char)Serial.read();
    // Ajout d'un a inputString:
     if (inChar == '\n') {
      // Si fin de ligne lancé la routine d'action
      action_serie();
    }else{
    inputString += inChar;
    }
  }
}

void action_serie(){
  int nb_step=0;
  int i=0;
    if (inputString == "CALIBRE=0"){
      ZeroBar=analogRead(AnalogPin);
    }else if (inputString == "DEFAULT_RESET"){
      ZeroBar=0.51;
      AnalogueRef=4.2;
      Facteur_conversion=50;
      AnalogPin = 2;
      EEPROM.write(0,0); // ignoré la configurations sauvé
    }else if (inputString.substring(0,11) == "CONVERSION="){
      // Configure le facteur de conversion
      Facteur_conversion=inputString.substring(11).toFloat();
    }else if (inputString.substring(0,10) == "ANALOGREF="){
      if (inputString.substring(10)=="DEFAULT"){
        // configure la source interne
        analogtype="INTERNAL";
        analogReference(DEFAULT);
       
      }else if(inputString.substring(10)=="EXTERNAL"){
        // configure la source externe
        analogtype="EXTERNAL";
        analogReference(EXTERNAL);
      }else{
      // configure la tension de référence
      AnalogueRef=inputString.substring(10).toFloat();
      }
    }else if (inputString.substring(0,10) == "ANALOGPIN="){
      AnalogPin=inputString.substring(10).toInt();
    }else if (inputString == "SAVE"){
      EEPROM_writeDouble(10,ZeroBar);
      EEPROM_writeDouble(20,AnalogueRef);
      EEPROM_writeDouble(30,Facteur_conversion);
      EEPROM.write(40,AnalogPin);
      if(analogtype=="EXTERNAL"){
        EEPROM.write(2,0);
      }else{
        EEPROM.write(2,1);
      }
      EEPROM.write(0,1); // indique d'une configuration est sauvé
    }
  }

void EEPROM_writeDouble(int ee, double value){
   byte* p = (byte*)(void*)&value;
   for (int i = 0; i < sizeof(value); i++)
       EEPROM.write(ee++, *p++);
  }

double EEPROM_readDouble(int ee){
   double value = 0.0;
   byte* p = (byte*)(void*)&value;
   for (int i = 0; i < sizeof(value); i++)
       *p++ = EEPROM.read(ee++);
   return value;
  }

