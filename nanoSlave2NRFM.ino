#include <AccelStepper.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24.h>
#include <RF24Network.h>

#define motorPin1  3      // IN1 pada ULN2003 driver
#define motorPin2  4      // IN2 pada ULN2003 driver
#define motorPin3  5     // IN3 pada ULN2003 driver
#define motorPin4  6     // IN4 pada ULN2003 driver
#define MotorInterfaceType 8
 
RF24 radio(7,8);
RF24Network network(radio);
const uint16_t node2 = 02; 

struct pembantuBelakang{
  int pesan;
};

typedef struct pembantuBelakang PembantuBelakang;
PembantuBelakang pembantubelakang;

const int rEx = 9;
const int rPompa = 10;
AccelStepper stepper = AccelStepper(MotorInterfaceType, motorPin1, motorPin3, motorPin2, motorPin4);

void setup() {
  // put your setup code here, to run once:
pinMode (rEx, OUTPUT);
pinMode (rPompa, OUTPUT);
radio.begin();
network.begin(90, node2);
radio.setPALevel(RF24_PA_LOW);
radio.setDataRate(RF24_250KBPS);
digitalWrite(rEx,HIGH);
digitalWrite(rPompa,HIGH);
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
    network.read(header, &pembantubelakang, sizeof(pembantubelakang)); // Read the incoming data
    if(pembantubelakang.pesan==0){
      putarKiri();
    }else if(pembantubelakang.pesan==1){
      putarKanan();
    }else if(pembantubelakang.pesan==2){
      putarKiri();
    }else if(pembantubelakang.pesan==3){
      putarKanan();
    }else if(pembantubelakang.pesan==4){
      digitalWrite(rPompa,LOW);
    }else if(pembantubelakang.pesan==5){
      digitalWrite(rPompa,HIGH);
    }else if(pembantubelakang.pesan==6){
      digitalWrite(rEx,LOW);
    }else if(pembantubelakang.pesan==7){
      digitalWrite(rEx,HIGH);
    }
  }
  delay (1000);
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
