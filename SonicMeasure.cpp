const int trigpin = 11;
const int echopin = 12;
int LEDpin = 9;
#include <Wire.h>
#define I2C_ADDR 0x27

void lcd_pulse(byte data) {
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(data | 0x04); 
  Wire.write(data | 0x08); 
  Wire.endTransmission();
  delayMicroseconds(1);

    
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(data & ~0x04);
  Wire.write(data | 0x08);
  Wire.endTransmission();
  delayMicroseconds(50);

}

void lcd_send(byte value, byte mode) {
  byte highnib = value & 0xF0;
  byte lownib = (value << 4) & 0xF0;
  lcd_pulse(highnib | mode);
  lcd_pulse(lownib | mode);
  
}

void lcd_write_string(const char *str) {
  while (*str) {
    lcd_send(*str++, 0x01); 
  }
}

void lcd_clear() {
  lcd_send(0x01, 0x00); 
  delay(2);
}

void setup(){
  Serial.begin(9600);
  pinMode(trigpin,OUTPUT);
  pinMode(echopin,INPUT);
  pinMode(LEDpin,OUTPUT);

  Wire.begin();
  delay(50);

  // 4-bit initialization sequence
  lcd_pulse(0x30); delay(5);
  lcd_pulse(0x30); delayMicroseconds(150);
  lcd_pulse(0x30);
  lcd_pulse(0x20); 

  lcd_send(0x28, 0x00); 
  lcd_send(0x0C, 0x00); 
  lcd_send(0x06, 0x00); 
  lcd_clear();

 
  lcd_send(0x80, 0x00); 
  lcd_write_string("Distance Calc");

}

void loop(){
  digitalWrite(trigpin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin,LOW);

  long duration = pulseIn(echopin,HIGH);
  int distance = duration*0.0343/2;

  int safedistance = constrain(distance,0,50);
  float brightnessratio = (50.0-safedistance)/50.0;
  int linearBrightness = brightnessratio * brightnessratio * 255;
  
  analogWrite(LEDpin,linearBrightness);

  
  if (distance>400){
    Serial.println("Object Out of range");

  }
  else{
    Serial.print(distance);
    Serial.println("cm");
  }
  delay(100);

  lcd_send(0xC0, 0x00);
  char buffer[16]; 
  snprintf(buffer, sizeof(buffer),"Dist: %d cm     ", distance); 
  lcd_write_string(buffer);
}
