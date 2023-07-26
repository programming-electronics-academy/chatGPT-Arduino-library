#include "chat.h"
#include <AUnit.h>

// Upload isue for XIAO -> hold B (boot) button, press and release R (reset) button, release B button.  Upload


test(ChatBox_itializes_with_valid_values) {
  ChatGPTuino::ChatBox chat{ -5, -5 };
  assertEqual(chat.numMessages(), MIN_MESSAGES);
  assertEqual(chat.maxTokens(), MIN_TOKENS);
  assertEqual(CHARS_PER_TOKEN * chat.maxTokens(), chat.MAX_MESSAGE_LENGTH());
}

test(init_allocates_space_for_message_contexts) {
  
  ChatGPTuino::ChatBox chat{ 10, 5 };
  chat.init();

  for (int i = 0; i < chat.numMessages() - 1; i++) {
    
    Serial.print(i);
    Serial.print(" - role PTR -> ");
    Serial.println((long)chat.rolePtrs(i));
    Serial.print(i);
    Serial.print(" - content PTR -> ");
    Serial.println((long)chat.contentPtrs(i));
    
    int contentPtrA = (long)chat.contentPtrs(i);
    int contentPtrB = (long)chat.contentPtrs(i + 1);
    int actual = contentPtrB - contentPtrA;
    int expected = chat.MAX_MESSAGE_LENGTH() * sizeof(char);

    assertEqual(actual, expected);
  }
}
// test(putMessage_puts_message_in_next_available_slot) {
//   ChatGPTuino::ChatBox chat{ 100, 4 };
//   const char testMessage[] = "Testing";
//   // chat.putMessage(testMessage);

// }

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("testing");
  aunit::TestRunner::run();
}
