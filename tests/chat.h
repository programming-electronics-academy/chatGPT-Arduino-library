#ifndef chatGPTuino_h
#define chatGPTuino_h

namespace ChatGPTuino {

#define MIN_TOKENS 50
#define MIN_MESSAGES 1

class ChatBox {

  enum Roles { sys,
               user,
               assistant };

  const char RoleNames[3][10] = { "system",
                                  "user",
                                  "assistant" };

  struct Message {
    enum Roles role;
    char content[];
  };

public:
  class Invalid {};

  ChatBox(int maxTokens, int numMsgs);
  int maxTokens() const { return _maxTokens; }

private:
  int _maxTokens;
  int _numMsgs;
};

bool is_chatBox();

}  //close namespace

#endif
