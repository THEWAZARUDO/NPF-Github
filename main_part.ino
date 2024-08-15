#include "Servo.h"
#include <PS2X_lib.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <Adafruit_PWMServoDriver.h>

#define PS2_DAT 12 // MISO 
#define PS2_CMD 13 // MOSI 
#define PS2_SEL 15 // SS 
#define PS2_CLK 14 // SLK
#define pressures false
#define rumble false
#define NUM_SAMPLE 10 // khởi tạo số mẫu thử để lọc dữ liệu


// Khởi tạo class của thư viện
PS2X ps2x; // khởi tạo class PS2x

// Khởi tạo class của thư viện
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Khởi tạo lớp cho các servo. normal: servo 180 độ; continuous: servo 360 độ.
Servo normalServo, normalServo2, continuousServo, continuousServo2; 

// Khởi tạo đối tượng TCS34725
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

const int trigPin = 18;
const int echoPin = 17;
int error = 0;
int LWheel_vel, RWheel_vel;

void setup(){
	//Khởi tạo Serial monitor với tốc độ 115200
	Serial.begin(115200);
	  
	delay(300);  //thêm khoảng thời gian dừng để ổn định tay cầm

	pwm.begin(); //khởi tạo PCA9685 
	pwm.setOscillatorFrequency(27000000); // cài đặt tần số dao động 
	pwm.setPWMFreq(50);// cài đặt tần số PWM.
	
	//khởi tạo config_game
	for(int i = 0; i <= 9; ++i){
		//thử kết nối 10 lần
		error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
		if(error == 0){
	   		Serial.print("Kết nối thành công");
			break; //nếu thành công thì ngừng thử
		 	}
		else Serial.print("Kết nối thất bại");  
		}
	}
    	if (tcs.begin())
    		Serial.println("Tìm thấy Sensor");
	else {
    		Serial.println("Kiểm tra kết nối");
    		while (1); // Nếu không tìm thấy cảm biến, dừng lại ở đây
  	}

	// khởi tạo pin cho các servo: 3;
  	normalServo.attach(5);
	normalServo2.attach(4);
	continuousServo.attach(2);
	continuousServo2.attach(7);
	// Đặt chân Trig là OUTPUT
  	pinMode(trigPin, OUTPUT);
  	// Đặt chân Echo là INPUT
  	pinMode(echoPin, INPUT);

    	// khởi tạo mode cho các pin 8, 9, 10, 11, 12, 13, 14, 15
 	//motor1 - điều khiển intake
	pinMode(8, OUTPUT);
  	pinMode(9, OUTPUT);
	pwm.setPWM(9, 127, 0);
	
	//motor2 - điều khiển bánh trái
  	pinMode(10, OUTPUT);
  	pinMode(11, OUTPUT);
	
	//motor3 - điều khiển bánh phải
	pinMode(12, OUTPUT);
  	pinMode(13, OUTPUT);
	
	//motor4 - điều khiển ròng rọc
  	pinMode(14, OUTPUT);
  	pinMode(15, OUTPUT);
	pwm.setPWM(15, 127, 0);
	digitalWrite(8, HIGH);
	digitalWrite(9, HIGH);
	digitalWrite(14, LOW);
  	digitalWrite(15, LOW);     	
	continuousServo1.write(90); // 90: Dừng servo, 0 - 89: quay servo ngược chiều kđh với tốc độ tỉ lệ nghịch với giá trị, 91 - 180: quay servo cùng chiều kđh với tốc độ tỉ lệ thuận với giá trị
  	continuousServo2.write(90);
	normalServo1.write(90);
	normalServo2.write(90);
}


void detect(){
	uint16_t r, g, b, c, d;
	uint16_t r_sum = 0, g_sum = 0, b_sum = 0, c_sum = 0, d_sum = 0; //tổng giá trị của mỗi lần đo

	//lọc dữ liệu đầu vào từ sensor
	for(int i = 0; i <= NUM_SAMPLES; ++i){
		// Đọc giá trị màu sắc từ cảm biến
		tcs.getRawData(&r, &g, &b, &c);
		r_sum += r; //lưu trữ giá trị của mỗi lần đo, chia lấy trung bình và từ đó hiệu chỉnh giá trị đầu vào và giảm sai số 
		g_sum += g;
		b_sum += b;
		c_sum += c;
	}
	// chia tổng giá trị nhận được với só lần thử để nhận được giá trị trung bình, hay là giá trị đầu vào đã lọc
	r = r_sum / NUM_SAMPLE;
	g = g_sum / NUM_SAMPLE;
	b = b_sum / NUM_SAMPLE;
	c = c_sum / NUM_SAMPLE;
	// Kiểm tra xem màu nhận được có phải là màu trắng hay không
	if (isWhite(r, g, b)){ //kiểm tra liệu màu nhận được có phải là màu trắng không
		normalServo2.write(70);
	}
	else { //nếu không => màu đen
		normalServo2.write(0);
  	}
	for(int i = 0; i < 5; ++i){
		// Đảm bảo chân Trig ở mức LOW để bắt đầu một tín hiệu mới
		digitalWrite(trigPin, LOW);
		delayMicroseconds(2);
		// Kích hoạt chân Trig trong 10 micro giây
		digitalWrite(trigPin, HIGH);
		delayMicroseconds(10);
		digitalWrite(trigPin, LOW);
		// Đọc thời gian tín hiệu Echo ở mức HIGH
		d_sum += pulseIn(echoPin, HIGH);
	}
	d = (int)(d_sum / 5) * 0.034 / 2; //(vì xung phát mỗi 12microsec)
	// In khoảng cách ra Serial Monitor
	Serial.print("Distance: ");
	Serial.print(d);
	Serial.println(" cm");
	if(distance <= 25)
	{
		//đặt tốc bằng 0 để không thể tiến.
		LWheel_vel = min(LWheel_vel, 0);
		RWheel_vel = min(RWheel_vel, 0);
	}
	// Đợi 1 giây trước khi đọc lại
	delay(1000);
}

