#include <WiFi.h>
#include <PubSubClient.h>
#include <WebServer.h>
#include <Preferences.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h> 

LiquidCrystal_I2C lcd(0x27, 16, 2); //LCD address

// Define button pins
const int buttonPin1 = 18;  // Long press button
const int buttonPin2 = 19;  // Regular press button

// Variables for long press detection
unsigned long buttonPressTime = 0;
unsigned long buttonReleaseTime = 0;

// Variables for Settings mode
bool buttonPressed = false;
bool settingMode = false;

// Variables for counting button presses
int buttonState2 = 0;
int lastButtonState = 0;
int count = 0;
unsigned long lastPressTime = 0;
unsigned long debounceDelay = 50;
unsigned long noPressTimeout = 1500;

//Variables for webserver
Preferences preferences;
WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

// HTML form for entering WiFi and MQTT credentials
const char* loginPage = R"(
  <!DOCTYPE html>
  <html>
  <body>
    <h2>Enter WiFi and MQTT Credentials</h2>
    <form action="/connect" method="POST">
      <label for="ssid">WiFi SSID:</label><br>
      <input type="text" id="ssid" name="ssid"><br>
      <label for="password">WiFi Password:</label><br>
      <input type="password" id="password" name="password"><br><br>
      <label for="mqtt_server">MQTT Server:</label><br>
      <input type="text" id="mqtt_server" name="mqtt_server"><br>
      <label for="mqtt_port">MQTT Port:</label><br>
      <input type="number" id="mqtt_port" name="mqtt_port"><br><br>
      <input type="submit" value="Connect">
    </form>

    <footer style="margin-top: 20px;">
      <p>Author: NGUYEN PHUC - N20DCVT039</p>
    </footer>
  </body>
  </html>
)";

// Wi-Fi and MQTT information
String ssid;       // Store WiFi SSID
String password;   // Store WiFi Password
const char* mqtt_topic = "esp32/test";

const int ledPin1 = 23; //Green led pin
const int ledPin2 = 4; //Yellow led pin
const int ledPin3 = 16; //Red led pin

//Fan led
const int pin33 = 33; //Livingroom
const int pin32 = 32; //Bedroom
const int pin3 = 3; //Bathroom

//Light led
const int pin2 = 2; //Livingroom
const int pin1 = 1; //Bathroom
const int pin17 = 17; //Garden
const int pin15 = 15; //Bedroom
const int pin0 = 0; //Kitchen
const int pin5 = 5; //Hallway

//Door led
const int pin14 = 14; //Livingroom
const int pin12 = 12; //Bedroom
const int pin26 = 26; //Bathroom
const int pin13 = 13; //Kitchen
const int pin25 = 25; //Garden
const int pin27 = 27; //Hallway



// MQTT server details
String mqtt_server;
int mqtt_port;

