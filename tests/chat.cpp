#include <stdlib.h>
#include <Arduino.h>
#include "chat.h"

namespace ChatGPTuino {

// Constructor
ChatBox::ChatBox(int maxTokens, const int maxMsgs)
  : _maxTokens{ maxTokens >= 0 ? maxTokens : MIN_TOKENS },
    _maxMsgs{ maxMsgs >= 0 ? maxMsgs : MIN_MESSAGES },
    _msgCount{ 0 },
    _MAX_MESSAGE_LENGTH{ _maxTokens * CHARS_PER_TOKEN } {

  _messages = new Message[_maxMsgs];
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
  char* memoryAllocated = (char*)malloc(_maxMsgs * _MAX_MESSAGE_LENGTH * sizeof(char));

  /*STEVE Q2 - I am trying to check for a NULL pointer returned by malloc, I think this expression does the job */
  // Assign segments of memory to message content strings
  if (memoryAllocated) {

    for (int i = 0; i < _maxMsgs; i++) {
      _messages[i].content = memoryAllocated + i * _MAX_MESSAGE_LENGTH * sizeof(char);
    }

    return true;
  } else {
    return false;
  }
}

char* ChatBox::getLastMessage() const {

  if (_msgCount == 0) {
    // No message yet, do nothing.
    Serial.println("No messages to get.");
  } else {
    return _messages[(_msgCount - 1) % _maxMsgs].content;
  }
}

int ChatBox::putMessage(char* msg) {
  strcpy(_messages[(_msgCount % _maxMsgs)].content,msg);
  _msgCount++;
}


}  // close namespace