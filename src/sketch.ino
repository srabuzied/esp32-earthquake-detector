#include <Wire.h>  // Library for I2C communication
#include <Adafruit_GFX.h>  // Graphics library for OLED
#include <Adafruit_SSD1306.h>  // OLED driver library
#include <MPU6050_light.h>  // MPU6050 library (light version)

// Define OLED screen dimensions correctly
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Initialize OLED display object with correct I2C object & address
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// Initialize MPU6050 object using I2C
MPU6050 mpu(Wire);

// Define pin numbers for LEDs and buzzer
#define RED_LED 25
#define YELLOW_LED 26
#define GREEN_LED 27
#define BUZZER 23

// Declare variables to store accelerometer values (don't forget semicolon)
float ax, ay, az;

void setup() {
  Serial.begin(115200);  // Start serial communication for debugging
  
  Wire.begin();  // Initialize I2C

  // Initialize MPU6050 sensor, check connection
  if (mpu.begin() != 0) {
    Serial.println("MPU6050 not connected!");  // Error if not connected
    while (1);  // Stop program here if error
  }
  Serial.println("MPU6050 initialized. Calibrating...");
  mpu.calcOffsets();  // Calibrate sensor for accurate readings
  Serial.println("Calibration done!");

  // Initialize OLED display and check if found
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!");  // Error if not connected
    while (1);  // Stop program if OLED is not found
  }
  display.clearDisplay();  // Clear the screen initially
  display.display();  // Show the cleared screen

  // Set all LED and buzzer pins as output
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(GREEN_LED, HIGH);  // Set normal state to green ON
}

void loop() {
  mpu.update();  // Refresh MPU6050 sensor readings

  // Read acceleration on all axes
  ax = mpu.getAccX();
  ay = mpu.getAccY();
  az = mpu.getAccZ();  // Correct case (capital Z)

  // Calculate total acceleration using Pythagoras theorem
  float totalAcceleration = sqrt(ax * ax + ay * ay + az * az);

  display.clearDisplay();  // Clear screen before new data
  display.setTextSize(1);  // Small text size
  display.setTextColor(SSD1306_WHITE);  // White text color
  display.setCursor(0, 0);  // Start at top-left

  // Check for high vibration (earthquake detected)
  if (totalAcceleration > 1.5) {
    digitalWrite(GREEN_LED, LOW);  // Turn off green LED
    digitalWrite(YELLOW_LED, LOW);  // Turn off yellow LED
    digitalWrite(RED_LED, HIGH);  // Red LED ON
    digitalWrite(BUZZER, HIGH);  // Buzzer ON

    display.println("Earthquake Detected!");  // Display message
    display.print("Vibration: ");
    display.print(totalAcceleration);  // Show total acceleration
    display.println(" g");  // Show unit (g-force)

  // Check for moderate vibration
  } else if (totalAcceleration > 1.2) {
    digitalWrite(GREEN_LED, LOW);  // Turn off green LED
    digitalWrite(YELLOW_LED, HIGH);  // Yellow LED ON
    digitalWrite(RED_LED, LOW);  // Turn off red LED
    digitalWrite(BUZZER, LOW);  // Buzzer OFF

    display.println("Moderate Vibrations");  // Display message
    display.print("Vibration: ");
    display.print(totalAcceleration);  // Show total acceleration
    display.println(" g");

  // Otherwise, normal conditions
  } else {
    digitalWrite(RED_LED, LOW);  // Turn off red LED
    digitalWrite(YELLOW_LED, LOW);  // Turn off yellow LED
    digitalWrite(GREEN_LED, HIGH);  // Green LED ON
    digitalWrite(BUZZER, LOW);  // Buzzer OFF

    display.println("Normal Conditions");  // Display message
    display.print("Vibration: ");
    display.print(totalAcceleration);  // Show total acceleration
    display.println(" g");
  }

  display.display();  // Show updated content on OLED
  delay(200);  // Small delay to avoid flicker
}