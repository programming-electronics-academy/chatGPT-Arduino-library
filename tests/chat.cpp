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
    _MAX_MESSAGE_LENGTH{ _maxTokens * CHARS_PER_TOKEN },
    _DYNAMIC_JSON_DOC_SIZE{
      (JSON_DATA_STRUCTURE_MEMORY_BASE + (_maxMsgs * JSON_DATA_STRUCTURE_MEMORY_PER_MSG)) + (JSON_KEY_STRING_MEMORY_BASE + ((_MAX_MESSAGE_LENGTH + JSON_VALUE_STRING_MEMORY_PER_MSG) * _maxMsgs)) + JSON_MEMORY_SLACK
    },
    _openAPIendPoint{ "https://api.openai.com/v1/chat/completions" },
    _server{ "api.openai.com" },
    _rootCACertificate{
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
      "-----END CERTIFICATE-----\n"
    } {
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


bool ChatBox::init(const char* key, const char* model) {

  // Allocate space for API key and assign
  char* keyAlloc = (char*)malloc(API_KEY_SIZE * sizeof(char));

  if (keyAlloc) {
    _secret_key = keyAlloc;
    strcpy(_secret_key, key);
  } else {
    Serial.println("keyAlloc failed");
    return false;
  }

  // Allocate space for model and assign
  char* modelAlloc = (char*)malloc(MODEL_NAME_SIZE * sizeof(char));

  if (modelAlloc) {
    _model = modelAlloc;
    strcpy(_model, model);
  } else {
    Serial.println("modelAlloc failed");
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
    return nullptr;  // I think this is what I want to return in the case there are no messages
  } else {
    return _messages[(_msgCount - 1) % _maxMsgs].content;
  }
}

/* STEVE Q6 
I have this function that returns a role, but only if a message exists.  If no message exist, it returns nothing...
Is there something I can return that makes sense, like "null" role?
*/
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

  return _msgCount;
}

DynamicJsonDocument ChatBox::generateJsonRequestBody() {

  DynamicJsonDocument doc(_DYNAMIC_JSON_DOC_SIZE);

  doc["model"] = _model;
  doc["max_tokens"] = _maxTokens;

  // Create nested array that will hold all the system, user, and assistant messages
  JsonArray messagesJSON = doc.createNestedArray("messages");

  int oldestMsgIdx = 0;

  if (_msgCount >= _maxMsgs) {
    oldestMsgIdx = _msgCount % _maxMsgs;
  }

  for (int i = 0; i < _msgCount && i < _maxMsgs; i++) {

    messagesJSON[i]["role"] = RoleNames[_messages[oldestMsgIdx].role];
    messagesJSON[i]["content"] = _messages[oldestMsgIdx].content;

    oldestMsgIdx++;
    oldestMsgIdx %= _maxMsgs;
  }

  return doc;
}

bool ChatBox::getResponse() {
  return false;
}



}  // close namespace