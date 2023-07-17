#include <stdlib.h>
#include <Arduino.h>
#include "chat.h"

namespace ChatGPTuino {

ChatBox::ChatBox(int maxTokens, const int numMsgs)
  : _maxTokens{ maxTokens >= 0 ? maxTokens : MIN_TOKENS },
    _numMsgs{ numMsgs >= 0 ? numMsgs : MIN_MESSAGES },
    _msgCount{ 0 },
    _MAX_MESSAGE_LENGTH{ _maxTokens * CHARS_PER_TOKEN } {
  
  _messages = new Message[numMsgs];

  for (int i = 0; i < _numMsgs; i++) {
    _messages[i].content = (char*) malloc(_MAX_MESSAGE_LENGTH * sizeof(char));
  }

};

// int ChatBox::putMessage(char *msg, int msgLength) {
//   memccpy(void *, const void *, int, size_t)
// }


}  // close namespace