// Function to handle MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Tin nhắn nhận được: ");
  Serial.println(message);
  //Normal led
  if (message == "1") {
    digitalWrite(ledPin1, HIGH);
    Serial.println("Đèn LED đã bật.");
  } 
  else if (message == "0") {
    digitalWrite(ledPin1, LOW);
    Serial.println("Đèn LED đã tắt.");
  }
  else if (message == "2") {
    digitalWrite(ledPin2, HIGH);
    Serial.println("Quạt đã bật.");
  } 
  else if (message == "3") {
    digitalWrite(ledPin2, LOW);
    Serial.println("Quạt đã tắt.");
  }
  else if (message == "4") {
    digitalWrite(ledPin3, HIGH);
    Serial.println("Cửa đã mở.");
  } 
  else if (message == "5") {
    digitalWrite(ledPin3, LOW);
    Serial.println("Cửa đã đóng.");
  }
  else if (message == "6") {
    digitalWrite(pin15, HIGH);
    Serial.println("Đèn phòng ngủ đã bật");
  }
  else if (message == "7") {
    digitalWrite(pin15, LOW);
    Serial.println("Đèn phòng ngủ đã tắt");
  }
  else if (message == "8") {
    digitalWrite(pin32, HIGH);
    Serial.println("Quạt phòng ngủ đã bật");
  }
  else if (message == "9") {
    digitalWrite(pin32, LOW);
    Serial.println("Quạt phòng ngủ đã tắt");
  }
  else if (message == "10") {
    digitalWrite(pin12, HIGH);
    Serial.println("Cửa phòng ngủ đã mở");
  }
  else if (message == "11") {
    digitalWrite(pin12, LOW);
    Serial.println("Cửa phòng ngủ đã đóng");
  }
  else if (message == "12") {
    digitalWrite(pin1, HIGH);
    Serial.println("Đèn phòng tắm đã bật");
  }
  else if (message == "13") {
    digitalWrite(pin1, LOW);
    Serial.println("Đèn phòng tắm đã tắt");
  }
  else if (message == "14") {
    digitalWrite(pin3, HIGH);
    Serial.println("Quạt phòng tắm đã mở");
  }
  else if (message == "15") {
    digitalWrite(pin3, LOW);
    Serial.println("Quạt phòng tắm đã tắt");
  }
  else if (message == "16") {
    digitalWrite(pin26, LOW);
    Serial.println("Cửa phòng tắt đã đóng");
  }
  else if (message == "17") {
    digitalWrite(pin26, HIGH);
    Serial.println("Cửa phòng tắm đã mở");
  }
  else if (message == "18") {
    digitalWrite(pin2, HIGH);
    Serial.println("Đèn phòng khách đã mở");
  }
  else if (message == "19") {
    digitalWrite(pin2, LOW);
    Serial.println("Đèn phòng khách đã đóng");
  }
  else if (message == "20") {
    digitalWrite(pin33, HIGH);
    Serial.println("Quạt phòng khách đã mở");
  }
  else if (message == "21") {
    digitalWrite(pin33, LOW);
    Serial.println("Quạt phòng khách đã tắt");
  }
  else if (message == "22") {
    digitalWrite(pin13, HIGH);
    Serial.println("Cửa phòng khách đã mở");
  }
  else if (message == "23") {
    digitalWrite(pin14, LOW);
    Serial.println("Cửa phòng khách đã đóng");
  }
  else if (message == "24") {
    digitalWrite(pin0, HIGH);
    Serial.println("Bật đèn phòng bếp");
  }
  else if (message == "25") {
    digitalWrite(pin0, LOW);
    Serial.println("Tắt đèn phòng bếp");
  }
  else if (message == "26") {
    digitalWrite(pin14, HIGH);
    Serial.println("Mở cửa phòng bếp");
  }
  else if (message == "27") {
    digitalWrite(pin13, LOW);
    Serial.println("Đóng cửa phòng bếp");
  }
  else if (message == "28") {
    digitalWrite(pin25, HIGH);
    Serial.println("Mở cửa sân vườn");
  }
  else if (message == "29") {
    digitalWrite(pin25, LOW);
    Serial.println("Đóng cửa sân vườn");
  }
  else if (message == "30") {
    digitalWrite(pin17, HIGH);
    Serial.println("Mở đèn sân vườn");
  }
  else if (message == "31") {
    digitalWrite(pin17, LOW);
    Serial.println("Đóng đèn sân vườn");
  }
  else if (message == "32") {
    digitalWrite(pin27, HIGH);
    Serial.println("Mở cửa chính");
  }
  else if (message == "33") {
    digitalWrite(pin27, LOW);
    Serial.println("Đóng cửa chính");
  }
  else if (message == "34") {
    digitalWrite(pin17, HIGH);
    Serial.println("Mở đèn hành lang");
  }
  else if (message == "35") {
    digitalWrite(pin17, LOW);
    Serial.println("Đóng đèn hành lang");
  }
  else if (message == "36") {
    digitalWrite(pin17, HIGH);
       //LED write
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, HIGH);
  //Led fan write
    digitalWrite(pin33, HIGH);
    digitalWrite(pin32, HIGH);
    digitalWrite(pin3, HIGH);
  //Led door write
    digitalWrite(pin14, HIGH);
    digitalWrite(pin12, HIGH);
    digitalWrite(pin26, HIGH);
    digitalWrite(pin13, HIGH);
    digitalWrite(pin25, HIGH);
    digitalWrite(pin27, HIGH);
  //Led light write
    digitalWrite(pin2, HIGH);
    digitalWrite(pin15, HIGH);
    digitalWrite(pin1, HIGH);
    digitalWrite(pin0, HIGH);
    digitalWrite(pin17, HIGH);
    digitalWrite(pin5, HIGH);
    
    Serial.println("Mở tất cả thiết bị");
  }
  else if (message == "37") {
    //LED write
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, LOW);
    //Led fan write
    digitalWrite(pin33, LOW);
    digitalWrite(pin32, LOW);
    digitalWrite(pin3, LOW);
    //Led door write
    digitalWrite(pin14, LOW);
    digitalWrite(pin12, LOW);
    digitalWrite(pin26, LOW);
    digitalWrite(pin13, LOW);
    digitalWrite(pin25, LOW);
    digitalWrite(pin27, LOW);
    //Led ligt write
    digitalWrite(pin2, LOW);
    digitalWrite(pin15, LOW);
    digitalWrite(pin1, LOW);
    digitalWrite(pin0, LOW);
    digitalWrite(pin17, LOW);
    digitalWrite(pin5, LOW);
    
    Serial.println("Tắt tất cả thiết bị");
  }
  else if (message == "38") {
    //Led light write
    digitalWrite(pin2, HIGH);
    digitalWrite(pin15, HIGH);
    digitalWrite(pin1, HIGH);
    digitalWrite(pin0, HIGH);
    digitalWrite(pin17, HIGH);
    digitalWrite(pin5, HIGH);
    Serial.println("Bật tất cả đèn");
  }
  else if (message == "39") {
  //Led fan write
    digitalWrite(pin33, HIGH);
    digitalWrite(pin32, HIGH);
    digitalWrite(pin3, HIGH);
    Serial.println("Bật tất cả quạt");
  }
  else if (message == "40") {
      //Led door write
    digitalWrite(pin14, HIGH);
    digitalWrite(pin12, HIGH);
    digitalWrite(pin26, HIGH);
    digitalWrite(pin13, HIGH);
    digitalWrite(pin25, HIGH);
    digitalWrite(pin27, HIGH);
    Serial.println("Mở tất cả cửa");
  }
  else if (message == "41") {
    //Led light write
    digitalWrite(pin2, LOW);
    digitalWrite(pin15, LOW);
    digitalWrite(pin1, LOW);
    digitalWrite(pin0, LOW);
    digitalWrite(pin17, LOW);
    digitalWrite(pin5, LOW);
    Serial.println("Tắt tất cả đèn");
  }
  else if (message == "42") {
    //Led light write
    digitalWrite(pin33, LOW);
    digitalWrite(pin32, LOW);
    digitalWrite(pin3, LOW);
    Serial.println("Tắt tất cả quạt");
  }
  else if (message == "43") {
    //Led door write
    digitalWrite(pin14, LOW);
    digitalWrite(pin12, LOW);
    digitalWrite(pin26, LOW);
    digitalWrite(pin13, LOW);
    digitalWrite(pin25, LOW);
    digitalWrite(pin27, LOW);
    Serial.println("Đóng tất cả cửa");
  }

  // Check the status of room lights to update the main LED
  if (digitalRead(pin2) == HIGH || digitalRead(pin15) == HIGH || digitalRead(pin1) == HIGH || digitalRead(pin0) == HIGH || digitalRead(pin17) == HIGH || digitalRead(pin5) == HIGH) {
    digitalWrite(ledPin1, HIGH); // Turn on main LED if any room light is on
  } else if(digitalRead(pin2) == LOW && digitalRead(pin15) == LOW && digitalRead(pin1) == LOW && digitalRead(pin0) == LOW  && digitalRead(pin17) == LOW && digitalRead(pin5) == LOW){
    digitalWrite(ledPin1, LOW); // Turn off main LED if all room lights are off
  }

  // Check the status of room fan to update the main LED
  if (digitalRead(pin33) == HIGH || digitalRead(pin32) == HIGH || digitalRead(pin3) == HIGH){
    digitalWrite(ledPin2, HIGH); // Turn on main fan LED
  } else if(digitalRead(pin33) == LOW && digitalRead(pin32) == LOW && digitalRead(pin3) == LOW){
    digitalWrite(ledPin2, LOW); //Turn off main fan LED
  }

  // Check the status of room door to update the main LED
  if (digitalRead(pin14) == HIGH || digitalRead(pin12) == HIGH || digitalRead(pin26) == HIGH || digitalRead(pin13) == HIGH || digitalRead(pin25) == HIGH || digitalRead(pin27) == HIGH) {
    digitalWrite(ledPin3, HIGH); // Turn on main LED if any room light is on
  } else if(digitalRead(pin14) == LOW && digitalRead(pin12) == LOW && digitalRead(pin26) == LOW && digitalRead(pin13) == LOW  && digitalRead(pin25) == LOW && digitalRead(pin27) == LOW){
    digitalWrite(ledPin3, LOW); // Turn off main LED if all room lights are off
  }

}

