// Load in the libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h> // Library for I2C communication
#include <MPU6050.h> // Library for MPU6050

// Set the CE & CSN pins
#define CE_PIN 9
#define CSN_PIN 10

// This is the address used to send/receive
const byte rxAddr[6] = "00001";

// Create a Radio
RF24 radio(CE_PIN, CSN_PIN);

// Create an instance of the MPU6050 object
MPU6050 mpu;

void setup() {
  // Start up the Serial connection
  while (!Serial);
  Serial.begin(9600);

  // Start the Radio!
  radio.begin();

  // Power setting. Due to likelihood of close proximity of the devices, set as RF24_PA_MIN (RF24_PA_MAX is default)
  radio.setPALevel(RF24_PA_MIN); // RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX

  // Slower data rate for better range
  radio.setDataRate(RF24_250KBPS); // RF24_250KBPS, RF24_1MBPS, RF24_2MBPS

  // Number of retries and set tx/rx address
  radio.setRetries(15, 15);
  radio.openWritingPipe(rxAddr);

  // Stop listening, so we can send!
  radio.stopListening();

  // Initialize the MPU6050 sensor
  Wire.begin();
  mpu.initialize();
}

void loop() {
  // Read data from the MPU6050 sensor
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Construct the message with sensor data and timestamp
  String str = "MPU6050 ";
  str += String(ax) + "," + String(ay) + "," + String(az) + "," + String(gx) + "," + String(gy) + "," + String(gz) + " ";
  str += String(millis());

  // Convert the message to a character array
  int str_len = str.length() + 1;
  char char_array[str_len];
  str.toCharArray(char_array, str_len);

  // Send the message
  radio.write(&char_array, sizeof(char_array));

  // Print the sent message
  Serial.print("Sent Message: ");
  Serial.print(char_array);
  Serial.println("");

  // Wait a short while before sending the next message
  delay(2000);
}
