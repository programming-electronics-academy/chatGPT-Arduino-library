#ifndef chatGPTuino_h
#define chatGPTuino_h

namespace ChatGPTuino {

#define MIN_TOKENS 50
#define MIN_MESSAGES 1
#define CHARS_PER_TOKEN 6

class ChatBox {

  enum Roles { sys,
               user,
               assistant };

  const char RoleNames[3][10] = { "system",
                                  "user",
                                  "assistant" };

  struct Message {
    enum Roles role;
    char* content;
  };

public:
  // @param maxTokens
  // @param numMsgs
  ChatBox(int maxTokens, int numMsgs);
  ~ChatBox();
  
  bool init();
  
  // Getters
  int maxTokens() const {
    return _maxTokens;
  }

  int numMessages() const {
    return _numMsgs;
  }

  int msgCount() const {
    return _msgCount;
  }

  int MAX_MESSAGE_LENGTH() const {
    return _MAX_MESSAGE_LENGTH;
  }

  // Dev
  char* contentPtrs(int i) const {
    return _messages[i].content;
  };

  Roles* rolePtrs(int i) const {
    return &_messages[i].role;
  };

  // Setters
  int putMessage(char* msg, int msgLength);

private:
  int _maxTokens;
  int _numMsgs;
  int _msgCount;
  int _MAX_MESSAGE_LENGTH;

  Message* _messages;
};


}  //close namespace

#endif
