#include <Ethernet.h>
#include <SPI.h>


#define pin_outdoor_pir 2
#define pin_indoor_pir 3
#define pin_bright_led 11
#define pin_trigger_ussrf 5
#define pin_echo_ussrf 4
#define pin_magnetic A8
#define pin_buzz 24
#define STATUS_CONNECTED 1
#define STATUS_DISCONNECTED 0
#define STATUS_NO_HUMAN_DETECTED "0"
#define STATUS_HUMAN_DETECTED "1"
#define STATE_NULL "NULL"
#define STATE_DETECTED "DETECTED"
#define DOOR_OPEN 1
#define DOOR_CLOSE 0
#define API_KEY "66589ae77387a90660219a2aad624e94"



char namaServer[] = "169.254.2.183";
char inString[1024];
char charFromWeb[9];


byte IP_eth[] = {169,254,2,184};
byte MAC_eth[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

int calibrationTime = 30; 
int iterasi = 0;
int counter_outdoor_detected = 0;
int counter_outdoor_no_detect = 0;
int counter_indoor_detected = 0;
int counter_indoor_no_detect = 0;
int counter_door_open = 0;
int counter_door_close = 0;


boolean lockLow = true;
boolean lockLow2 = true;
boolean takeLowTime;
boolean takeLowTime2;  
boolean outdoor_pin_status = false;
boolean indoor_pin_status = false;
boolean startRead = false; 

long unsigned int lowIn;
long unsigned int lowIn2; 
long unsigned int pause = 100;  

String password;
String secure_key;

EthernetClient myEthernet;

void setup() {
  Serial.begin(9600);
  Serial.println("--------------------------------------------------"); 
  Serial.println("Setting Perangkat");
  Serial.println("Setting PIR dan teman-teman");
  pinMode(pin_trigger_ussrf, OUTPUT);
  pinMode(pin_echo_ussrf, INPUT);
  pinMode(pin_outdoor_pir, INPUT);
  digitalWrite(pin_outdoor_pir, LOW);
  pinMode(pin_indoor_pir, INPUT);
  digitalWrite(pin_indoor_pir, LOW);
  pinMode(pin_bright_led, OUTPUT);
  //--
  pinMode(pin_buzz,OUTPUT);
  pinMode(pin_magnetic,INPUT);
  //--
  Serial.print("calibrating sensor ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(".");
    delay(100);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
  Serial.println("Mohon menunggu . . . ");
  Serial.println("Setting Ethernet MAC Address dan IP Address");
  Serial.println("Mohon menunggu . . . ");
  if (Ethernet.begin(MAC_eth) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(MAC_eth,IP_eth);
  }
 // Ethernet.begin(MAC_eth,IP_eth);
  delay(1000);
  Serial.println("Setting Perangkat selesai!");
  Serial.println("--------------------------------------------------");
}

void loop() {
  Serial.println("--------------------------------------------------------------");
  iterasi++;
  Serial.print("Iterasi ke : ");
  Serial.println(iterasi);
  int resultBukaKoneksi = bukaKoneksi();
  String data = collecting_sensor(outdoor_pir(),indoor_pir(),ussrf(),magnetic());
  if(resultBukaKoneksi==1){
      kirim_data(data);
      Serial.println();
  }
  Serial.println("--------------------------------------------------------------\n");
  delay(3000);
}

String collecting_sensor(boolean outdoor_pir,boolean indoor_pir,int ussrf,int magnetic_sw){
  String data,outdoor,indoor,state,state_indoor,state_outdoor,state_magnetic;
  if(outdoor_pir){
    outdoor = STATUS_HUMAN_DETECTED;
    counter_outdoor_detected ++;
    counter_outdoor_no_detect = 0;
  }else{
    outdoor = STATUS_NO_HUMAN_DETECTED;
    counter_outdoor_detected = 0;
    counter_outdoor_no_detect ++;   
  }
  if(indoor_pir){
    indoor = STATUS_HUMAN_DETECTED;
    counter_indoor_detected ++;
    counter_indoor_no_detect = 0;
  }else{
    indoor = STATUS_NO_HUMAN_DETECTED;    
    counter_indoor_detected = 0;
    counter_indoor_no_detect ++;
  }

  if(magnetic_sw == DOOR_OPEN){
    counter_door_open ++;
    counter_door_close = 0;
  }else{
    counter_door_open = 0;
    counter_door_close ++;
  }

  if(counter_outdoor_detected < 3){
    state_outdoor = "NORMAL";
  }else
  if(counter_outdoor_detected >= 3 && counter_outdoor_detected <= 5 ){
    state_outdoor = "SIAGA";
  }else
  if(counter_outdoor_detected > 5){
    state_outdoor = "AWAS";
  }
  
  if(counter_indoor_detected < 3){
    state_indoor = "NORMAL";
  }else
  if(counter_indoor_detected >= 3 && counter_indoor_detected <= 5 ){
    state_indoor = "SIAGA";
  }else
  if(counter_indoor_detected > 5){
    state_indoor = "AWAS";
  }

  if(counter_door_open < 3){
    state_magnetic = "NORMAL";
  }else
  if(counter_door_open >= 3 && counter_door_open <= 5 ){
    state_magnetic = "SIAGA";
  }else
  if(counter_door_open > 5){
    state_magnetic = "AWAS";
  }

  state = state_outdoor + "_" + state_indoor + "_" + state_magnetic ;  
  
  data =  state + "/" + outdoor + "/" + indoor + "/" + ussrf + "/"  + magnetic_sw + "/" + API_KEY + "/";
  return data;
}

boolean outdoor_pir(){
  if (digitalRead(pin_outdoor_pir) == HIGH){
    if (lockLow) {
      lockLow = false;
      outdoor_pin_status = true;
      delay(50);
    }
    takeLowTime = true;
  }
  if (digitalRead(pin_outdoor_pir) == LOW) {
    if (takeLowTime) {
      lowIn = millis();          //save the time of the transition from high to LOW
      takeLowTime = false;       //make sure this is only done at the start of a LOW phase
    }
    if (!lockLow && millis() - lowIn > pause) {
      lockLow = true;
      outdoor_pin_status = false;
      delay(50);
    }
  }
  return outdoor_pin_status;
}


boolean indoor_pir(){
  if (digitalRead(pin_indoor_pir) == HIGH){
    if (lockLow2) {
      lockLow2 = false;
      indoor_pin_status = true;
      delay(50);
    }
    takeLowTime2 = true;
  }
  if (digitalRead(pin_indoor_pir) == LOW) {
    if (takeLowTime2) {
      lowIn2 = millis();          //save the time of the transition from high to LOW
      takeLowTime2 = false;       //make sure this is only done at the start of a LOW phase
    }
    if (!lockLow2 && millis() - lowIn2 > pause) {
      lockLow2 = true;
      indoor_pin_status = false;
      delay(50);
    }
  }
  return indoor_pin_status;
}

int ussrf(){
  long duration, distance;
  digitalWrite(pin_trigger_ussrf, LOW);   // Added this line
  delayMicroseconds(2);         // Added this line
  digitalWrite(pin_trigger_ussrf, HIGH);  //  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10);        // Added this line
  digitalWrite(pin_trigger_ussrf, LOW);
  duration = pulseIn(pin_echo_ussrf, HIGH);
  distance = (duration/2) / 29.1;
  delay(500);
  return distance;
}


int magnetic(){
  int magnet;
  int baca_switch = analogRead(pin_magnetic);
  Serial.print("  > > > > ");
  Serial.println(baca_switch);
  if(baca_switch < 1020){
    digitalWrite(24,HIGH);
    delay(500);
    digitalWrite(24,LOW);
    delay(100);
    magnet = DOOR_OPEN;
  }else{
    digitalWrite(24,LOW);
    magnet = DOOR_CLOSE;
  }  
  return magnet;
}

int bukaKoneksi(){
  Serial.print("Mencoba sambungan ke server http://"); 
  Serial.println(namaServer);  
  Serial.println("Mohon menunggu . . . ");
  if(myEthernet.connect(namaServer,80)){
    Serial.println("Sambungan ke server berhasil!");
    return STATUS_CONNECTED; 
  }else{
    Serial.print("Sambungan ke server gagal!");
    Serial.println();
    return STATUS_DISCONNECTED;
  }
}

void kirim_data(String data){
    Serial.println("Menjalankan perintah kirim data");
    int ln = data.length();
    String uri_segment;
    uri_segment = "/keamananrumah/index.php/api/post_sensor_data/" + data;
    myEthernet.print("GET ");
    myEthernet.print(uri_segment); 
    Serial.print("Data yang dikirim di ke server : ");
    Serial.println(data);
    myEthernet.println(" HTTP/1.1");
    myEthernet.print( "Host: " );
    myEthernet.println(" 169.254.2.183 \r\n");
    Serial.println("Host OK");
    myEthernet.println( "Content-Type: application/x-www-form-urlencoded \r\n" );
    Serial.println("Content type OK");
    myEthernet.print( "Content-Length: " );
    myEthernet.print(ln);
    myEthernet.print(" \r\n");
    myEthernet.println( "Connection: close" );
    myEthernet.println();
    String res;
    res = baca_response_web();
    if(res.equals("")==false){
      Serial.print("Response server : ");
      Serial.println(res);
      String raw_secure_key = get_secure_key(res, '-', 1);
      secure_key = get_secure_key(raw_secure_key, '#', 0);
      Serial.print("Secure key : ");
      Serial.println(secure_key);
    }
}

String baca_response_web(){
  unsigned int time;
  Serial.println("Baca respon dari server . . . "); 
  Serial.println("Mohon menunggu . . . ");
  time = millis();
  Serial.print("Timer Millis () : ");
  Serial.println(time);
  int stringPos = 0;
  memset( &inString, 0, 1024 );
  int unvailable_ctr = 0;
  while(true){
    if (myEthernet.available()) {
      char c = myEthernet.read();
      Serial.print(c);
      if (c == '#' ) { 
        Serial.print("Menemukan start key # dengan isi : ");
        startRead = true;  
      }else if(startRead){
        if(c != '^'){ 
          inString[stringPos] = c;
          stringPos ++;
        }else{
          startRead = false;
          Serial.println();
          Serial.println("Baca respon dari server selesai!");
          myEthernet.stop();
          myEthernet.flush();
          Serial.println("Sambungan diputuskan . . . ");
          return inString;
        }
      }
    }else{
       delay(50);
       unvailable_ctr++;
       if(unvailable_ctr == 25){
         myEthernet.stop();
         myEthernet.flush();
         Serial.println("Koneksi mengalami time out");
         Serial.println("Sambungan diputuskan . . . ");
         Serial.println("Reset...");
         return inString;
       }
    }
  } 
}

String get_secure_key(String data, char separator, int index){
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}  




