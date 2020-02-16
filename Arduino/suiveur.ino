#include "WiFiEsp.h"
#include <WiFiEspClient.h>
#include <PubSubClient.h>
#include "SoftwareSerial.h"

//E2 LEFT
//E1 RIGHT
#define rxPin 11 // Broche 11 en tant que RX, à raccorder sur TX du HC-05
#define txPin 10 // Broche 10 en tant que TX, à raccorder sur RX du HC-05
const int ButtonPin=3;

int tmep_flag=0;
SoftwareSerial Serial1(12,13); // RX, TX
SoftwareSerial mySerial(rxPin, txPin);


const char clientID[] = "test001";
const char inTopic[]= "robot";
const char outTopic[]= "PC";
char ssid[] = "FSFR";            // your network SSID (name)
char pass[] = "LUSHUAISHUAI.1996";        // your network password


int status = WL_IDLE_STATUS;     // the Wifi radio's status
char MqttServer[] = "192.168.43.149";
unsigned long lastSend;
WiFiEspClient espClient;
PubSubClient client(espClient);

char cmd;
/* Photo sensor pins */
int left_PS   = 2;
int middle_PS = 8;
int right_PS  = 9;

/* CONFIGURATION FOR TABLE COMMAND */
#define RENVOYER '0'
#define TABL_1  '1'
#define TABL_2  '2'
#define TABL_3  '3'
#define TABL_4  '4'

#define TABL_1_INIT  'a'
#define TABL_2_INIT  'b'
#define TABL_3_INIT  'c'
#define TABL_4_INIT  'd'

#define STOP      'S'
#define FORWARD   'F'
#define BACKWARD  'B'
#define TURNLEFT  'L'
#define TURNRIGHT 'R'

#define MODE_BLE  'B'
#define MODE_AUTO 'A'

char num_table='0';

int button=0;

int tabl1_flag=1;
int tabl2_flag=1;
int tabl3_flag=1;
int tabl4_flag=1;

int tabl1_init_flag=1;
int tabl2_init_flag=1;
int tabl3_init_flag=1;
int tabl4_init_flag=1;
/************************************/

/* PIN CONFIGURATION FOR ROBOT WHEELS */
int E1 = 6;
int M1 = 7;
int E2 = 5;
int M2 = 4;

void motorRun(int cmd)
{
  //Serial.println("Start!");
  switch(cmd){
    case FORWARD:
      Serial.println("FORWARD"); //输出状态
      digitalWrite(M1,HIGH);
      digitalWrite(M2,HIGH);
      analogWrite(E1, 60);   
      analogWrite(E2, 80); 
      delay(20);
      break;
     case BACKWARD:
      Serial.println("BACKWARD"); //输出状态
      digitalWrite(M1,LOW);
      digitalWrite(M2,LOW);
      analogWrite(E1, 60);   
      analogWrite(E2, 80); 
      delay(100);
      break;
     case TURNRIGHT:
      Serial.println("TURN  RIGHT 90"); //输出状态
      digitalWrite(M1,HIGH);
      digitalWrite(M2,HIGH);
      analogWrite(E1, 0);
      analogWrite(E2, 90);
      delay(3400);  
      break;
     case TURNLEFT:
      Serial.println("TURN LEFT 90"); //输出状态
      digitalWrite(M1,HIGH);
      digitalWrite(M2,HIGH);
      analogWrite(E1, 80);   
      analogWrite(E2, 0); 
      delay(2200);  
      break;
     case STOP:
      Serial.println("STOP"); //输出状态
      digitalWrite(M1,LOW);
      digitalWrite(M2,LOW);
      analogWrite(E1, LOW);   
      analogWrite(E2, LOW);   
      delay(200);
  }
}

void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  num_table = (char)payload[0];
  Serial.println(cmd);
  //motorRun(cmd);
}


