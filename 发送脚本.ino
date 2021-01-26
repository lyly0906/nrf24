�����ַ��C:\Users\ASUS\Documents\Arduino\nrf24_stm32_20200824

��������ң�˰�ť��Ϊ���ƶ���͵���ķ�����ٶȣ�
�������ֻ����x��������������������ֱ��Ӧx��y���������
nrf24�ķ��Ͷ˴������ߣ�ֱ��7������

���ƶ����ҡ��x������ӵ�  PA0
���Ƶ����ҡ��x�ᡢy��ֱ�ӵ�PA1��PA2
ҡ�˵�+��-��stm32��5v��GND

VCC->  3.3V
GND->  GND
CE->   PB10
CSN->  PB11
SCK->  PA5
MOSI-> PA7
MISO-> PA6

// 4 Channel Transmitter | 4 Kanal Verici
// 4 Channel Transmitter | 4 Kanal Verici
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
const uint64_t pipeOut = 0xE9E8F0F0E1LL;   //IMPORTANT: The same as in the receiver 0xE9E8F0F0E1LL | Bu adres al?c? ile ayn? olmal?
RF24 radio(PB10, PB11); // select CE,CSN pin | CE ve CSN pinlerin se?imi
int flag = 0;
int down = 2;
struct Signal {
byte throttle;
byte pitch;
byte roll;
byte yaw;
};
Signal data;
void ResetData() 
{
data.throttle = 107; // Motor Stop (254/2=127)| Motor Kapal? (Signal lost position | sinyal kesildi?indeki pozisyon)
data.pitch = 107; // Center | Merkez (Signal lost position | sinyal kesildi?indeki pozisyon)
data.roll = 127; // Center | merkez (Signal lost position | sinyal kesildi?indeki pozisyon)
data.yaw = 127; // Center | merkez (Signal lost position | sinyal kesildi?indeki pozisyon)
}
void setup()
{
//Start everything up
pinMode(PC13, OUTPUT);
radio.begin();
radio.openWritingPipe(pipeOut);
radio.stopListening(); //start the radio comunication for Transmitter | Verici olarak sinyal ileti?imi ba?lat?l?yor
ResetData();
Serial.begin(9600);
}
// Joystick center and its borders | Joystick merkez ve s?n?rlar?
int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
{
val = constrain(val, lower, upper);
if ( val < middle )
val = map(val, lower, middle, 0, 128);
else
val = map(val, middle, upper, 128, 255);
return ( reverse ? 255 - val : val );
}
void loop()
{
if(flag==0){
  digitalWrite(PC13, LOW);
} 
flag++;  
// Control Stick Calibration | Kumanda Kol Kalibrasyonlar?
// Setting may be required for the correct values of the control levers. | Kollar?n do?ru de?erleri i?in ayar gerekebilir.
data.throttle = mapJoystickValues( analogRead(PA0), 12, 524, 1020, true );  //���ƶ��

data.roll = mapJoystickValues( analogRead(PA2), 524, 524, 1015, true );   //�����ת    // "true" or "false" for servo direction | "true" veya "false" servo y?n��n�� belirler

data.pitch = mapJoystickValues( analogRead(PA1), 525, 524, 1015, true ); //�����ת mapJoystickValues( 14, 12, 524, 1020, true );     // "true" or "false" for servo direction | "true" veya "false" servo y?n��n�� belirler
Serial.println(data.pitch);
data.yaw = mapJoystickValues( 15, 12, 524, 1020, true );       // "true" or "false" for servo direction | "true" veya "false" servo y?n��n�� belirler
radio.write(&data, sizeof(Signal));

Serial.println(flag);
if(flag>30){
  Serial.println(flag);
  digitalWrite(PC13, HIGH);
  down--;
}
if(down == 0){
  flag = 0;
  down=30;
}
}
