#include <ChatGPTuino.h>
#include <WiFi.h>     // ESP32
#include <secrets.h>  // Network name, password, and private API key

const int TOKENS = 400;
const int CHARS_IN_TOKEN = 6;
const int MAX_CHARS = TOKENS * CHARS_IN_TOKEN;
const int NUM_MESSAGES = 10;
const int SERIAL_MONITOR_LINE_LENGTH = 40;

ChatGPTuino chat{ TOKENS, NUM_MESSAGES };  // Create an array with 20 message slots.  50 Tokens is max response length (about 38 words)
const char *model = "gpt-3.5-turbo";       // OpenAI Model being used

void setup() {

  Serial.begin(115200);  // Start serial communication

  // WiFi Setup
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nReady for chat!\n...");

  // Initialize messages array
  chat.init(key, model);
}

void loop() {

  static boolean newMessage = false;

  char userMessage[MAX_CHARS] = "";
  // Get new message from user, put in message array
  // If it exceeds a certain length, trash everything longer
  int i = 0;
  while (Serial.available()) {

    if (i < MAX_CHARS) {
      userMessage[i] = Serial.read();
    }
    i++;

    newMessage = true;
  }


  if (newMessage) {

    Serial.println(userMessage);                        // Display the user message
    chat.putMessage(userMessage, strlen(userMessage));  // Add user message to messages array
    chat.getResponse();                                 // Get response from ChatGPT

    boolean startNewLine = false;
    for (int i = 0; i < chat.getLastMessageLength(); i++) {

      Serial.print(chat.getLastMessageContent()[i]);  // Print Response char by char
      
      // If you come get to the of a line, set newline flag
      if (i % SERIAL_MONITOR_LINE_LENGTH == 0 && i != 0) {
        startNewLine = true;
      }

      if(chat.getLastMessageContent()[i] == '\n'){
        startNewLine = false;
      }

      // Only make a new line after a finished word
      if (chat.getLastMessageContent()[i] == ' ' && startNewLine) {
        Serial.println("");
        startNewLine = false;  // reset flag
      }

      // Delay between words
      if (chat.getLastMessageContent()[i] == ' ') {
        delay(100);
      }

    }
    Serial.println("...");  // spacer
    newMessage = false;
  }
}