void goto_table_1(){
  //int flag=1;
  int count=0;
  Serial.println("FORWARD"); //输出状态
  digitalWrite(M1,HIGH);
  digitalWrite(M2,HIGH);
  analogWrite(E1, 60);   
  analogWrite(E2, 80); 
  delay(1000);  
  Serial.println("go to table 1!\n");
  
  while(tabl1_flag){
    int read_left = digitalRead(left_PS);
    int read_middle = digitalRead(middle_PS);
    int read_right = digitalRead(right_PS);
    Serial.print(read_left);
    Serial.print(read_middle);
    Serial.println(read_right);
    if(read_left==1&&read_middle==0&&read_right==1){ //101
      motorRun(FORWARD);
    }
    
    if(read_left==1&&read_middle==0&&read_right==0){//100 right
      digitalWrite(M1,HIGH);
      digitalWrite(M2,HIGH);
      analogWrite(E1, 0);
      analogWrite(E2, 80);
      delay(170);
    }
    
    if(read_left==0&&read_middle==0&&read_right==1){//001 left
      digitalWrite(M1,HIGH);
      digitalWrite(M2,HIGH);
      analogWrite(E1, 80);   
      analogWrite(E2, 0); 
      delay(170);
    }

        
    if(read_left==1&&read_middle==1&&read_right==0){//110 right
      digitalWrite(M1,HIGH);
      digitalWrite(M2,HIGH);
      analogWrite(E1, 0);
      analogWrite(E2, 80);
      delay(170);
      }

        
    if(read_left==0&&read_middle==1&&read_right==1){ //011
      digitalWrite(M1,HIGH);
      digitalWrite(M2,HIGH);
      analogWrite(E1, 80);   
      analogWrite(E2, 0); 
      delay(170);
    }
    
    if(read_left==1&&read_middle==1&&read_right==1){ //111
      int stop_flag=0;
      digitalWrite(M1,HIGH);
      digitalWrite(M2,HIGH);
      analogWrite(E1, 80);   
      analogWrite(E2, 0); 
      delay(170);
      read_left = digitalRead(left_PS);
      read_middle = digitalRead(middle_PS);
      read_right = digitalRead(right_PS);
      if(read_left==1&&read_middle==1&&read_right==1)
         stop_flag=1;
      else
         stop_flag=0;

      digitalWrite(M1,HIGH);
      digitalWrite(M2,HIGH);
      analogWrite(E1, 0);
      analogWrite(E2, 80);
      delay(170);
      read_left = digitalRead(left_PS);
      read_middle = digitalRead(middle_PS);
      read_right = digitalRead(right_PS);
      if(read_left==1&&read_middle==1&&read_right==1)
         stop_flag=1;
       else
       stop_flag=0;
      if(stop_flag==1){
        motorRun(STOP);
        tabl1_flag=0;
        tabl1_init_flag=1;
       while(!digitalRead(ButtonPin));
       num_table=TABL_1_INIT;
      }
    }
    
    if(read_left==0&&read_middle==0&&read_right==0){
      count++;
      if(count==1){
        digitalWrite(M1,HIGH);
        digitalWrite(M2,HIGH);
        analogWrite(E1, 60);   
        analogWrite(E2, 80); 
        delay(1800);
        /*
        digitalWrite(M1,HIGH);
        digitalWrite(M2,HIGH);
        analogWrite(E1, 0);
        analogWrite(E2, 80);
        delay(700);  */
        motorRun(TURNLEFT);
      }
      else{
         motorRun(FORWARD);
      }
    }
  }
}

void goto_table_2(){
  int count=0;
  Serial.println("go to table 2!\n");
  digitalWrite(M1,HIGH);
  digitalWrite(M2,HIGH);
  analogWrite(E1, 60);   
  analogWrite(E2, 80); 
  delay(1000);  
  while(tabl2_flag){
    int read_left = digitalRead(left_PS);
    int read_middle = digitalRead(middle_PS);
    int read_right = digitalRead(right_PS);
    Serial.print(read_left);
    Serial.print(read_middle);
    Serial.println(read_right);
    if(read_left==1&&read_middle==0&&read_right==1){ //101
      motorRun(FORWARD);
    }
    
    if(read_left==1&&read_middle==0&&read_right==0){//100 right
      digitalWrite(M1,HIGH);
      digitalWrite(M2,HIGH);
      analogWrite(E1, 0);
      analogWrite(E2, 80);
      delay(150);
    }
    
    if(read_left==0&&read_middle==0&&read_right==1){//001 left
      digitalWrite(M1,HIGH);
      digitalWrite(M2,HIGH);
      analogWrite(E1, 80);   
      analogWrite(E2, 0); 
      delay(150);
    }

        
    if(read_left==1&&read_middle==1&&read_right==0){//110 right
      digitalWrite(M1,HIGH);
      digitalWrite(M2,HIGH);
      analogWrite(E1, 0);
      analogWrite(E2, 80);
      delay(150);
      }

        
    if(read_left==0&&read_middle==1&&read_right==1){ //001
      digitalWrite(M1,HIGH);
      digitalWrite(M2,HIGH);
      analogWrite(E1, 80);   
      analogWrite(E2, 0); 
      delay(150);
    }
    
    if(read_left==1&&read_middle==1&&read_right==1){ //111
      int stop_flag=0;
      digitalWrite(M1,HIGH);
      digitalWrite(M2,HIGH);
      analogWrite(E1, 80);   
      analogWrite(E2, 0); 
      delay(150);
      read_left = digitalRead(left_PS);
      read_middle = digitalRead(middle_PS);
      read_right = digitalRead(right_PS);
      if(read_left==1&&read_middle==1&&read_right==1)
         stop_flag=1;
      else
         stop_flag=0;

      digitalWrite(M1,HIGH);
      digitalWrite(M2,HIGH);
      analogWrite(E1, 0);
      analogWrite(E2, 80);
      delay(150);
      read_left = digitalRead(left_PS);
      read_middle = digitalRead(middle_PS);
      read_right = digitalRead(right_PS);
      if(read_left==1&&read_middle==1&&read_right==1)
         stop_flag=1;
       else
        stop_flag=0;
       
      if(stop_flag==1){
        motorRun(STOP);
        tabl2_flag=0;
        tabl2_init_flag=1;
       while(!digitalRead(ButtonPin));
       num_table=TABL_2_INIT;
      }
    }
    
    if(read_left==0&&read_middle==0&&read_right==0){
      count++;
      if(count==1){
        digitalWrite(M1,HIGH);
        digitalWrite(M2,HIGH);
        analogWrite(E1, 60);   
        analogWrite(E2, 80); 
        delay(2000);
        motorRun(TURNRIGHT);
      }
      else{
         motorRun(FORWARD);
      }
    }
  }
}

