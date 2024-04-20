//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////
// определение режима соединения и подключение библиотеки RemoteXY 
#define REMOTEXY_MODE__ESP32CORE_BLE

#include <BLEDevice.h>

// настройки соединения 
#define REMOTEXY_BLUETOOTH_NAME "IPhone(Egor)"
#define REMOTEXY_ACCESS_PASSWORD "201272"

#include <RemoteXY.h>

// конфигурация интерфейса RemoteXY  
#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 89 bytes
  { 255,4,0,131,0,82,0,17,0,0,0,31,1,200,198,1,1,5,0,1,
  56,93,53,53,0,6,31,83,101,110,100,32,67,111,100,101,0,3,40,166,
  158,25,135,135,16,10,133,91,57,57,112,4,1,31,208,147,208,187,209,131,
  209,136,208,184,208,187,208,186,208,176,0,31,67,254,24,206,7,0,2,26,
  131,3,4,67,34,124,4,36,16 };
  
// структура определяет все переменные и события вашего интерфейса управления 
struct {

    // input variables
  uint8_t sendcodes; // =1 если кнопка нажата, иначе =0
  uint8_t select_1; // =0 если переключатель в положении A, =1 если в положении B, ...
  uint8_t btglush; // =1 если включено, иначе =0
  uint8_t select_2; // =0 если переключатель в положении A, =1 если в положении B, ...

    // output variables
  char text_01[131]; // =строка UTF8 оканчивающаяся нулем

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)
 
/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

#include "BluetoothSerial.h"
BluetoothSerial SerialBT; // Объект для Bluetooth

#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();

long value;
int bitlength;
int protocol;
int pulselength;
int incoming;
#define txPin 32  // Передатчик
#define rxPin 33 // Приемник
int speeds = 4;
unsigned int freq;

//#define pulseAN 412                     // длительность импульса AN-Motors
//AN MOTORS
//volatile long c1 = 0;                   // переменная для отправки
//volatile long c2 = 0;                   // переменная для отправки



void setup()
{
  pinMode(txPin, OUTPUT);
  Serial.begin(115200);
  SerialBT.begin("IPhone(Egor)");
  speeds = 4;
  freq = 1000;
  mySwitch.enableReceive(rxPin);
  mySwitch.enableTransmit(txPin);
  Serial.println("B-Sniff,C-Nice,D-Came: Vertical");
  SerialBT.println("B-Sniff,C-Nice,D-Came: Vertical");
  char str[] = "B-Sniff,C-Nice,D-Came: Vertical";
  delay(1000);
  RemoteXY_Init();
  strcpy(RemoteXY.text_01, str);
}

void loop ()
{
  RemoteXY_Handler ();
  if (RemoteXY.sendcodes!=0) {
    send();
  }
  switch (RemoteXY.select_1) {
    case 0:
      break;
    case 1:
      speeds = 2;
      break;
    case 2:
      speeds = 3;
      break;
    case 3:
      speeds = 4;
      break;
    case 4:
      speeds = 5;
      break;
    case 5:
      speeds = 6;
      break;
    case 6:
      speeds = 7;
      break;
  }
  switch (RemoteXY.select_2) {
    case 0:
      break;
    case 1:
      priem();
      break;
    case 2:
      Serial.println("[+] Nice");
      nice();
      break;
    case 3:
      Serial.println("[+] Came");
      came();
      break;
  }
  if(RemoteXY.btglush!=0) {
    tone(txPin, freq);
    char str[] = "Jummer ON!";
    sprintf(RemoteXY.text_01, "%s Freq is: %d Hz", str, freq);
  }
  else {
    noTone(txPin);
  }
  restarts();
}

void restarts()
{
  if (SerialBT.available()) // Проверяем, не получили ли мы что-либо от Bluetooth модуля
  {
    incoming = SerialBT.read(); // Читаем, что получили
    if (incoming == 33)  // Если значение равно !
    {
      ESP.restart();
    }
    if (incoming == 43)  // Если значение равно +
    {
      freq=freq+1000;
      SerialBT.print("JM:Freq+:  ");
      SerialBT.println(freq);
    }
    if (incoming == 45)  // Если значение равно -
    {
      freq=freq-1000;
      SerialBT.print("JM:Freq-:  ");
      SerialBT.println(freq);
    }  
  }
}

void send()
{
  mySwitch.setProtocol(protocol);
  mySwitch.setPulseLength(pulselength);
  mySwitch.send(value, bitlength);
  mySwitch.send(value, bitlength);
  RemoteXY_delay(1);
  char str[] = "Send";
  sprintf(RemoteXY.text_01, "%s value %d bit: %d protocol: %d pulse: %d", str, value,bitlength,protocol,pulselength);
}

