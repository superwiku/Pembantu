
#include <AccelStepper.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24.h>
#include <RF24Network.h>

RF24 radio(7,8);
RF24Network network(radio);

#define motorPin1  3      // IN1 pada ULN2003 driver
#define motorPin2  4      // IN2 pada ULN2003 driver
#define motorPin3  5     // IN3 pada ULN2003 driver
#define motorPin4  6     // IN4 pada ULN2003 driver
#define relay 2


#define MotorInterfaceType 8

const uint16_t node1 = 01;  
struct dataku{
  int gelapTerang;  
  bool gorden;
  bool jawab;
};
typedef struct dataku Dataku;
Dataku kiriman;
int perintah;

AccelStepper stepper = AccelStepper(MotorInterfaceType, motorPin1, motorPin3, motorPin2, motorPin4);
int x;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW); 
  radio.begin();
  network.begin(90, node1);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);

  stepper.setMaxSpeed(1000);
  Serial.println("slave");
  stepper.setCurrentPosition(0);
  while (stepper.currentPosition() != 4096) {
    stepper.setSpeed(1000);
    stepper.runSpeed();
  }
  delay(500);
  stepper.setCurrentPosition(0);
  while (stepper.currentPosition() != -4096) {
    stepper.setSpeed(-1000);
    stepper.runSpeed();
  }
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  network.update();
  while(network.available()){
    RF24NetworkHeader header;
    network.read(header,&kiriman,sizeof(kiriman));  
    if(kiriman.gelapTerang==1){     
        digitalWrite(relay, LOW); 
        putarKiri();
    }
    else if(kiriman.gelapTerang==2){     
        digitalWrite(relay, HIGH); 
        putarKanan();
    }else if(kiriman.gelapTerang==3){
        putarKanan();
    }else if(kiriman.gelapTerang==4){
        putarKiri();
    }else if(kiriman.gelapTerang==11){
        digitalWrite(relay, LOW);
    }else if(kiriman.gelapTerang==21){
        digitalWrite(relay, HIGH);
    }
   delay(500);
  }
}


void putarKanan(){
  stepper.setCurrentPosition(0);
  while (stepper.currentPosition() != 8192) {
    stepper.setSpeed(1000);
    stepper.runSpeed();
  }
}

void putarKiri(){
  stepper.setCurrentPosition(0);
  while (stepper.currentPosition() != -8192) {
    stepper.setSpeed(-1000);
    stepper.runSpeed();
  }
}
