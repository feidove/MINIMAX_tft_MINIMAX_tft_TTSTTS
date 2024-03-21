/*
    主控板采用ESP32S3
    TB链接：【淘宝】https://m.tb.cn/h.5xpu6r6?tk=fflWWMMiwQD CZ0002 「ESP32-S3-EYE兼容OpenMV核心板SP开发板语音识别 酷世diy wifible」
点击链接直接打开 或者 淘宝搜索直接打开
    TTS模块：【淘宝】https://m.tb.cn/h.5DKpeKD?tk=Big9WMM8Wm2 MF1643 「中文TTS文字转语音播报合成模块 单片机串口机器人播放提示广告板」
点击链接直接打开 或者 淘宝搜索直接打开
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <TFT_eSPI.h>  // Graphics library
#include <SoftwareSerial.h>

#include <stdio.h>
#include <string.h>
#include "UTF-8toGB2312.h"
//#include "cp936.h"

typedef unsigned char uint8_t; // 8bit无符号类型


SoftwareSerial mySerial(1, 2);
TFT_eSPI tft = TFT_eSPI();  // Invoke library
char a[4]={0xC4,0xE3,0xBA,0xC3};
// 1. Replace with your network credentials
const char* ssid = "Inkfolio";
const char* password = "lbt20011117";
setup_t user;  // The library defines the type "setup_t" as a struct
// 2. Replace with your OpenAI API key
const char* apiKey = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJHcm91cE5hbWUiOiJJbmtmb2xpbyIsIlVzZXJOYW1lIjoiSW5rZm9saW8iLCJBY2NvdW50IjoiIiwiU3ViamVjdElEIjoiMTc2ODUzNzY0NTgzMjQ4Njk0NyIsIlBob25lIjoiMTg2NzIxODU0MDkiLCJHcm91cElEIjoiMTc2ODUzNzY0NTgyNDA5ODMzOSIsIlBhZ2VOYW1lIjoiIiwiTWFpbCI6IiIsIkNyZWF0ZVRpbWUiOiIyMDI0LTAzLTE2IDE2OjIyOjQ5IiwiaXNzIjoibWluaW1heCJ9.wTdvtIeB2yCabhpGKzZXpmy7IHpMJlF0KDUPxN8yJTe2RVNDlMcYWsGXxndYnjcke75hJHofxKPQgmRjECWMx3W1yOk2bOwjAdc28TOCIc-CXY3cJs-u96b7l15Fla6x4ip-CZe-bgRgIOzF8hKQRnRbeM6wGQ5eI3rQPlFY806TncJoDV7zuGMBtj4S9fVfn8fTXaFxEVaCj17H6PKToy_SFZzhP-vY19GnXSU3lyeLCKVoWAfgldA4NHDgQ3Bgwdc3BbhJXxWdgIpKsKb9yjRnwKElhk9nimv_wludyJDEC4BZ29bpWy1SYwLvVX8VjS3r3jd2roEJujIhRvZLQw";

// Send request to OpenAI API
String inputText = "你好，minimax！";
String apiUrl = "https://api.minimax.chat/v1/text/chatcompletion_v2";

String answer;
String getGPTAnswer(String inputText) {
  HTTPClient http;
  http.setTimeout(10000);
  http.begin(apiUrl);
  http.addHeader("Content-Type", "application/json");
  String token_key = String("Bearer ") + apiKey;
  http.addHeader("Authorization", token_key);
  String payload = "{\"model\":\"abab5.5s-chat\",\"messages\":[{\"role\": \"system\",\"content\": \"你是韬韬的生活助手机器人，回答用GBK码。\"},{\"role\": \"user\",\"content\": \"" + inputText + "\"}]}";
  int httpResponseCode = http.POST(payload);  //，要求下面的回答用英文,字符控制在50个以内
  if (httpResponseCode == 200) {
    String response = http.getString();
    http.end();
    Serial.println(response);

    // Parse JSON response
    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, response);
    String outputText = jsonDoc["choices"][0]["message"]["content"];
    // Set "cursor" at top left corner of display (0,0) and select font 4
    tft.fillScreen(TFT_BLACK);
    tft.drawRect(0, 0, tft.width(), tft.height(), TFT_GREEN);
    tft.setCursor(0, 4, 4);
    // Set the font colour to be white with a black background
    tft.setTextColor(TFT_WHITE);
    // We can now plot text on screen using the "print" class
    tft.println(outputText);
    return outputText;
    // Serial.println(outputText);
  } else {
    http.end();
    Serial.printf("Error %i \n", httpResponseCode);
    return "<error>";
  }
}

void setup() {
  // Initialize Serial
  //Serial.begin(115200);
  mySerial.begin(9600);
  mySerial.println(a);
  Serial.begin(9600);
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_GREEN);

  // Connect to Wi-Fi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  answer = getGPTAnswer(inputText);
  Serial.println("Answer: " + answer);
  Serial.println("Enter a prompt:");
}

void loop() {
  // do nothing
  if (Serial.available()) {
    inputText = Serial.readStringUntil('\n');
    // inputText.trim();
    Serial.println("\n Input:" + inputText);

    answer = getGPTAnswer(inputText);
    Serial.println("Answer: " + answer);
    Serial.println("Enter a prompt:");
    int num = answer.length();
    uint8_t s[num * 2];
    int len = Utf8ToGb2312(answer.c_str(),num,s);
    //mySerial.println(answer);
    mySerial.write(s,len);
  }
  // delay(2);
}
