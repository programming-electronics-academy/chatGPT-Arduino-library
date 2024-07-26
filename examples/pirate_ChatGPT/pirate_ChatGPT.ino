#include <ChatGPTuino.h>
#include <WiFi.h>  // ESP32
// #include "credentials.h"  // Network name, password, and private API key
#include "secrets.h"  // Network name, password, and private API key

// A quick primer on the chatGPT API https://www.programmingelectronics.com/chatgpt-api/
const int TOKENS = 100;  // How lengthy a response you want, every token is about 3/4 a word
const int NUM_MESSAGES = 20;

ChatGPTuino chat{ TOKENS, NUM_MESSAGES };  // Will store and send your most recent messages (up to NUM_MESSAGES)
const char *model = "gpt-3.5-turbo";       // OpenAI Model being used

void setup() {

  Serial.begin(115200);  // Start serial communication

  // WiFi Setup
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  char *sysMsgTest = "If this is a test, please respond with only the word PASS, otherwise please respond with only the word FAIL";
  // chat.init(key, model);
  // Initialize messages array
  chat.init(key, model);
  // chat.systemMessageMode(Insert, sysMsgTest);

  const char *userMessage = "Hello ChatGPT";                   // User message to ChatGPT
  const char *systemMessage = "Please respond like a pirate";  // System message to ChatGPT

  // Add user message to messages array
  chat.putMessage(userMessage, strlen(userMessage));
  // Add system message to messages array
  chat.putMessage(systemMessage, strlen(systemMessage), Sys);

  // Send message array and receive response
  chat.getResponse();

  // Print response
  Serial.println(chat.getLastMessageContent());
}

void loop() {
}