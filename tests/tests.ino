#include "chat.h"
#include <AUnit.h>

test(ChatBox_tokens_must_be_positive) {
  
  ChatGPTuino::ChatBox chat{ -5, 4 };
  assertMore(chat.maxTokens(), 0);
}



void setup() {
  Serial.begin(9600);
}

void loop() {
  aunit::TestRunner::run();
}
