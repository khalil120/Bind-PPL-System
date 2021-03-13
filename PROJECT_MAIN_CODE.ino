#include <NewPing.h>
int Pb = 51, PbVal;
int MaxSonar = 5;
int pir = 8;
int piezo = 6 ,rp = 4 ,lp = 11;// rp: right side piezo, lp: left side piezo.
int MaxIn, rdur, ldur; // rdur: right side durtion, ldur: left side duration.
int c=0 ,c1=0 ,lc=0 ,rc=0,a=0; // counter for one time speak only !!!
float Inches;
float centimeter ,rd, ld; //rd: distance from right side, ld: distance from left side.
int lecho = 12, recho = 2, ltrig = 13, rtrig = 3; // recho & lecho: right $ left sensor echo,
//ltrig & rtrig: left & right sensor triger.
// integers and strings fo GPS modulation...
int updates;
int failedUpdates;
int pos;
int stringplace = 0;
String timeUp;
String nmea[15];
String nmea1[15];
String labels[12] {"Time: ", "Status: ", "Latitude: ", "Hemisphere: ", "Longitude: ", "Hemisphere: ", "Speed: ", "Track Angle: ", "Date: "};
void setup() 
{
  Serial.begin(57600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  pinMode(piezo,OUTPUT);
  pinMode(lp,OUTPUT);
  pinMode(rp,OUTPUT);
  pinMode(rtrig,OUTPUT);
  pinMode(ltrig,OUTPUT);
  pinMode(MaxSonar,INPUT);
  pinMode(pir, INPUT);
  pinMode(lecho,INPUT);
  pinMode(recho,INPUT);
  pinMode(Pb,INPUT);  
  Serial2.print('\n');
  Serial2.println("V18");
  Serial2.println("W180");
  Serial2.println("N1");
  while(Serial2.read() != ':');
  delay(10);
  Serial2.flush();
  }
void loop() 
{
  digitalWrite(rtrig, HIGH);
  delay(1);
  digitalWrite(rtrig, LOW);
  rdur = pulseIn(recho,HIGH);
  rd = (rdur / 2) / 29.1;
  Serial.print("right distance = ");
  Serial.print(rd);
  Serial.println(" cm ");
  digitalWrite(ltrig, HIGH);
  delay(1);
  digitalWrite(ltrig, LOW);
  ldur = pulseIn(lecho,HIGH);
  ld = (ldur / 2) / 29.1;
  Serial.print("left distance = ");
  Serial.print(ld);
  Serial.println(" cm ");
  MaxIn = pulseIn(MaxSonar,HIGH);
  Inches = MaxIn/147;
  centimeter = Inches * 2.54;
  Serial.print("middle distance = ");
  Serial.print(centimeter);
  Serial.println(" cm ");
  Serial.println(" ");
     if(centimeter < 50 & centimeter > 12.70){
    digitalWrite(piezo,HIGH);
    while(c < 1){
    Serial2.print('S');
    Serial2.println("Caution there is something in front of you in less than 50 centimeter ");
    c++;
    }   
  }
  else{
      digitalWrite(piezo,LOW);
      c=0;
  }
        if(centimeter < 20 & centimeter > 12.70){
         while(c1 < 1){
    Serial2.print('S');
    Serial2.println("Caution there is something in front of you in less than 20 centimeter ");
    c1++;
    }
    }
    else{
      digitalWrite(piezo,LOW);
      c1=0;
    }
    if(rd < 50 & rd > 5){
      digitalWrite(rp,HIGH);
      while(rc < 1){
        Serial2.print('S');
        Serial2.println(" caution there is something behind you from your right side in less than 50 centimeters ");
        rc++;
      }
    }
    else{
      digitalWrite(rp,LOW);
      rc = 0;
    }
    if(ld < 50 & ld >5){
      digitalWrite(lp,HIGH);
      while(lc < 1){
        Serial2.print('S');
        Serial2.println(" caution there is something behind you from your left side in less than 50 centimeters ");
        lc++;
      }
    }
    else{
      digitalWrite(lp,LOW);
      lc = 0;
    }
    if(ld<50 & centimeter<50 & rd<50 || ld<50 & rd<50){
      digitalWrite(piezo,HIGH);
      while(a < 1){
        Serial2.print('S');
        Serial2.println(" caution there is something big in front of you in less than 50 centimeter ");
        a++;
      }
    }
      else{
        digitalWrite(piezo, LOW);
        a=0;
      }
  Serial.flush();
  Serial3.flush();
  while (Serial3.available() > 0)
  {
    Serial3.read();
  }
  if (Serial3.find("$GPRMC,")) {
    String tempMsg = Serial3.readStringUntil('\n');
    for (int i = 0; i < tempMsg.length(); i++) {
      if (tempMsg.substring(i, i + 1) == ",") {
        nmea[pos] = tempMsg.substring(stringplace, i);
        stringplace = i + 1;
        pos++;
      }
      if (i == tempMsg.length() - 1) {
        nmea[pos] = tempMsg.substring(stringplace, i);
      }
    }
    updates++;
    nmea[2] = ConvertLat();
    nmea[4] = ConvertLng();
          if(nmea[2] != 0 || nmea[4] != 0){
    int i = 0;
    while(nmea[i]){
      nmea1[i] = nmea[i];
      i++;
    }
  }
    PbVal = digitalRead(Pb);
  if(PbVal == HIGH){
    Serial2.print('S');
    Serial2.print("i'm lost my location is: latitude: ");
    Serial2.print(nmea1[2]);
    Serial2.print("longtude: ");
    Serial2.print(nmea1[4]);
    Serial2.print("you can find me on google maps hurry up please im waiting...");
    Serial2.flush();
  }
    for (int i = 0; i < 9; i++) {
      Serial.print(labels[i]);
      Serial.print(nmea[i]);
      Serial.println("");
    }
  }
  else {
    failedUpdates++;
  }
  stringplace = 0;
  pos = 0;
  delay(500);
}
// functions to get GPS location...
String ConvertLat() {
  String posneg = "";
  if (nmea[3] == "S") {
    posneg = "-";
  }
  String latfirst;
  float latsecond;
  for (int i = 0; i < nmea[2].length(); i++) {
    if (nmea[2].substring(i, i + 1) == ".") {
      latfirst = nmea[2].substring(0, i - 2);
      latsecond = nmea[2].substring(i - 2).toFloat();
    }
  }
  latsecond = latsecond / 60;
  String CalcLat = "";
  char charVal[9];
  dtostrf(latsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLat += charVal[i];
  }
  latfirst += CalcLat.substring(1);
  latfirst = posneg += latfirst;
  return latfirst;
}
String ConvertLng() {
  String posneg = "";
  if (nmea[5] == "W") {
    posneg = "-";
  }
  String lngfirst;
  float lngsecond;
  for (int i = 0; i < nmea[4].length(); i++) {
    if (nmea[4].substring(i, i + 1) == ".") {
      lngfirst = nmea[4].substring(0, i - 2);
      //Serial.println(lngfirst);
      lngsecond = nmea[4].substring(i - 2).toFloat();
      //Serial.println(lngsecond);
    }
  }
  lngsecond = lngsecond / 60;
  String CalcLng = "";
  char charVal[9];
  dtostrf(lngsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLng += charVal[i];
  }
  lngfirst += CalcLng.substring(1);
  lngfirst = posneg += lngfirst;
  return lngfirst;
}
