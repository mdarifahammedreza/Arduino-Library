#include <ESP8266WiFi.h>
#include "PID_v1.h"
#include "LMotorController.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

//wifi setup
const char* ssid = "WiFi Name (SSID)";
const char* password = "WiFi Password";

int LED = 0; // led connected to D0
WiFiServer server(80);
//balance robo define part
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

#define MIN_ABS_SPEED 20

MPU6050 mpu;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

//PID
double originalSetpoint = 165; 
double setpoint = originalSetpoint;
double movingAngleOffset = 0.1;
double input, output;
int moveState=0; //0 = balance; 1 = back; 2 = forth
double Kp = 0; // First adjustment
double Kd = 0; // Second adjustment
double Ki = 0; // Third adjustment
PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT); // kp = 55,kd=0.9

double motorSpeedFactorLeft = 0.6;
double motorSpeedFactorRight = 0.6;
//MOTOR CONTROLLER
// Motor A
int ENA = 12; // Replace with the desired GPIO pin
int IN1 = 26; // Replace with the desired GPIO pin
int IN2 = 27; // Replace with the desired GPIO pin

// Motor B
int ENB = 13; // Replace with the desired GPIO pin
int IN3 = 14; // Replace with the desired GPIO pin
int IN4 = 15; // Replace with the desired GPIO pin
// int ENA = 10;
// int IN1 = 6;
// int IN2 = 7;
// int IN3 = 8;
// int IN4 = 9;
// int ENB = 11;
LMotorController motorController(ENA, IN1, IN2, ENB, IN3, IN4, motorSpeedFactorLeft, motorSpeedFactorRight);

//timers
long time1Hz = 0;
long time5Hz = 0;

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady()
{
    mpuInterrupt = true;
}

void setup()
{
 Serial.begin(115200);

 Serial.print("Connecting to the Newtork");
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED)
 {
   delay(500);
   Serial.print(".");
 }
 Serial.println("WiFi connected");
 server.begin();  // Starts the Server
 Serial.println("Server started");

 Serial.print("IP Address of network: ");
 Serial.println(WiFi.localIP());
 Serial.println("/");

 //balance part
  // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000);// TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (115200 chosen because it is required for Teapot Demo output, but it's
    // really up to you depending on your project)
    Serial.begin(115200);
    while (!Serial); // wait for Leonardo enumeration, others continue immediately

    // initialize device
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0)
    {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
        
        //setup PID
        
        pid.SetMode(AUTOMATIC);
        pid.SetSampleTime(10);
        pid.SetOutputLimits(-255, 255);  
    }
    else
    {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
}
void loop(){

// balance part

    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize)
    {
        //no mpu data - performing PID calculations and output to motors
        
        pid.Compute();
        motorController.move(output, MIN_ABS_SPEED);
        
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024)
    {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    }
    else if (mpuIntStatus & 0x02)
    {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        #if LOG_INPUT
            Serial.print("ypr\t");
            Serial.print(ypr[0] * 180/M_PI);
            Serial.print("\t");
            Serial.print(ypr[1] * 180/M_PI);
            Serial.print("\t");
            Serial.println(ypr[2] * 180/M_PI);
        #endif
        input = ypr[1] * 180/M_PI + 180;
   }





  clint();
}
































void clint()
{
 WiFiClient client = server.available();
 if (!client)
 {
   return;
 }
 Serial.println("Waiting for new client");
 while(!client.available())
 {
   delay(1);
 }

 String request = client.readStringUntil('\r');
 Serial.println(request);
 client.flush();

 int value = LOW;
 if(request.indexOf("/walle=goAhead") != -1)
 {
   Serial.println("going ahead....");
  }

  else if (request.indexOf("/walle=goBack") != -1) {
   Serial.println("going back....");
  }
  else if (request.indexOf("/walle=goLeft") != -1) {
   Serial.println("going Left....");
  }
  else if (request.indexOf("/walle=goRight") != -1) {
   Serial.println("going Right....");
  }
  else if (request.indexOf("/walle=goAheadRight") != -1) {
   Serial.println("going AheadRight....");
  }
  else if (request.indexOf("/walle=goAheadLeft") != -1) {
   Serial.println("going AheadLeft....");
  }
  else if (request.indexOf("/walle=goBackRight") != -1) {
   Serial.println("going BackRight....");
  }
  else if (request.indexOf("/walle=goBackLeft") != -1) {
   Serial.println("going BackLeft....");
  }
  else if (request.indexOf("/walle=hold") != -1) {
   Serial.println("Holding....");
  }

 client.println("HTTP/1.1 200 OK"); // standalone web server with an ESP8266
 client.println("Content-Type: text/html");
 client.println("");
 client.println("<!DOCTYPE HTML>");
 client.println("<html>");

 client.print("Current status: ");

 if(value == HIGH)
 {
   client.print("Command Successfully Acepted");
 }
 else
 {
   client.print("Getting no command,Power save mode on");
 }
 client.println("<br><br>");
 client.println("<a href=\"/walle=goAhead\"\"><button>Go-Ahead</button></a>");
 client.println("<a href=\"/walle=goBack\"\"><button>Go-Back</button></a>");
 client.println("<a href=\"/walle=goLeft\"\"><button>Go-Left</button></a>");
 client.println("<a href=\"/walle=goRight\"\"><button>Go-Right</button></a>");
 client.println("<a href=\"/walle=goAheadRight\"\"><button>Go-AheadRight</button></a>");
 client.println("<a href=\"/walle=goAheadLeft\"\"><button>Go-AheadLeft</button></a>");
 client.println("<a href=\"/walle=goBackRight\"\"><button>Go-BackRight</button></a>");
 client.println("<a href=\"/walle=goBackLeft\"\"><button>Go-BackLeft</button></a>");
 client.println("<a href=\"/walle=hold\"\"><button>Hold</button></a>");
 client.println("</html>");

 delay(1);
 Serial.println("Client disonnected");
 Serial.println("");
}