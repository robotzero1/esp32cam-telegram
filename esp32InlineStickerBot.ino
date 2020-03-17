/*******************************************************************
   Basic inline sticker bot code for ESP32
   written by Giacarlo Bacchio (Gianbacchio on Github)
   adapted by Brian Lough
   bodged by Robot Zero One
   
   Basic script showing how an inline bot running on the ESP32 can 
   display a fixed set of stickers when summoned with the @bot
   command
*******************************************************************/
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp_camera.h"
#include "UniversalTelegramBotRZO.h"


// Initialize Wifi connection to the router
char ssid[] = "NSA";     // your network SSID (name)
char password[] = "orange"; // your network key

String chat_id;

// Initialize Telegram BOT
#define BOTtoken "10101010101"  // your Bot Token (Get from Botfather)
String token = BOTtoken;

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 3000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

camera_fb_t * fb;
uint8_t* fb_buffer;
size_t fb_length;
int currentByte;

// (Ai- Thinker defines)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void setup()
{
  Serial.begin(115200);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 1;  // Trying to reduce memory use
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();

  s->set_framesize(s, FRAMESIZE_XGA);

  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

bool isMoreDataAvailable() {
  return (fb_length - currentByte);
}

uint8_t photoNextByte() {
  currentByte++;
  return (fb_buffer[currentByte - 1]);
}

void take_send_photo(String chat_id, String message)
{
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  currentByte = 0;
  fb_length = fb->len;
  fb_buffer = fb->buf;

  bot.sendChatAction(chat_id, "upload_photo");
  String response = bot.sendPhotoByBinary(chat_id, "image/jpeg", fb->len, isMoreDataAvailable, photoNextByte, nullptr, nullptr);

  esp_camera_fb_return(fb);
  fb_length = NULL;
  fb_buffer = NULL;
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      chat_id = bot.messages[0].chat_id;

      for (int i = 0; i < numNewMessages; i++) {

        if (bot.messages[i].type == "inline_query") {
          String stickers_json = "["
           "{\"type\": \"sticker\", \"id\": \"rndm_1\", \"sticker_file_id\": \"CAACAgIAAxkBAAIEGF5ZAu8QOdzQwuVRC8bx1RzCiYmFAAIVAAPANk8TzVamO2GeZOcYBA\"},"
           "{\"type\": \"sticker\", \"id\": \"rndm_2\", \"sticker_file_id\": \"CAACAgIAAxkBAAIEl15aieJNgLyeZBIWIEY9dtLD_DmIAAKjAQACVp29CrgdFd218xk9GAQ\"},"
           "{\"type\": \"sticker\", \"id\": \"rndm_3\", \"sticker_file_id\": \"CAACAgIAAxkBAAIEml5ailRBIGearS6ZYit_DQPYe0ypAAILAwAC8-O-CwVcfQRNCcKwGAQ\"},"
           "{\"type\": \"sticker\", \"id\": \"rndm_4\", \"sticker_file_id\": \"CAACAgIAAxkBAAIEm15ail7AkPKmb9IXAydc_MlJ4Lb-AAJUCAACYyviCTfLtlzTZ0XfGAQ\"},"
           "{\"type\": \"sticker\", \"id\": \"rndm_5\", \"sticker_file_id\": \"CAACAgIAAxkBAAIEnF5ail8XyYPkm0Uh725Vyx-jXXwpAAKPAQACVp29CpMEvY4OJJHGGAQ\"}"
           "]";
          String thing = "/bot" + token + "/answerInlineQuery?inline_query_id=" + bot.messages[i].inline_query_id + "&results=" + stickers_json;
          bot.sendGetToTelegram(thing);

        } else {
          // probably don't want a photo as well so comment this line out.
          take_send_photo(bot.messages[i].chat_id, bot.messages[i].text);
        }
        
      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}
