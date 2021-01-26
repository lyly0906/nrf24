代码地址：C:\Users\ASUS\Documents\Arduino\nrf24_stm32_receiver_20200824

读取接收端nrf24的数据
数据中包含控制舵机、电机的数据，接线和发送端一至对应就可以了


舵机的控制（橙色线）接STM32的PA0

电机控制还需要增加一块L298H的精简模块，该模块有两路控制电机的输入和输出，我们只控制一个电机所以只用到一对 IN和OUT

IN1、IN2 分别对应 OUT1、OUT2，其中IN1、IN2 通过PWM脉冲方式控制电机的转速，IN1 pwm和IN2 0 与  IN1 0 IN2 pwm  分别正反方向 IN 0 IN 0 同时为零则停转。

IN1、IN2 分别接stm32 的PA1和 PA2， +、- 接 stm的5v和GND，out1 和out2  接电机的+ 、-

VCC->  3.3V
GND->  GND
CE->   PB10
CSN->  PB11
SCK->  PA5
MOSI-> PA7
MISO-> PA6



//  4 Channel Receiver | 4 Kanal Alıcı
//  PWM output on pins D2, D3, D4, D5 (Çıkış pinleri)
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
int ch_width_1 = 0;
int ch_width_2 = 0;
int ch_width_3 = 0;
int ch_width_4 = 0;
Servo ch1;
Servo ch2;
Servo ch3;
Servo ch4;
struct Signal {
byte throttle;      
byte pitch;
byte roll;
byte yaw;
};
Signal data;
const uint64_t pipeIn = 0xE9E8F0F0E1LL;
RF24 radio(PB10, PB11); 
int flag = 0;
int down = 300;


void ResetData()
{
// Define the inicial value of each data input. | Veri girişlerinin başlangıç değerleri
// The middle position for Potenciometers. (254/2=127) | Potansiyometreler için orta konum
data.throttle = 127; // Motor Stop | Motor Kapalı
data.pitch = 127;  // Center | Merkez
data.roll = 127;   // Center | Merkez
data.yaw = 107;   // Center | Merkez
}
void setup()
{
  //Set the pins for each PWM signal | Her bir PWM sinyal için pinler belirleniyor.
  ch1.attach(PA0);
  ch2.attach(PA1);
  ch3.attach(PA2);
  ch4.attach(5);
  pinMode(PC13, OUTPUT);
  //Configure the NRF24 module
  ResetData();
  radio.begin();
  radio.openReadingPipe(1,pipeIn);
  radio.startListening(); //start the radio comunication for receiver | Alıcı olarak sinyal iletişimi başlatılıyor
  Serial.begin(115200);
}
unsigned long lastRecvTime = 0;
void recvData()
{
 
while ( radio.available() ) {
radio.read(&data, sizeof(Signal));
lastRecvTime = millis();   // receive the data | data alınıyor
}
}
void loop()
{
if(flag==0){
  digitalWrite(PC13, LOW);
} 
flag++;
recvData();

unsigned long now = millis();
if ( now - lastRecvTime > 1000 ) {
  Serial.println("-------5555555------------");
  ResetData(); // Signal lost.. Reset data | Sinyal kayıpsa data resetleniyor
}

Serial.println(data.yaw);
Serial.println("-------------------");
Serial.println(data.pitch);
ch_width_1 = map(data.throttle, 0, 255, 1000, 2000);     // pin D2 (PWM signal)
ch_width_2 = map(data.pitch,    0, 127, 0, 255);     // pin D3 (PWM signal)
ch_width_3 = map(data.roll,     0, 127, 0, 255);     // pin D4 (PWM signal)
ch_width_4 = map(data.yaw,      0, 255, 1000, 2000);     // pin D5 (PWM signal)
// Write the PWM signal | PWM sinyaller çıkışlara gönderiliyor
Serial.println(ch_width_2);
Serial.println(ch_width_3);
//ch1.writeMicroseconds(ch_width_1);
if(ch_width_2){
  analogWrite(PA1,ch_width_2);
  analogWrite(PA2,0);
}else if(ch_width_3){
  analogWrite(PA2,ch_width_3);
  analogWrite(PA1,0);  
}else{
  analogWrite(PA1,0);
  analogWrite(PA2,0);
}
ch1.writeMicroseconds(ch_width_1);
//ch3.writeMicroseconds(ch_width_3);
ch4.writeMicroseconds(ch_width_4);

Serial.println(flag);
if(flag>3000){
  Serial.println(flag);
  digitalWrite(PC13, HIGH);
  down--;
}
if(down == 0){
  flag = 0;
  down=3000;
}

}