void goto_table_3(){
}


void goto_table_4(){
}

void tabl1_to_init(){
  int count=0;
  while(tabl1_init_flag){
    int read_left = digitalRead(left_PS);
    int read_middle = digitalRead(middle_PS);
    int read_right = digitalRead(right_PS);
    Serial.print(read_left);
    Serial.print(read_middle);
    Serial.println(read_right);
    if(read_left==1&&read_middle==0&&read_right==1){ //101
        motorRun(BACKWARD);
    }
    
    if(read_left==1&&read_middle==0&&read_right==0){//100 right
      digitalWrite(M1,LOW);
      digitalWrite(M2,LOW);
      analogWrite(E1, 80);
      analogWrite(E2, 0);
      delay(100);
    }
    
    if(read_left==0&&read_middle==0&&read_right==1){//001 left
      digitalWrite(M1,LOW);
      digitalWrite(M2,LOW);
      analogWrite(E1, 0);   
      analogWrite(E2, 80); 
      delay(100);
    }

        
    if(read_left==1&&read_middle==1&&read_right==0){//110 right
      digitalWrite(M1,LOW);
      digitalWrite(M2,LOW);
      analogWrite(E1, 70);
      analogWrite(E2, 0);
      delay(100);
      }

        
    if(read_left==0&&read_middle==1&&read_right==1){ //011
      digitalWrite(M1,LOW);
      digitalWrite(M2,LOW);
      analogWrite(E1, 0);   
      analogWrite(E2, 70); 
      delay(100);
    }
 
    if(read_left==1&&read_middle==1&&read_right==1){ //111 stop
        motorRun(BACKWARD);
    }
    
    if(read_left==0&&read_middle==0&&read_right==0){
      count++;
      if(count==1){
        digitalWrite(M1,HIGH);
        digitalWrite(M2,HIGH);
        analogWrite(E1, 60);   
        analogWrite(E2, 80); 
        delay(800);

        Serial.println("TURN  RIGHT 90"); 
        digitalWrite(M1,LOW);
        digitalWrite(M2,LOW);
        analogWrite(E1, 80);
        analogWrite(E2, 0);
        delay(3800);  
        digitalWrite(M1,LOW);
        digitalWrite(M2,LOW);
        analogWrite(E1, 0);
        analogWrite(E2, 80);
        delay(800);  
         //delay(1000);  //gai le

        digitalWrite(M1,LOW);
        digitalWrite(M2,LOW);
        analogWrite(E1, 80);   
        analogWrite(E2, 100); 
        delay(2000);
      }
      if(count==2){
        motorRun(STOP);
        tabl1_init_flag=0;
        tabl1_flag=1;
        tabl2_flag=1;
        tabl3_flag=1;
        tabl4_flag=1;
        //client.publish("PC","R");
        num_table='0';
      }
    }
  }
}

