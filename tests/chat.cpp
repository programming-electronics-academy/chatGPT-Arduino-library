#include <stdlib.h>
#include <Arduino.h>
#include "chat.h"

namespace ChatGPTuino {

// Constructor
ChatBox::ChatBox(int maxTokens, const int numMsgs)
  : _maxTokens{ maxTokens >= 0 ? maxTokens : MIN_TOKENS },
    _numMsgs{ numMsgs >= 0 ? numMsgs : MIN_MESSAGES },
    _msgCount{ 0 },
    _MAX_MESSAGE_LENGTH{ _maxTokens * CHARS_PER_TOKEN } {

  _messages = new Message[_numMsgs];
};

// Destructor
ChatBox::~ChatBox() {

  /*STEVE Q1 - I *think* I am freeing all the memory here that had been allocated in init() */
  // Free message content strings
  free(_messages[0].content);
  
  // Delete message structs
  delete[] _messages;
};


bool ChatBox::init() {

  // Allocate space for message content
  char* memoryAllocated = (char*)malloc(_numMsgs * _MAX_MESSAGE_LENGTH * sizeof(char));

  /*STEVE Q2 - I am trying to check for a NULL pointer returned by malloc, I think this expression does the job */ 
  // Assign segments of memory to message content strings
  if (memoryAllocated) {

    for (int i = 0; i < _numMsgs; i++) {
      _messages[i].content = memoryAllocated + i * _MAX_MESSAGE_LENGTH * sizeof(char);
    }

    return true;
  } else {
    return false;
  }
}

// int ChatBox::putMessage(char *msg, int msgLength) {
//   memccpy(void *, const void *, int, size_t)
// }


}  // close namespace