void priem()
{
  if (mySwitch.available()) {
    value = mySwitch.getReceivedValue();
    bitlength = mySwitch.getReceivedBitlength();
    protocol = mySwitch.getReceivedProtocol();
    pulselength = mySwitch.getReceivedDelay();
    output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(),mySwitch.getReceivedProtocol());
    char str[] = "Rx";
    sprintf(RemoteXY.text_01, "%s value %d bit: %d protocol: %d pulse: %d", str, value,bitlength,protocol,pulselength);
    mySwitch.resetAvailable();
  }
}

void nice()
{
for (int send_code = 0; send_code < 4096; send_code++) // цикли перебора всех кодов для 12 бит
  {
    for (int j = 0; j < speeds; j++) // в пультах используется 4 повторения
    {
      digitalWrite(txPin, HIGH); // стартовый импульс
      delayMicroseconds(700);
      digitalWrite(txPin, LOW);
      for (int i = 12; i > 0; i--)
      {
        boolean bit_code = bitRead(send_code, i - 1);
        if (bit_code)
        {
          digitalWrite(txPin, LOW); // единица
          delayMicroseconds(1400);
          digitalWrite(txPin, HIGH);
          delayMicroseconds(700);
        }
        else
        {
          digitalWrite(txPin, LOW); // ноль
          delayMicroseconds(700);
          digitalWrite(txPin, HIGH);
          delayMicroseconds(1400);
        }
      }
      digitalWrite(txPin, LOW); // пилотный период
      delayMicroseconds(25200);
      Serial.println("Nice");
      Serial.println(send_code);
      SerialBT.println(send_code);
      restarts();
    }
  }
}

void came()
{
  for (int send_code = 0; send_code < 4096; send_code++) // цикли перебора всех кодов для 12 бит
  {
    for (int j = 0; j < speeds; j++) // в пультах используется 4 повторения
    {
      digitalWrite(txPin, HIGH); // стартовый импульс
      delayMicroseconds(320);
      digitalWrite(txPin,LOW);
      for (int i = 12; i > 0; i--)
      {
        boolean bit_code = bitRead(send_code, i - 1);
        if (bit_code)
        {
          digitalWrite(txPin, LOW); // единица
          delayMicroseconds(640);
          digitalWrite(txPin, HIGH);
          delayMicroseconds(320);
        }
        else
        {
          digitalWrite(txPin, LOW); // ноль
          delayMicroseconds(320);
          digitalWrite(txPin, HIGH);
          delayMicroseconds(640);
        }
      }
      digitalWrite(txPin, LOW); // пилотный период
      delayMicroseconds(11520);
      Serial.println("Came");
      Serial.println(send_code);
      SerialBT.println(send_code);
      restarts();
    }
  }
}
/*
void SendANMotors(long c1, long c2) 
{
  for (int j = 0; j < speeds; j++)  {
    //отправка 12 начальных импульсов 0-1
    for (int i = 0; i < 12; i++) {
      delayMicroseconds(pulseAN);
      digitalWrite(txPin, HIGH);
      delayMicroseconds(pulseAN);
      digitalWrite(txPin, LOW);
    }
    delayMicroseconds(pulseAN * 10);
    //отправка первой части кода
    for (int i = 32; i > 0; i--) {
      SendBit(bitRead(c1, i - 1), pulseAN);
    }
    //отправка второй части кода
    for (int i = 32; i > 0; i--) {
      SendBit(bitRead(c2, i - 1), pulseAN);
    }
    //отправка бит, которые означают батарею и флаг повтора
    SendBit(1, pulseAN);
    SendBit(1, pulseAN);
    delayMicroseconds(pulseAN * 39);
  }
  Serial.println("AN-Motors");
  Serial.println(String(c1, HEX) + " " + String(c2, HEX));
  SerialBT.println(String(c1, HEX) + " " + String(c2, HEX));
}

void SendBit(byte b, int pulse) {
  if (b == 0) {
    digitalWrite(txPin, HIGH);        // 0
    delayMicroseconds(pulse * 2);
    digitalWrite(txPin, LOW);
    delayMicroseconds(pulse);
  }
  else {
    digitalWrite(txPin, HIGH);        // 1
    delayMicroseconds(pulse);
    digitalWrite(txPin, LOW);
    delayMicroseconds(pulse * 2);
  }
}
*/