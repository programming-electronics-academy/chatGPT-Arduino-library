void helloChatGPT() {

  Serial.println("helloChatGPT--> STARTED");

  ChatBox chat{ 50, 20 };
  chat.init(openAI_Private_key,model);

  char *testMessage = "Hello ChatGPT!";
  chat.putMessage(testMessage, strlen(testMessage));
  Serial.println(chat.getLastMessageContent());

  char *testMessage_2 = "I really like AI...";
  chat.putMessage(testMessage_2, strlen(testMessage_2));
  Serial.println(chat.getLastMessageContent());

  Serial.println("helloChatGPT--> ENDED");
}