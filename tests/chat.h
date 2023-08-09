#ifndef chatGPTuino_h
#define chatGPTuino_h

#include <ArduinoJson.h>       // Handle JSON formatting for API calls

namespace ChatGPTuino {

#define MIN_TOKENS 50
#define MIN_MESSAGES 5
#define CHARS_PER_TOKEN 6
#define API_KEY_SIZE 100

// These constants are used for calculating the size of Dynamic JSON Array
// TODO Consider using the JSON_ARRAY_SIZE and JSON_OBJECT_SIZE macros
// https://arduinojson.org/v6/how-to/determine-the-capacity-of-the-jsondocument/
#define JSON_DATA_STRUCTURE_MEMORY_BASE 32
#define JSON_DATA_STRUCTURE_MEMORY_PER_MSG 48

#define JSON_KEY_STRING_MEMORY_BASE 29 
#define JSON_VALUE_STRING_MEMORY_PER_MSG 35

#define JSON_MEMORY_SLACK 1000

enum Roles { sys,
             user,
             assistant };

const char RoleNames[3][10] = { "system",
                                "user",
                                "assistant" };

class ChatBox {


  struct Message {
    enum Roles role;
    char* content;
  };

public:
  // @param maxTokens
  // @param numMsgs
  ChatBox(int maxTokens, int numMsgs);
  ~ChatBox();

  bool init(const char* key);

  // Getters
  int maxTokens() const {
    return _maxTokens;
  }

  int numMessages() const {
    return _maxMsgs;
  }

  int msgCount() const {
    return _msgCount;
  }

  int MAX_MESSAGE_LENGTH() const {
    return _MAX_MESSAGE_LENGTH;
  }

  long DYNAMIC_JSON_DOC_SIZE() const {
    return _DYNAMIC_JSON_DOC_SIZE;
  }

  char* getLastMessageContent() const;

  Roles getLastMessageRole() const;

  /* STEVE Q3 - I have always used byte, int, long as datatypes, but I am starting to wonder if ought to use fixed width types uint8_t, uint16_t, etc... */
  // Dev
  char* contentPtrs(int i) const {
    return _messages[i].content;
  };

  Roles* rolePtrs(int i) const {
    return &_messages[i].role;
  };

  // Setters
  int putMessage(char* msg, Roles msgRole = user);

  DynamicJsonDocument generateJsonRequestBody();


private:
  int _maxTokens;
  int _maxMsgs;
  int _msgCount;
  int _MAX_MESSAGE_LENGTH;
  long _DYNAMIC_JSON_DOC_SIZE;
  char* _secret_key;
  Message* _messages;
};


}  //close namespace

#endif
