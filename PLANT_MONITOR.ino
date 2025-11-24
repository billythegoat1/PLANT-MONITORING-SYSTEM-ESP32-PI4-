#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>


#include <DHT.h>
#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#include<Adafruit_SSD1306.h>
#include<Wire.h>
Adafruit_SSD1306 screen(128,64, &Wire, -1);


#define LDR 32
#define SOIL 33

//Credentials for the wifi
const char* ssid = "SSID";
const char* password = "SSID_PASSWD";
//Credentials for the MQTT broker/server
const char* mqtt_server = "IP_ADDR_BROKER";
const int mqtt_port = 1883;
const char* mqtt_user = "MQTT_USERNAME";
const char* mqtt_pass = "MQTT_PASSWD";

//IN CASE YOU CONFIGURED YOUR ESP32'S WIFI STATICALLY
//CHANGE THIS PART IF YOU WANT
IPAddress local_ip(10,195,169,9);
IPAddress gateway(10,195,169,1);
IPAddress subnet(255,255,255,0);
IPAddress primaryDNS(8,8,8,8);


//Create a communication pipe 
WiFiClient plant1_wifi;
PubSubClient client(plant1_wifi);

void wifi_connect(const char* ssid, const char* password){
  
  if(!WiFi.config(local_ip, gateway, subnet, primaryDNS)){
    showMessage("STATIC_IP_ERR");
  }
  WiFi.begin(ssid, password);
  showMessage("Connecting...");
  
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
  }

  IPAddress ip = WiFi.localIP();
  String ipStr = ip.toString();
  showMessage(ipStr.c_str());
  showMessage("Connected to WiFi");
  
}

void showMessage(const char* msg){
  screen.clearDisplay();
  screen.setTextColor(SSD1306_WHITE);
  screen.setTextSize(1);
  screen.setCursor(15, 30);
  screen.println(msg);
  screen.display();
}


void setup() {
  Serial.begin(115200);
  
  dht.begin();
  Wire.begin(22,23);

  
  screen.begin(SSD1306_SWITCHCAPVCC, 0X3C);
  screen.clearDisplay();
  
  wifi_connect(ssid, password);
  client.setServer(mqtt_server, mqtt_port);
  
  analogSetAttenuation(ADC_11db);
}

void loop() {

      
      delay(2000);
      //mqtt persistent tcp connection
      client.loop();
      //check if the mqtt connection is still active
      if(!client.connected()){
        showMessage("Connecting MQTT...");
        while(!client.connect("plant1", mqtt_user, mqtt_pass)){
          showMessage("MQTT_FAIL_RECONN");
          delay(500);
        }
        showMessage("MQTT Connected");
        delay(1000);
      }

      //check if the wifi

  
      //Read values from the DHT11
      float air_temp = dht.readTemperature();
      float air_humid = dht.readHumidity();
  
      //check if the readings for the DHT11 are ok
      if (isnan(air_temp) || isnan(air_humid)){
      
      showMessage("ERR_DHT_SENSOR");
      return;
      
      }

      //Read values from SOIL MOISTURE SENSOR
      float dry = 4095.0;
      float wet = 1344.0;
      float soil_moist_ADC = analogRead(SOIL);
      float moisturePercent = 100.0*((soil_moist_ADC - dry) / (wet-dry));
      if(moisturePercent < 0) moisturePercent = 0;
      if(moisturePercent > 100) moisturePercent = 100;

      //Read values from the LDR
      float dark = 4000.0;
      float bright = 500;
      
      float light_ADC = analogRead(LDR);
      float lightPercent = 100.0*((dark - light_ADC) / (dark - bright));
      if(lightPercent < 0) lightPercent = 0;
      if(lightPercent > 100) lightPercent = 100;
      
    
      StaticJsonDocument<200> doc;
      doc["air_temp"] = air_temp;
      doc["air_humid"] = air_humid;
      doc["light"] = lightPercent;
      doc["soil_moist"] = moisturePercent;

      String jsonString;
      serializeJson(doc, jsonString);
      Serial.println(jsonString);
      showMessage("JSON_READY");
      delay(1000);
      showMessage("Checking WiFi...");
      delay(500);
      if(WiFi.status() != WL_CONNECTED){
        showMessage("WiFi_FAIL_RECONN...");
        wifi_connect(ssid, password);
        delay(1000);
        return;
      }else{
        showMessage("WiFi_OK!");
        delay(500);
        client.publish("plant1/data", jsonString.c_str());
        showMessage("Data Sent!");
        delay(3000);
      }
      
      
    
  

  

}
