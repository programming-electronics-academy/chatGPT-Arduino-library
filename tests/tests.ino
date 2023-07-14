#include "chat.h"
#include <AUnit.h>

test(ChatBox_itializes_with_valid_values) {
  ChatGPTuino::ChatBox chat{ -5, -5 };
  assertEqual(chat.numMessages(), MIN_MESSAGES);
  assertEqual(chat.maxTokens(), MIN_TOKENS);
  assertEqual(CHARS_PER_TOKEN * chat.numMessages(), chat.MAX_MESSAGE_LENGTH());
}

// test(putMessage_puts_message_in_next_available_slot) {
//   ChatGPTuino::ChatBox chat{ 100, 4 };
//   const char testMessage[] = "Testing";
//   // chat.putMessage(testMessage);

// }

void setup() {
  Serial.begin(9600);
}

void loop() {
  aunit::TestRunner::run();
}
