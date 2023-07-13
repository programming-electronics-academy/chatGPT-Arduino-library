#include "chat.h"

namespace ChatGPTuino {

ChatBox::ChatBox(int maxTokens, const int numMsgs)
  : _maxTokens{ maxTokens >= 0 ? maxTokens : MIN_TOKENS },
    _numMsgs{ numMsgs >= 0 ? numMsgs : MIN_MESSAGES } {
  _messages = new Message[numMsgs];
};


}  // close namespace