void setup() {

// シリアルポートを57600 bpsで初期化
  Serial.begin(57600);

}

void loop() {

  long input_s; //シリアル読込変数
  long inputs[13];//シリアル読込格納配列
  int count = 0;
  long pend[4];//シリアルデコード後の倒立振子の変数

  for(int i=0; i < 4; i++){

    pend[i]=0;
    
  }

  for(int i=0; i < 13; i++){

    inputs[i]=0;
    
  }

  input_s = Serial.read();
  
  
  while(input_s != 0x0a || count < 13){

    if(input_s != -1){
    
      inputs[count] = input_s;
      count++;

      Serial.print("input=");
      Serial.println(inputs[count-1]);

      Serial.print("count=");
      Serial.println(count); 

    }
    
  }

  serial_decode(inputs, pend);

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
  

}

void serial_decode(long inputs[13], long pend[4]){

  long all_sum = 0; //データの合計
  int low_sum; //合計の下位

  //誤り検出
  for(int i=0; i < 11; i++){

    all_sum = all_sum + inputs[i]; //全データの合計算出
    inputs[i] = inputs[i] - 0x30;
    
  }

  low_sum = (all_sum & 0x3f) + 0x30;

  if(low_sum != inputs[12]){Serial.println("Find bit error.");}

  //タイムスタンプ
  pend[0] = (inputs[0] << 6*3) + (inputs[1] << 6*2) + (inputs[2] << 6) + inputs[3];

  //土台角度
  pend[1] = (inputs[4] << 6*2) + (inputs[5] << 6) + inputs[6];

  //振子角度
  pend[2] = (inputs[7] << 6) + inputs[8];

  //印加電圧
  pend[3] = (inputs[9] << 6*2) + (inputs[10] << 6) + inputs[11];
  
}
