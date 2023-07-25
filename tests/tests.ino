#include "chat.h"
#include <AUnit.h>

test(ChatBox_itializes_with_valid_values) {
  ChatGPTuino::ChatBox chat{ -5, -5 };
  assertEqual(chat.numMessages(), MIN_MESSAGES);
  assertEqual(chat.maxTokens(), MIN_TOKENS);
  assertEqual(CHARS_PER_TOKEN * chat.maxTokens(), chat.MAX_MESSAGE_LENGTH());
}

test(malloc_allocates_space_for_messages) {
  
  ChatGPTuino::ChatBox chat{ 10, 5 };

  for (int i = 0; i < chat.numMessages() - 1; i++) {
    
    Serial.print(i);
    Serial.print(" - role PTR -> ");
    Serial.println((long)chat.rolePtrs(i));
    Serial.print(i);
    Serial.print(" - content PTR -> ");
    Serial.println((long)chat.contentPtrs(i));
    
    // int contentPtrA = (int)chat.contentPtrs(i);
    // int contentPtrB = (int)chat.contentPtrs(i + 1);
    // int actual = contentPtrB - contentPtrA;
    // int expected = chat.MAX_MESSAGE_LENGTH() * sizeof(char);

    // int rolePtrA = (int)chat.rolePtrs(i);
    // int rolePtrB = (int)chat.rolePtrs(i + 1);

    // Serial.print("contentPtrA ");
    // Serial.println(contentPtrA);
    // Serial.print("contentPtrB ");
    // Serial.println(contentPtrB);
    // Serial.print("Actual ");
    // Serial.println(actual);
    
    // Serial.print("rolePtrA ");
    // Serial.println(rolePtrA);
    // Serial.print("rolePtrB ");
    // Serial.println(rolePtrB);
    
    // Serial.println(rolePtrA);
    // Serial.println(rolePtrB);
    // Serial.println(actual);
    // assertEqual(actual, expected);
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
  aunit::TestRunner::run();
}