bool isWhite(uint16_t r, uint16_t g, uint16_t b) {
	const uint16_t threshold = 51200;  // ngưỡng giá trị màu tối thiểu, giá trị ba màu đỏ, lục, lam phải lớn hơn 51200 thì mới được coi là màu trắng 
	const float maxDifferencePercent = 0.05;  // sai số phần trăm tối đa là 5%
	// Kiểm tra liệu cả 3 màu lớn hơn ngưỡng trắng
	if (r > threshold && g > threshold && b > threshold) {
		//tìm màu có giá trị lớn nhất và giá trị nhỏ nhất
		uint16_t maxVal = max(max(r, g), b); 
		uint16_t minVal = min(min(r, g), b);
		//tính sai số giữa 2 màu có giá trị lớn nhất và nhỏ nhất, sau đó kiểm tra nếu sai số tính được có bé hơn 15% không
		if ((float)(maxVal - minVal) / maxVal < maxDifferencePercent) {
			return true; //nếu tất cả điều kiện trên được đáp ứng => màu trắng => trả về sai
    	  	}	
	}
	return false;// nếu không, trả về sai
}



void control(int LJoyY, int RJoyX) // state: trạng thái di chuyển của robot, F: tiến; B: lùi;
{
	//dùng hàm map để chuyển đổi giá trị của joystick về tốc độ của motor
	LWheel_vel = map(LJoyY - RJoyX, 0, 255, -255, 255);
  	RWheel_vel = map(LJoyY + RJoyX, 0, 255, -255, 255);
	
	// kiểm tra liệu chỉ số chu kỳ xung có bé hơn 128 (vị trí cân bằng của joystick) không, nếu có thì đảo chiều động cơ và đổi dấu 
  	if(LWheel_vel > 0){
		//dừng motor
		digitalWrite(12, LOW);
		digitalWrite(13, LOW);
		//dừng trước khi đảo chiều quay của motor
		delay(10);
		//đặt motor 1 quay ngược chiều dương
  		digitalWrite(13, HIGH);
	}
	else if(LWheel_vel < 0){
		//dừng motor
		digitalWrite(12, LOW);
		digitalWrite(13, LOW);
		//dừng trước khi đảo chiều quay của motor
		delay(10);
		//đặt motor 1 quay theo chiều dương
		digitalWrite(12, HIGH);
	}
	if(RWheel_vel > 0){
		//dừng motor
		digitalWrite(10, LOW);
		digitalWrite(11, LOW);
		//dừng
		delay(10);
		//đặt motor 2 quay theo chiều dương
      		digitalWrite(10, HIGH);
  	}
	else if(RWheel_vel < 0){
		//dừng motor
		digitalWrite(10, LOW);
		digitalWrite(11, LOW);
		//delay
		delay(10);
		//đặt motor 2 quay ngược chiều dương	
		digitalWrite(11, HIGH);
	}
  	// motor 1: (+) - 10; (-) - 11
  	// motor 2: (+) - 12 ; (-) - 13
  	// điều khiển 2 cực chéo -> RPM xấp xỉ nhau (dựa trên mô phỏng ở tinkercad.com)

	//điều khiển tốc độ của 2 bánh bằng cách băm xung PWM (đúng, tôi vừa ghi là xung xung điều chỉnh độ rộng đấy)
	analogWrite(11, abs(LWheel_vel * 0.6) + random(-10, 10)); // kỹ thuật phân tán xung PWM
  	analogWrite(12, abs(RWheel_vel * 0.6) + random(-10, 10));

  	Serial.println(abs(LWheel_vel * 0.6) + random(-10, 10));
  	Serial.println(abs(RWheel_vel * 0.6) + random(-10, 10));
 
}

