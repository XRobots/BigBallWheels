// Radio
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

#include <Servo.h>

Servo servo1; 
Servo servo2; 
Servo servo3; 

//**************remote control****************
struct RECEIVE_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
    int16_t menuDown;  
    int16_t Select;    
    int16_t menuUp;  
    int16_t toggleBottom;  
    int16_t toggleTop; 
    int16_t toggle1;
    int16_t toggle2;
    int16_t mode;  
    int16_t RLR;
    int16_t RFB;
    int16_t RT;
    int16_t LLR;
    int16_t LFB;
    int16_t LT;
};

RECEIVE_DATA_STRUCTURE mydata_remote;

int RLR = 0;
int RFB = 0;
int RFBa = 0;
int RT = 0;
int LLR = 0;
int LFB = 0;
int LT = 0;

int output1;
int output1a;
int output2;
int output2a;
int output3;
int output3a;

unsigned long currentMillis;
long previousMillis = 0;    // set up timers
long interval = 10;        // time constant for timer

long previousSafetyMillis;


void setup() {

    // initialize serial communication
    Serial.begin(115200);
    
    radio.begin();
    radio.openWritingPipe(addresses[0]); // 00002
    radio.openReadingPipe(1, addresses[1]); // 00001
    radio.setPALevel(RF24_PA_MIN);
    radio.startListening();

    servo1.attach(2);
    servo2.attach(3);
    servo3.attach(4);
   
}   // end of setup

// ********************* MAIN LOOP *******************************

void loop() {  

      
        currentMillis = millis();
        if (currentMillis - previousMillis >= 10) {  // start timed event
          
            previousMillis = currentMillis;


            // check for radio data
            if (radio.available()) {
                    radio.read(&mydata_remote, sizeof(RECEIVE_DATA_STRUCTURE));  
                    previousSafetyMillis = currentMillis;  
            }             

            if (currentMillis - previousSafetyMillis >= 300) {   
                  Serial.println("STOP");
                  mydata_remote.RFB = 512;
                  mydata_remote.RLR = 512;
                  mydata_remote.LT = 512;
            }


            // threshold remote data
            // some are reversed based on stick wiring in remote
            RFB = thresholdStick(mydata_remote.RFB);   
            RLR = thresholdStick(mydata_remote.RLR);
            LT = thresholdStick(mydata_remote.LT); 

            output1 = (RFB*-1) - (RLR * 0.66) + LT;
            output2 = RFB + (RLR * -0.66) + LT;
            output3 = RLR + LT;

            output1 = output1 + 1500;
            output2 = output2 + 1500;
            output3 = output3 + 1500;

            output1 = constrain(output1,1000,2000);
            output2 = constrain(output2,1000,2000);
            output3 = constrain(output3,1000,2000);

            servo1.writeMicroseconds(output1);
            servo2.writeMicroseconds(output2);
            servo3.writeMicroseconds(output3);

            Serial.print(output1);
            Serial.print(" , ");
            Serial.print(output2);
            Serial.print(" , ");
            Serial.println(output3);            

            


            
        }     // end of timed loop         

   
}       // end  of main loop
