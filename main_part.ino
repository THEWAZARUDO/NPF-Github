#include "Servo.h"
#include <PS2X_lib.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>

#define PS2_DAT 12 // MISO 
#define PS2_CMD 13 // MOSI 
#define PS2_SEL 15 // SS 
#define PS2_CLK 14 // SLK
#define pressures false
#define rumble false

//Khởi tạo class của thư viện
PS2X ps2x; // khởi tạo class PS2x

// khởi tạo lớp cho các servo. normal: servo 180 độ; continuous: servo 360 độ.
Servo normalServo, normalServo2, continuousServo, continuousServo2; 

// Khởi tạo đối tượng TCS34725
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// Ngưỡng để xác định màu trắng
const float WHITE_THRESHOLD = 255.0; // Ngưỡng độ sáng (c)
const int trigPin = IO32;
const int echoPin = IO2;
int error = 0;

void setup(){
	//Khởi tạo Serial monitor với tốc độ 115200
	Serial.begin(115200);
	  
	delay(300);  //thêm khoảng thời gian dừng để ổn định tay cầm
	
	//khởi tạo config_game
	for(int i = 0; i <= 9; ++i{
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

	delay(10);
	// Đặt chân Trig là OUTPUT
  	pinMode(trigPin, OUTPUT);
  	// Đặt chân Echo là INPUT
  	pinMode(echoPin, INPUT);

    	// khởi tạo mode cho các pin 8, 9, 10, 11, 12, 13, 14, 15
 	//motor1 - điều khiển intake
	pinMode(8, OUTPUT);
  	pinMode(9, OUTPUT);
	analogWrite(9, 127);
	
	//motor2 - điều khiển bánh trái
  	pinMode(10, OUTPUT);
  	pinMode(11, OUTPUT);
	
	//motor3 - điều khiển bánh phải
	pinMode(12, OUTPUT);
  	pinMode(13, OUTPUT);
	
	//motor4 - điều khiển ròng rọc
  	pinMode(14, OUTPUT);
  	pinMode(15, OUTPUT);
	analogWrite(15, 127);

	delay(10);

	digitalWrite(8, HIGH);
	digitalWrite(9, HIGH);
	digitalWrite(14, LOW);
  	digitalWrite(15, LOW);     	
	continuousServo1.write(90); // 90: Dừng servo, 0 - 89: quay servo ngược chiều kđh với tốc độ tỉ lệ nghịch với giá trị, 91 - 180: quay servo cùng chiều kđh với tốc độ tỉ lệ thuận với giá trị
  	continuousServo2.write(90);
	normalServo1.write(90);
	normalServo2.write(90);
}

void detech(){
	uint16_t r, g, b, c;

	// Đọc giá trị màu sắc từ cảm biến
	tcs.getRawData(&r, &g, &b, &c);

	// Kiểm tra xem màu nhận được có phải là màu trắng hay không
	if (c > WHITE_THRESHOLD){ //nếu màu clear nhận được lớn hơn 255 => màu trắng
		continuousServo1.write(180);
		delay(1000);
		continuousServo1.write(90);
	}
	else { //nếu không => màu đen
		continuousServo2.write(180);
		delay(1000);
		continuousServo2.write(90);
  	}
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
	if(distance == 15 && LWheel_vel > 0 && RWheel_vel > 0)
	{
		digitalWrite(10, LOW);
		digitalWrite(11, LOW);
		digitalWrite(12, LOW);
		digitalWrite(13, LOW);
	}
	// Đợi 1 giây trước khi đọc lại
	delay(1000);
}


void control(int LWheel_vel,  int RWheel_vel) // state: trạng thái di chuyển của robot, F: tiến; B: lùi;
{
	// kiểm tra liệu chỉ số chu kỳ xung có âm không, nếu có thì đảo chiều động cơ và đổi dấu 
  	if(LWheel_vel > 0){
		//đặt 2 motor xoay ngược chiều nhau => robot đi thẳng (vì 2 motor đối diện nhau)
  		digitalWrite(12, LOW);
  		digitalWrite(13, HIGH); 
	}
	else if(LWheel_vel < 0)
	{
		//đặt ngược chiều quay của motor để khiến motor chạy lùi
		digitalWrite(12, HIGH);
		digitalWrite(13, LOW);
		LWheel_vel *= -1;
	}
	if(RWheel_vel > 0){
		
      		digitalWrite(10, HIGH);
  		digitalWrite(11, LOW);
  	}
	else if(RWheel_vel < 0)
	{
	      	digitalWrite(10, LOW);	  		
		digitalWrite(11, HIGH);
		RWheel_vel *= -1;
	}
  	// motor 1: (+) - 10; (-) - 11
  	// motor 2: (+) - 12 ; (-) - 13
  	// điều khiển 2 cực chéo nhau -> RPM xấp xỉ nhau
  	// 11 - 12 : ~700 - ~1300 RPM
  	// 10 - 13 : ~1600 - ~1800 RPM

	//điều khiển tốc độ của 2 bánh bằng cách băm xung PWM (đúng, tôi vừa ghi là xung xung điều chỉnh độ rộng đấy)
	analogWrite(11, LWheel_vel);
  	analogWrite(12, RWheel_vel);

  	Serial.println(LWheel_vel);
  	Serial.println(RWheel_vel);
 
}

void loop(){
	detech();
	//tiến hành di chuyển robot bằng hàm băm
	control(- ps2x.Analog(PSS_LY) + ps2x.Analog(PSS_RX), - ps2x.Analog(PSS_LY) - ps2x.Analog(PSS_RX));

	if(ps2x.Button(PSB_TRIANGLE) && !ps2x.button(PSB_SQUARE)) //nếu đang nhấn nút tam giác và nút vuông không được nhấn thì
	{
		digitalWrite(14, HIGH); //chạy ròng rọc theo chiều kim đồng hồ
  		digitalWrite(15, LOW); 
	}
	else if(!ps2x.Button(PSB_TRIANGLE)){ // nếu không thì
		digitalWrite(14, LOW); // dừng ròng rọc
  		digitalWrite(15, LOW); 
	}
	
	if(ps2x.Button(PSB_SQUARE) && !ps2x.button(PSB_TRIANGLE))//nếu nút vuông đang được nhấn và nút tam giác không được nhấn thì
	{
		digitalWrite(14, LOW); //chạy ròng rọc theo chiều kim đồng hồ = hạ ròng rọc
  		digitalWrite(15, HIGH); 
	}
	else if(!ps2x.Button(PSB_SQUARE)){ // nếu không thì
		digitalWrite(14, LOW); // dừng ròng rọc
  		digitalWrite(15, LOW); 
	}
		
	if(ps2x.Button(PSB_L2)) //nếu nút L2 đang được giữ thì
        	normalServo1.write(0); // đưa servo 1 (nắp hộp trắng) về góc 0 <=> hộp trắng mở nắp
	else normalServo1.write(90); // khi thả nút L2 thì đóng nắp
	
      	if(ps2x.Button(PSB_R2)) //nếu nút R2 đang được giữ thì
        	normalServo2.write(180); // đưa servo 2 (nắp hộp đen) về góc 180 <=> hộp đen mở nắp 
	else normalServo2.write(90); // khi thả nút R2 thì đóng nắp
	return 0;
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