// Function to reconnect to MQTT
void reconnect() {
  while (!client.connected()) {
    //Try connect to MQTT server
    Serial.print("Kết nối đến MQTT broker...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    //MQTT check connect status
    if (client.connect(clientId.c_str())) {
      Serial.println("Kết nối thành công!");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("Lỗi, mã lỗi: ");
      Serial.print(client.state());
      delay(5000);
    }
    
    //turn off main led

    //Checking button status
    if (!settingMode) {
    //Neu che do setitng chua duoc bat, kiem tra qua trinh nhan giu nut de vao che do setting
      checkLongPress();
    } else {
    //Neu che do setitng duoc bat, kiem tra tuy chon trong che do setting
      checkSettingMode();
    }
  }
}

// Function to handle WiFi and MQTT connection form submission
void handleConnect() {
  if (server.method() == HTTP_POST) {
    //Information field to fill
    ssid = server.arg("ssid");
    password = server.arg("password");
    mqtt_server = server.arg("mqtt_server");
    mqtt_port = server.arg("mqtt_port").toInt();

    // Save SSID, password, MQTT server, and port to flash memory
    preferences.begin("wifi", false);
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.putString("mqtt_server", mqtt_server);
    preferences.putInt("mqtt_port", mqtt_port);
    preferences.end();

    // Connect to WiFi with new credentials
    WiFi.begin(ssid.c_str(), password.c_str());
    delay(10);  // Small delay after WiFi.begin
    int retries = 0;

    //Try connect to Wifi
    lcd.clear();
    while (WiFi.status() != WL_CONNECTED && retries < 10) {
      lcd.setCursor(0, 0);
      lcd.print("Connecting...");
      delay(1000);
      retries++;
    }

    lcd.clear();
    if (WiFi.status() == WL_CONNECTED) {
      server.send(200, "text/html", "WiFi connected successfully!");
      lcd.setCursor(0, 0);
      lcd.print("Connected!");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(WiFi.SSID());
      lcd.setCursor(0, 1);
      lcd.print(WiFi.localIP().toString());
      Serial.println("\nWiFi connected successfully!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      // Set MQTT server details
      client.setServer(mqtt_server.c_str(), mqtt_port);
      client.setCallback(callback);
      reconnect();  // Attempt MQTT connection here
    } else {
      server.send(200, "text/html", "Failed to connect to WiFi. Please try again.");
      lcd.setCursor(0, 0);
      lcd.print("Failed connect!");
      Serial.println("\nFailed to connect to WiFi.");
    }
  }
}

// Function to start AP mode
void startAPMode() {
  WiFi.softAP("ESP32-Access-Point", "12345678"); //Information of ESP AP mode
  IPAddress apIP = WiFi.softAPIP();
  lcd.setCursor(0, 0);
  lcd.print("AP Mode:");
  lcd.setCursor(0, 1);
  lcd.print(apIP.toString());
}

// Setup function
void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  //LED pin configure
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  //LED fan configure
  pinMode(pin33, OUTPUT);
  pinMode(pin32, OUTPUT);
  pinMode(pin3, OUTPUT);
  //Door led configure
  pinMode(pin14, OUTPUT);
  pinMode(pin12, OUTPUT);
  pinMode(pin26, OUTPUT);
  pinMode(pin13, OUTPUT);
  pinMode(pin25, OUTPUT);
  pinMode(pin27, OUTPUT);
  //Light led configure
  pinMode(pin2, OUTPUT);
  pinMode(pin15, OUTPUT);
  pinMode(pin1, OUTPUT);
  pinMode(pin0, OUTPUT);
  pinMode(pin17, OUTPUT);
  pinMode(pin5, OUTPUT);


  //LED write
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);
  //Led fan write
  digitalWrite(pin33, LOW);
  digitalWrite(pin32, LOW);
  digitalWrite(pin3, LOW);
  //Led door write
  digitalWrite(pin14, LOW);
  digitalWrite(pin12, LOW);
  digitalWrite(pin26, LOW);
  digitalWrite(pin13, LOW);
  digitalWrite(pin25, LOW);
  digitalWrite(pin27, LOW);
  //Led ligt write
  digitalWrite(pin2, LOW);
  digitalWrite(pin15, LOW);
  digitalWrite(pin1, LOW);
  digitalWrite(pin0, LOW);
  digitalWrite(pin17, LOW);
  digitalWrite(pin5, LOW);

  //Button Configure
  pinMode(buttonPin1, INPUT_PULLUP); 
  pinMode(buttonPin2, INPUT_PULLUP); 

  // Load Wi-Fi and MQTT credentials from flash
  preferences.begin("wifi", true);
  String savedSSID = preferences.getString("ssid", "");
  String savedPassword = preferences.getString("password", "");
  mqtt_server = preferences.getString("mqtt_server", "192.168.1.14");
  mqtt_port = preferences.getInt("mqtt_port", 1883);
  preferences.end();

  if (savedSSID != "" && savedPassword != "") {
    WiFi.begin(savedSSID.c_str(), savedPassword.c_str());

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 10) {
      delay(1000);
      retries++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      lcd.setCursor(0, 0);
      lcd.print(WiFi.SSID());
      lcd.setCursor(0, 1);
      lcd.print(WiFi.localIP().toString());
      client.setServer(mqtt_server.c_str(), mqtt_port);
      client.setCallback(callback);
      reconnect();  // Attempt MQTT connection here
    } else {
      startAPMode();
    }
  } else {
    startAPMode();
  }

  server.on("/", []() { server.send(200, "text/html", loginPage); });
  server.on("/connect", handleConnect);
  server.begin();
}

