void helloChatGPT() {

  Serial.println("helloChatGPT--> STARTED");

  ChatGPTuino::ChatBox chat{ 50, 20 };
  chat.init(test_key,model);

  char *testMessage = "Hello ChatGPT!";
  chat.putMessage(testMessage);
  Serial.println(chat.getLastMessageContent());

  char *testMessage_2 = "I really like AI...";
  chat.putMessage(testMessage_2);
  Serial.println(chat.getLastMessageContent());

  Serial.println("helloChatGPT--> ENDED");
}