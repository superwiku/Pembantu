#include <CTBot.h>
#include <Wire.h>
#include <SPI.h>
#define ldr A0
#define sensor_hujan D2
#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24Network.h>
#include <RF24.h>
RF24 radio(D4,D8);
RF24Network network(radio);
const uint16_t pusat = 00;
const uint16_t node1 = 01;  
const uint16_t node2 = 02;

struct dataku{
  int gelapTerang; 
  bool gorden;
  bool jawab;
};

typedef struct dataku Dataku;
Dataku kiriman;

struct pembantuBelakang{
  int pesan;
};

typedef struct pembantuBelakang PembantuBelakang;
PembantuBelakang pembantubelakang;

CTBot myBot;
String ssid = "superwiku";
String pass = "7129wiku";
String token = "1222734353:AAEWyRCPWIClinpb0m1C26V1ev5AZFpOqsY";
const int id = 896305362;
TBMessage msg;
int nilai;
int kondisi_sensor;
String statusx;
String statusxh;

String statusaktual;
String statusaktualh;
bool statusGorden;
bool statusAtap;
bool statusPompa;

void setup() {
  // put your setup code here, to run once:
  pinMode(sensor_hujan,INPUT);
  Wire.begin(4,5);
  SPI.begin();
  Serial.begin(115200);
  radio.begin();
  network.begin(90, pusat);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);

  myBot.wifiConnect(ssid, pass);  
  myBot.setTelegramToken(token);
  myBot.sendMessage(id,"Pembantu siap melayani boss...");
}

void loop() {  
 network.update();
 
 if(!myBot.testConnection()){ 
  reconnect();  
  Serial.println("terkoneksi kembali");
  delay(1000);
 }
 
 kondisi_sensor = digitalRead(sensor_hujan);
 Serial.print("Kondisi cuaca = ");
 Serial.println(kondisi_sensor);
 if (kondisi_sensor == HIGH){    
    statusaktualh="panas";
 }else if(kondisi_sensor==LOW){
    statusaktualh="hujan";
 }
 jawabHujan(statusaktualh);
  
 nilai=analogRead(ldr);
 Serial.print("Nilai LDR = ");
 Serial.println(nilai);
 if(nilai<=600){
  statusaktual="terang"; 
 }else{
  statusaktual="gelap";    
 } 
 jawabGorden(statusaktual); 
 
 if(myBot.getNewMessage(msg)){
  if(msg.text.equalsIgnoreCase("gorden")){
    gorden();
  }else if(msg.text.equalsIgnoreCase("buka")){
    bukaGorden();
  }else if(msg.text.equalsIgnoreCase("tutup")){
    tutupGorden();
  }else if(msg.text.equalsIgnoreCase("siapa kamu")){
    myBot.sendMessage(id,"saya pembantumu yang setia boss..");
  }else if(msg.text.equalsIgnoreCase("kamu cewek apa cowok")){
    myBot.sendMessage(id,"nah, yang itu gak jelas boss, hehe..");
  }else if(msg.text.equalsIgnoreCase("siapa namamu")){
    myBot.sendMessage(id,"waktu lahir sih Juminten boss, tapi sekarang Juminto, kan gak jelas cewek apa cowok, hehe");
  }else if(msg.text.equalsIgnoreCase("berapa umurmu")){
    myBot.sendMessage(id,"emm..mungkin sekitar 3 minggu boss, baru kok aku diciptakan ama mas wiku yang ganteng");
  }else if(msg.text.equalsIgnoreCase("kamu udah punya pacar")||msg.text.equalsIgnoreCase("kamu udah punya istri")||msg.text.equalsIgnoreCase("kamu udah punya suami")){
    myBot.sendMessage(id,"kan aku cewek apa cowok juga gak jelas boss, gimana nyari pasangannya??");
  }else if(msg.text.equalsIgnoreCase("atap")){
    atap();
  }else if(msg.text.equalsIgnoreCase("buka atap")){
    bukaAtap();
  }else if(msg.text.equalsIgnoreCase("tutup atap")){
    tutupAtap();
  }else if(msg.text.equalsIgnoreCase("pompa")){
    pompa();
  }else if(msg.text.equalsIgnoreCase("pompa on")){
    nyalakanPompa();
  }else if(msg.text.equalsIgnoreCase("pompa off")){
    matikanPompa();
  }else{
    myBot.sendMessage(id,"saya gak ngerti maksudmu boss..");
  }
 }
}

void jawabGorden(String statusnya){  
  RF24NetworkHeader header2(node1);
  RF24NetworkHeader header3(node2);
  String statusy=statusnya;
  if(statusx!=statusy){
    if(statusy.equalsIgnoreCase("gelap")){
      pembantubelakang.pesan=6;  
      if(statusGorden==true){
        kiriman.gelapTerang=1;            
        bool ok = network.write(header2,&kiriman,sizeof(kiriman));
        bool ok2 = network.write(header3,&pembantubelakang,sizeof(pembantubelakang));
        myBot.sendMessage(id,"lampu nyala & gorden sudah ditutup");
        statusGorden=false;
      }else{
        kiriman.gelapTerang=11;
        bool ok = network.write(header2,&kiriman,sizeof(kiriman));
        bool ok2 = network.write(header3,&pembantubelakang,sizeof(pembantubelakang));
        myBot.sendMessage(id,"lampu nyala & gorden kondisi tertutup");
      }      
    } else if(statusy.equalsIgnoreCase("terang")){
      pembantubelakang.pesan=7;  
      if(statusGorden==false){
        kiriman.gelapTerang=2;
        bool ok = network.write(header2,&kiriman,sizeof(kiriman));
        bool ok2 = network.write(header3,&pembantubelakang,sizeof(pembantubelakang));
        myBot.sendMessage(id,"lampu mati & gorden sudah dibuka");
        statusGorden=true;
      }else{
        kiriman.gelapTerang=21;
        bool ok = network.write(header2,&kiriman,sizeof(kiriman));
        bool ok2 = network.write(header3,&pembantubelakang,sizeof(pembantubelakang));
        myBot.sendMessage(id,"lampu mati & gorden kondisi terbuka");
      }      
    }
    statusx=statusy;
  }  
}

