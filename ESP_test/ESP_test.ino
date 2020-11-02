//UART2をSHとのシリアル通信に使う
//SHから読み込んだデータをUART0でArduinoIDEシリアルモニタに送信
HardwareSerial Serial2(2);

long inputs[13];//シリアル読込格納配列
int count; //格納用カウント変数
long pend[4];//シリアルデコード後の倒立振子の変数

void setup() {

// シリアルポートを57600 bpsで初期化
  Serial.begin(57600);
  Serial2.begin(57600);

  count = 0;

  //シリアル通信データ格納配列初期化
  for(int i=0; i < 13; i++){
    inputs[i]=0; 
  }

  //倒立振子変数格納配列初期化
  for(int i=0; i < 4; i++){
    pend[i]=0;
  }

}

void loop() {

  long input_s; //シリアル読込変数
  int seri_d;  //serial_decodeの返り値を入れる

//  Serial.println(Serial.available());
//  Serial.print("count=");
//  Serial.println(count);

  while(Serial2.available() > 0){

    input_s = Serial2.read();

    if(input_s != -1 && (input_s != 0x0a || count < 13)){

      Serial.print("count=");
      Serial.print(count); 
   
      Serial.print("input=");
      Serial.println(input_s);

      inputs[count] = input_s;
      count++;

    }

    if(input_s == 0x0a){

      if(count == 13){//きちんと全要素読み込めた場合
      
      count = 0;
      seri_d=serial_decode();

      }else{//読み込めていなければcountリセット
        
        count = 0;
        
        }
      
    }
    
  }  

}


//受信値デコード
int serial_decode(){

  Serial.print("size of long=");
  Serial.println(sizeof(long));

  long all_sum = 0; //データの合計
  int low_sum; //合計の下位

  //誤り検出
  for(int i=0; i < 12; i++){

    all_sum = all_sum + inputs[i]; //全データの合計算出
    inputs[i] = inputs[i] - 0x30;
    
  }

  low_sum = (all_sum & 0x3f) + 0x30;

  Serial.print("all_sum=");
  Serial.println(all_sum);
  
  Serial.print("all_sum&=");
  Serial.println((all_sum & 0x3f), BIN); 

  Serial.print("check_sum=");
  Serial.println(inputs[12]); 

  Serial.print("low_sum=");
  Serial.println(low_sum);

  Serial.print("low_sum_2=");
  Serial.println(low_sum, BIN); 
  

  if(low_sum != inputs[12]){
    
    Serial.println("Find bit error.");
    return -1;
    
    }

  //タイムスタンプ
  pend[0] = (inputs[0] << 6*3) + (inputs[1] << 6*2) + (inputs[2] << 6) + inputs[3];

  //土台角度
  pend[1] = (inputs[4] << 6*2) + (inputs[5] << 6) + inputs[6];

  //振子角度
  pend[2] = (inputs[7] << 6) + inputs[8];

  //印加電圧
  pend[3] = (inputs[9] << 6*2) + (inputs[10] << 6) + inputs[11];

  //-----------------------------
  //受信値確認
  Serial.print("time=");
  Serial.println(pend[0]);

  Serial.print("base=");
  Serial.println(pend[1]);

  Serial.print("arm=");
  Serial.println(pend[2]);

  Serial.print("pwm=");
  Serial.println(pend[3]);

  return 0;
  
}
