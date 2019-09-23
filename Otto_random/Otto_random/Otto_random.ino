#include <Servo.h> 
#include <Oscillator.h>
#include <US.h>
#include <Otto.h>
Otto Otto;  //This is Otto!

  #define PIN_YL 2 //servo[2]
  #define PIN_YR 3 //servo[3]
  #define PIN_RL 4 //servo[4]
  #define PIN_RR 5 //servo[5]

  #define ECHOPIN 9        // Pin to receive echo pulse
  #define TRIGPIN 8        // Pin to send trigger pulse


int T=10;              //Initial duration of movement
int moveId=0;            //Number of movement
int moveSize=5;         //Asociated with the height of some movements

bool obstacleDetected = false;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  //Set the servo pins
  Otto.init(PIN_YL,PIN_YR,PIN_RL,PIN_RR,true, -1, 10, 8, 9);
  Otto.sing(S_connection); //Otto wake up!
  Otto.home();
  delay(1000);
  Otto.sing(S_happy); // a happy Otto :)


}

void loop() {
  // put your main code here, to run repeatedly:

Otto.moonwalker(8,400, 15, 1);
Otto.shakeLeg(3, 500, 1);
Otto.updown(5, 500, 25);
Otto.tiptoeSwing(3, 600, 20);
Otto.ascendingTurn(5, 400, 10);


}


//void obstacleDetector(){
//// Start Ranging
//  digitalWrite(TRIGPIN, LOW);
//  delayMicroseconds(2);
//  digitalWrite(TRIGPIN, HIGH);
//  delayMicroseconds(10);
//  digitalWrite(TRIGPIN, LOW);
//// Compute distance
//  float distance = pulseIn(ECHOPIN, HIGH);
//  distance= distance/58;
//  Serial.print(distance);
//  Serial.println("cm");
// if(distance<15){
//          obstacleDetected = true;
//          Serial.println ("Obstacle Detected! Avoid collision");
//        }else{
//          obstacleDetected = false;
//          Serial.println ("No Obstacle detected! - Keep on walking");
//        }
//  
//}
