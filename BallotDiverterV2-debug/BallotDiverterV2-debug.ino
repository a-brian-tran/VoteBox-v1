/* Ballot diverter control mechanism
* -----------------------
*/
#include <Servo.h>  // servo library

// I/O
const int OptInterrupt1Pin = A2; // select the input pin for the interrupter
const int OptInterrupt2Pin = A3; // select the input pin for the interrupter

const int SoundOption1=10; // Success Sound Pin
const int SoundOption2=11; // Fail Unrecognized Ballot pin
const int SoundOption3=12; // Fail Ballot Tracker pin
const int ExperimenterButton = 3;      // select the pin for the Experimenter's Hand Button
const int AcceptDeny = 4; //select pin for Accept/deny switch: high=deny, low=accept
const int BallotRejectPath = 5; //select pin for rejection path switch: low= front, high=reverse 
const int MotorRev = 6; //select pin for Feed Out signal
const int MotorEnable = 7; //select pin for Feed Enable signal
const int MotorFwd = 8; //select pin for Feed In signal
const int ServoControlPin = 9; //select pin for Servo Control
const int ledPin = 13;      // select the pin for the LED

// Constants
const int Threshold = 150;   // Optical Sensor threshold. Range 0-1023 in theory. If below threshold, there is paper present between emitter and sensor.
const int PositionDivert = 109; // Servo Position value to divert ballot
const int PositionStraight = 132; // Servo position value to allow straight-through ballot 
unsigned long fullFeed = 1500; //time in ms to feed a sheet of paper fully after clearing the sensors
unsigned long partialFeed = 1500; //Time in ms to partially feed the ballot 
unsigned long reverseFeed = 2500; //Time in ms to reject the ballot by reversing
unsigned long frontRejectFeed = 2500; //Time in ms to reject the ballot out the front after clearing the sensors
unsigned long Timer0 = 0;  // Variable to hold elapsed time for Timer 0

// Variables
unsigned long timer0 = 0;  // Variable to hold elapsed time for Timer 0
int optSensor1Val = 0; // variable to store the value coming from L sensor
int optSensor2Val = 0; // variable to store the value coming from R sensor
int experimenterButtonState = 0;
int acceptDenyState = 0;
int ballotPathState = 0;

Servo diverterservo;  // servo control object


void setup()
{
    // Initialize Button pins as input
    pinMode(ExperimenterButton, INPUT_PULLUP); 
    pinMode(AcceptDeny, INPUT_PULLUP);
    pinMode(BallotRejectPath, INPUT_PULLUP);
    // Initialize Control pins as output
    pinMode(SoundOption1, OUTPUT);
    pinMode(SoundOption2, OUTPUT);
    pinMode(SoundOption3, OUTPUT);
    pinMode(MotorRev, OUTPUT);
    pinMode(MotorEnable, OUTPUT);
    pinMode(MotorFwd, OUTPUT);
    pinMode(ServoControlPin, OUTPUT);
    pinMode(ledPin, OUTPUT); 
    // Initialize Servo
    diverterservo.attach(ServoControlPin);
    //Serial.begin(9600); // set up Serial library at 9600 bps
    //Set everything to off before we start
    digitalWrite(MotorEnable, LOW); //Turn motor on
    digitalWrite(MotorFwd, LOW); //Turn motor on forward
    digitalWrite(MotorRev, LOW); //   
    diverterservo.write(PositionStraight);      // sets the servo position to straight
    delay(15);                                  // waits for the servo to get there
    digitalWrite(SoundOption1, HIGH);
    digitalWrite(SoundOption2, HIGH);
    digitalWrite(SoundOption3, HIGH);
}

 
void acceptBallot(){    

     diverterservo.write(PositionStraight);      // sets the servo position to straight
     delay(15);                                  // waits for the servo to get there

     Timer0 = millis();

     while (millis()- Timer0 < fullFeed) //checks if fullFeed milliseconds have passed since the sensor has been clear of objects
     {
       optSensor1Val = analogRead(OptInterrupt1Pin);   // read the value from the sensor
       optSensor2Val = analogRead(OptInterrupt2Pin);   // read the value from the sensor

       if (optSensor1Val < Threshold || optSensor2Val < Threshold )   //check if either optical sensor is still tripped
           {
             Timer0 = millis();
           }
      digitalWrite(MotorEnable, HIGH); //Turn motor on
      digitalWrite(MotorFwd, HIGH); //Turn motor on forward
      digitalWrite(MotorRev, LOW); //
    }
      digitalWrite(MotorEnable, LOW);
      digitalWrite(SoundOption1, LOW);
      //Serial.println("--------------Ding?");
      
      delay (1500);
      digitalWrite(SoundOption1, HIGH);
      //Serial.println("+++++++++++  Dong?");

      delay (2000);
     
}
void rejectBackUp(){    

     diverterservo.write(PositionStraight);           // sets the servo position to divert
     delay(15);                                       // waits for the servo to get there

      digitalWrite(MotorEnable, HIGH); //Turn motor on
      digitalWrite(MotorFwd, HIGH); //Turn motor on forward
      digitalWrite(MotorRev, LOW); //^
      delay (partialFeed);
      digitalWrite(MotorFwd, HIGH); //Briefly hold motor
      digitalWrite(MotorRev, HIGH); //^
      delay (10);
      digitalWrite(MotorFwd, LOW); //Reverse motor
      digitalWrite(MotorRev, HIGH); //^
      delay (reverseFeed);      
      digitalWrite(MotorEnable, LOW); //Turn off motor
      digitalWrite(SoundOption2, LOW); // Play reject sound
      delay (1500);
      digitalWrite(SoundOption2, HIGH);
      
      while (optSensor1Val < Threshold || optSensor2Val < Threshold )
      { // Wait for paper path to clear
      digitalWrite(MotorEnable, LOW);
      optSensor1Val = analogRead(OptInterrupt1Pin);   // read the value from the sensor
      optSensor2Val = analogRead(OptInterrupt2Pin);   // read the value from the sensor

      }
        
}

