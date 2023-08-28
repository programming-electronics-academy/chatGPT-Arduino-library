#include <cstddef>
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
    } {
  /* Steve Q4 *************************************************************************************
  Maybe I should move this to init()?  
  Maybe I should be be checking that "new" succeeds in it's memory allocation?
*/
  _messages = new Message[_maxMsgs];
};

// Destructor
ChatBox::~ChatBox() {

  /* Steve Q1 *************************************************************************************
  I *think* I am freeing all the memory here that had been allocated in init()
*/
  // Free message content strings
  free(_messages[0].content);  // This is the pointer returned from init()
  free(_secret_key);

  // Delete message structs
  delete[] _messages;
};


bool ChatBox::init(const char* key, const char* model) {

  bool initSuccess = true;

  /* Steve Q2 *************************************************************************************
  I am trying to check for a NULL pointer returned by malloc, I think these expressions do the job
*/
  // Allocate space for API key and assign
  char* keyAlloc = (char*)malloc(API_KEY_SIZE * sizeof(char));

  if (keyAlloc) {
    _secret_key = keyAlloc;
    strcpy(_secret_key, key);
  } else {
    Serial.println("keyAlloc failed");
    initSuccess = false;
  }

  // Allocate space for model and assign
  char* modelAlloc = (char*)malloc(MODEL_NAME_SIZE * sizeof(char));

  if (modelAlloc) {
    _model = modelAlloc;
    strcpy(_model, model);
  } else {
    Serial.println("modelAlloc failed");
    initSuccess = false;
  }

  // Allocate space for message content
  char* contentAlloc = (char*)malloc(_maxMsgs * _MAX_MESSAGE_LENGTH * sizeof(char));

  // Assign segments of memory to message content strings
  if (contentAlloc) {

    for (int i = 0; i < _maxMsgs; i++) {
      _messages[i].content = contentAlloc + i * _MAX_MESSAGE_LENGTH * sizeof(char);
    }

  } else {
    initSuccess = false;
  }

  return initSuccess;
}

char* ChatBox::getLastMessageContent() const {

  if (_msgCount == 0) {
    // No message yet, do nothing.
    Serial.println("No message to get.");
    return nullptr;  // Steve Q99 - I think this is what I want to return in the case there are no messages
  } else {
    return _messages[(_msgCount - 1) % _maxMsgs].content;
  }
}

/* Steve Q6 *************************************************************************************
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

int ChatBox::getLastMessageLength() const {

  if (_msgCount == 0) {
    // No message yet, do nothing.
    Serial.println("No message to get.");
    return -1;
  } else {
    return _messages[(_msgCount - 1) % _maxMsgs].length;
  }
}

void ChatBox::safe_strncpy(char* dest, size_t destSize, const char* src) {
  size_t srcSize = strlen(src);  // crash here if not nul-terminated
  if (srcSize > destSize - 1)
    srcSize = destSize - 1;
  memmove(dest, src, srcSize);  // memmove is safe if dest and src overlap
  dest[srcSize] = '\0';
}

int ChatBox::putMessage(const char* msg, int msgLength, Roles msgRole) {

  safe_strncpy(_messages[(_msgCount % _maxMsgs)].content, _MAX_MESSAGE_LENGTH, msg);
  _messages[(_msgCount % _maxMsgs)].role = msgRole;
  _messages[(_msgCount % _maxMsgs)].length = msgLength;
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

getResponseCodes ChatBox::getResponse() {

  // Create a secure wifi client
  WiFiClientSecure client;
  client.setCACert(ROOT_CA_CERT);

  // Generate JSON Request body from messages array
  DynamicJsonDocument jsonRequestBody = generateJsonRequestBody();

  // Connect to OpenAI
  int conn = client.connect(OPEN_AI_SERVER, PORT);

  // If connection is successful, send JSON
  if (conn == 1) {
    // Send JSON Request body to OpenAI API endpoint URL
    postRequest(&jsonRequestBody, &client);

#ifdef DEBUG_SERVER_RESPONSE_BREAKING
    /* Seeing the headers of the server response can be extremely useful to troubleshooting
      connection errors.  However, this readout of the server response header breaks 
      how the message is parsed from the response.  So you'll be able to send and receive one message,
      but no more.  So make sure you only use this when debugging server response issues. */

    String line = client.readStringUntil('X');
    Serial.print(line);
#endif

    /* Steve Q10 *************************************************************************************
      The end user may have to wait for the server response, 
      I want them to be able to get an indication that they are waiting so they can do something
      else, but I am not sure how to handle this.

      For example, it would be nice if the end user could use
      
      while(!getResponse()){
        //do something
      }

      But my issue is, getReponse may take a couple seconds to return if it is waiting on the server
      and I want the user to be able to do something in those seconds.  
      
      I guess what I want is for getResponse to be working "in the background"? So it doesn't hold up the rest of the users program.
    */

    //  Wait for OpenAI response
    bool responseSuccess = waitForServerResponse(&client);

    // If you receive a response, parse the JSON and copy the response to messages[]
    if (responseSuccess) {

      bool responseSaved = putResponseInMsgArray(&client);

    } else {
      // Server did not responsd to POST request, go through loop and try again.
      Serial.println("    | Server did not respond. Trying again.");
      return serverDidNotRespond;
    }

  } else {
    // Failed to connect to server, go through loop and try again.
    Serial.println("    | Could not connect to server. Trying again.");
    return couldNotConnectToServer;
  }

  // Disconnect from server after response received, server timeout, or connection failure
  client.stop();

  return getResponseSuccess;
}

