#include "HardwareSerial.h"
#include <stdlib.h>
#include <Arduino.h>
#include "chat.h"

namespace ChatGPTuino {

// Constructor
ChatBox::ChatBox(int maxTokens = MIN_TOKENS, const int maxMsgs = MIN_MESSAGES)
  : _maxTokens{ maxTokens >= 0 ? maxTokens : MIN_TOKENS },
    _maxMsgs{ maxMsgs >= 0 ? maxMsgs : MIN_MESSAGES },
    _msgCount{ 0 },
    _MAX_MESSAGE_LENGTH{ _maxTokens * CHARS_PER_TOKEN } {

  /* STEVE Q4 ----Maybe I should move this to init()?  
 Maybe I should be be checking that "new" succeeds in it's memory allocation?*/
  _messages = new Message[_maxMsgs];
};

// Destructor
ChatBox::~ChatBox() {

  /*STEVE Q1 - I *think* I am freeing all the memory here that had been allocated in init() */
  // Free message content strings
  free(_messages[0].content);  // This is the pointer returned from init()
  free(_secret_key);
  
  // Delete message structs
  delete[] _messages;
};


bool ChatBox::init(const char* key) {

  // Allocate space for API key and assign
  char* keyAlloc = (char*)malloc(API_KEY_SIZE * sizeof(char));

  if (keyAlloc) {
    _secret_key = keyAlloc;
    strcpy(_secret_key, key);
  } else {
    Serial.println("keyAlloc failed");
    return false;
  }

  // Allocate space for message content
  char* contentAlloc = (char*)malloc(_maxMsgs * _MAX_MESSAGE_LENGTH * sizeof(char));

  /*STEVE Q2 - I am trying to check for a NULL pointer returned by malloc, I think this expression does the job */
  // Assign segments of memory to message content strings
  if (contentAlloc) {

    for (int i = 0; i < _maxMsgs; i++) {
      _messages[i].content = contentAlloc + i * _MAX_MESSAGE_LENGTH * sizeof(char);
    }

    return true;
  } else {
    return false;
  }
}

char* ChatBox::getLastMessageContent() const {

  if (_msgCount == 0) {
    // No message yet, do nothing.
    Serial.println("No message to get.");
  } else {
    return _messages[(_msgCount - 1) % _maxMsgs].content;
  }
}

Roles ChatBox::getLastMessageRole() const {

  if (_msgCount == 0) {
    // No message yet, do nothing.
    Serial.println("No message to get.");
  } else {
    return _messages[(_msgCount - 1) % _maxMsgs].role;
  }
}

int ChatBox::putMessage(char* msg, Roles msgRole) {
  strcpy(_messages[(_msgCount % _maxMsgs)].content, msg);
  _messages[(_msgCount % _maxMsgs)].role = msgRole;
  _msgCount++;
}


}  // close namespace