void tabl2_to_init(){
  int count=0;
  while(tabl2_init_flag){
    int read_left = digitalRead(left_PS);
    int read_middle = digitalRead(middle_PS);
    int read_right = digitalRead(right_PS);
    Serial.print(read_left);
    Serial.print(read_middle);
    Serial.println(read_right);
    if(read_left==1&&read_middle==0&&read_right==1){ //101
        motorRun(BACKWARD);
    }
    
    if(read_left==1&&read_middle==0&&read_right==0){//100 right
      digitalWrite(M1,LOW);
      digitalWrite(M2,LOW);
      analogWrite(E1, 80);
      analogWrite(E2, 0);
      delay(100);
    }
    
    if(read_left==0&&read_middle==0&&read_right==1){//001 left
      digitalWrite(M1,LOW);
      digitalWrite(M2,LOW);
      analogWrite(E1, 0);   
      analogWrite(E2, 80); 
      delay(100);
    }

        
    if(read_left==1&&read_middle==1&&read_right==0){//110 right
      digitalWrite(M1,LOW);
      digitalWrite(M2,LOW);
      analogWrite(E1, 70);
      analogWrite(E2, 0);
      delay(100);
      }

        
    if(read_left==0&&read_middle==1&&read_right==1){ //011
      digitalWrite(M1,LOW);
      digitalWrite(M2,LOW);
      analogWrite(E1, 0);   
      analogWrite(E2, 70); 
      delay(100);
    }
 
    if(read_left==1&&read_middle==1&&read_right==1){ //111 stop
        motorRun(BACKWARD);
    }
    
    if(read_left==0&&read_middle==0&&read_right==0){
      count++;
      if(count==1){
        digitalWrite(M1,HIGH);
        digitalWrite(M2,HIGH);
        analogWrite(E1, 60);   
        analogWrite(E2, 80); 
        delay(1000);

        Serial.println("TURN  LEFT 90"); 
        digitalWrite(M1,LOW);
        digitalWrite(M2,LOW);
        analogWrite(E1, 0);
        analogWrite(E2, 90);
        delay(3500);  
        
        digitalWrite(M1,LOW);
        digitalWrite(M2,LOW);
        analogWrite(E1, 80);
        analogWrite(E2, 0);
        delay(100);  
        
        digitalWrite(M1,LOW);
        digitalWrite(M2,LOW);
        analogWrite(E1, 70);   
        analogWrite(E2, 90); 
        delay(1800);
      }
      if(count==2){
        motorRun(STOP);
        tabl2_init_flag=0;
        tabl1_flag=1;
        tabl2_flag=1;
        tabl3_flag=1;
        tabl4_flag=1;
        //client.publish("PC","R");
        num_table='0';
      }
    }
  }
}

void tabl3_to_init(){
  
}

void tabl4_to_init(){
  
}

void setup()
{
    //motor
    pinMode(M1, OUTPUT);
    pinMode(M2, OUTPUT);
    pinMode(ButtonPin, INPUT);
    // define pin modes for tx, rx pins:
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    mySerial.begin(9600);

    // initialize serial for debugging
    Serial.begin(9600);
    // initialize serial for ESP module
    Serial1.begin(9600);

    
    // initialize ESP module
    WiFi.init(&Serial1);
  
    // check for the presence of the shield
    if(WiFi.status() == WL_NO_SHIELD){
      Serial.println("WiFi shield not present");
      // don't continue
      while (true);
    }
  
    // attempt to connect to WiFi network
    while (status != WL_CONNECTED){
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(ssid);
      status = WiFi.begin(ssid, pass);
    }
  
    Serial.println("You're connected to the network");
  
    client.setServer( MqttServer, 1883 );
    client.setCallback(callback);  
    

    //client.publish("PC","R");

    delay(1500);
}

void loop()
{
    if(!client.connected()) {
      reconnect();
    }
    //delay(200);
    client.loop();
    
    if(num_table==MODE_BLE){
       //usart read
      if(mySerial.available()>0)
      {
        cmd = mySerial.read();//读取蓝牙模块发送到串口的数据
        Serial.println(cmd);
        motorRun(cmd);
      }  
    }
    else{
       switch(num_table){
          case TABL_1: client.publish("PC","O");goto_table_1();break;
          case TABL_2: client.publish("PC","O");goto_table_2();break;
          case TABL_3: client.publish("PC","O");goto_table_3();break;
          case TABL_4: client.publish("PC","O");goto_table_4();break;
          case TABL_1_INIT:   tabl1_to_init();break;
          case TABL_2_INIT:   tabl2_to_init();break;
          case TABL_3_INIT:   tabl3_to_init();break;
          case TABL_4_INIT:   tabl4_to_init();break;
          case RENVOYER:      client.publish("PC","R"); Serial.println("Wait table number!");delay(1000);break;

 
          default:
            Serial.println("Unrecognized table number!\n");
      }
    }
}

void reconnect(){
  while (!client.connected()) {
    if (client.connect(clientID)){
      Serial.println("connected");
      //client.publish("PC","OK");
      if(tmep_flag==0){
         client.publish("PC","R");
         tmep_flag++;
      }
      client.subscribe(inTopic);
      Serial.println("wait topic");
      Serial.println(inTopic);
    } 
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
