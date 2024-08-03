#include <Wire.h>
#include <Adafruit_TCS34725.h>

// Khởi tạo đối tượng TCS34725
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// Ngưỡng để xác định màu trắng
const float WHITE_THRESHOLD = 200.0; // Ngưỡng độ sáng (c)

void setup() {
  Serial.begin(9600);
  // Bắt đầu giao tiếp với cảm biến
  if (tcs.begin()) {
    Serial.println("TCS34725 found!");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // Nếu không tìm thấy cảm biến, dừng lại ở đây
  }
}

void loop() {
  uint16_t r, g, b, c;

  // Đọc giá trị màu sắc từ cảm biến
  tcs.getRawData(&r, &g, &b, &c);

  // Kiểm tra xem màu có phải là màu trắng hay không
  if (c > WHITE_THRESHOLD) {
    Serial.println("detected color là màu trắng");
  } else {
    Serial.println("detected color ko là màu trắng");
  }

  // Đợi 1 giây trước khi đọc lại
  delay(1000);
}
