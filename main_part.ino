#include "Servo.h"
#include <PS2X_lib.h>

Servo normalServo, normalServo2, continuousServo, continuousServo2; 
// khởi tạo lớp cho các servo. normal: servo 180 độ; continuous: servo 360 độ.

#define PS2_DAT 12 // MISO 
#define PS2_CMD 13 // MOSI 
#define PS2_SEL 15 // SS 
#define PS2_CLK 14 // SLK

//Khởi tạo class của thư viện
PS2X ps2x; // khởi tạo class PS2x

/******************************************************************
 * chọn chế độ của tay cầm
 bỏ bấm analog của các nút
 bỏ chế độ rung motor
 ******************************************************************/
#define pressures false
#define rumble false

int error = 0;
byte type = 0;
byte vibrate = 0;

void setup(){
	//Khởi tạo Serial monitor với tốc độ 115200
	Serial.begin(115200);
	  
	delay(300);  //thêm khoảng thời gian dừng để ổn định tay cầm
	   
	  //khởi tạo config_game
	for(int i = 0; i <= 9; ++i{
		error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
		if(error == 0){
	   		Serial.print("Kết nối thành công");
			break;
		 	}
		else Serial.print("Kết nối thất bại");  
		}
	}
    	// khởi tạo pin cho các servo: 3;
  	normalServo.attach(5);
	normalServo2.attach(4);
	continuousServo.attach(2);
	continuousServo2.attach(7);

	delay(10);

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
		
	if(ps2x.Button(PSB_L2))
        	normalServo1.write(0);
	else normalServo1.write(90);
	
      	if(ps2x.Button(PSB_R2))
        	normalServo2.write(180);
	else normalServo2.write(90);
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

