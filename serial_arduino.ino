// HX711 define
#include "HX711.h"
#define DOUT 23
#define CLK  22
HX711 scale(DOUT, CLK);
float calibration_factor = 2125; //-7050 worked for my 440lb max scale setup
float units;
float ounces;
float pesoiniziale;
int outputValue = 0;        // value output to the PWM (analog out)

//Metal detector define
const int analogInPin1 = A0;  // Analog input pin that the potentiometer is attached to
const int analogInPin2 = A1;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 13; // Analog output pin that the LED is attached to
int sensorMetalValue1 = 0;        // value read from the pot
int sensorMetalValue2 = 0;       // value read from the pot

// Stepper
byte ledPin = 13;
byte enbPin = 8;
byte dire2Pin = 7;
byte dire1Pin = 6;
byte slee2Pin = 5;
byte slee1Pin = 4;
byte step2Pin = 3;
byte step1Pin = 2;
byte OptoLimit1=10;
byte OptoLimit2=11;
byte Homing1=3; // rimettere a 0
byte Homing2=3;

int numberOfSteps = 1300; // mezzo giro 6300

int pulseWidthMicros = 450;  // microseconds
int millisbetweenSteps = 1; // milliseconds - or try 1000 for slower steps

// Servo

#include <Servo.h>
Servo myservo;  // oggetto servo
int potpin = 2;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin


// PROTOTIPI

void homing();
void muoviPosizioneCam();
void servoVaiA(int pos)
{

  for(int i=0;i< pos;i+=5)
  {
    myservo.write(i);        
    delay(50);                           
  }
  
}

void setup() {

  Serial.begin(57600); // opens serial port, sets data rate to 57600 baud


              
    
  // inizializzazione stepper e homing + inizializza servo
  homing();

  // HX711 
  scale.set_scale();
  scale.tare();  //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  pesoiniziale = scale.get_units(), 10;
  //pesoiniziale = 10;
  //Serial.println(pesoiniziale);
  
  pinMode(LED_BUILTIN, OUTPUT);
  
  
}

void loop() {
while(true){
///// versione alterativa

 // 
 while (Serial.available() <= 0) {

  delay(1000);
 }
  while (Serial.available() > 0) { char incomingByte = Serial.read(); } // consumo

 // arrivato qualcosa, 
 
 // DA IMPLEMENTARE: SBLOCCO SPORTELLO
 
 // procedo ad aspettare che il peso sia diverso da quello iniziale

 scale.set_scale(calibration_factor); //Adjust to this calibration factor

 // CICLO FINCHE' non arriva qualcosa. A u certo punto dovrò, se non arriva nulla, fermarmi
 int attesa = 0;
 do{
    units = scale.get_units(), 10; // peso
    delay(1000);
    attesa++; // aspetto max un minuto
 }while((abs(pesoiniziale-units) < 0.5) && (attesa < 60));
 
 if(pesoiniziale == units) continue; // se non è arrivato nulla e è passato un minuto
  
 // DA IMPLEMENTARE: BLOCCO SPORTELLO

  muoviPosizioneCam(); // muovo in posizione per camera e altri sensori
 
 String r="pe";
 r=units +  r;
 Serial.println(r); // restituisce al raspy il peso
 
 // ATTENDO SULLA SERIALE
 while (Serial.available() <= 0) {
   delay(1000);
 }
 while (Serial.available() > 0) { char incomingByte = Serial.read(); } // consumo

 // DOPO CHE RASPY HA COMUNICATO DI AVER FATTO LA FOTO
 // MUOVO SERVO BRACCIO
   
   servoVaiA(160);    
      
 // SESORE METALLI E UV

 sensorMetalValue1 = analogRead(analogInPin1);
 sensorMetalValue2 = analogRead(analogInPin2);
 int totalmetal = sensorMetalValue2 + sensorMetalValue1;
 String res="";
 res = res + totalmetal + "|" + 21;
 
 // COMUNICO VALORI AL RASP
 Serial.println(res);

  // servo in posizione iniziale
  
  int pos=150;
  while(pos > 90)
  {
    myservo.write(pos);                  // valori tra 0 e 180
    delay(150);
    pos-=5;
  }

 // RIMANGO IN ATTESA DEL RESPONSO DEL RASPBERRY
 while (Serial.available() <= 0) {
   delay(1000);
 }
 while (Serial.available() > 0) { char incomingByte = Serial.read(); } // consumo

 // DA IMPLEMETARE:  A SECONDA DEL CARATTERE RICEVUTO MI MUOVO E GETTO RIFIUTO

 // DA IMPLEMETARE: FACCIO HOMING
 // COMUICO AL RASPY DI AVER FINITO
 Serial.println("T");

 
  delay(1000);
}
}


