const int encoder[3] = {8, 9, 5};
int old = 0;
const int saved_patterns[][3] = {{1, 2, -1},{-1,2,1}};
int pattern[] = {};
int pattern_pos = 0;
 
void setup() {
  //encoder setup
  pinMode(encoder[0], INPUT);           //CLK
  pinMode(encoder[1], INPUT);           //DT
  pinMode(encoder[2], INPUT_PULLUP);    //SW

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int n = getEncoderState();
  if (old != n) {
    
    if (n == 0){
      //TODO: add pattern matching
      pattern_pos = 0;
      if ( > 3){
        for (int i = 0; i < sizeof(
      }
    } else {
      pattern[pattern_pos] = n;
      pattern_pos++;
    }
  }
}

int getEncoderState() {
  if (digitalRead(encoder[0]) and digitalRead(encoder[1]))
    return 0;
  else if (digitalRead(encoder[0]) and !digitalRead(encoder[1]))
    return 1;
  else if (!digitalRead(encoder[0]) and digitalRead(encoder[1]))
    return -1;
  else if (!digitalRead(encoder[0]) and !digitalRead(encoder[1]))
    return 2;
}

int sizeArray(*arr){
  return sizeof(*arr)/sizeof(*arr[0]);
}
