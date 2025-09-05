#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BLYNK_TEMPLATE_ID "TMPLGJXSfl9Vk"
#define BLYNK_TEMPLATE_NAME "voltage"
#define BLYNK_AUTH_TOKEN "4NBdQIZUBPAMR2soxlj015uJEuztKZkv"

#define BLYNK_PRINT Serial

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char ssid[] = "WALE";
char pass[] = "062018163";

// Define analog input
#define ANALOG_IN_PIN A0

// Floats for ADC voltage
float adc_voltage = 0.0;

// Input voltage, Current & Power
float in_voltage = 0.0;
float in_current = 0.0;
float in_power = 0.0;

// Floats for resistor values in divider (in ohms)
float R1 = 30000.0;
float R2 = 7500.0;
float R = 0.0;

// Float for Reference Voltage
float ref_voltage = 5.0;

// Integer for ADC value
int adc_value = 0;

void setup() {
  // Setup Serial Monitor
  Serial.begin(115200);
  
  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();
  
  // Connect to WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  
  // Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
  Serial.println("DC Voltage Test");
}

void loop() {
  Blynk.run();
  
  // Read the Analog Input
  adc_value = analogRead(ANALOG_IN_PIN);
  
  // Determine voltage at ADC input
  adc_voltage = (adc_value * ref_voltage) / 1024.0;
  
  // Calculate voltage divider ratio
  R = R2 / (R1 + R2);
  
  // Calculate voltage at divider input
  in_voltage = adc_voltage / R;
  
  // Calculate current (assuming R1 + R2 is the total resistance)
  in_current = in_voltage / (R1 + R2);
  
  // Calculate power
  in_power = in_voltage * in_current;
  
  // Print results to Serial Monitor
  Serial.print("Input Voltage = ");
  Serial.println(in_voltage, 2);
  Serial.print("Input Current = ");
  Serial.println(in_current, 2);
  Serial.print("Input Power = ");
  Serial.println(in_power, 2);
  
  // Send data to Blynk
  Blynk.virtualWrite(V1, in_voltage);
  Blynk.virtualWrite(V2, in_current);
  Blynk.virtualWrite(V3, in_power);
  
  // Display voltage on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.print(in_voltage, 2);
  display.println(" V");
  
  // Display current on OLED
  display.setCursor(0, 20);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.print(in_current, 4); // Show more decimals for small current values
  display.println(" A");
  
  // Display power on OLED
  display.setCursor(0, 40);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.print(in_power, 2);
  display.println(" W");
  
  display.display();
  
  delay(2000); // Delay between readings
}