void loop(){
	//đọc dữ liệu từ gamepad
	ps2x.read_gamepad();
	//đọc dữ liệu từ sensor
	detect();
	//tiến hành di chuyển robot bằng hàm băm
	control(ps2x.Analog(PSS_LY) + ps2x.Analog(PSS_RX), ps2x.Analog(PSS_LY) - ps2x.Analog(PSS_RX));

	if(ps2x.Button(PSB_TRIANGLE) && !ps2x.button(PSB_SQUARE)) //nếu đang nhấn nút tam giác và nút vuông không được nhấn thì
	{	
		digitalWrite(14, LOW);
		digitalWrite(15, LOW);
		delay(10);
		digitalWrite(14, HIGH); //chạy ròng rọc theo chiều dương
	}
	else if(!ps2x.Button(PSB_TRIANGLE)){ // nếu không thì
		delay(10);
		digitalWrite(14, LOW); // dừng ròng rọc
  		digitalWrite(15, LOW); 
	}
	
	if(ps2x.Button(PSB_SQUARE) && !ps2x.button(PSB_TRIANGLE))//nếu nút vuông đang được nhấn và nút tam giác không được nhấn thì
	{
		digitalWrite(14, LOW);
		digitalWrite(15, LOW);
		delay(10);
  		digitalWrite(15, HIGH); //chạy ròng rọc ngược chiều dương = hạ ròng rọc 
	}
	else if(!ps2x.Button(PSB_SQUARE)){ // nếu không thì
		delay(10);
		digitalWrite(14, LOW); // dừng ròng rọc
  		digitalWrite(15, LOW); 
	}
		
	if(ps2x.Button(PSB_L2)) //nếu nút L2 đang được giữ thì
        	normalServo.write(0); // đưa servo 1 (nắp hộp trắng) về góc 0 <=> hộp trắng mở nắp
	else normalServo.write(90); // khi thả nút L2 thì đóng nắp
	
      	if(ps2x.Button(PSB_R2)){ //nếu nút R2 đang được giữ thì
        	continuousServo.write(120); // đưa servo 2 (nắp hộp đen) về góc 180 <=> hộp đen mở nắp 
		delay(500);
		continuousServo.write(90);
	}
	else{    // khi thả nút R2 thì đóng nắp
		continuousServo.write(60);
		delay(500);
		continuousServo.write(90);
	}
	delay(10); //tránh chồng lệnh
}
/*
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⢿⣿⢿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣶⣶⣾⢟⣛⣛⣋⣛⣛⣛⣛⣛⡻⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣭⣿⣭⣭⣭⡟⠛⠛⠛⠻⠿⠿⠗⠒⠿⢻⣛⡙⠛⠈⠓⠈⠙⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⠀⠀⠈⢹⡆⠀⠀⠀⠀⠀⠀⢻⣞⠛⠛⠉⠉⠛⠷⣦⡄⠀⠀⠈⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣦⣄⠘⠿⠀⠀⠀⠀⠀⠀⣾⡏⠀⠀⠀⠀⠀⠀⠈⠁⠀⠀⠀⠈⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⣀⠀⠀⠀⠀⣠⣿⣷⣶⣿⣿⣿⣿⣿⣶⣄⠀⠀⠀⠀⠹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⣿⣿⡿⠿⣿⣿⣿⣿⣿⣿⣷⠀⠠⣾⣿⣿⣿⣿⣿⣿⡿⠿⠿⠿⠟⠙⠛⠛⢋⣻⢾⣝⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣾⣿⣿⣅⠀⢀⣽⣿⣿⣿⣿⣧⠀⠀⠈⢹⣿⣿⣿⣿⣍⣀⣀⡀⠀⣠⣶⣶⣶⣾⣿⣷⡽⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣾⣿⣿⣿⣿⣿⣿⣆⠀⠀⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⢛⣿⣿⣿⣿⣿⡇⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡏⠀⠀⠀⠈⠛⣿⣿⣿⣯⡀⠀⠀⢀⣾⣿⣿⣿⣿⣿⣇⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣤⣤⣤⣴⣿⡿⠿⠛⠻⠷⢄⣾⣿⣿⣿⣿⣿⠿⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⣤⣤⣶⣶⣶⣿⣿⣿⣿⣿⣿⡏⢰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠿⠿⣿⣿⡿⠿⠟⢻⣿⡏⠉⣿⣇⣿⣿⣿⡀⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⣤⣤⣼⣿⣧⣤⣤⣶⣿⣿⣾⣿⣿⣿⣿⣿⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡏⣿⣿⠉⠛⠛⢻⣿⡿⠿⠿⣿⣿⢿⣿⣿⣿⣿⣿⣿⠇⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡀⠀⠀⣸⣿⣇⢀⣾⣿⣇⣾⣿⣷⣿⡿⠟⠁⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠿⠿⠟⠋⠁⣠⡀⠀⠀⠀⢹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣟⣛⣛⣛⠒⡛⠛⠃⠀⢀⣤⣴⡾⠛⠁⢀⣾⠀⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⣭⣿⣿⣋⣉⣉⣉⣉⣉⣁⣠⣴⡾⠛⠁⠀⢀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⡉⠉⠉⠉⠉⠉⠉⠉⠉⠉⠁⠀⠀⠀⣀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣤⣤⣀⣀⣀⣀⣀⣀⣀⣠⣴⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿*/

