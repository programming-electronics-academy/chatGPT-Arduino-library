#ifndef chatGPTuino_h
#define chatGPTuino_h

// #include <sys/_stdint.h>
#include <ArduinoJson.h>      // Handle JSON formatting for API calls
#include <WiFiClientSecure.h> // ESP32
#include <WiFi.h> // ESP32
#include <API_endpoint_cert.h>

#define MIN_TOKENS 50
#define MAX_TOKENS 2000 // Used for sizing JSON response
#define MIN_MESSAGES 5
#define CHARS_PER_TOKEN 6

// These constants are used for calculating the size of Dynamic JSON Array
// TODO Consider using the JSON_ARRAY_SIZE and JSON_OBJECT_SIZE macros
// https://arduinojson.org/v6/how-to/determine-the-capacity-of-the-jsondocument/
#define JSON_DATA_STRUCTURE_MEMORY_BASE 32
#define JSON_DATA_STRUCTURE_MEMORY_PER_MSG 64

#define JSON_KEY_STRING_MEMORY_BASE 29
#define JSON_VALUE_STRING_MEMORY_PER_MSG 35

#define JSON_MEMORY_SLACK 1000

#define PORT 443                              // The port you'll connect to on the server - this is standard.
#define SERVER_RESPONSE_WAIT_TIME (15 * 1000) // How long to wait for a server response (seconds * 1000)

// #define DEBUG_SERVER_RESPONSE_BREAKING
#define VERBOSE_PRINTS

#define OPEN_AI_END_POINT "https://api.openai.com/v1/chat/completions"
#define OPEN_AI_SERVER "api.openai.com"

enum Roles
{
  Sys,
  User,
  Assistant,
  Function,
  None
};

const char RoleNames[5][10] = {"system",
                               "user",
                               "assistant",
                               "function",
                               "none"};

enum GetResponseCodes
{
  GetResponseSuccess,
  CouldNotConnectToServer,
  ServerDidNotRespond
};

enum SysMessageModes
{
  Insert,
  Default
};

class ChatGPTuino
{

  struct Message
  {
    enum Roles role;
    char *content;
    uint32_t length;
  };

public:
  // @param maxTokens
  // @param numMsgs
  ChatGPTuino(uint32_t maxTokens, uint16_t numMsgs);
  ~ChatGPTuino();

  bool init(const char *key, const char *model);

  // Getters
  uint32_t maxTokens() const
  {
    return _maxTokens;
  }

  uint16_t numMessages() const
  {
    return _maxMsgs;
  }

  uint16_t msgCount() const
  {
    return _msgCount;
  }

  uint32_t MAX_MESSAGE_LENGTH() const
  {
    return _MAX_MESSAGE_LENGTH;
  }

  uint32_t JSON_DOC_SIZE() const
  {
    return _JSON_DOC_SIZE;
  }

  char *getLastMessageContent() const;

  uint32_t getLastMessageLength() const;

  Roles getLastMessageRole() const;

  // Dev
  char *contentPtrs(uint16_t i) const
  {
    return _messages[i].content;
  };

  char *model() const
  {
    return _model;
  };

  Roles *rolePtrs(uint16_t i) const
  {
    return &_messages[i].role;
  };

  // Setters
  uint32_t putMessage(const char *msg, uint32_t msgLength, Roles msgRole = User);
  void systemMessageMode(SysMessageModes mode, char *sysMsg = nullptr);

  // Functions
  JsonDocument generateJsonRequestBody();
  GetResponseCodes getResponse();
  void postRequest(JsonDocument *pJsonRequestBody, WiFiClientSecure *pClient);
  bool waitForServerResponse(WiFiClientSecure *pClient);
  bool putResponseInMsgArray(WiFiClientSecure *pClient);
  void safe_strncpy(char *dest, size_t destSize, const char *src);

private:
  uint32_t _maxTokens;
  uint16_t _maxMsgs;
  uint16_t _msgCount;
  uint32_t _MAX_MESSAGE_LENGTH;
  uint32_t _JSON_DOC_SIZE; // NOTE: I BELIEVE THIS WILL BE DEPRECATED
  char *_secret_key;
  char *_model;
  char *_sysMessageContent;
  SysMessageModes _sysMsgMode;
  Message *_messages;
};

#endif // ChatGPTuino.h