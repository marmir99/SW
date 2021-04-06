#include <Keypad.h>
#include <Wire.h>

#define I2C_SLAVE_ADDRESS 12

#define PAYLOAD_SIZE 2

int sequence[2][10];
int readSeq[2][10];
int n=0, flag = 0, levelCounter, check = 1;
const byte rows = 4;
const byte cols = 4;
char keys[rows][cols] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
byte rowPins[rows] = {12, 11, 10, 9};
byte colPins[cols] = {8, 7, 6, 5};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

byte rowPins2[rows] = {4, 3, 2, 0};
byte colPins2[cols] = {A0, A1, A2, A3};
Keypad keypad2 = Keypad( makeKeymap(keys), rowPins2, colPins2, rows, cols );

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
    flag = 5;
    Serial.println("read level");
    Serial.println(levelCounter);
  }
}

void readKeys()
{
  char pom;
  Serial.println("readKeys");
  int ind = 0;
  check = 1;
  while (ind < levelCounter)
  {
    char key = keypad.getKey();
    char key2 = keypad2.getKey();
    if (key != NO_KEY)
    {
      pom = key;
      for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
          if (keys[i][j] == pom)
          {
            Serial.println(keys[i][j]);
            readSeq[0][ind] = i;
          	readSeq[1][ind] = j;
            break;
          }
        }
      }
      ind++;
    }
    else if (key2 != NO_KEY)
    {
      pom = key2;
      for (int i=0; i<4; i++) 
      {
        for (int j=0; j<4; j++) 
        {
          if (keys[i][j] == pom)
          {
            Serial.println(keys[i][j]);
            readSeq[0][ind] = i;
          	readSeq[1][ind] = j+4;
            break;
          }
        }
      }
      ind++;
    }
  }
  Serial.println("READ");
  for (int i=0; i<levelCounter; i++) {
    if (readSeq[0][i] != sequence[0][i] || readSeq[1][i] != sequence[1][i]) {
      Serial.println("CHECK");
      check = 2;
      break;
    }
  }
  Serial.println(check);
  flag = 6;
}



void receiveEvents(int numBytes)
{  
  if (flag == 1){
    readSequence();
  }
  else if (flag == 3){
    readLevel();
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
  if (flag == 6)
  {
    Wire.write(check);
    delay(500);
    if (levelCounter == 10 && check == 1)
      flag = 1;
    else
      flag = 3;
  }
}



void setup()
{
  Wire.begin(I2C_SLAVE_ADDRESS);
  Wire.onReceive(receiveEvents);
  Wire.onRequest(requestEvents);
  flag = 1;
  Serial.begin(9600);
}

void loop()
{
  if (flag == 1)
  {
    receiveEvents(1);
  }
  if (flag == 2)
  {
  	requestEvents();  
  }
  if (flag == 3) 
  {
    receiveEvents(1);
  }
  if (flag == 5)
  {
    readKeys();
  }
  if (flag == 6)
  {
  	requestEvents();  
    
  }
}