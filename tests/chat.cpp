#include <stdlib.h>
#include <Arduino.h>
#include "chat.h"

namespace ChatGPTuino {

//Constructor
ChatBox::ChatBox(int maxTokens, const int numMsgs)
  : _maxTokens{ maxTokens >= 0 ? maxTokens : MIN_TOKENS },
    _numMsgs{ numMsgs >= 0 ? numMsgs : MIN_MESSAGES },
    _msgCount{ 0 },
    _MAX_MESSAGE_LENGTH{ _maxTokens * CHARS_PER_TOKEN } {

  _messages = new Message[_numMsgs];

  // char* buf = (char*)malloc(_numMsgs * _MAX_MESSAGE_LENGTH * sizeof(char));
  
  // for (int i = 0; i < _numMsgs; i++) {
  //   _messages[i].content = buf + i * _MAX_MESSAGE_LENGTH * sizeof(char);
  // }
};

//Destructor
ChatBox::~ChatBox(){
  free(_messages[0].content);
  delete [] _messages;
  Serial.println("Destructor Called!");
};

bool ChatBox::init() {

  char* buf = (char*)malloc(_numMsgs * _MAX_MESSAGE_LENGTH * sizeof(char));
  
  for (int i = 0; i < _numMsgs; i++) {
    _messages[i].content = buf + i * _MAX_MESSAGE_LENGTH * sizeof(char);
  }
}

// int ChatBox::putMessage(char *msg, int msgLength) {
//   memccpy(void *, const void *, int, size_t)
// }


}  // close namespace