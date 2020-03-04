#include <EEPROM.h>  // Read/Write in EEPROM
#include <LiquidCrystal.h>  // LCD management

//******************************************************//
// Define Constante of project                          //
//******************************************************//
// Note with 4.1 Voltage Reference the maximum pressure is arround 180PSI (12.4Bar) using a linear 5V 0-200PSI sensor with 0,5V offset 
// this value is over the maximum compresion of engine at 300RPM (maximun mazda graph is 135PSI)

//#define Internal_Ref // Uncomment this if you not use LM4040-4.1 on AREF pin


#ifndef Internal_Ref
  #define Voltage_ref 4.096            /**< Use LM4040-4.1 voltage Referenc */
  #define Voltage_Reference EXTERNAL   /**< Use External AnalogReference */
  #define Default_Offset_Value 127     /**< Set Default 0,5V value */
  #define Default_Calibration_Value 50 /**< Default Volt to PSI conversion */
# else
  #define Voltage_ref 5                /**< Use VCC voltage */
  #define Voltage_Reference DEFAULT    /**< Use Default AnalogReference */
  #define Default_Offset_Value 103     /**< Set Default 0,5V value */
  #define Default_Calibration_Value 50 /**< Default Volt to PSI conversion */
#endif
#define eeAddress_IsWrite 0  /**< address where store iswrite flag*/
#define eeAddress_Offset 2  /**< address where store Offset value*/
#define eeAddress_CalibrationPSI 4 /**< address where store CalibrationPSI value*/

//******************************************************//
// Pin Define                                           //
//******************************************************//
#define LCD_ENABLE 3  /**< Pin to enable LCD display */
#define Sensor_Pin A0 /**< Pin where connect the pressure sensor*/
#define BtReset A1    /**< Pin where connect the Reset Button Of LCD*/
#define BtPlus  A2    /**< Pin where connect the Plus Button Of LCD*/
#define BtConfig A3   /**< Pin where connect the Config Button Of LCD*/
#define BtMinus A4    /**< Pin where connect the Minus Button Of LCD*/
#define BtStart A5    /**< Pin where connect the Start Button Of LCD*/

LiquidCrystal lcd(2,4,9,10,11,12); /**< Initialisation for LCD lib use, see liquidCrystal documentation */

//******************************************************//
// Global var                                           //
//******************************************************//
  int Offset;                /**< 0,50V with 4.1V reference */
  int CalibrationPSI;          /**< Default conversion 1V=50PSI*/
  String inputString;         /**< a String to hold incoming data */
  bool stringComplete;     /**< Flag when the string is complete */
  bool Mesure_run;           /**< Flag Run mesure */
  bool runTest;              /**< flag Run Test for see if calibration is OK */


/**
 * \brief Configure micro controler
 * 
 */
void setup() {
  pinMode(LCD_ENABLE,OUTPUT);          /**< Set to output the Enable pin of LCD */
  pinMode(BtReset,INPUT);              /**< Set to input Reset button pin of LCD */ 
  pinMode(BtPlus,INPUT);               /**< Set to input Plus button pin of LCD */ 
  pinMode(BtConfig,INPUT);             /**< Set to input Config button pin of LCD */
  pinMode(BtMinus,INPUT);              /**< Set to input Minus button pin of LCD */ 
  pinMode(BtStart,INPUT);              /**< Set to input Start button pin of LCD */
  Serial.begin(115200,SERIAL_8E1);     /**< Initilise Serial Port*/
  analogReference(Voltage_Reference);  /**< Set analog Voltage*/
  Init_Value();                          /**< Init the global value*/
  }


/**
 * \brief Main loop
 * 
 */
void loop() {
  if (stringComplete)
  {
    if (inputString=="Start")
    {
      Mesure_run=true;
      Serial_Mesure();
    }
    if (inputString=="Stop") runTest=false;
    if (inputString=="Test") runTest=true;
    if (inputString== "SetOffset") SetOffset();
    if (inputString== "SetCalibration6") SetCalibration6();
    if (inputString== "SetCalibration8") SetCalibration8();
    if (inputString== "SetCalibration10") SetCalibration10();
    if (inputString== "SetCalibration60PSI") SetCalibration60PSI();
    if (inputString== "SetCalibration80PSI") SetCalibration80PSI();
    if (inputString== "SetCalibration100PSI") SetCalibration100PSI();
    if (inputString== "SetCalibration150PSI") SetCalibration150PSI();
    inputString="";
    stringComplete=false;
  }
  if(runTest) RunSerial_Test();
  delay(100);
}

