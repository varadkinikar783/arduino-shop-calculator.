#include<Arduino.h>
#include<math.h>
#include<EEPROM.h>

double num1;
double num2;
char sign;
double result;
char Continue;
bool hascarryover = false;
uint8_t bootC = 45; //Used to check boot status.

void saveCalculation(double n1, char op, double n2, double res);
void overflowcontrol(int count);
void printhistory();
void clearhistory();
void wipehistory();

void setup() {
   Serial.begin(115200);
    while (!Serial) {
        ; // Wait for serial port to connect
    }
    if(EEPROM.read(0) != bootC){
      EEPROM.write(0, bootC);
      EEPROM.write(1, 0);
    }
}

void loop(){
  //operator input and check.  
Serial.print("Enter the operator: ");
while(Serial.available() == 0){};
sign = Serial.read();
Serial.print("operator: ");
Serial.println(sign);
delay(10);
//bufffer clearing.
while(Serial.available() > 0){
  Serial.read();
}
//sign checking.
if(sign != '+' && sign != '-' && sign!= '*' && sign != '/'){
  do{Serial.println("Enter a valid operator(+,-,*,/): ");
  while(Serial.available() == 0){};
  sign = Serial.read();
  Serial.print("operator: ");
  Serial.print(sign);
  delay(10);
  while(Serial.available() > 0){
  Serial.read();
  }
  }while(sign != '+' && sign != '-' && sign!= '*' && sign != '/');
}
if(!hascarryover){
//1st no. input.
Serial.print("Enter the first no.: ");
while(Serial.available() == 0){}//Waiting for input.
num1 = Serial.parseFloat();
Serial.print("num1: ");
Serial.println(num1);
delay(10);
while(Serial.available() > 0){
  Serial.read(); //for bufffer clearing.
}
}
//2nd no. input
Serial.print("Enter the second no.: ");
while(Serial.available() == 0){}
num2 = Serial.parseFloat();
Serial.print("num2: ");
Serial.println(num2);
delay(10);
while(Serial.available() > 0){
  Serial.read();
}
if(sign == '+'){
  result = num1 + num2;
  Serial.print("Result: ");
  Serial.println(result);
}
else if(sign == '-'){
  result = num1 - num2;
  Serial.print("Result: ");
  Serial.println(result);
}
else if(sign == '*'){
  result = num1 * num2;
  Serial.print("Result: ");
  Serial.println(result);
}

else if(sign == '/'){
  if(num2 == 0){
    Serial.println("You can't divide by 0.");
  }
  else{
    result = num1 / num2;
    Serial.print("Result: ");
    Serial.print(result);
  }
}
Serial.println();
Serial.println("Would you like to continue doing more operations on result?(Y/N): ");
while(Serial.available() == 0){};
Continue = Serial.read();
delay(10);
while(Serial.available() > 0 ){
  Serial.read();
}
if(Continue == 'Y' || Continue =='y'){
  uint8_t count = EEPROM.read(1);
  overflowcontrol(count);
  saveCalculation(num1, sign, num2, result);
  num1 = result;
  num2 = 0;
  Serial.println("num1: ");
  Serial.print(num1);
  hascarryover = true;
}
else if(Continue == 'N' || Continue == 'n'){
  uint8_t count = EEPROM.read(1);
  overflowcontrol(count);
  saveCalculation(num1, sign, num2, result);
  Serial.print("result: ");
  Serial.println(result);
  hascarryover = false;
  result = 0;
  sign = '\0';
  num2 = 0; 
  while(Serial.available() > 0){ 
    Serial.read();
  }
}
Serial.println();
}

void overflowcontrol(int count){
  if(count >= 40){
    Serial.print("Calculation limit reached, memory will be cleared please write down the result.->");
    Serial.println("Printing calculation history:-");
    printhistory();
    Serial.print("Clearing in 10 secs.");
    delay(10000);
    wipehistory();
  }
}

void saveCalculation(double n1, char op, double n2, double res){
  uint8_t count = EEPROM.read(1);  // how many entries exist
  int startAddress = 2 + (count * 25);
  EEPROM.put(startAddress, n1);
  EEPROM.put(startAddress + 8, op);
  EEPROM.put(startAddress + 9, n2);
  EEPROM.put(startAddress + 17, res);
  count++;
  EEPROM.write(1, count);
  
}

void printhistory(){
  uint8_t count = EEPROM.read(1);
  for(int i = 0; i < count; i++){
    int startAddress = 2 + (i * 25);
    double n1, n2, res;
    char op;
    EEPROM.get(startAddress, n1);
    EEPROM.get(startAddress + 8, op);
    EEPROM.get(startAddress + 9, n2);
    EEPROM.get(startAddress + 17, res);
    Serial.print("entry No.: ");
    Serial.print(i + 1);
    Serial.println("->");
    Serial.print(n1);
    Serial.print(" ");
    Serial.print(op);
    Serial.print(" ");
    Serial.print(n2);
    Serial.print(" = ");
    Serial.println(res);
    Serial.println();
  }
}

void clearhistory(){
  EEPROM.write(1, 0);
}

void wipehistory(){
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
}