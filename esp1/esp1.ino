/*
  ESP MASTER + WIFI + MQTT
*/

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

WiFiClient wifiClient;
PubSubClient clientMQTT(wifiClient);

#define BASIC_TOP "client"
#define BOARD_KEY "84bcc6e8"

int32_t CHANNEL = 10;

#define LED_GRN 0
#define LED_WHT 2
#define PSH_BTN 5
#define LDR_LUM A0
#define LED_BLU 14

/*
  Idle –without function-   ESP_NOW_ROLE_IDLE = 0           no interface
  Master                    ESP_NOW_ROLE_CONTROLLER = 1     priority is given to the station interface
  Slave                     ESP_NOW_ROLE_SLAVE = 2          priority is given to the SoftAP interface
  Master + Slave            ESP_NOW_ROLE_MAX = 3            priority is given to the station interface
  Master + Slave            ESP_NOW_ROLE_COMBO              priority is given to the SoftAP interface

*/

#define ESP_SELF_ROLE 3
#define ESP_PEER_ROLE 3

uint8_t e1MAC[] = { 0x98, 0xF4, 0xAB, 0xDC, 0x18, 0x48 };  // ESP1 - 84bcc6e8
uint8_t e2MAC[] = { 0x98, 0xF4, 0xAB, 0xDC, 0xE0, 0xAB };  // ESP2 - 7855d065
uint8_t e3MAC[] = { 0x98, 0xF4, 0xAB, 0xDC, 0x17, 0xF7 };  // ESP3 - dc70f0d2

//END POINT
uint8_t *macDestiny = NULL;

bool PSH_LST_STATUS = false;
bool PSH_BTN_STATUS = false;
bool LED_GRN_STATUS = false;
bool LED_WHT_STATUS = false;
bool LED_BLU_STATUS = false;

bool RECEIVED_DATA_RECEIVED = false;
bool SERVER_BOARD = true;

/*
  TIMMERS
*/
long PUSH_BOUNCE = 100;
long PUSH_TIMMER;
long LED_BLU_COUNTER = 5000;
long LED_BLU_TIMMER;
long INTERNET_CHECKER = 5000;
long INTERNET_TIMMER;
// -------------------------------------------------

typedef struct struct_data {
  uint8_t pin;
  float value;
  char unit[32];
  char board[32];
} data;

struct_data myData;
struct_data receivedData;

void setup() {

  setupSerial();
  setupDigital();
  setupVariables();
  setupWiFi();
  setupEspNow(macDestiny);

  if (SERVER_BOARD) setupMQTT();

  Serial.println("System OK!");
  Serial.println("------------------------------------------------------");
  Serial.println();

  LED_GRN_STATUS = true;
  digitalWrite(LED_GRN, LED_GRN_STATUS);
}

void loop() {

  if (SERVER_BOARD) checkInternet();
  monitoringDataReceived();
  monitoringPushButton();
  monitogingLedBlue(macDestiny);
}