// Main loop function
void loop() {
  server.handleClient();

  //Checking wifi and MQTT status
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
  }

  //Checking button status
  if (!settingMode) {
    //Neu che do setitng chua duoc bat, kiem tra qua trinh nhan giu nut de vao che do setting
    checkLongPress();
  } else {
    //Neu che do setitng duoc bat, kiem tra tuy chon trong che do setting
    checkSettingMode();
  }
}

void checkLongPress() {
  int buttonState1 = digitalRead(buttonPin1);

  if (buttonState1 == HIGH) {  // If button is pressed
    if (!buttonPressed) {
      buttonPressed = true;
      buttonPressTime = millis();
    }
  } else {  // If button is not pressed
    if (buttonPressed) {
      buttonReleaseTime = millis();
      unsigned long pressDuration = buttonReleaseTime - buttonPressTime;
      
      if (pressDuration >= 3000) {  // Button held for at least 3 seconds
        settingMode = true;
        lcd.clear();
        lcd.print("Setting Mode");
        delay(1000); // Brief delay to display "Setting Mode"
        lcd.clear();
        lcd.print("1: MOTT Status");
        lcd.setCursor(0, 1);
        lcd.print("2: MQTT info");
      } else {
        Serial.print("Nút được giữ trong: ");
        Serial.print(pressDuration);
        Serial.println(" mili giây");
      }
      buttonPressed = false;
    }
  }
}

