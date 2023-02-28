
void monitoringPushButton() {

  bool btn_pressed = !digitalRead(PSH_BTN);

  if (btn_pressed != PSH_LST_STATUS) {
    PUSH_TIMMER = millis();
  }

  if ((millis() - PUSH_TIMMER) > PUSH_BOUNCE) {

    if (btn_pressed != PSH_BTN_STATUS) {
      PSH_BTN_STATUS = btn_pressed;

      if (PSH_BTN_STATUS == HIGH) {
        LED_BLU_STATUS = !LED_BLU_STATUS;
      }
    }
  }

  digitalWrite(LED_BLU, LED_BLU_STATUS);
  PSH_LST_STATUS = btn_pressed;
}

void monitogingLedBlue(uint8_t* macToSend) {

  if (LED_BLU_STATUS) {

    if ((millis() - LED_BLU_TIMMER) > LED_BLU_COUNTER) {
      LED_BLU_TIMMER = millis();
      getLDR(macToSend);
    }

  } else {
    LED_BLU_TIMMER = millis();
  }
}

void monitoringDataReceived() {

  if (RECEIVED_DATA_RECEIVED) {
    blinkReceived();
    
    RECEIVED_DATA_RECEIVED = false;
  }
}

void checkInternet() {

  if ((millis() - INTERNET_TIMMER) > INTERNET_CHECKER) {
    INTERNET_TIMMER = millis();

    if (WiFi.status() != WL_CONNECTED) {

      Serial.println("Connection on WiFi is lost! Trying to reconnect...");
      Serial.println();

      connectToWifi();

      Serial.println();
      Serial.println("WiFi OK!");
      Serial.println("------------------------------------------------------");
      Serial.println();
    }
  }

  if (!clientMQTT.connected()) {

    Serial.println("Connection on MQTT is lost! Trying to reconnect...");
    Serial.println();

    connectToMQTT();

    Serial.println();
    Serial.println("MQTT OK!");
    Serial.println("------------------------------------------------------");
    Serial.println();
  }
}