/**
 *\brief Run a signel measure of PSI
 */
void RunSerial_Test(){
  int AnalogValeur = 0; /**< Result of the ADC conversion*/
  double  Valeur = 0;  /**< Result of pressure calculation*/
  AnalogValeur= analogRead(Sensor_Pin); /**< Get ADC value */
  if(AnalogValeur<=Offset)
  {
    Valeur=0;
  }
  else
  {
    Valeur=((((AnalogValeur-Offset)*Voltage_ref)/1024)*CalibrationPSI); 
  }
  Serial.println("Test:"+String(Valeur,0));
}


void Serial_Mesure(){
  int AnalogValeur = 0; /**< Result of the ADC conversion*/
  double  Valeur = 0;  /**< Result of pressure calculation*/
  unsigned long StartTime; /**< Timelaps for calculation time between 2 conversions */
  while(Mesure_run)
  {
    AnalogValeur= analogRead(Sensor_Pin); /**< Get ADC value */
    StartTime=micros();  
    if(AnalogValeur<=Offset)
    {
      Valeur=0;
    }
    else
    {
      Valeur=((((AnalogValeur-Offset)*Voltage_ref)/1024)*CalibrationPSI); 
    }
    Serial.println(String(Valeur,0)+";"+String(StartTime));
    delay(1);
  } 
}


/**
 * \brief Initialise global value
 * 
 * Set default value
 * Read data from EEPROM write and default value if null
 */
void Init_Value(){
  int IsWrite;                /**< Temporary value to indicate if a data is already write*/
  inputString = "" ;           /**< Init to empty string */
  stringComplete = false;     /**< Init Flag when the string is complete */
  Mesure_run=false;           /**< Init Flag Run mesure */
  runTest=false;              /**< Init Flag Run Test for see if calibration is OK */
  EEPROM.get(eeAddress_IsWrite,IsWrite); /**< Read iswrite from EEPROM */
  EEPROM.get(eeAddress_Offset, Offset);  /**< Read Offset from EEPROM */
  EEPROM.get(eeAddress_CalibrationPSI, CalibrationPSI); /**< Read CalibrationPSI from EEPROM */
  if (IsWrite != 1 or CalibrationPSI == 0){
      CalibrationPSI=Default_Calibration_Value;
      Offset=Default_Offset_Value;
      EEPROM.write(eeAddress_Offset,Default_Offset_Value);
      EEPROM.write(eeAddress_CalibrationPSI,Default_Calibration_Value);
      EEPROM.write(eeAddress_IsWrite,1);
  }
}

/**
 * \brief Get data from serialEvent
 *
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      Mesure_run = false;
    }
  }
}

/**
 * \brief Offset calculation
 * 
 * Read 10 time the value and do an int average
 * Write the value to EEPROM
 */
void SetOffset(){
  double Calcul_offset=0;
  for(int i=0;i++;i<=10)
  {
    Calcul_offset += analogRead(Sensor_Pin);
    delay(100);
  }
  Offset = int(Calcul_offset/10);
  EEPROM.write(eeAddress_Offset,Offset);
  Serial.println("Offset is Set");
}

/**
 * \brief Calibration of Voltage to PSI at 6 bar (87.02PSI)
 * 
 * Read 10 time the value of analg pin minus offset 
 * Done an average int 
 * Calculate the conversion factor
 * Write the value to EEPROM
 */
void SetCalibration6(){
  int Pression=87; //en psi Calibration a pression 6bars
  double Calcul_Calibration=0;
  for(int i=0;i++;i<=10)
  {
    Calcul_Calibration += (analogRead(Sensor_Pin)-Offset);
    delay(100);
  }
  Calcul_Calibration = int(Calcul_Calibration/10);
  CalibrationPSI=int(Pression/Calcul_Calibration*Voltage_ref/1024);
  EEPROM.write(eeAddress_CalibrationPSI, CalibrationPSI);
  Serial.println("Calibration Done");
}

/**
 * \brief Calibration of Voltage to PSI at 8 bar (116.03 PSI)
 * 
 * Read 10 time the value of analg pin minus offset 
 * Done an average int 
 * Calculate the conversion factor
 * Write the value to EEPROM
 */
