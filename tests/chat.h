#ifndef chatGPTuino_h
#define chatGPTuino_h

/* Steve Q9 *************************************************************************************
  If library A dependant on library B, is it best to include library B in library A's header file?
  Or expect the end user to include it in their own program?
*/
#include <ArduinoJson.h>       // Handle JSON formatting for API calls
#include <WiFiClientSecure.h>  // ESP32


/* Steve Q42 *************************************************************************************
  I think I am using namespace correct, but then again - not feeling overly confident :)
*/
namespace ChatGPTuino {

#define MIN_TOKENS 50
#define MAX_TOKENS 2000  // Used for sizing JSON reponse
#define MIN_MESSAGES 5
#define CHARS_PER_TOKEN 6
#define API_KEY_SIZE 100
#define MODEL_NAME_SIZE 50

// These constants are used for calculating the size of Dynamic JSON Array
// TODO Consider using the JSON_ARRAY_SIZE and JSON_OBJECT_SIZE macros
// https://arduinojson.org/v6/how-to/determine-the-capacity-of-the-jsondocument/
#define JSON_DATA_STRUCTURE_MEMORY_BASE 32
#define JSON_DATA_STRUCTURE_MEMORY_PER_MSG 64

#define JSON_KEY_STRING_MEMORY_BASE 29
#define JSON_VALUE_STRING_MEMORY_PER_MSG 35

#define JSON_MEMORY_SLACK 1000


#define PORT 443                               // The port you'll connect to on the server - this is standard.
#define SERVER_RESPONSE_WAIT_TIME (15 * 1000)  // How long to wait for a server response (seconds * 1000)



// #define DEBUG_SERVER_RESPONSE_BREAKING

/* Steve Q8 *************************************************************************************
  I wanted to have these constants defined below, but I was getting a "first defined here" error
  when using them here, so I made each of these private data members, and initialize them in the constructor.
  Clearly, there is some error in my thinking about where constant data should go...
*/
#define OPEN_AI_END_POINT "https://api.openai.com/v1/chat/completions"
#define OPEN_AI_SERVER "api.openai.com"
#define ROOT_CA_CERT "-----BEGIN CERTIFICATE-----\n"\
  "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n"\
  "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n"\
  "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n"\
  "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n"\
  "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n"\
  "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n"\
  "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n"\
  "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n"\
  "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n"\
  "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n"\
  "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n"\
  "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n"\
  "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n"\
  "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n"\
  "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n"\
  "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n"\
  "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n"\
  "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n"\
  "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"\
  "-----END CERTIFICATE-----\n"

/*************** Open AI endpoint and connection details ****************/
// const char* openAPIendPoint = "https://api.openai.com/v1/chat/completions";
// const char* server = "api.openai.com";

//OpenAI API endpoint root certificate used to ensure response is actually from OpenAPI
/*
const char* rootCACertificate =
  "-----BEGIN CERTIFICATE-----\n"
  "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n"
  "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n"
  "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n"
  "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n"
  "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n"
  "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n"
  "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n"
  "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n"
  "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n"
  "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n"
  "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n"
  "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n"
  "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n"
  "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n"
  "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n"
  "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n"
  "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n"
  "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n"
  "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"
  "-----END CERTIFICATE-----\n";

  */

enum Roles { sys,
             user,
             assistant };

const char RoleNames[3][10] = { "system",
                                "user",
                                "assistant" };


enum getResponseCodes { getResponseSuccess,
                        couldNotConnectToServer,
                        serverDidNotRespond
};

class ChatBox {

  struct Message {
    enum Roles role;
    char* content;
    int length;
  };

public:
  // @param maxTokens
  // @param numMsgs
  ChatBox(int maxTokens, int numMsgs);
  ~ChatBox();

  bool init(const char* key, const char* model);

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

  int getLastMessageLength() const;

  Roles getLastMessageRole() const;

  // const char* openAPIendPoint() const {
  //   return _openAPIendPoint;
  // };

  // const char* server() const {
  //   return _server;
  // };

/* Steve Q3 *************************************************************************************
  I have always used byte, int, long as datatypes, 
  but I am starting to wonder if ought to use fixed width types uint8_t, uint16_t, etc...
*/
  // Dev
  char* contentPtrs(int i) const {
    return _messages[i].content;
  };

  char* model() const {
    return _model;
  };

  Roles* rolePtrs(int i) const {
    return &_messages[i].role;
  };

  // Setters
  int putMessage(const char* msg, int msgLength, Roles msgRole = user);

  // Functions
  DynamicJsonDocument generateJsonRequestBody();
  getResponseCodes getResponse();
  void postRequest(DynamicJsonDocument* pJsonRequestBody, WiFiClientSecure* pClient);
  bool waitForServerResponse(WiFiClientSecure* pClient);
  bool putResponseInMsgArray(WiFiClientSecure* pClient);
  void safe_strncpy(char* dest, size_t destSize, const char* src);


private:
  int _maxTokens;
  int _maxMsgs;
  int _msgCount;
  int _MAX_MESSAGE_LENGTH;
  long _DYNAMIC_JSON_DOC_SIZE;
  char* _secret_key;
  char* _model;
  Message* _messages;

  // const char* _openAIendPoint;
  // const char* _server;
  // const char* _rootCACertificate;
};


}  //close namespace

#endif
