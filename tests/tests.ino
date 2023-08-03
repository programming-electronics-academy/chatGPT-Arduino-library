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

    assertEqual(expected, actual);
  }
}

test(putMessage_puts_message_in_next_available_slot) {

  ChatGPTuino::ChatBox chat{ 10, 4 };
  chat.init();

  Serial.print("chat msgCount ->");
  Serial.println(chat.msgCount());

  const byte MSG_SIZE = chat.MAX_MESSAGE_LENGTH();
  char testMessage_Expected[MSG_SIZE] = "YO, I am Testing!";
  char testMessage_Actual[MSG_SIZE] = "Testing";
  
  // char * testMessage_Expected = "Testing";
  // char * testMessage_Actual = "Testing";
  

  
  chat.putMessage(testMessage_Expected);

  Serial.print("chat msgCount ->");
  Serial.println(chat.msgCount());
  
  Serial.println("Last Message ->");
  Serial.println(chat.getLastMessage());


  assertEqual((const char *)chat.getLastMessage(), (const char *)testMessage_Actual);
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  aunit::TestRunner::run();
}
