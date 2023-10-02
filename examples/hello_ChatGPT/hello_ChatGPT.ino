#include <ChatGPTuino.h>
#include <WiFi.h>     // ESP32
#include <secrets.h>  // Network name, password, and private API key

ChatGPTuino chat{ 50, 20 }; // Create an array with 20 message slots.  50 Tokens is max response length (about 38 words)
const char *model = "gpt-3.5-turbo";  // OpenAI Model being used

char *myMessage = "Hello ChatGPT";  // User message to ChatGPT

void setup() {

  Serial.begin(115200);
  
  // WiFi Setup
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("WiFi connected to IP address: ");
  Serial.println(WiFi.localIP());

  chat.init(openAI_Private_key, model);

  char *myMessage = "Hello ChatGPT";  // User message to ChatGPT
  // Add message to messages array
  chat.putMessage(myMessage, strlen(myMessage));
  chat.getResponse();

  Serial.println(chat.getLastMessageContent());
}

void loop() {
}