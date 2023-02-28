
void readMyData(struct_data dataToRead) {

  Serial.println();
  Serial.println(".....READING DATA.....");
  Serial.println();

  Serial.println("Board: " + String(dataToRead.board));
  Serial.println("Value: " + String(dataToRead.value));
  Serial.println("Pin  : " + String(dataToRead.pin));
  Serial.println("Unit : " + String(dataToRead.unit));

  Serial.print("------------------------------------------------------");
  Serial.println();
}

void sendData(uint8_t *macToSend, struct_data dataToSend) {

  Serial.println();
  Serial.println("Sending data....");
  Serial.println("Bytes to send: " + sizeof(dataToSend));

  readMyData(dataToSend);

  if (SERVER_BOARD) {
    sendToMQTT(dataToSend);
  } else {
    esp_now_send(macToSend, (uint8_t *)&dataToSend, sizeof(dataToSend));
  };
}

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {

  Serial.println();

  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery fail");
  }

  Serial.print("------------------------------------------------------");
  Serial.println();
}

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {

  memcpy(&receivedData, incomingData, sizeof(receivedData));

  Serial.println();
  Serial.println("Recebeing data....");
  Serial.println("Bytes received: " + String(len));

  readMyData(receivedData);

  if (SERVER_BOARD) {
    sendToMQTT(receivedData);
  } else {
    sendData(macDestiny, receivedData);
  };

  RECEIVED_DATA_RECEIVED = true;

  Serial.print("------------------------------------------------------");
  Serial.println();
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {

  //RECEBE BOARDKEY, e DÁ SEND

  Serial.println();
  Serial.println(".....MQTT RECEIVED....");
  Serial.println();

  char message[5] = { 0x00 };
  for (int i = 0; i < length; i++)
    message[i] = (char)payload[i];
  message[length] = 0x00;

  Serial.println("Topic: " + String(topic));
  Serial.println("Message: " + String(message));

  RECEIVED_DATA_RECEIVED = true;

  Serial.print("------------------------------------------------------");
  Serial.println();
}

void sendToMQTT(struct_data dataToSend) {

  Serial.println();
  Serial.println(".....MQTT SEND....");
  Serial.println();

  const size_t capacity = JSON_OBJECT_SIZE(10);
  char payload[256];
  DynamicJsonDocument doc(capacity);

  String TOPIC = String(BASIC_TOP) + "/" + String(dataToSend.board);
  doc["data"]["pin"] = String(dataToSend.pin);
  doc["data"]["unitName"] = String(dataToSend.unit);
  doc["data"]["value"] = String(dataToSend.value);
  serializeJson(doc, payload);
  clientMQTT.publish((char *)String(TOPIC).c_str(), payload);

  Serial.println("Sent to: " + String(TOPIC) + " :: Payload: " + String(payload));
  Serial.println("-----------------------------------------------------");
  Serial.println();
}

void getLDR(uint8_t *macToSend) {  //DEPENDS FROM BLUE LED

  float percent = map(analogRead(LDR_LUM), 0, 1023, 0, 100);

  strcpy(myData.board, BOARD_KEY);
  myData.value = percent;
  myData.pin = LDR_LUM;
  strcpy(myData.unit, "luminosidade");

  if (percent <= 50) {
    LED_WHT_STATUS = true;
  } else {
    LED_WHT_STATUS = false;
  }
  digitalWrite(LED_WHT, LED_WHT_STATUS);

  blinkRead();
  sendData(macToSend, myData);
}

void blinkRead() {  //GREEN LED

  uint8_t i = 0;
  uint8_t times = 5;
  uint8_t _break = 65;

  do {

    digitalWrite(LED_WHT, !LED_WHT_STATUS);
    delay(_break);

    digitalWrite(LED_WHT, LED_WHT_STATUS);
    delay(_break);

    i++;

  } while (i < times);
}

void blinkReceived() {

  uint8_t i = 0;
  uint8_t times = 3;
  uint8_t _break = 50;

  do {

    digitalWrite(LED_BLU, HIGH);
    delay(_break);

    digitalWrite(LED_WHT, HIGH);
    delay(_break);

    digitalWrite(LED_GRN, HIGH);
    delay(_break);

    digitalWrite(LED_BLU, LOW);
    delay(_break);

    digitalWrite(LED_WHT, LOW);
    delay(_break);

    digitalWrite(LED_GRN, LOW);
    delay(_break * 2);

    i++;

  } while (i < times);

  digitalWrite(LED_BLU, LED_BLU_STATUS);
  digitalWrite(LED_WHT, LED_WHT_STATUS);
  digitalWrite(LED_GRN, LED_GRN_STATUS);
}

int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
    for (uint8_t i = 0; i < n; i++) {
      if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
        return WiFi.channel(i);
      }
    }
  }
  return 0;
}