void jawabHujan(String statusnya){  
  RF24NetworkHeader header3(node2);
  String statusyh=statusnya;
  if(statusxh!=statusyh){
    if(statusyh.equalsIgnoreCase("panas")){
      if(statusAtap==false){
        pembantubelakang.pesan=0;      
        bool ok = network.write(header3,&pembantubelakang,sizeof(pembantubelakang));
        myBot.sendMessage(id,"cuaca panas dan atap sudah dibuka");
        statusAtap=true;
      }else{
        myBot.sendMessage(id,"cuaca panas dan atap sudah kondisi terbuka");
      }      
    } else if(statusyh.equalsIgnoreCase("hujan")){
      if(statusAtap==true){
        pembantubelakang.pesan=1;
        bool ok = network.write(header3,&pembantubelakang,sizeof(pembantubelakang));
        myBot.sendMessage(id,"cuaca hujan dan atap sudah ditutup");
        statusAtap=false;
      }else{
       myBot.sendMessage(id,"cuaca hujan dan atap sudah kondisi tertutup");
      }      
    }
    statusxh=statusyh;
  }  
}

void gorden(){
  if(statusGorden==false){
      myBot.sendMessage(id,"Gorden tertutup boss...");
    }else if(statusGorden==true){
      myBot.sendMessage(id,"Gorden terbuka boss...");
    }  
}

void atap(){
  if(statusAtap==false){
      myBot.sendMessage(id,"Atap tertutup boss...");
    }else if(statusAtap==true){
      myBot.sendMessage(id,"Atap terbuka boss...");
    }  
}

void pompa(){
  if(statusPompa==false){
      myBot.sendMessage(id,"Pompa mati boss...");
    }else if(statusPompa==true){
      myBot.sendMessage(id,"Pompa hidup boss...");
    }  
}

void bukaGorden(){
  RF24NetworkHeader header4(node1);
  if(statusGorden==false){
        kiriman.gelapTerang=3;
        bool ok = network.write(header4,&kiriman,sizeof(kiriman));        
        myBot.sendMessage(id,"Gorden sudah dibuka boss...");
    }else{
        myBot.sendMessage(id,"Gorden sudah terbuka boss...");
    } 
   statusGorden=true;  
}

void tutupGorden(){
  RF24NetworkHeader header5(node1);
  if(statusGorden==true){
        kiriman.gelapTerang=4;
        bool ok = network.write(header5,&kiriman,sizeof(kiriman));        
        myBot.sendMessage(id,"Gorden sudah ditutup boss...");
    }else{
        myBot.sendMessage(id,"Gorden sudah tertutup boss...");
    } 
   statusGorden=false;  
}

void bukaAtap(){
  RF24NetworkHeader header6(node2);
  if(statusAtap==false){
        pembantubelakang.pesan=2;
        bool ok = network.write(header6,&pembantubelakang,sizeof(pembantubelakang));        
        myBot.sendMessage(id,"Atap sudah dibuka boss...");
    }else{
        myBot.sendMessage(id,"Atap sudah terbuka boss...");
    } 
   statusAtap=true;  
}

void tutupAtap(){
  RF24NetworkHeader header7(node2);
  if(statusAtap==true){
        pembantubelakang.pesan=3;
        bool ok = network.write(header7,&pembantubelakang,sizeof(pembantubelakang));        
        myBot.sendMessage(id,"Atap sudah ditutup boss...");
    }else{
        myBot.sendMessage(id,"Atap sudah tertutup boss...");
    } 
   statusAtap=false;  
}


void nyalakanPompa(){
  RF24NetworkHeader header8(node2);
  if(statusPompa==false){
        pembantubelakang.pesan=4;
        bool ok = network.write(header8,&pembantubelakang,sizeof(pembantubelakang));        
        myBot.sendMessage(id,"Pompa sudah dinyalakan boss...");
    }else{
        myBot.sendMessage(id,"Pompa kondisi nyala boss...");
    } 
   statusPompa=true;  
}

void matikanPompa(){
  RF24NetworkHeader header9(node2);
  if(statusPompa==true){
        pembantubelakang.pesan=5;
        bool ok = network.write(header9,&pembantubelakang,sizeof(pembantubelakang));        
        myBot.sendMessage(id,"Pompa sudah dimatikan boss...");
    }else{
        myBot.sendMessage(id,"Pompa kondisi mati boss...");
    } 
   statusPompa=false;  
}

void reconnect(){
  myBot.wifiConnect(ssid, pass);  
  myBot.setTelegramToken(token);
}
