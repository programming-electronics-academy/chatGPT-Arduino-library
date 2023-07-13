#include "chat.h"
#include <AUnit.h>

test(ChatBox_itializes_with_valid_values) {
  ChatGPTuino::ChatBox chat{ -5, -5 };
  assertMore(chat.numMessages(), 0);
  assertMore(chat.maxTokens(), 0);
}

test(putMessage_puts_message_in_next_available_slot) {
  ChatGPTuino::ChatBox chat{ 100, 4 };
  const char testMessage[] = "Testing";
  // chat.putMessage(testMessage);

}

void setup() {
  Serial.begin(9600);
}

void loop() {
  aunit::TestRunner::run();
}
