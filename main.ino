int LED1=16;//GPIO16--D0
int LED2=0;//GPIO0--D3
int LED3=2;//GPIO2--D4
int LED4=15;//GPIO15--D8

int SW1=5;//GPIO5--D1
int SW2=4;//GPIO4--D2
int SW3=14;//GPIO14--D5
int SW4=12;//GPIO12--D6
int SW5=13;//GPIO13--D7

byte STX = 0x7E;
String CMD_SYNC_RECV = "7EA0";
String CMD_SYNC_REPLY = "7E81";
String CMD_CHECK_RECV = "7E0003000000";
String CMD_CHECK_REPLY = "7EA3";

byte variable[8];
byte variableIndex = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);

  pinMode(SW1,INPUT);
  pinMode(SW2,INPUT);
  pinMode(SW3,INPUT);
  pinMode(SW4,INPUT);
  pinMode(SW5,INPUT);
}

void loop() {
  while(Serial.available()) {
    add_data(Serial.read());
  }
}

void add_data(byte data){
  variable[variableIndex++] = data;
  if (variable[0] != 0x7E) {
    variableIndex = 0;
    return;
  }
  if(variableIndex < 2){
    return;
  }
  
  for (int i = 1; i < sizeof(variable); i++) {
    
  }

  variable_abort(variableIndex-sizeof(variable));
  String str = bytes_2_hex_string(variable,variableIndex);
  if (CMD_SYNC_RECV.equals(str)){
    byte bytes[CMD_SYNC_REPLY.length()/2];
    hex_string_2_bytes(bytes, sizeof(bytes) ,CMD_SYNC_REPLY);
    for (int i = 0; i < sizeof(bytes); i++) {
      Serial.write(bytes[i]&0xff);
    }
    variableIndex = 0;
  } else if (CMD_CHECK_RECV.equals(str)){
    byte bytes[CMD_CHECK_REPLY.length()/2];
    hex_string_2_bytes(bytes, sizeof(bytes) ,CMD_CHECK_REPLY);
    for (int i = 0; i < sizeof(bytes); i++) {
      Serial.write(bytes[i]&0xff);
    }
    variableIndex = 0;
  }
}

void variable_abort(int count){
  if (count<=0){
      return;
  }
  for (int i = 0; i < count; j++) {
    for (int j = 0; i < sizeof(variable); j++) {
      variable[j] = variable[j+1];
      variable[j+1] = 0;
    }
    variableIndex--;
  }
}

String bytes_2_hex_string(byte bytes[],int length) {
  String str = "";
  for (int i = 0; i < length; i++) {
    char buffer[1];
    sprintf(buffer, "%02x", bytes[i] & 0xFF);
    str += String(buffer);
  }
  str.toUpperCase();
  return str;
}

void hex_string_2_bytes(byte bytes[],int length, String hex) {
  String data = hex.substring(0,length*2);
  for (int i = 0; i < length; i++) {
      bytes[i] = (byte)hstol(data.substring(i * 2, i * 2 + 2));
  }
}

long hstol(String recv){
  char c[recv.length() + 1];
  recv.toCharArray(c, recv.length() + 1);
  return strtol(c, NULL, 16); 
}


// void loop()
// {
//   while (Serial.available() > 0) {
//     byte b = Serial.read();
//     variable[variableIndex++] = b;
//   }
//   if(variableIndex != 2){
//     return;
//   }
//   if(variable[0] != STX){
//     variableIndex = 0;
//     return;
//   }
//   array_to_string(variable, 6, str);
//   StrUID = str;
  
//   if (StrUID == CMD_SYNC_RECV) {
//     Serial.write(STX);
//     Serial.write(CMD_SYNC_REPLY);
//   } else if(variable[1] == 0x00){
//     if (variable[2] == 0x03){
//       if(variable[3] == 0x00 || variable[4] == 0x00  || variable[5] == 0x00){
//         // 自检
//       Serial.write(STX);
//       Serial.write(CMD_CHECK_REPLY);
//      }else{
      
//      }
//     }
//     delay(100);
//     Serial.write(STX);
//     Serial.write(0x00);
//     Serial.write(0x03);
//     Serial.write(0xFF);
//     Serial.write(0x03);
//     Serial.write(0x02);
//   }
//   variableIndex = 0;
//  digitalWrite(LED1,LOW);
//  delay(100);
//  digitalWrite(LED1,HIGH);
//  delay(100);
//
//  digitalWrite(LED2,LOW);
//  delay(100);
//  digitalWrite(LED2,HIGH);
//  delay(100);
//
//  digitalWrite(LED3,LOW);
//  delay(100);
//  digitalWrite(LED3,HIGH);
//  delay(100);
//
//  digitalWrite(LED4,LOW);
//  delay(100);
//  digitalWrite(LED4,HIGH);
//  delay(100);
//
//  int sws1 = digitalRead(SW1);  
//  int sws2 = digitalRead(SW2);
//  int sws3 = digitalRead(SW3);
//  int sws4 = digitalRead(SW4);
//  int sws5 = digitalRead(SW5);
  
//  Serial.printf("%d%d%d%d%d\n", sws1,sws2,sws3,sws4,sws5);
//Serial.print(78, HEX) 得到 "4E"
//  Serial.swap();
// }
// char nibble2c(char c) 
// { 
//     if ((c>='0') && (c<='9')) 
//      return c-'0' ; 
//     if ((c>='A') && (c<='F')) 
//      return c+10-'A' ; 
//     if ((c>='a') && (c<='a')) 
//      return c+10-'a' ; 
//     return -1 ; 
// } 

// char hex2c(char c1, char c2) 
// { 
//     if(nibble2c(c2) >= 0) 
//     return nibble2c(c1)*16+nibble2c(c2) ; 
//     return nibble2c(c1) ; 
// } 

// String hex2str(char *data) 
// { 
//     String result = "" ; 
//     for (int i=0 ; nibble2c(data[i])>=0 ; i++) 
//     { 
//      result += hex2c(data[i],data[i+1]) ; 
//      if(nibble2c(data[i+1])>=0) 
//      i++ ; 
//     } 
//     return result; 
// } 
