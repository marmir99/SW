#include <LiquidCrystal.h>
#include <Wire.h>

#define I2C_SLAVE_ADDRESS 11

#define PAYLOAD_SIZE 2
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int sequence[2][10];
int n=0, flag = 1, levelCounter, check =0 ;

char keys[4][4] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

void showSeq() 
{
  lcd.setCursor(0,0);
  lcd.print("Level");
  lcd.setCursor(0,1);
  lcd.print(levelCounter);
  delay(500);
  lcd.clear();
  delay(200);
  for (int i=0; i<levelCounter; i++) {
    if (sequence[1][i] >3)  {
      lcd.setCursor(0,0);
      lcd.print("2");
    }
    else {
  	  lcd.setCursor(0,0);
      lcd.print("1");
    }
    int colPom = sequence[1][i]%4;
    lcd.setCursor(0,1);
    lcd.print(keys[sequence[0][i]][colPom]);
    delay(300);
    lcd.clear();
    delay(100);
  }
  flag = 6;
}



void readSequence()
{
  int i = 0;
  while (Wire.available() > 0) {
    int pom = Wire.read();
    delay(20);
    if (pom == 10){
      flag = 2;
    }
    else{
    	sequence[0][i] = pom;
        pom = Wire.read();
      	sequence[1][i] = pom;
    	i++;
    }
  }
}

void readLevel()
{
  if (Wire.available() > 0) {
    levelCounter = Wire.read();
    flag = 4;
    Serial.println("read level");
    Serial.println(levelCounter);
  }
}

void readCheck()
{
  if (Wire.available() > 0) {
    check = Wire.read();
    Serial.println("check");
    Serial.println(check);
  }
}




void requestEvents()
{
  if (flag == 2)
  {
    Wire.write(1);
    delay(500);
    flag = 3;
    
  }
  if(flag == 7)
  {
    Wire.write(3);
    delay(500);
    flag = 3;
  }
}

void receiveEvents(int numBytes)
{  
  if (flag == 1){
    readSequence();
  }
  else if (flag == 3){
    readLevel();
  }
  else if (flag == 6) {
    readCheck();
    Serial.println("CHECKALERT");
    Serial.println(check);
    if (check == 1) {
      lcd.setCursor(0,0);
      lcd.print("DOBRZE!");
      flag = 7;
      if (levelCounter == 10) {
        lcd.setCursor(0,1);
        lcd.print("WYGRANA!");
        flag = 1;
      }
      delay(5500);
      check = 0;
      lcd.clear();
    }
    else if (check == 2) {
      lcd.setCursor(0,0);
      lcd.print("ZLE!");
      delay(5500);
      check = 0;
      lcd.clear();
      flag = 7;
    } 
  }
}



void setup() {
  Wire.begin(I2C_SLAVE_ADDRESS);
  Wire.onReceive(receiveEvents);
  Wire.onRequest(requestEvents);
  flag = 1;
  Serial.begin(9600); 
  lcd.begin(16, 2);
}

void loop() {
  if (flag == 1) {
   	receiveEvents(1);
  }
  if (flag == 2) {
    requestEvents();
  }
  if (flag == 3)
  { 
     receiveEvents(1);
     delay(800);
   	 Serial.println("Level Counter ---------------");
     Serial.println(levelCounter);
   }
  if (flag == 4) 
  {
  	showSeq();  
  }
  if (flag == 6)
  {
    receiveEvents(1);
  }
  if (flag == 7)
  {
    requestEvents();
  }
}