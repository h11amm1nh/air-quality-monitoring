#include <DHT.h> //dht sensor
#include <Wire.h> //oled
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// goddman mit

#define DHT_OUT_PIN 23 //dht sensor
#define SCREEN_WIDTH 128 //oled
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define DUST_PIN 4 //dust sensor
#define LED_POWER_PIN 19

DHT dht(DHT_OUT_PIN, DHT22);
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,OLED_RESET);

// interrupt variable
unsigned long currentDHT_time = 0;
unsigned long DHT_time;
unsigned long currentDUST_time = 0;
unsigned long DUST_time;
unsigned long currentOLED_time = 0;
unsigned long OLED_time;

// dust sensor variable
int delayTime=280;
int delayTime2=40;
float offTime=9680; //10000-280-40

int dustVal=0;
float voltage = 0;
float dustdensity = 0;

// dht variable
float temp = 0;
float humidity = 0;

void setup() {
  // put your setup code here, to run once:
  dht.begin();
  delay(2000);

  pinMode(LED_POWER_PIN,OUTPUT);  

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.display();

  // Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  DHT_time = millis();
  if(DHT_time - currentDHT_time > 2000)
  {
    currentDHT_time = millis();
    temp = dht.readTemperature();
    humidity = dht.readHumidity();
    // Serial.println("Temp: " + String(temp) + ", Humidity: " + String(humidity));
  }
  DUST_time = millis();
  if(DUST_time - currentDUST_time > 2000)
  {
    currentDUST_time = millis();
    digitalWrite(LED_POWER_PIN,LOW); // power on the LED
    delayMicroseconds(delayTime);
    dustVal=analogRead(DUST_PIN); // read the dust value
    delayMicroseconds(delayTime2);
    digitalWrite(LED_POWER_PIN,HIGH); // turn the LED off
    delayMicroseconds(offTime);
    
    voltage = dustVal*0.0012;//dustval*5/4096 (12-bit ESP32)
    dustdensity = 0.172*voltage-0.1;
    
    if (dustdensity < 0 )
    dustdensity = 0;
    if (dustdensity > 0.5)
    dustdensity = 0.5;
    dustdensity = dustdensity*1000;
  } 
  OLED_time = millis();
  if(OLED_time - currentOLED_time > 5000)
  {
    currentOLED_time = millis();
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    // display.print("Temp: ");
    display.print(temp); display.write(0xF7); display.println("C");
    // display.print("Humid: ");
    display.print(humidity); display.println("%");
    // display.print("PM2.5: ");
    display.print(dustdensity); display.write(0xE5); display.println("g/m3");
    display.display();
  }

}