// Function to count button presses for option selection in setting mode
void checkSettingMode() {
  unsigned long currentMillis = millis();
  buttonState2 = digitalRead(buttonPin2);

  // Check if button state has changed and is not bouncing
  if (buttonState2 != lastButtonState) {
    delay(debounceDelay); // Debounce delay
    if (buttonState2 == HIGH) { // Check for button press
      count++;
      lastPressTime = currentMillis;
    }
  }

  lastButtonState = buttonState2;

  // Print the result if no button press within timeout
  if (currentMillis - lastPressTime >= noPressTimeout && count > 0) {
    lcd.clear();
    
    if (count == 1) {
      lcd.print("MQTT Status...");
      delay(2000);
      lcd.clear();
      int exitCount1 = 0;

      while (true){
        lcd.setCursor(0, 0);
        lcd.print("MQTT Status:");
        lcd.setCursor(0, 1);
        if (client.connected()) {
          lcd.print("Connected");
        } else {
          lcd.print("Disconnected");
        }
        int buttonExitState = digitalRead(buttonPin2);  // Read button state
        if (buttonExitState == HIGH && lastButtonState == LOW) {
          exitCount1++;
          delay(debounceDelay); // Debounce delay to ensure stable button press
        }

        // Exit loop and setting mode if button pressed twice
        if (exitCount1 >= 1) {
          lcd.clear();
          checkLongPress();
          break;
        }
        lastButtonState = buttonExitState;  // Update last button state
      }
    } else if (count == 2) {
      lcd.print("MQTT info...");
      delay(2000);
      lcd.clear();
      int exitCount2 = 0;

      while (true){
        lcd.setCursor(0, 0);
        lcd.print("IP:");
        lcd.setCursor(4, 0);
        lcd.print(mqtt_server); // Display MQTT IP
        lcd.setCursor(0, 1);
        lcd.print("Port:");
        lcd.setCursor(5, 1);
        lcd.print(mqtt_port);

        int buttonExitState = digitalRead(buttonPin2);  // Read button state
        if (buttonExitState == HIGH && lastButtonState == LOW) {
          exitCount2++;
          delay(debounceDelay); // Debounce delay to ensure stable button press
        }

        // Exit loop and setting mode if button pressed twice
        if (exitCount2 >= 1) {
          lcd.clear();
          checkLongPress();
          break;
        }
        lastButtonState = buttonExitState;  // Update last button state
      }
    } else if (count >= 3) {
      lcd.print("Exiting...");
      delay(2000);
      Serial.println("Exiting Setting Mode");
      lcd.clear();    
      settingMode = false;
      
      if (WiFi.status() == WL_CONNECTED) {
        lcd.setCursor(0, 0);
        lcd.print(WiFi.SSID());
        lcd.setCursor(0, 1);
        lcd.print(WiFi.localIP().toString());
      } else {
        startAPMode();
      } // Exit setting mode
    }
    
    count = 0; // Reset count for next time
    delay(2000); // Delay to show result on LCD
    if (settingMode) {
      lcd.clear();
      lcd.print("1: MOTT Status");
      lcd.setCursor(0, 1);
      lcd.print("2: MQTT info");
    }
  }
}