/* Function:  postRequest
 * -------------------------
 * Makes a POST request to OpenAI 
 *
 * DynamicJsonDocument * pJSONRequestBody: The JSON Request body to send with the POST
 * WiFiClientSecure * pClient: The wifi object handling the sending
 *
 * returns: void
 */
void ChatBox::postRequest(DynamicJsonDocument* pJsonRequestBody, WiFiClientSecure* pClient) {

  Serial.println("    | Making POST Request to OpenAI");
  // Make request
  pClient->print("POST ");
  pClient->print(OPEN_AI_END_POINT);
  pClient->println(" HTTP/1.1");
  // Send headers
  pClient->print("Host: ");
  pClient->println(OPEN_AI_SERVER);
  pClient->println("Content-Type: application/json");
  pClient->print("Content-Length: ");
  pClient->println(measureJson(*pJsonRequestBody));
  pClient->print("Authorization: Bearer ");
  pClient->println(_secret_key);
  pClient->println("Connection: Close");
  /* The empty println below inserts a stand-alone carriage return and newline (CRLF) 
      which is part of the HTTP protocol following sending headers and prior to sending the body. */
  pClient->println();
  serializeJson(*pJsonRequestBody, *pClient);  // Serialize the JSON doc and append to client object
  pClient->println();                          // Send the body to the server

  Serial.println("    | JSON sent");
}

/* Function:  waitForServerResponse
 * -------------------------
 * Holds program in loop while waiting for response from server.
 * Times out after defined interval.
 *
 * WiFiClientSecure * pClient: The wifi object handling the response
 *
 * returns: bool - 0 for timeout, 1 for success
 */
bool ChatBox::waitForServerResponse(WiFiClientSecure* pClient) {

  bool responseSuccess = true;
  long startWaitTime = millis();  // Measure how long it takes

  while (pClient->available() == 0) {
    /* If you've been waiting too long, perhaps something went wrong,
        break out and try again. */
    if (millis() - startWaitTime > SERVER_RESPONSE_WAIT_TIME) {
      Serial.println("    | SERVER_RESPONSE_WAIT_TIME exceeded.");
      return false;
    }
  }

  return responseSuccess;
}


/* Function:  putResponseInMsgArray
 * -------------------------
 * Applies filter to JSON reponse and saves response to messages array. 
 *
 * WiFiClientSecure * pClient: The wifi object handling the response
 * int numMessages:  Number of messages in the messages array
 *
 * returns: bool - 0 for failure to extract JSON, 1 for success
 */
bool ChatBox::putResponseInMsgArray(WiFiClientSecure* pClient) {

  Serial.println("    | putResponseInMsgArray");
  pClient->find("\r\n\r\n");  // This search gets us to the body section of the http response

  /* Create a filter for the returning JSON 
        https://arduinojson.org/news/2020/03/22/version-6-15-0/ */
  StaticJsonDocument<500> filter;
  JsonObject filter_choices_0_message = filter["choices"][0].createNestedObject("message");
  filter_choices_0_message["role"] = true;
  filter_choices_0_message["content"] = true;

  // Deserialize the JSON
  //TODO - I want capacity to be determined by the tokens the end user initilazes with.
  //But I can't figure out how to make this work.  JSON_OBJECT_SIZE() does not seem to size for what's will be stored in the JSON doc.
  //https://arduinojson.org/v6/how-to/determine-the-capacity-of-the-jsondocument/#technique-2-compute-the-capacity-with-macros
  //const int capacityTest = MAX_TOKENS * CHARS_PER_TOKEN; //-> this did not work either, was getting a stack overflow, i think trying to take up too much space
  const int capacity = 2000;
  StaticJsonDocument<capacity> jsonResponse;
  DeserializationError error = deserializeJson(jsonResponse, *pClient, DeserializationOption::Filter(filter));

  // If deserialization fails, exit immediately and try again.
  if (error) {

    pClient->stop();

    Serial.print("    | deserializeJson() failed->");
    Serial.println(error.c_str());

    return 0;
  }


  const char* newMsg = jsonResponse["choices"][0]["message"]["content"] | "...";
  Serial.println(newMsg);
  Serial.print("measureJSON ");
  Serial.print(measureJson(jsonResponse["choices"][0]["message"]["content"]));
  Serial.print("  | strlng ");
  Serial.println(strlen(jsonResponse["choices"][0]["message"]["content"]));
  // Benoît Q1
  // Why does measureJson return 2 more than strlen?
  putMessage(newMsg, measureJson(jsonResponse["choices"][0]["message"]["content"]) - 2, assistant);

  return 1;
}


}  // close namespace