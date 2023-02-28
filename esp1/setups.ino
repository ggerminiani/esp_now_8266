#include "secrets.h"

void setupSerial() {

  Serial.begin(115200);
  Serial.println();

  uint8_t i = 0;
  do {
    Serial.print(".");
    delay(100);
    i++;
  } while (i < 10);

  Serial.println();
  Serial.println();
  Serial.println("Initializing system...");
  Serial.println("------------------------------------------------------");
  Serial.println();
}

void setupDigital() {

  Serial.println("Preparing LEDs && Push Buttons...");

  pinMode(PSH_BTN, INPUT_PULLUP);
  pinMode(LED_GRN, OUTPUT);
  pinMode(LED_WHT, OUTPUT);
  pinMode(LED_BLU, OUTPUT);

  delay(500);

  digitalWrite(LED_GRN, HIGH);
  delay(200);
  digitalWrite(LED_WHT, HIGH);
  delay(200);
  digitalWrite(LED_BLU, HIGH);
  delay(500);

  digitalWrite(LED_GRN, LOW);
  delay(200);
  digitalWrite(LED_WHT, LOW);
  delay(200);
  digitalWrite(LED_BLU, LOW);
  delay(500);

  Serial.println();
  Serial.println("LEDs && Push Buttons OK!");
  Serial.println("------------------------------------------------------");
  Serial.println();
}

void setupVariables() {

  Serial.println("Preparing variables...");

  PUSH_TIMMER = millis();
  LED_BLU_TIMMER = millis();
  INTERNET_TIMMER = millis();

  Serial.println();
  Serial.println("Variables OK!");
  Serial.println("------------------------------------------------------");
  Serial.println();
}

void setupWiFi() {

  Serial.println("Preparing WiFi...");

  if (SERVER_BOARD) {

    WiFi.mode(WIFI_AP_STA);

  } else {

    WiFi.mode(WIFI_STA);
    CHANNEL = getWiFiChannel(SSID);

    WiFi.printDiag(Serial);  // Uncomment to verify channel number before
    wifi_promiscuous_enable(1);
    wifi_set_channel(CHANNEL);
    wifi_promiscuous_enable(0);
    WiFi.printDiag(Serial);  // Uncomment to verify channel change after
  }

  if (SERVER_BOARD) connectToWifi();

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  Serial.println();
  Serial.println("WiFi OK!");
  Serial.println("------------------------------------------------------");
  Serial.println();
}

void connectToWifi() {

  Serial.println();
  Serial.print("Connecting to SSID " + String(SSID));

  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);


  Serial.println();
  Serial.println();

  Serial.print("Connected to SSID: ");
  Serial.println(WiFi.SSID());

  Serial.print("Connected to CHANNEL: ");
  Serial.println(WiFi.channel());

  Serial.print("Connected on IP: ");
  Serial.println(WiFi.localIP());
}

void setupEspNow(uint8_t* macToSend) {

  Serial.println("Preparing ESP-Now...");

  while (esp_now_init() != 0) {
    Serial.print('.');
  }

  Serial.println();

  esp_now_set_self_role(ESP_SELF_ROLE);
  Serial.print("SELF role setted: ");
  switch (ESP_SELF_ROLE) {
    case 0:
      Serial.println("IDLE");
      break;

    case 1:
      Serial.println("CONTROLLER");
      break;

    case 2:
      Serial.println("SLAVE");
      break;

    case 3:
      Serial.println("MAX");
      break;

    case 4:
      Serial.println("COMBO");
      break;
  }

  esp_now_add_peer(macToSend, ESP_PEER_ROLE, CHANNEL, NULL, 0);
  Serial.print("ESP peer role setted: ");
  switch (ESP_PEER_ROLE) {
    case 0:
      Serial.println("IDLE");
      break;

    case 1:
      Serial.println("CONTROLLER");
      break;

    case 2:
      Serial.println("SLAVE");
      break;

    case 3:
      Serial.println("MAX");
      break;

    case 4:
      Serial.println("COMBO");
      break;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println();
  Serial.println("ESP-Now OK!");
  Serial.println("------------------------------------------------------");
  Serial.println();
}

void setupMQTT() {

  Serial.println("Preparing MQTT...");

  clientMQTT.setServer(MQTT_SRV, MQTT_PRT);
  clientMQTT.setCallback(mqttCallback);
  connectToMQTT();

  Serial.println();
  Serial.println();
  Serial.println("MQTT OK!");
  Serial.println("------------------------------------------------------");
  Serial.println();
}

void connectToMQTT() {

  Serial.println();
  Serial.print("Connecting to MQTT " + String(MQTT_SRV));

  while (!clientMQTT.connected()) {

    delay(500);
    Serial.print(".");

    if (clientMQTT.connect(BOARD_KEY, MQTT_USR, MQTT_PWD)) {
      Serial.println("connected");
      //clientMQTT.subscribe(command1_topic); -> SUBSCRIBE TOPIC
    }
  }

  clientMQTT.setKeepAlive(90);

  Serial.print("MQTT connected to: " + String(MQTT_SRV) + ":" + String(MQTT_PRT));
}