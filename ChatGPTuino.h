#ifndef chatGPTuino_h
#define chatGPTuino_h

// #include <sys/_stdint.h>
#include <ArduinoJson.h>      // Handle JSON formatting for API calls
#include <WiFiClientSecure.h> // ESP32

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
// #define VERBOSE_PRINTS

#define OPEN_AI_END_POINT "https://api.openai.com/v1/chat/completions"
#define OPEN_AI_SERVER "api.openai.com"
// OpenAI API endpoint root certificate used to ensure response is actually from OpenAPI
//  TODO - Verify that the certificate matters!  Have a check that verifies the connection is secure.
#define ROOT_CA_CERT "-----BEGIN CERTIFICATE-----\n"                                      \
                     "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n" \
                     "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n" \
                     "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n" \
                     "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n" \
                     "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n" \
                     "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n" \
                     "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n" \
                     "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n" \
                     "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n" \
                     "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n" \
                     "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n" \
                     "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n" \
                     "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n" \
                     "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n" \
                     "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n" \
                     "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n" \
                     "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n" \
                     "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n" \
                     "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"                             \
                     "-----END CERTIFICATE-----\n"

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