//Version 2 of
//Control and regulation software for Vihron(R)
//Vihron and it's operational software are developed
//by Hristo Topalov,Bulgaria, 2015-2016
//For more information contact me at: htopalov@gmail.com
//www.vihron.wordpress.com
//www.facebook.com/vihrongen
//cheers
#include <LiquidCrystal.h>
#include<SPI.h>
const int CS = 43;
int PotWiperVoltage = 1;
int RawVoltage = 0;
float Voltage = 0;
LiquidCrystal lcd(8,9,4,5,6,7);
int analogInput = A8;
float vout = 0.0;
float vin = 0.0;
float R3 = 30000.0; //  
float R4 = 7500.0; // 
int value = 0;
//*****************
int red_led = 44;//power pins for LED indicators
int green_led = 48;
int blue_led = 46;
int ssr = 47;//power pin for solid state relay
int transistor_base = 50;//power pin for trasistor switch
float current_sensor = 4.98;//current sensor voltage input
float amps = 0;//actual amps
float watts = 0;//actual power
int reading = 0;//variable for tmp36 voltage reading
float temp = 0;
float volt_temp = 0;


void setup() {
  lcd.begin(16,2);//Start display
  SPI.begin();
  lcd.setCursor(0,0);
  pinMode (CS, OUTPUT);
  pinMode(ssr,OUTPUT);//Setting digital pins to output
  pinMode(green_led,OUTPUT);
  pinMode(blue_led,OUTPUT);
  pinMode(red_led,OUTPUT);
  pinMode(transistor_base,OUTPUT);
  pinMode(analogInput, INPUT);
  digitalWrite(ssr,HIGH);//turns ssr on till overvoltage occurs
  digitalWrite(green_led,HIGH);//turns green led on;generator operational
}

void loop() {
  //****************************************
  //current
 amps = ((current_sensor/2) - analogRead(A13)*current_sensor/1023.0)/0.028;//-0.60 less-noise of current sensor 
 //voltage after regulation,charging voltage
   value = analogRead(analogInput);
   vout = (value * 5.0) / 1024.0; // see text
   vin = vout / (R4/(R3+R4));
 //****************************************
 //charging power, Ohms Law
 watts = vin*amps;
 //****************************************
 //temperature
 reading = analogRead(A12);//voltage reading
 volt_temp = reading*5.0;
 volt_temp /= 1024.0;
 temp = (volt_temp - 0.5)*100;//actual temperature

//****************************************
//print monitor data to LCD
 lcd.setCursor(0,0);
 lcd.print(F("U:"));
 lcd.setCursor(2,0);
 lcd.print(vin);
 lcd.setCursor(7,0);
 lcd.print(F("V"));
 lcd.setCursor(9,0);
 lcd.print(F("I:"));
 lcd.setCursor(11,0);
 lcd.print(amps);
 lcd.setCursor(15,0);
 lcd.print(F("A"));
 lcd.setCursor(0,1);
 lcd.print(F("P:"));
 lcd.setCursor(2,1);
 lcd.print(watts);
 lcd.setCursor(7,1);
 lcd.print(F("W"));
 lcd.setCursor(9,1);
 lcd.print(F("T:"));
 lcd.setCursor(11,1);
 lcd.print(temp);
 lcd.setCursor(15,1);
 lcd.print(F("C"));
  // move the potentiometer in one direction
  for (int level = 0; level < 255; level++) 
  {
    MCP41010Write(level);
    delay(100);
    RawVoltage = analogRead(PotWiperVoltage);
    Voltage = (RawVoltage * 5.0 )/ 1024.0;
    Serial.print("Level = " );                      
    Serial.print(level);      
    Serial.print("\t Voltage = ");
    Serial.println(Voltage,3);  
  }
  delay(2000);  // wait a couple seconds
  // Now mover potentiometer in other directions
  
  for (int level = 255; level > 0; level--) 
  {
    MCP41010Write(level);
    delay(100);
    RawVoltage = analogRead(PotWiperVoltage);
    Voltage = (RawVoltage * 5.0 )/ 1024.0;
    Serial.print("Level = " );                      
    Serial.print(level);      
    Serial.print("\t Voltage = ");
    Serial.println(Voltage,3); 
  }
   delay(2000);
 //****************************
 //turns on and off blue led to display generetor is generating voltage
  if(vin > 0.50)
  {
    digitalWrite(blue_led,HIGH);//turns on and off blue led to display generetor is generating voltage
    delay(400);
    digitalWrite(blue_led,LOW);
    delay(400);
  }
//**********************************
//PWM regulator overheat protection
if(temp > 20)
  {
   digitalWrite(transistor_base,HIGH);//turns transistor switch on to start the fan
  }
//********************************
  delay(400);
}
void MCP41010Write(byte value) 
{
  // Note that the integer vale passed to this subroutine
  // is cast to a byte
  
  digitalWrite(CS,LOW);
  SPI.transfer(B00010001); // This tells the chip to set the pot
  SPI.transfer(value);     // This tells it the pot position
  digitalWrite(CS,HIGH); 
}
