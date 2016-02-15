
  int ZeroBar=0;
  int AnalogValeur=0;
  double  Valeur=0;
  double  Valeur2=0;
  String Value;
  unsigned long StartTime;
  
void setup() {
  // initialize both serial ports:
  Serial.begin(115200,SERIAL_8E1);
  }

void loop() {
  // read from port 0, send to port 1:
    AnalogValeur= analogRead(A3);
    StartTime=micros();
    //Value=String(Valeur,1);
    //Valeur=(AnalogValeur-ZeroBar)*(4.95/1024.0)*(200.0/4.5);
    Valeur=abs((((AnalogValeur*4.784)/1024.0))-0.51)*50; //4,91 avec pile 9V
    //Valeur2=(AnalogValeur*4.92)/1024.0;
    Serial.println(String(Valeur,0)+";"+String(StartTime)); 
    //Serial.println(String(Valeur,0)+";"+String("4425874631254"));
    delay(1);    
}
