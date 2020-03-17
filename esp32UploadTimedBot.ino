/*******************************************************************
   Basic bot command example for ESP32 Camera
   written by Giacarlo Bacchio (Gianbacchio on Github)
   adapted by Brian Lough
   bodged by Robot Zero One
   
   Send a command to your bot to request a new photo every X minutes
*******************************************************************/
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp_camera.h"
#include "UniversalTelegramBotRZO.h"


// Initialize Wifi connection to the router
char ssid[] = "NSA";     // your network SSID (name)
char password[] = "orange"; // your network key

String chat_id;
long photo_sent;
String last_chat_id;
long millsecs_delay;

// Initialize Telegram BOT
#define BOTtoken "10101010101010"  // your Bot Token (Get from Botfather)
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

  // comment out for Ai-thinker
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);

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

void take_send_photo(String chat_id)
{
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  currentByte = 0;
  fb_length = fb->len;
  fb_buffer = fb->buf;

  bot.sendChatAction(chat_id, "upload_photo");
  bot.sendPhotoByBinary(chat_id, "image/jpeg", fb->len, isMoreDataAvailable, photoNextByte, nullptr, nullptr);

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
          //
        } else {
          String delay_command = (bot.messages[i].text); // message sent to bot ie /5 for 5 minutes
          delay_command.remove(0, 1); // remove /
          last_chat_id = bot.messages[i].chat_id;
          bot.sendMessage(last_chat_id, "Sending a photo every " + delay_command + " minutes", "");
          millsecs_delay = delay_command.toInt() * 60 * 1000;
        }

      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    if (millis() > photo_sent + millsecs_delay) {
      take_send_photo(last_chat_id);
      photo_sent = millis();
    }

    Bot_lasttime = millis();
  }
}
