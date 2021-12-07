#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define LEFT_MOTOR_F 12
#define LEFT_MOTOR_B 13
#define RIGHT_MOTOR_F 15
#define RIGHT_MOTOR_B 14

#define STATION_ESSID "RoombaControl"
#define STATION_PASSWORD "Roombas!"

IPAddress local_IP(192, 168, 94, 100);
IPAddress gateway(192, 168, 94, 153);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

/*
 * MOTOR CONTROL CODE
 *
 *
 */

void stopMotors(){
  digitalWrite(LEFT_MOTOR_F, LOW);
  digitalWrite(LEFT_MOTOR_B, LOW);
  digitalWrite(RIGHT_MOTOR_F, LOW);
  digitalWrite(RIGHT_MOTOR_B, LOW);
  Serial.println("STOPPING");
}

void leftMotor(int motorDirection){

  // Stop all motors, prevents shorting H bridge
  digitalWrite(LEFT_MOTOR_F, LOW);
  digitalWrite(LEFT_MOTOR_B, LOW);
  
  if(motorDirection > 0){
    // Go forward
    digitalWrite(LEFT_MOTOR_F, HIGH);
  } else if(motorDirection < 0){
    digitalWrite(LEFT_MOTOR_B, HIGH);
  }
}

void rightMotor(int motorDirection){

  // Stop all motors, prevents shorting H bridge
  digitalWrite(RIGHT_MOTOR_F, LOW);
  digitalWrite(RIGHT_MOTOR_B, LOW);
  
  if(motorDirection > 0){
    // Go forward
    digitalWrite(RIGHT_MOTOR_F, HIGH);
  } else if(motorDirection < 0){
    digitalWrite(RIGHT_MOTOR_B, HIGH);
  }
}

void handleMessage(String msg){

  Serial.println(msg);
  
  if(msg == "FORWARD"){
    // Move forward
    leftMotor(1);
    rightMotor(1);
    Serial.println("Moving FORWARD");
  } else if(msg == "BACKWARD"){
    // Move forward
    leftMotor(-1);
    rightMotor(-1);
    Serial.println("Moving BACKWARD");
  } else if(msg == "LEFT"){
    // Move forward
    leftMotor(-1);
    rightMotor(1);
    Serial.println("Moving LEFT");
  } else if(msg == "RIGHT"){
    // Move forward
    leftMotor(1);
    rightMotor(-1);
    Serial.println("Moving RIGHT");
  } else {
    stopMotors();
  }
}

void initWiFi(){

  // Config connection
  if(!WiFi.config(local_IP, gateway, subnet)){
    Serial.println("Failed to config wifi!!!");
  }

  // Start WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(STATION_ESSID, STATION_PASSWORD);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED){
    digitalWrite(4, HIGH);
    delay(100);
    digitalWrite(4, LOW);
    delay(200);
  }

  // Set up handlers
  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request){

    Serial.println("Request recieved!");
    String message;

    // Check if request has the direction param
    if(request->hasHeader("direction")){
      message = request->getHeader("direction")->value();
      handleMessage(message);
      request->send(200);
    } else {
      request->send(500, "text/plain", "MALFORMED_PACKET");
    }
  });

  // Wifi has connected, start web server
  server.begin();
}

void setup() {
  // put your setup code here, to run once:

  // Initialize LED
  pinMode(4, OUTPUT);

  // Initialize motor pins
  pinMode(LEFT_MOTOR_F, OUTPUT);
  pinMode(LEFT_MOTOR_B, OUTPUT);
  pinMode(RIGHT_MOTOR_F, OUTPUT);
  pinMode(RIGHT_MOTOR_B, OUTPUT);

  // Start serial
  Serial.begin(115200);

  // Config WiFi
  initWiFi();

}

void loop() {
  // put your main code here, to run repeatedly:
  if(WiFi.status() != WL_CONNECTED){
    stopMotors();
  }
  while(WiFi.status() != WL_CONNECTED){
    digitalWrite(4, HIGH);
    delay(100);
    digitalWrite(4, LOW);
    delay(200);
  }
}
