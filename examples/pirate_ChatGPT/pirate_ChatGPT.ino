#include <ChatGPTuino.h>
#include <WiFi.h>     // ESP32
#include <secrets.h>  // Network name, password, and private API key

ChatGPTuino chat{ 50, 20 }; // Create an array with 20 message slots.  50 Tokens is max response length (about 38 words)
const char *model = "gpt-3.5-turbo";  // OpenAI Model being used

void setup() {

  Serial.begin(115200); // Start serial communication
  
  // WiFi Setup
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Initialize messages array
  chat.init(key, model);

  const char *userMessage = "Hello ChatGPT";  // User message to ChatGPT
  const char *systemMessage = "Please respond like a pirate";  // System message to ChatGPT
  
  // Add user message to messages array
  chat.putMessage(userMessage, strlen(userMessage));
  // Add system message to messages array
  chat.putMessage(systemMessage, strlen(systemMessage), sys);
  
  // Send message array and receive response
  chat.getResponse();

  // Print response
  Serial.println(chat.getLastMessageContent());
}

void loop() {
}