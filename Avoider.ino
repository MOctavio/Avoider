#include <Servo.h>

#define ECHO_PIN 4
#define TRIGGER_PIN 5
#define SERVO_PIN 6

// Distance in cm
#define ACTION_DIST 25
#define MIN_ACTION_DIST 10

#define SPEED 150
#define CENTER_POS 80 // usually 90deg, set in 80 to adjust the physical servo position
#define TURN_DELAY 500
#define SERVO_SPEED 0.1/60 // SG90 TowerPro angular speed

Servo servo; // create servo object to control a Servo

unsigned long dist_fwd, left_dist, right_dist; // Distance measured

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

  Serial.begin(9600);
  Serial.println("Starting...");
  
  halt();
  delay(4000);
}

void loop() {
  // Get a reading from the current sensor direction
  dist_fwd = get_distance();

  // Go forward while nothing is in the distance sensors read area
  if (dist_fwd >= ACTION_DIST) {
    go_forward();
  } else {
    if (dist_fwd <= MIN_ACTION_DIST) {
      go_backward();
    } else {
      halt();
      find_path();
    }
  }
}

// Measure distance find a new path
void find_path() {
  for (int pos = CENTER_POS; pos >= 0; pos -= 1) {
    servo.write(pos);
    delay(1000 * SERVO_SPEED);
  }
  right_dist = get_distance(); //Look to the right
  delay(TURN_DELAY);

  for (int pos = 0; pos <= 160; pos += 1) {
    servo.write(pos);
    delay(1000 * SERVO_SPEED);
  }    
  left_dist = get_distance(); // Look to the left
  delay(TURN_DELAY);

  // Set the servo back to the center pos
  for (int pos = 160; pos >= CENTER_POS; pos -= 1) {
    servo.write(pos);
    delay(1000 * SERVO_SPEED);
  }
  turn();
}

// Read the HC-SR04 uSonic sensor and calculates the distance
unsigned long get_distance() {
  // The PING is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 5 micro seconds
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIGGER_PIN, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
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
  digitalWrite(9, HIGH); //Engage the Brake for Channel A
  digitalWrite(8, HIGH); //Engage the Brake for Channel B
}

// Turns on the max distance
void turn() {
  (left_dist >= right_dist) ? go_left(): go_right();
}

void go_left() {
  // Prints the direction on the Serial Monitor
  Serial.println("Left");

  // Motor A forward
  digitalWrite(12, HIGH); // Establishes forward direction of Channel A
  digitalWrite(9, LOW); // Disengage the Brake for Channel A
  analogWrite(3, SPEED); // Spins the motor on Channel A at full speed

  delay(TURN_DELAY);
  halt();
}

void go_right() {
  // Prints the direction on the Serial Monitor
  Serial.println("Right");

  // Motor B forward
  digitalWrite(13, HIGH); // Establishes forward direction of Channel A
  digitalWrite(8, LOW); // Disengage the Brake for Channel A
  analogWrite(11, SPEED); // Spins the motor on Channel A at full speed

  delay(TURN_DELAY);
  halt();
}

void go_forward() {
  // Prints the direction on the Serial Monitor
  Serial.println("Forward");

  // Motor A forward
  digitalWrite(12, HIGH); // Establishes forward direction of Channel A
  digitalWrite(9, LOW); // Disengage the Brake for Channel A
  analogWrite(3, SPEED); // Spins the motor on Channel A at full speed

  //   Motor B forward
  digitalWrite(13, HIGH); // Establishes forward direction of Channel A
  digitalWrite(8, LOW); // Disengage the Brake for Channel A
  analogWrite(11, SPEED); // Spins the motor on Channel A at full speed
}

void go_backward() {
  // Prints the direction on the Serial Monitor
  Serial.println("Backward");

  // Motor A backward
  digitalWrite(12, LOW); // Establishes backward direction of Channel A
  digitalWrite(9, LOW); // Disengage the Brake for Channel A
  analogWrite(3, SPEED); // Spins the motor on Channel A at half speed

  // Motor B backward
  digitalWrite(13, LOW); // Establishes backward direction of Channel B
  digitalWrite(8, LOW); // Disengage the Brake for Channel B
  analogWrite(11, SPEED); // Spins the motor on Channel B at half speed
}
