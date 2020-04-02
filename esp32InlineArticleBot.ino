/*******************************************************************
   Inline bot that sends simulated sensor status to any Telegram chat

   written by Giacarlo Bacchio (Gianbacchio on Github)
   adapted by Brian Lough
   bodged by Robot Zero One

   Using the article message type you can request sensor data and 
   send any important ones into the chat
*******************************************************************/
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "UniversalTelegramBotRZO.h"

// Initialize Wifi connection to the router
char ssid[] = "NSA";     // your network SSID (name)
char password[] = "orange"; // your network key

String chat_id;

// Initialize Telegram BOT
#define BOTtoken "101010101"  // your Bot Token (Get from Botfather)
String token = BOTtoken;

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 3000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done


void setup()
{
  Serial.begin(115200);

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


void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      chat_id = bot.messages[0].chat_id;

      for (int i = 0; i < numNewMessages; i++) {

        // simulate reactor core readings. Don't have access to actual nuclear power station
        int reactor_core_reading_1 = random(5);
        int reactor_core_reading_2 = random(5);
        int reactor_core_reading_3 = random(5);
        int reactor_core_reading_4 = random(5);

        String reactor_1_thumburl = "https://robotzero.one/reactor-gifs/" + String(reactor_core_reading_1) + ".gif";
        String reactor_2_thumburl = "https://robotzero.one/reactor-gifs/" + String(reactor_core_reading_2) + ".gif";
        String reactor_3_thumburl = "https://robotzero.one/reactor-gifs/" + String(reactor_core_reading_3) + ".gif";
        String reactor_4_thumburl = "https://robotzero.one/reactor-gifs/" + String(reactor_core_reading_4) + ".gif";
        
        if (bot.messages[i].type == "inline_query") { // inline request from Telegram
          String reactors_json = "["
                                 "{\"type\": \"article\", \"id\": \"rndm_1\", \"thumb_url\": \"" + reactor_1_thumburl + "\", \"title\": \"Reactor 1\", \"description\": \"Request Reactor 1 Check\", \"input_message_content\": {\"message_text\": \"<b>Check Reactor One!!</b>\", \"parse_mode\": \"HTML\" }},"
                                 "{\"type\": \"article\", \"id\": \"rndm_2\", \"thumb_url\": \"" + reactor_2_thumburl + "\", \"title\": \"Reactor 2\", \"description\": \"Request Reactor 2 Check\", \"input_message_content\": {\"message_text\": \"<b>Check Reactor Two!!</b>\", \"parse_mode\": \"HTML\" }},"
                                 "{\"type\": \"article\", \"id\": \"rndm_3\", \"thumb_url\": \"" + reactor_3_thumburl + "\", \"title\": \"Reactor 3\", \"description\": \"Request Reactor 3 Check\", \"input_message_content\": {\"message_text\": \"<b>Check Reactor Three!!</b>\", \"parse_mode\": \"HTML\" }},"
                                 "{\"type\": \"article\", \"id\": \"rndm_4\", \"thumb_url\": \"" + reactor_4_thumburl + "\", \"title\": \"Reactor 4\", \"description\": \"Request Reactor 4 Check\", \"input_message_content\": {\"message_text\": \"<b>Check Reactor Four!!</b>\", \"parse_mode\": \"HTML\" }}"
                                 "]";
          String thing = "/bot" + token + "/answerInlineQuery?inline_query_id=" + bot.messages[i].inline_query_id + "&cache_time=5&results=" + reactors_json;
          bot.sendGetToTelegram(thing);
        } else {
          //
        }

      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    Bot_lasttime = millis();
  }
}
