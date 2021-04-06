#include <Wire.h>

#define I2C_SLAVE1_ADDRESS 11
#define I2C_SLAVE2_ADDRESS 12

#define PAYLOAD_SIZE 2
#define MAX_LEVEL 10

int sequence[2][MAX_LEVEL];

int levelCounter = 1, n1, n2, flag=1, check = 0;


void generateSequence(){
  for (int i = 0; i<MAX_LEVEL; i++)
  {
    sequence[0][i] = random(0,4);
    sequence[1][i] = random(0,8);
  }
  Serial.println("Sequence generated");
}


void receiveInfo() {
  Wire.requestFrom(I2C_SLAVE1_ADDRESS, 1);
  if (Wire.available() > 0)
  {
  	n1 = Wire.read();
  }
}

void receiveCheck() {
  Wire.requestFrom(I2C_SLAVE2_ADDRESS, 1);   
  if (Wire.available() > 0)
  {
  	check = Wire.read();
  }
}



void sendSequence(){
  Wire.beginTransmission(I2C_SLAVE1_ADDRESS); 
  for (int i = 0; i<MAX_LEVEL; i++)
  {
  	Wire.write(sequence[0][i]); 
    Wire.write(sequence[1][i]);
  }
  Wire.write(10);
  delay(200);
  Wire.endTransmission();
  Wire.beginTransmission(I2C_SLAVE2_ADDRESS); 
  for (int i = 0; i<MAX_LEVEL; i++)
  {
  	Wire.write(sequence[0][i]); 
    Wire.write(sequence[1][i]);
  }
  Wire.write(10);
  delay(200);
  Wire.endTransmission();
  Serial.println("Sequence sended");
}

void sendLevel(){
  Wire.beginTransmission(I2C_SLAVE1_ADDRESS); 
  Wire.write(levelCounter);
  delay(200);
  Wire.endTransmission();
  Wire.beginTransmission(I2C_SLAVE2_ADDRESS); 
  Wire.write(levelCounter);
  delay(200);
  Wire.endTransmission();
  flag = 3;
}

void sendInfo() {
  Wire.beginTransmission(I2C_SLAVE2_ADDRESS); 
  Wire.write(1);
  delay(200);
  Wire.endTransmission();
}

void sendCheck() {
  Wire.beginTransmission(I2C_SLAVE1_ADDRESS); 
  Wire.write(check);
  delay(200);
  Wire.endTransmission();
}  



void setup()
{
  Wire.begin();        
  Serial.begin(9600);  
  randomSeed(analogRead(0));
  generateSequence();
  for (int i = 0; i<10; i++){
    Serial.println(sequence[0][i]);
    Serial.println(sequence[1][i]);
  }
  sendSequence();
  Serial.println("Send!");
  Wire.requestFrom(I2C_SLAVE1_ADDRESS, 1);   
  if (Wire.available() > 0)
  {
  	n1 = Wire.read();
  }
  Wire.requestFrom(I2C_SLAVE2_ADDRESS, 1);   
  if (Wire.available() > 0)
  {
  	n2 = Wire.read();
  }
}

void loop()
{
  delay(500);
  if (n1 == 1 && n2 == 1 && flag == 1)
  {
    Serial.println("Send level");
    sendLevel();
    n1 = 0;
  }
  if(flag == 3)
  {
    receiveCheck();
    if (check != 0) 
    {
      	Serial.println("check");
      	Serial.println(check);
        sendCheck(); 
      if (check == 2) {
        levelCounter = 1;
        flag = 4; 
      }
      else if (check == 1 && levelCounter != 10) {
          levelCounter++;
          flag = 4; 
      }
        else if (check == 1 && levelCounter == 10){
          levelCounter=1;
          generateSequence();
          for (int i=0; i<10; i++)
          {
            Serial.println(sequence[0][i]);
            Serial.println(sequence[1][i]);
          }
          sendSequence();
            Wire.requestFrom(I2C_SLAVE1_ADDRESS, 1);   
  		if (Wire.available() > 0)
  		{
  			n1 = Wire.read();
  		}
  		Wire.requestFrom(I2C_SLAVE2_ADDRESS, 1);   
  		if (Wire.available() > 0)
  		{
  			n2 = Wire.read();
  		}
          flag = 1;
       }
        check = 0;
    }
  }
  if(flag == 4)
  {
    receiveInfo();
    if (n1 == 3) {
      flag = 1;
      n1 = 1;
      n2 = 1;
    }
  }
}