void rejectOutFront()
{
     diverterservo.write(PositionDivert);                  // sets the servo position to divert
     delay(15);                                            // waits for the servo to get there

     Timer0 = millis();

     while (millis()- Timer0 < frontRejectFeed)
     {
       optSensor1Val = analogRead(OptInterrupt1Pin);   // read the value from the sensor
       optSensor2Val = analogRead(OptInterrupt2Pin);   // read the value from the sensor

       if (optSensor1Val < Threshold || optSensor2Val < Threshold )   //check if either optical sensor is still tripped, feed paper
           {
             Timer0 = millis();
           }
      digitalWrite(MotorEnable, HIGH); //Turn motor on
      digitalWrite(MotorFwd, HIGH);  //Turn motor on forward
      digitalWrite(MotorRev, LOW);   //^
    }
      digitalWrite(MotorEnable, LOW); //turn off motor
      digitalWrite(SoundOption2, LOW);// Play reject sound
      delay (1500);
      digitalWrite(SoundOption2, HIGH);
      diverterservo.write(PositionStraight);           // sets the servo position to straight

}
   
void loop()
{
    optSensor1Val = analogRead(OptInterrupt1Pin);   // read the value from the sensor
//    Serial.print("Opt 1 ");
//    Serial.println(optSensor1Val);                // print the sensor value to the serial monitor
    optSensor2Val = analogRead(OptInterrupt2Pin);  // read the value from the sensor
//    Serial.print("Opt 2 ");
//    Serial.println(optSensor2Val);                // print the sensor value to the serial monitor
    acceptDenyState = digitalRead(AcceptDeny);
    ballotPathState = digitalRead(BallotRejectPath);
    experimenterButtonState = digitalRead(ExperimenterButton);
    //Serial.print("Accept= ");
    //Serial.println(acceptDenyState==experimenterButtonState);     // print the accept/deny state to the serial monitor
    //Serial.print("Path= ");
    //if (ballotPathState==LOW)
    //{
    // Serial.println("Front");
    //}
    //else
    //{
    // Serial.println("Reverse Back");
    //}
      
  
     if (optSensor1Val < Threshold || optSensor2Val < Threshold )    //check if either optical sensor has been tripped
       {   
         if (acceptDenyState==experimenterButtonState)               //once paper is detected, check if ballot is accepted. 
         {   
          acceptBallot();                                            // Accept ballot function...                           
         } 
         else //otherwise reject
         {
 //            Serial.println("Rejected");
           //check reject path
            if (ballotPathState == LOW)                            // Switch off = reject front. on = reverse
               {
            //  Serial.println("--------------Reject Front");
                
                 rejectOutFront();                                  // Reject out the front
               }
            if (ballotPathState == HIGH)                           // if ballot is rejected, reverse it out
               {
            //   Serial.println("--------------Reject Back");
                rejectBackUp();                                    // Or spit it back
               }
          }
        }
}


   
 
