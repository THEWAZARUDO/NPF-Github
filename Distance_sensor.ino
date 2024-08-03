const int trigPin = IO32;
const int echoPin = IO2;

void setup() {
  // Khởi tạo cổng Serial để in kết quả
  Serial.begin(9600);

  // Đặt chân Trig là OUTPUT
  pinMode(trigPin, OUTPUT);
  
  // Đặt chân Echo là INPUT
  pinMode(echoPin, INPUT);
}

void loop() {
  // Khởi tạo biến thời gian và khoảng cách
  long duration;
  int distance;

  // Đảm bảo chân Trig ở mức LOW để bắt đầu một tín hiệu mới
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Kích hoạt chân Trig trong 10 micro giây
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Đọc thời gian tín hiệu Echo ở mức HIGH
  duration = pulseIn(echoPin, HIGH);

  // Tính khoảng cách theo công thức
  distance = duration * 0.034 / 2;

  // In khoảng cách ra Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Đợi 1 giây trước khi đọc lại
  delay(1000);
}