void muoviPosizioneCam()
{


  digitalWrite(slee1Pin, HIGH);
  digitalWrite(slee2Pin, HIGH);
  digitalWrite(enbPin, LOW);
  delay(1000);


  digitalWrite(dire1Pin, LOW); // disco sotto
  digitalWrite(dire2Pin, LOW);
  delay(1000);
  
 for (int n = 0; n < numberOfSteps; n++) {

   digitalWrite(step1Pin, HIGH);
    digitalWrite(step2Pin, HIGH);
    //delay(millisbetweenSteps);
    delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
    digitalWrite(step1Pin, LOW);
    digitalWrite(step2Pin, LOW);
    delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary

    
  }
    digitalWrite(slee2Pin, LOW);
    digitalWrite(slee1Pin, LOW);
  
}


void homing()
{


  delay(1000);

  pinMode(dire1Pin, OUTPUT);
  pinMode(dire2Pin, OUTPUT);
  pinMode(step1Pin, OUTPUT);
  pinMode(step2Pin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(enbPin, OUTPUT);
  pinMode(slee1Pin, OUTPUT);
  pinMode(slee2Pin, OUTPUT);
  pinMode(OptoLimit1, INPUT);
  pinMode(OptoLimit2, INPUT);

  digitalWrite(slee1Pin, HIGH);
  digitalWrite(slee2Pin, HIGH);
  digitalWrite(enbPin, LOW);
  delay(1000);
  digitalWrite(dire1Pin, HIGH);
  digitalWrite(dire2Pin, HIGH);
  delay(1000);
  //for (int n = 0; n < numberOfSteps; n++) {
  while(Homing1!=3 || Homing2!=3) {
    if(Homing1==0)digitalWrite(step1Pin, HIGH);
    if(Homing2==0) digitalWrite(step2Pin, HIGH);
    
    delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
    if(Homing1==0)digitalWrite(step1Pin, LOW);
    if(Homing2==0)digitalWrite(step2Pin, LOW);
    delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
    if(digitalRead(OptoLimit1)==0)Homing1=3;
    if(digitalRead(OptoLimit2)==0)Homing2=3;
   
  }
    digitalWrite(slee2Pin, LOW);
    digitalWrite(slee1Pin, LOW);
    
  // inizializzazione servo
  myservo.attach(9);
  int pos=150;
  while(pos > 90)
  {
    myservo.write(pos);                  // valori tra 0 e 180
    delay(150);
    pos-=5;
  }
  
}

/*
#include <Servo.h>

Servo myservo;  // create servo object to control a servo

int potpin = 2;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  myservo.attach(9); 
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue0 = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage0 = sensorValue0 * (5.0 / 1023.0);
  // print out the value you read:
   int sensorValue1 = analogRead(A1);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage1 = sensorValue0 * (5.0 / 1023.0);
  // print out the value you read:
  Serial.print(voltage0);
  Serial.print("-M0");
  Serial.print(sensorValue0);
  //Serial.print();
  Serial.print(voltage1);
  Serial.print("-M1");
  Serial.print(sensorValue1);
  Serial.print("--Servo");
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, 1023, 92, 158);     // scale it to use it with the servo (value between 0 and 180)
  Serial.println(val);
  myservo.write(val);                  // sets the servo position according to the scaled value
  delay(15);                           // waits for the servo to get there
}

// testing a stepper motor with a Pololu A4988 driver board or equivalent
// on an Uno the onboard led will flash with each step
// this version uses delay() to manage timing

byte ledPin = 13;
byte enbPin = 8;
byte dire2Pin = 7;
byte dire1Pin = 6;
byte slee2Pin = 5;
byte slee1Pin = 4;
byte step2Pin = 3;
byte step1Pin = 2;
byte OptoLimit1=10;
byte OptoLimit2=11;
byte Homing1=0;
byte Homing2=0;

int numberOfSteps = 12600;

int pulseWidthMicros = 450;  // microseconds
int millisbetweenSteps = 1; // milliseconds - or try 1000 for slower steps


void setup() {

  Serial.begin(9600);
  Serial.println("Starting StepperTest");
  digitalWrite(ledPin, LOW);

  delay(1000);

  pinMode(dire1Pin, OUTPUT);
  pinMode(dire2Pin, OUTPUT);
  pinMode(step1Pin, OUTPUT);
  pinMode(step2Pin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(enbPin, OUTPUT);
  pinMode(slee1Pin, OUTPUT);
  pinMode(slee2Pin, OUTPUT);
  pinMode(OptoLimit1, INPUT);
  pinMode(OptoLimit2, INPUT);

  digitalWrite(slee1Pin, HIGH);
  digitalWrite(slee2Pin, HIGH);
  digitalWrite(enbPin, LOW);
  delay(1000);
  digitalWrite(dire1Pin, HIGH);
  digitalWrite(dire2Pin, HIGH);
  delay(1000);
  //for (int n = 0; n < numberOfSteps; n++) {
  while(Homing1!=3 || Homing2!=3) {
    if(Homing1==0)digitalWrite(step1Pin, HIGH);
    if(Homing2==0) digitalWrite(step2Pin, HIGH);
    //delay(millisbetweenSteps);
    delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
    if(Homing1==0)digitalWrite(step1Pin, LOW);
    if(Homing2==0)digitalWrite(step2Pin, LOW);
    delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
    if(digitalRead(OptoLimit1)==0)Homing1=3;
    if(digitalRead(OptoLimit2)==0)Homing2=3;
    // delay(millisbetweenSteps);
  }
    digitalWrite(slee2Pin, LOW);
    digitalWrite(slee1Pin, LOW);
    digitalWrite(ledPin, !digitalRead(ledPin));
  //}
  
}
// These constants won't change.  They're used to give names
// to the pins used:

//UV system define
const int UV4InPin = A5;
const int UV3InPin = A4;
const int UV2InPin = A3;
const int UV1InPin = A2;
const int UVInPin[4] = {A2, A3, A4, A5};
int sensorUV[2][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}};

//Metal detector define
const int analogInPin1 = A0;  // Analog input pin that the potentiometer is attached to
const int analogInPin2 = A1;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 13; // Analog output pin that the LED is attached to
int sensorMetalValue1 = 0;        // value read from the pot
int sensorMetalValue2 = 0;       // value read from the pot

// HX711 define
#include "HX711.h"
#define DOUT 22
#define CLK  23
HX711 scale(DOUT, CLK);
float calibration_factor = 2125; //-7050 worked for my 440lb max scale setup
float units;
float ounces;
int outputValue = 0;        // value output to the PWM (analog out)

// servomotori define
//pin in uso 
//#define PIC32_USE_PIN9_FOR_M1_PWM
//#define PIC32_USE_PIN10_FOR_M1_PWM
//#define PIC32_USE_PIN11_FOR_M2_PWM
//#define MOTORLATCH 12
//#define MOTORCLK 4
//#define MOTORENABLE 7
//#define MOTORDATA 8

#include <AFMotor.h>
AF_Stepper motor1(48,1);
AF_Stepper motor2(48,2);

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  
  pinMode(analogOutPin, OUTPUT);
  for (int i = 0; i < 4; i++)
  {
    sensorUV[0][i] = analogRead(UVInPin[i]);// vettore iniziale
  }
  // HX711 init
  scale.set_scale();
  scale.tare();  //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
 
  //motor init
  motor1.setSpeed(600);  // 10 rpm  
  motor2.setSpeed(600);  // 10 rpm  
  motor1.step(1600, FORWARD, INTERLEAVE); 
  motor2.step(1600, BACKWARD, INTERLEAVE);
  motor1.release(); 
  motor2.release();
  delay(1000);
  motor2.step(1600, FORWARD, INTERLEAVE);
  delay(1000);
  motor2.release();
  delay(1000);
}

void loop() {
  // read the analog in value:
  sensorMetalValue1 = analogRead(analogInPin1);
  sensorMetalValue2 = analogRead(analogInPin2);

  for (int i = 0; i < 4; i++)
  {
    sensorUV[1][i] = analogRead(UVInPin[i]);// vettore di analisi
  }

  // print the results to the serial monitor:
  Serial.print("MetalDet1 = ");
  Serial.print(sensorMetalValue1);
  Serial.print("\t MetalDet2 = ");
  Serial.print(sensorMetalValue2);
  for (int i = 0; i < 4; i++)
  {
    Serial.print("\t UV");
    Serial.print(i);
    Serial.print(" = ");
    Serial.print(sensorUV[1][i]);//- sensorUV[0][i]);
  }
  Serial.println();
  
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  Serial.print("Reading: ");
  units = scale.get_units(), 10;
  if (units < 0) {
    units = 0.00;
  }
  ounces = units * 0.035274;
  Serial.print(units);
  Serial.print(" grams"); 
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();
  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  if ((sensorMetalValue1 > 900) || (sensorMetalValue2 > 900)) {
    digitalWrite(analogOutPin, HIGH);
  } else {
    digitalWrite(analogOutPin, LOW);
  }
  delay(1000);
}*/
