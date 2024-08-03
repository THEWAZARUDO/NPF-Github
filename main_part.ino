#include "Servo.h"
#include <PS2X_lib.h>

Servo normalServo, normalServo2, continuousServo, continuousServo2; 
// khởi tạo lớp cho các servo. normal: servo 180 độ; continuous: servo 360 độ.

void setup()
{
    // khởi tạo pin cho các servo: 3;
  	normalServo.attach(5);
	normalServo2.attach(4);
	continuousServo.attach(2);
	continuousServo2.attach(7);
    // khởi tạo mode cho các pin 8, 9, 10, 11, 12, 13, 14, 15
 	//motor1
	pinMode(8, OUTPUT);
  	pinMode(9, OUTPUT);
	//motor2 - điều khiển bánh trái
  	pinMode(10, OUTPUT);
  	pinMode(11, OUTPUT);
	//motor3 - điều khiển bánh phải
	pinMode(12, OUTPUT);
  	pinMode(13, OUTPUT);
	//motor4
  	pinMode(14, OUTPUT);
  	pinMode(15, OUTPUT);
  	Serial.begin(13600);
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
    	continuousServo1.write(90); // 90: Dừng servo, 0 - 89: quay servo ngược chiều kđh với tốc độ tỉ lệ nghịch với giá trị, 91 - 180: quay servo cùng chiều kđh với tốc độ tỉ lệ thuận với giá trị
  	continuousServo2.write(90);
	
    	for(pos = 0; pos <= 180; pos++){
  		normalServo1.write(pos);
    		delay(15);
  	}
  	for(pos = 180; pos >= 0; pos--){
  		normalServo2.write(pos);
    		delay(15);
 	}
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

//
