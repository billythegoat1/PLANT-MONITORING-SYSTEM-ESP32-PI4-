#include <DHT.h>

#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#include<Adafruit_SSD1306.h>
#include<Wire.h>
Adafruit_SSD1306 screen(128,64, &Wire, -1);


#define LDR 15

#define SOIL 4

void showMessage(const char* msg){
  screen.clearDisplay();
  screen.setTextColor(SSD1306_WHITE);
  screen.setTextSize(1);
  screen.setCursor(30, 30);
  screen.println(msg);
  screen.display();
}

void setup() {
  Serial.begin(115200);

  dht.begin();
  Wire.begin(22,23);

  
  screen.begin(SSD1306_SWITCHCAPVCC, 0X3C);
  screen.clearDisplay();
  
  analogSetAttenuation(ADC_11db);
}

void loop() {

  bool error = false;

  
  //Read values from the DHT11
  float air_temp = dht.readTemperature();
  float air_humid = dht.readHumidity();
  //check if the readings are good
  if (isnan(air_temp) || isnan(air_humid)){
      error = true;
      showMessage("ERR_DHT_SENSOR");
      delay(2000);
    }

  //Read values from SOIL MOISTURE SENSOR
  int soil_moist_ADC = analogRead(SOIL);
  if (soil_moist_ADC < 0 || soil_moist_ADC > 4095){
    error=true;
    showMessage("Soil_Sensor_ERR");
    delay(2000);
  }

  //Read values from the LDR
  int light = analogRead(LDR);
  if (light < 0 || light > 4095){
    error= true;
    showMessage("LDR_ERR");
    delay(2000);
  }

  

  
    
    
   if(!error){
      showMessage("OK!");
      Serial.print("Air Temperature");Serial.print(air_temp);Serial.print(" C");Serial.print("\t Air humidity");Serial.print(air_humid);Serial.println("%");
      Serial.print("Soil Moisture: ");Serial.println(soil_moist_ADC);
      Serial.print("Light: ");Serial.println(light);
      Serial.println("");
      Serial.println("");
    }
  

  delay(3000);

}
