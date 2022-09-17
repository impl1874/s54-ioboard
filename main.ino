int LED1=16;//GPIO16--D0
int LED2=0;//GPIO0--D3
int LED3=2;//GPIO2--D4
int LED4=15;//GPIO15--D8

int SW1=5;//GPIO5--D1
int SW2=4;//GPIO4--D2
int SW3=14;//GPIO14--D5
int SW4=12;//GPIO12--D6
int SW5=13;//GPIO13--D7

byte variable[8];
byte variableIndex = 0;
byte sw_status;

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
    while(Serial.available()){
      variable[variableIndex++] = Serial.read();
    }
    handle_frame();
    write_sw_status();
    delay(10);
}

//解析收到的数据
void handle_frame(){
  if (is_abort()) {
    //过滤脏数据
    abort_first();
    return;
  }
  if(variableIndex < 2){
    return;
  }
  //ACK指令
  if(is_ack()){
    variableIndex = 0;
  }
  //同步指令
  else if (is_sync()){
    variableIndex = 0;
    byte bytes[2];
    bytes[0] = 0x7E;
    bytes[1] = 0x81;
    for (int i = 0; i < sizeof(bytes); i++) {
      Serial.write(bytes[i]&0xFF);
    }
  }
  //自检指令
  else if (is_self_check()){
    variableIndex = 0;
    sw_status = 0;
    //ACK
    {
      byte bytes[2];
      bytes[0] = 0x7E;
      bytes[1] = 0xA3;
      for (int i = 0; i < sizeof(bytes); i++) {
        Serial.write(bytes[i]&0xFF);
      }
    }
    //设备状态
    {
      byte bytes[6];
      bytes[0] = 0x7E;
      bytes[1] = 0x00;
      bytes[2] = 0x03;
      bytes[3] = 0xDF;//TODO重复发送标记
      bytes[4] = sw_status;
      bytes[5] = bytes[3] + bytes[4]%256;//完整性
      for (int i = 0; i < sizeof(bytes); i++) {
        Serial.write(bytes[i]&0xFF);
      }
    }
  }
  //初始化指令
  else if (is_init()){
    variableIndex = 0;
    //ACK
    {
      byte bytes[2];
      bytes[0] = 0x7E;
      bytes[1] = 0x83;
      for (int i = 0; i < sizeof(bytes); i++) {
        Serial.write(bytes[i]&0xFF);
      }
    }
    {
      write_led();
    }
  }
  //状态上报返回
  else if (is_status_replay()){
    variableIndex = 0;
  }
  //控制指令
  else if (is_ctrol()){
    variableIndex = 0;
    //ACK
    {
      byte bytes[2];
      bytes[0] = 0x7E;
      bytes[1] = 0xA3;
      for (int i = 0; i < sizeof(bytes); i++) {
        Serial.write(bytes[i]&0xFF);
      }
    }
    {
      write_led();
    }
  }
}

//是否过滤脏数据
boolean is_abort(){
  return variable[0]&0xFF != 0x7E || variableIndex > 6;
}

//ACK指令
boolean is_ack(){
  return variable[1] == 0xA3;
}

//同步指令
boolean is_sync(){
  return variable[1] == 0xA0;
}

//自检指令
boolean is_self_check(){
  return variable[1] == 0x00 && variable[2] == 0x03 && variable[3] == 0x00 && variable[4] == 0x00 && variable[5] == 0x00;
}

//初始化指令
boolean is_init(){
  return variable[1] == 0x20 && variable[2] == 0x03 && variableIndex >= 6;
}

//状态上报返回
boolean is_status_replay(){
  return variable[1] == 0x83;
}

//控制指令
boolean is_ctrol(){
  return variable[1] == 0x00 && variable[2] == 0x03 && variableIndex >= 6;
}

//发送开关状态
void write_sw_status(){
  int new_sw = read_sw();
    if(new_sw!=sw_status){
      sw_status=new_sw;
      byte bytes[6];
      bytes[0] = 0x7E;
      bytes[1] = 0x20;
      bytes[2] = 0x03;
      bytes[3] = 0xDF;//TODO重复发送标记
      bytes[4] = sw_status;
      bytes[5] = bytes[3] + bytes[4]%256;//完整性
      for (int i = 0; i < sizeof(bytes); i++) {
        Serial.write(bytes[i]&0xFF);
      }
    }
}

//读取开关状态
int read_sw(){
      int sw = 0;
      if(digitalRead(SW1) == HIGH){
        sw+=B1;
      }
      if(digitalRead(SW2) == HIGH){
        sw+=B10;
      }
      if(digitalRead(SW3) == HIGH){
        sw+=B100;
      }
      if(digitalRead(SW4) == HIGH){
        sw+=B1000;
      }
      if(digitalRead(SW5) == HIGH){
        sw+=B10000;
      }
      return sw;
}

//控制led灯
void write_led(){
  //LED1
      if((variable[3]&0B1)==0B1){
        digitalWrite(LED1,LOW);
      } else{
        digitalWrite(LED1,HIGH);
      }
   //LED2
      if((variable[3]&0B10)==0B10){
        digitalWrite(LED2,LOW);
      } else{
        digitalWrite(LED2,HIGH);
      }
      
   //LED3
      if((variable[3]&0B100)==0B100){
        digitalWrite(LED3,LOW);
      } else{
        digitalWrite(LED3,HIGH);
      } 
      
   //LED4
      if((variable[3]&0B1000)==0B1000){
        digitalWrite(LED4,LOW);
      } else{
        digitalWrite(LED4,HIGH);
      }  
}

//过滤脏数据
void abort_first(){
  for (int i = 0; i < variableIndex; i++) {
      variable[i] = variable[i+1];
  }
  variableIndex--;
}
