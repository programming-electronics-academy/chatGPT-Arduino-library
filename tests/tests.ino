#include "chat.h"
#include <AUnit.h>

/* Assert( expected value (Known Value), actual value(value under test)) */

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

    // Serial.print(i);
    // Serial.print(" - role PTR -> ");
    // Serial.println((long)chat.rolePtrs(i));
    // Serial.print(i);
    // Serial.print(" - content PTR -> ");
    // Serial.println((long)chat.contentPtrs(i));

    long contentPtrA = (long)chat.contentPtrs(i);
    long contentPtrB = (long)chat.contentPtrs(i + 1);
    long actual = contentPtrB - contentPtrA;
    long expected = chat.MAX_MESSAGE_LENGTH() * sizeof(char);

    assertEqual(expected, actual);
  }
}

test(putMessage_puts_message_in_next_available_slot) {

  ChatGPTuino::ChatBox chat{ 10, 4 };
  chat.init();

  char *testMessage = "testing_1";
  chat.putMessage(testMessage);

  assertEqual((const char *)chat.getLastMessage(), (const char *)testMessage);

  char *testMessage2 = "testing_2";
  chat.putMessage(testMessage2);

  assertEqual((const char *)chat.getLastMessage(), (const char *)testMessage2);
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  aunit::TestRunner::run();
}
