#include "chat.h"
#include <AUnit.h>

#define TESTING_ON 1

/* Assert( expected value (Known Value), actual value(value under test)) */

const char *test_key = "sk-VT65uEtK8cUfB1KuEx0QT3BlbkFJHnIvsADF3rJw5-XXXXXX";

#if TESTING_ON
test(ChatBox_itializes_with_valid_values) {

  ChatGPTuino::ChatBox chat{ -5, -5 };
  chat.init(test_key);
  long testDocSize = 2976;
  assertEqual(chat.numMessages(), MIN_MESSAGES);
  assertEqual(chat.maxTokens(), MIN_TOKENS);
  assertEqual(CHARS_PER_TOKEN * chat.maxTokens(), chat.MAX_MESSAGE_LENGTH());
  assertEqual(testDocSize, chat.DYNAMIC_JSON_DOC_SIZE());
}

test(init_allocates_space_for_message_contexts) {

  ChatGPTuino::ChatBox chat{ 10, 5 };
  chat.init(test_key);

  for (int i = 0; i < chat.numMessages() - 1; i++) {

    long contentPtrA = (long)chat.contentPtrs(i);
    long contentPtrB = (long)chat.contentPtrs(i + 1);
    long actual = contentPtrB - contentPtrA;
    long expected = chat.MAX_MESSAGE_LENGTH() * sizeof(char);

    assertEqual(expected, actual);
  }
}

test(putMessage_puts_message_in_next_available_slot) {

  ChatGPTuino::ChatBox chat{ 10, 4 };
  chat.init(test_key);

  char *testMessage = "testing_1";
  chat.putMessage(testMessage);

  assertEqual((const char *)chat.getLastMessageContent(), (const char *)testMessage);

  char *testMessage2 = "testing_2";
  chat.putMessage(testMessage2);

  assertEqual((const char *)chat.getLastMessageContent(), (const char *)testMessage2);
}

test(putMessage_assigns_default_role_of_message_to_user) {
  ChatGPTuino::ChatBox chat{ 10, 4 };
  chat.init(test_key);

  char *testMessage = "testing_1";
  chat.putMessage(testMessage);

  assertEqual(1, chat.getLastMessageRole());
}

test(putMessage_assigns_specified_role_to_message) {
  ChatGPTuino::ChatBox chat{ 10, 4 };
  chat.init(test_key);

  char *testMessage = "testing_1";
  chat.putMessage(testMessage, ChatGPTuino::Roles::assistant);

  assertEqual(ChatGPTuino::Roles::assistant, chat.getLastMessageRole());
}

test(generateJsonRequestBody_returns_valid_Json) {
  ChatGPTuino::ChatBox chat{ 10, 4 };
  chat.init(test_key);

  char *testMessage = "JSON body testing";

  DynamicJsonDocument testDoc = chat.generateJsonRequestBody();

  assertEqual(0, 1);
}

#endif

void setup() {
  Serial.begin(115200);
}

void loop() {

#if TESTING_ON
  aunit::TestRunner::run();
#else
  static bool runOnce = true;
  if (runOnce) {
    helloChatGPT();
    runOnce = false;
  }
#endif
}
