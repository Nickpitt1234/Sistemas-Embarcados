void setup() {
  // put your setup code here, to run once:
  pinMode(53, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(53, HIGH);
  delay(500);
  digitalWrite(53, LOW);
  delay(500);
}
