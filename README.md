# esp32cam-telegram

Some Arduino scripts to do various things with ESP32 based modules and the Telegram Bot API.

### TelegramSendPhotoChunked1_2.ino 
Use an ESP32-CAM to send photos on request to Telegram via a Bot. Details on the blog here: https://robotzero.one/telegram-bot-esp32cam/

### esp32UploadTimedBot.ino 
Use this to create a bot that takes and uploads a photo every X minutes.

### esp32InlinePhotoUpload.ino
Taking a photo using 'inline' mode so the bot can be accessed in any chat

### esp32InlineArticleBot.ino
Using the Inline Article mode of Telegram Bots to request sensor data and send any important ones into the chat. Works with any ESP32.

### esp32InlineStickerBot.ino
This creates an Inline bot that when requested, offers a preset list of stickers to insert into the chat. Works with any ESP32.


The following two files are required UniversalTelegramBotRZO.cpp and UniversalTelegramBotRZO.h for the scripts above

UniversalTelegramBot1_2.cpp and UniversalTelegramBot1_2.h are the 1.2 version release of the https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/tree/V1.2.0 library used as the base for the RZO files.

