
#include "chat.h"

namespace ChatGPTuino {

ChatBox::ChatBox(int maxTokens, const int numMsgs)
  : _maxTokens{ maxTokens >= 0 ? maxTokens : MIN_TOKENS },
    _numMsgs{ numMsgs >= 0 ? numMsgs : MIN_MESSAGES },
    _msgCount{ 0 },
    _MAX_MESSAGE_LENGTH{ _numMsgs * CHARS_PER_TOKEN } {
  
  _messages = new Message[numMsgs];
};

// int ChatBox::putMessage(char *msg, int msgLength) {
//   memccpy(void *, const void *, int, size_t)
// }


}  // close namespace