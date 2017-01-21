#include <Servo.h>

// Ultrasonic sensor
#define ECHO_PIN 4
#define TRIGGER_PIN 5
#define SERVO_PIN 6

// Distance in cm
#define ACTION_DIST 20
#define MIN_ACTION_DIST 10

#define SPEED_A 115
#define SPEED_B 112
#define TURN_DELAY 450

#define CENTER_POS 80 // usually 90deg, set in 80 to adjust the physical servo position
#define SERVO_SPEED 0.1/60 // SG90 TowerPro angular speed

#define Z_PIN A1 // Accelerometer z-axis

Servo servo; // create servo object to control a Servo

unsigned long left_dist, right_dist; // Distance measured
int sensor_max = 0; // maximum acceleration
boolean initial_acceleration = false;
boolean constant_acceleration = false;
boolean calibrating = true;

void setup() {
  servo.attach(SERVO_PIN);
  servo.write(CENTER_POS);

  //Setup Channel A
  pinMode(12, OUTPUT); //Initiates Motor Channel A pin
  pinMode(9, OUTPUT); //Initiates Brake Channel A pin

  //Setup Channel B
  pinMode(13, OUTPUT); //Initiates Motor Channel B pin
  pinMode(8, OUTPUT); //Initiates Brake Channel B pin

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  halt();
  delay(2000);
  Serial.begin(9600);
  Serial.println("Hi, I'm Antonio");
  Serial.println("Let's ride!");
  go_forward();
  calibrate();
}

// calibrate during the first 3 seconds
// and record the maximum sensor value
void calibrate() {
  Serial.println("Starting calibration...");
  delay(750); // to avoid initial bump
  int sensor_value = 0; // the sensor value
  while (millis() < 3000) {
    sensor_value = analogRead(Z_PIN);
    if (sensor_value > sensor_max) {
      sensor_max = sensor_value;
    }
  }
  // Max sensor read less 10% margin
  sensor_max = sensor_max - (((float)sensor_max / 100) * 10);
  calibrating = false;
  Serial.print("Sensor Max: ");
  Serial.println(sensor_max);
}

void loop() {
  if (calibrating)return;

  // Get a reading from the sensor
  int dist_fwd = get_distance();
  boolean acceleration = is_accelerating();

  if (acceleration &&
      (dist_fwd > ACTION_DIST)) {
    go_forward();
    return;
  }

  if (!acceleration ||
      dist_fwd <= MIN_ACTION_DIST) {
    halt();
    go_backward();
    delay(750);
    find_path();
    go_forward();
    return;
  }
}

boolean is_accelerating() {
  int sensor_value = 0;
  int z_axis = 0;
  for (int x = 0; x < 10; x++) {
    z_axis = analogRead(Z_PIN);
    sensor_value += z_axis;
  }
  sensor_value = sensor_value / 10;

  if (initial_acceleration &&
      constant_acceleration &&
      sensor_value > sensor_max) {
    Serial.println("Road block!");
    Serial.println(sensor_value);
    return false;
  }
  if (sensor_value > sensor_max &&
      !initial_acceleration) {
    Serial.println("Accelerating");
    Serial.println(sensor_value);
    initial_acceleration = true;
    return true;
  }
  if (!constant_acceleration &&
      initial_acceleration &&
      sensor_value <= sensor_max) {
    Serial.println("Constant acceleration");
    Serial.println(sensor_value);
    constant_acceleration = true;
    return true;
  }

  return true;
}

// Measure distance and find a new path
void find_path() {
  // make it stop first
  halt();
  // Look to the left
  for (int pos = CENTER_POS; pos <= 160; pos++) {
    servo.write(pos);
    delay(1000 * SERVO_SPEED);
  }
  left_dist = get_distance();
  delay(TURN_DELAY);
  //Look to the right
  for (int pos = 160; pos >= 0; pos--) {
    servo.write(pos);
    delay(1000 * SERVO_SPEED);
  }
  right_dist = get_distance();
  delay(TURN_DELAY);
  // Set the servo back to the center position
  for (int pos = 0; pos <= CENTER_POS; pos++) {
    servo.write(pos);
    delay(1000 * SERVO_SPEED);
  }
  turn();
}

// Read the HC-SR04 uSonic sensor and calculates the distance in cm
unsigned long get_distance() {
  // The PING is triggered by a HIGH pulse of 2 or more microseconds
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 5 micro seconds
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIGGER_PIN, LOW);

  // Reads the echoPin, returns the microseconds it takes
  // the sound wave to travel
  unsigned long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance
  // HC-SR04 uSonic sensor velocity (340M/S) / 2
  unsigned long distance = duration * 0.034 / 2;

  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);

  return distance;
}

// Engage the motor breaks
void halt() {
  // reset acceleration status
  initial_acceleration = false;
  constant_acceleration = false;

  digitalWrite(9, HIGH); //Engage the Brake for Channel A
  digitalWrite(8, HIGH); //Engage the Brake for Channel B
}

// Turns on the max distance
void turn() {
  (left_dist >= right_dist) ? go_left() : go_right();
}

void go_left() {
  // Prints the direction on the Serial Monitor
  Serial.println("Turning left");

  // Motor B forward
  digitalWrite(13, HIGH); // Establishes forward direction of Channel A
  digitalWrite(8, LOW); // Disengage the Brake for Channel A
  analogWrite(11, SPEED_B); // Spins the motor on Channel A at full speed

  delay(TURN_DELAY);
  halt();
}

void go_right() {
  // Prints the direction on the Serial Monitor
  Serial.println("Turning right");

  // Motor A forward
  digitalWrite(12, HIGH); // Establishes forward direction of Channel A
  digitalWrite(9, LOW); // Disengage the Brake for Channel A
  analogWrite(3, SPEED_A); // Spins the motor on Channel A at full speed

  delay(TURN_DELAY);
  halt();
}

void go_forward() {
  // Prints the direction on the Serial Monitor
  //  Serial.println("Moving forward");

  // Motor A forward
  digitalWrite(12, HIGH); // Establishes forward direction of Channel A
  digitalWrite(9, LOW); // Disengage the Brake for Channel A
  analogWrite(3, SPEED_A); // Spins the motor on Channel A at full speed

  //   Motor B forward
  digitalWrite(13, HIGH); // Establishes forward direction of Channel A
  digitalWrite(8, LOW); // Disengage the Brake for Channel A
  analogWrite(11, SPEED_B); // Spins the motor on Channel A at full speed
}

void go_backward() {
  // Prints the direction on the Serial Monitor
  Serial.println("Moving backward");

  // Motor A backward
  digitalWrite(12, LOW); // Establishes backward direction of Channel A
  digitalWrite(9, LOW); // Disengage the Brake for Channel A
  analogWrite(3, SPEED_A); // Spins the motor on Channel A at half speed

  // Motor B backward
  digitalWrite(13, LOW); // Establishes backward direction of Channel B
  digitalWrite(8, LOW); // Disengage the Brake for Channel B
  analogWrite(11, SPEED_B); // Spins the motor on Channel B at half speed
}
