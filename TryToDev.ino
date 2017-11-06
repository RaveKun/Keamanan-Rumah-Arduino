#define pin_outdoor_pir 2
#define pin_indoor_pir 3
#define pin_bright_led 11
#define trigPin 5
#define echoPin 4

int ctr_detected = 0;
int ctr_undetected = 0;
int calibrationTime = 30; 

boolean lockLow = true;
boolean lockLow2 = true;
boolean takeLowTime;
boolean takeLowTime2;  
boolean outdoor_pin_status = false;
boolean indoor_pin_status = false;

long unsigned int lowIn;
long unsigned int lowIn2; 
long unsigned int pause = 100;  


void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pin_outdoor_pir, INPUT);
  digitalWrite(pin_outdoor_pir, LOW);
  pinMode(pin_indoor_pir, INPUT);
  digitalWrite(pin_indoor_pir, LOW);
  pinMode(pin_bright_led, OUTPUT);
  Serial.print("calibrating sensor ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(".");
    delay(100);
    }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
}

void loop() {
  Serial.println("----------------------------------------------");
  Serial.print(ussrf());
  Serial.println(" cm");
  Serial.println("----------------------------------------------");
  if(outdoor_pir()){
    Serial.println("Human detected on outdoor PIR sensor");
  }else{
    Serial.println("No human detected on outdoor PIR sensor");
  }
  Serial.println("----------------------------------------------");
  if(indoor_pir()){
    Serial.println("Human detected on indoor PIR sensor");
  }else{
    Serial.println("No human detected on indoor PIR sensor");
  }
  Serial.println("----------------------------------------------\n\n\n");
  delay(3000);
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
  digitalWrite(trigPin, LOW);   // Added this line
  delayMicroseconds(2);         // Added this line
  digitalWrite(trigPin, HIGH);  //  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10);        // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  delay(500);
  return distance;
}

