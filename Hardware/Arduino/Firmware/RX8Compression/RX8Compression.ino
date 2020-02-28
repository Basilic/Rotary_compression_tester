
  int ZeroBar=0;
  int AnalogValeur=0;
  double  Valeur=0;
  double  Valeur2=0;
  String Value;
  unsigned long StartTime;
#define External_ref // Comment this if you not use LM4040 on AREF pin

#ifdef External_ref
  #define Voltage_ref 4.1
  #define Voltage_Reference EXTERNAL
# else
  #define Voltage_ref 5
  #define Voltage_Reference DEFAULT
#endif
  
void setup() {
  // initialize both serial ports:
  Serial.begin(115200,SERIAL_8E1);
  analogReference(Voltage_Reference);
  }

void loop() {
  // read from port 0, send to port 1:
    AnalogValeur= analogRead(A3);
    StartTime=micros();  
    Valeur=abs((((AnalogValeur*Voltage_ref)/1024.0))-0.51)*50; 
    Serial.println(String(Valeur,0)+";"+String(StartTime)); 
    delay(1);    
}