void SetCalibration8(){
  int Pression=116 ;//en psi Calibration a pression 8bars
  double Calcul_Calibration=0;
  for(int i=0;i++;i<=10)
  {
    Calcul_Calibration += (analogRead(Sensor_Pin)-Offset);
    delay(100);
  }
  Calcul_Calibration = int(Calcul_Calibration/10);
  CalibrationPSI=int(Pression/Calcul_Calibration*Voltage_ref/1024);
  EEPROM.write(eeAddress_CalibrationPSI, CalibrationPSI);
  Serial.println("Calibration Done");
}

/**
 * \brief Calibration of Voltage to PSI at 10 bar (145.04PSI)
 * 
 * Read 10 time the value of analg pin minus offset 
 * Done an average int 
 * Calculate the conversion factor
 * Write the value to EEPROM
 */
void SetCalibration10(){
  int Pression=145; //en psi Calibration a pression 10bars
  double Calcul_Calibration=0;
  for(int i=0;i++;i<=10)
  {
    Calcul_Calibration += (analogRead(Sensor_Pin)-Offset);
    delay(100);
  }
  Calcul_Calibration = int(Calcul_Calibration/10);
  CalibrationPSI=int(Pression/Calcul_Calibration*Voltage_ref/1024);
  EEPROM.write(eeAddress_CalibrationPSI, CalibrationPSI);
  Serial.println("Calibration Done");
}

/**
 * \brief Calibration of Voltage to PSI at 60PSI
 * 
 * Read 10 time the value of analg pin minus offset 
 * Done an average int 
 * Calculate the conversion factor
 * Write the value to EEPROM
 */
void SetCalibration60PSI(){
  int Pression=60; //en psi Calibration a pression 10bars
  double Calcul_Calibration=0;
  for(int i=0;i++;i<=10)
  {
    Calcul_Calibration += (analogRead(Sensor_Pin)-Offset);
    delay(100);
  }
  Calcul_Calibration = int(Calcul_Calibration/10);
  CalibrationPSI=int(Pression/Calcul_Calibration*Voltage_ref/1024);
  EEPROM.write(eeAddress_CalibrationPSI, CalibrationPSI);
  Serial.println("Calibration Done");
}

/**
 * \brief Calibration of Voltage to PSI at 80PSI
 * 
 * Read 10 time the value of analg pin minus offset 
 * Done an average int 
 * Calculate the conversion factor
 * Write the value to EEPROM
 */
void SetCalibration80PSI(){
  int Pression=80; //en psi Calibration a pression 10bars
  double Calcul_Calibration=0;
  for(int i=0;i++;i<=10)
  {
    Calcul_Calibration += (analogRead(Sensor_Pin)-Offset);
    delay(100);
  }
  Calcul_Calibration = int(Calcul_Calibration/10);
  CalibrationPSI=int(Pression/Calcul_Calibration*Voltage_ref/1024);
  EEPROM.write(eeAddress_CalibrationPSI, CalibrationPSI);
  Serial.println("Calibration Done");
}

/**
 * \brief Calibration of Voltage to PSI at 100PSI
 * 
 * Read 10 time the value of analg pin minus offset 
 * Done an average int 
 * Calculate the conversion factor
 * Write the value to EEPROM
 */
void SetCalibration100PSI(){
  int Pression=100; //en psi Calibration a pression 10bars
  double Calcul_Calibration=0;
  for(int i=0;i++;i<=10)
  {
    Calcul_Calibration += (analogRead(Sensor_Pin)-Offset);
    delay(100);
  }
  Calcul_Calibration = int(Calcul_Calibration/10);
  CalibrationPSI=int(Pression/Calcul_Calibration*Voltage_ref/1024);
  EEPROM.write(eeAddress_CalibrationPSI, CalibrationPSI);
  Serial.println("Calibration Done");
}

/**
 * \brief Calibration of Voltage to PSI at 150PSI
 * 
 * Read 10 time the value of analg pin minus offset 
 * Done an average int 
 * Calculate the conversion factor
 * Write the value to EEPROM
 */
void SetCalibration150PSI(){
  int Pression=150; //en psi Calibration a pression 10bars
  double Calcul_Calibration=0;
  for(int i=0;i++;i<=10)
  {
    Calcul_Calibration += (analogRead(Sensor_Pin)-Offset);
    delay(100);
  }
  Calcul_Calibration = int(Calcul_Calibration/10);
  CalibrationPSI=int(Pression/Calcul_Calibration*Voltage_ref/1024);
  EEPROM.write(eeAddress_CalibrationPSI, CalibrationPSI);
  Serial.println("Calibration Done");
}
