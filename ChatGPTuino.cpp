#include "ChatGPTuino.h"

// Constructor
ChatGPTuino::ChatGPTuino(uint32_t maxTokens = MIN_TOKENS, const uint16_t maxMsgs = MIN_MESSAGES)
    : _maxTokens{maxTokens > MIN_TOKENS ? maxTokens : MIN_TOKENS},
      _maxMsgs{maxMsgs > MIN_MESSAGES ? maxMsgs : (uint16_t)MIN_MESSAGES},
      _msgCount{0},
      _sysMsgMode{Default},
      _MAX_MESSAGE_LENGTH{_maxTokens * CHARS_PER_TOKEN},
      _JSON_DOC_SIZE{
          (JSON_DATA_STRUCTURE_MEMORY_BASE + (_maxMsgs * JSON_DATA_STRUCTURE_MEMORY_PER_MSG)) + (JSON_KEY_STRING_MEMORY_BASE + ((_MAX_MESSAGE_LENGTH + JSON_VALUE_STRING_MEMORY_PER_MSG) * _maxMsgs)) + JSON_MEMORY_SLACK} {};

// Destructor
ChatGPTuino::~ChatGPTuino()
{

  free(_messages[0].content); // Free message content strings
  free(_secret_key);
  free(_model);
  free(_sysMessageContent);

  delete[] _messages; // Delete message structs
};

bool ChatGPTuino::init(const char *key, const char *model)
{

  bool initSuccess = true;

  // Allocate space for API key and assign
  char *keyAlloc = (char *)malloc((strlen(key) + 1) * sizeof(char));

  if (keyAlloc)
  {
    _secret_key = keyAlloc;
    strcpy(_secret_key, key);
  }
  else
  {
    Serial.println("keyAlloc failed");
    initSuccess = false;
  }

  // Allocate space for model and assign
  char *modelAlloc = (char *)malloc((strlen(model) + 1) * sizeof(char));

  if (modelAlloc)
  {
    _model = modelAlloc;
    strcpy(_model, model);
  }
  else
  {
    Serial.println("modelAlloc failed");
    initSuccess = false;
  }

  // Allocate space for message structs
  Message *messagesAlloc = new Message[_maxMsgs];

  if (messagesAlloc)
  {
    _messages = messagesAlloc;
  }
  else
  {
    Serial.println("messageAlloc failed");
    initSuccess = false;
  }

  // Allocate space for message content
  char *contentAlloc = (char *)malloc(_maxMsgs * _MAX_MESSAGE_LENGTH * sizeof(char));

  // Assign segments of memory to message content strings
  if (contentAlloc)
  {

    for (int i = 0; i < _maxMsgs; i++)
    {
      _messages[i].content = contentAlloc + i * _MAX_MESSAGE_LENGTH * sizeof(char);
    }
  }
  else
  {
    Serial.println("contentAlloc failed");
    initSuccess = false;
  }

  // Allocate space for system message content
  char *sysMsgContentAlloc = (char *)malloc(_MAX_MESSAGE_LENGTH * sizeof(char));

  // Assign segments of memory to message content strings
  if (sysMsgContentAlloc)
  {
    _sysMessageContent = sysMsgContentAlloc;
  }
  else
  {
    Serial.println("sysMsgContentAlloc failed");
    initSuccess = false;
  }

  return initSuccess;
}

// Set system message mode and optionally system message
void ChatGPTuino::systemMessageMode(SysMessageModes mode, char *sysMsg)
{

#ifdef VERBOSE_PRINTS
  Serial.println("    | systemMessageMode | START");
#endif

  _sysMsgMode = mode;

  if (sysMsg)
  {
    safe_strncpy(_sysMessageContent, _MAX_MESSAGE_LENGTH, sysMsg);
#ifdef VERBOSE_PRINTS
    Serial.println("    | systemMessageMode | _sysMessageContent ->");
    Serial.println(_sysMessageContent);
#endif
  }

#ifdef VERBOSE_PRINTS
  Serial.println("    | systemMessageMode | END");
#endif
}

char *ChatGPTuino::getLastMessageContent() const
{

  if (_msgCount == 0)
  {
    // No message yet, do nothing.
    Serial.println("No message to get.");
    return nullptr;
  }
  else
  {
    return _messages[(_msgCount - 1) % _maxMsgs].content;
  }
}

Roles ChatGPTuino::getLastMessageRole() const
{

  if (_msgCount == 0)
  {
    Roles noMessage = None;
    Serial.println("No message to get.");
    return noMessage;
  }
  else
  {
    return _messages[(_msgCount - 1) % _maxMsgs].role;
  }
}

uint32_t ChatGPTuino::getLastMessageLength() const
{

  if (_msgCount == 0)
  {
    // No message yet, do nothing.
    Serial.println("No message to get.");
    return -1;
  }
  else
  {
    return _messages[(_msgCount - 1) % _maxMsgs].length;
  }
}

void ChatGPTuino::safe_strncpy(char *dest, size_t destSize, const char *src)
{
  size_t srcSize = strlen(src); // crash here if not nul-terminated
  Serial.println("safe_strncpy - strlen success");
  if (srcSize > destSize - 1)
  {
    srcSize = destSize - 1;
  }
  memmove(dest, src, srcSize); // memmove is safe if dest and src overlap
  dest[srcSize] = '\0';
}

uint32_t ChatGPTuino::putMessage(const char *msg, uint32_t msgLength, Roles msgRole)
{

  safe_strncpy(_messages[(_msgCount % _maxMsgs)].content, _MAX_MESSAGE_LENGTH, msg);
  Serial.println("putMessage - safe_strncpy success");
  _messages[(_msgCount % _maxMsgs)].role = msgRole;
  _messages[(_msgCount % _maxMsgs)].length = msgLength;
  _msgCount++;

  return _msgCount;
}

JsonDocument ChatGPTuino::generateJsonRequestBody()
{

  JsonDocument doc;

  doc["model"] = _model;
  doc["max_tokens"] = _maxTokens;

  // Create nested array that will hold all the system, user, and assistant messages
  // JsonArray messagesJSON = doc.createNestedArray("messages");
  JsonArray messagesJSON = doc["messages"].to<JsonArray>();

  int oldestMsgIdx = 0;

  if (_msgCount >= _maxMsgs)
  {
    oldestMsgIdx = _msgCount % _maxMsgs;
  }

  for (int i = 0; i < _msgCount && i < _maxMsgs; i++)
  {

    // System Message Inject Mode - inject system message at end
    if (_sysMsgMode == Insert && (i == _msgCount - 1 || i == _maxMsgs - 1))
    {
      Serial.println("inject system message at end");
      // messagesJSON[i]["role"] = Sys;
      messagesJSON[i]["role"] = RoleNames[Sys];
      messagesJSON[i]["content"] = _sysMessageContent;
      i++;
    }

    messagesJSON[i]["role"] = RoleNames[_messages[oldestMsgIdx].role];
    messagesJSON[i]["content"] = _messages[oldestMsgIdx].content;

    oldestMsgIdx++;
    oldestMsgIdx %= _maxMsgs;
  }
  serializeJson(doc, Serial);
  return doc;
}

GetResponseCodes ChatGPTuino::getResponse()
{

// Create a secure wifi client
  SecureClient client;
#ifndef ARDUINO_CORE_API
  client.setCACert(ROOT_CA_CERT);
#endif

  // Generate JSON Request body from messages array
  JsonDocument jsonRequestBody = generateJsonRequestBody();
  Serial.println("getResponse - generateJsonRequestBody success");

  // Connect to OpenAI
  int conn = client.connect(OPEN_AI_SERVER, PORT);

  // If connection is successful, send JSON
  if (conn == 1)
  {
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

    //  Wait for OpenAI response
    bool responseSuccess = waitForServerResponse(&client);

    // If you receive a response, parse the JSON and copy the response to messages[]
    if (responseSuccess)
    {

      bool responseSaved = putResponseInMsgArray(&client);
      Serial.println("getResponse  putResponseInMsgArray success");
    }
    else
    {
      // Server did not respond to POST request, go through loop and try again.
      Serial.println("    | Server did not respond. Trying again.");
      return ServerDidNotRespond;
    }
  }
  else
  {
    // Failed to connect to server, go through loop and try again.
    Serial.println("    | Could not connect to server. Trying again.");
    return CouldNotConnectToServer;
  }

  // Disconnect from server after response received, server timeout, or connection failure
  client.stop();

  return GetResponseSuccess;
}

/* Function:  postRequest
 * -------------------------
 * Sends a POST request using a WiFiClientSecure object (ESP32).
 *
 * @param JsonDocument * pJSONRequestBody: The JSON Request body to send with the POST
 * @param SecureClient * pClient: The wifi object handling the sending
 *
 * returns: void
 */
void ChatGPTuino::postRequest(JsonDocument *pJsonRequestBody, SecureClient *pClient)
{

#ifdef VERBOSE_PRINTS
  Serial.println("    | Making POST Request to OpenAI");
#endif

  // Make request
  pClient->print("POST ");
  pClient->print(OPEN_AI_END_POINT);
  pClient->println(" HTTP/1.0"); // Can not seem to configure this for 1.1
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
  serializeJson(*pJsonRequestBody, *pClient); // Serialize the JSON doc and append to client object
  pClient->println();                         // Send the body to the server

#ifdef VERBOSE_PRINTS
  Serial.println("    | JSON sent");
#endif
}

/* Function:  waitForServerResponse
 * -------------------------
 * Holds program in loop while waiting for response from server.
 * Times out after defined interval.
 *
 * SecureClient * pClient: The wifi object handling the response
 *
 * returns: bool - 0 for timeout, 1 for success
 */
bool ChatGPTuino::waitForServerResponse(SecureClient *pClient)
{

  bool responseSuccess = true;
  long startWaitTime = millis();        // Measure how long it takes
  long displayWaitTime = startWaitTime; // Display a "." to indicate waiting
  const long displayWaitInterval = 500;

  while (pClient->available() == 0)
  {

    // Give visual indication of waiting
    if (millis() - displayWaitTime > displayWaitInterval)
    {
      Serial.print("...");
      displayWaitTime = millis();
    }

    /* If you've been waiting too long, perhaps something went wrong,
        break out and try again. */
    if (millis() - startWaitTime > SERVER_RESPONSE_WAIT_TIME)
    {
      Serial.println("    | SERVER_RESPONSE_WAIT_TIME exceeded.");
      return false;
    }
  }

  Serial.println("");
  return responseSuccess;
}

/* Function:  putResponseInMsgArray
 * -------------------------
 * Applies filter to JSON response and saves response to messages array.
 *
 * @param SecureClient * pClient: The wifi object handling the response
 * @param int numMessages:  Number of messages in the messages array
 *
 * returns: bool - 0 for failure to extract JSON, 1 for success
 */
bool ChatGPTuino::putResponseInMsgArray(SecureClient *pClient)
{

#ifdef VERBOSE_PRINTS
  Serial.println("    | putResponseInMsgArray");
#endif

  pClient->find("\r\n\r\n"); // This search gets us to the body section of the http response

  /* Create a filter for the returning JSON
        https://arduinojson.org/news/2020/03/22/version-6-15-0/ */
  // StaticJsonDocument<500> filter;  // JSON 7 UPDATE
  JsonDocument filter;
  // JsonObject filter_choices_0_message = filter["choices"][0].createNestedObject("message"); // JSON 7 UPDATE
  JsonObject filter_choices_0_message = filter["choices"][0]["message"].to<JsonObject>();

  filter_choices_0_message["role"] = true;
  filter_choices_0_message["content"] = true;

  // Deserialize the JSON
// #ifdef VERBOSE_PRINTS
#if defined(VERBOSE_PRINTS) && !defined(ARDUINO_CORE_API)
  Serial.print("    | putResponseInMsgArray | ESP.getMaxAllocHeap -> ");
  Serial.println(ESP.getMaxAllocHeap());
#endif

  // JsonDocument jsonResponse(ESP.getMaxAllocHeap() - 1024);
  JsonDocument jsonResponse;
  DeserializationError error = deserializeJson(jsonResponse, *pClient, DeserializationOption::Filter(filter));
  jsonResponse.shrinkToFit();
  Serial.println("    | putResponseInMsgArray | deserialize success");

  // If deserialization fails, exit immediately and try again.
  if (error)
  {

    pClient->stop();

    Serial.print("    | putResponseInMsgArray | deserializeJson() failed->");
    Serial.println(error.c_str());

    return 0;
  }

  const char *newMsg = jsonResponse["choices"][0]["message"]["content"] | "...";

#ifdef VERBOSE_PRINTS
  Serial.print("    | putResponseInMsgArray | newMsg -> ");
  Serial.println(newMsg);
  Serial.print("    | putResponseInMsgArray | measureJSON ");
  Serial.print(measureJson(jsonResponse["choices"][0]["message"]["content"]));
  Serial.print("  | strlen ");
  Serial.println(strlen(jsonResponse["choices"][0]["message"]["content"]));
#endif

  putMessage(newMsg, strlen(jsonResponse["choices"][0]["message"]["content"]), Assistant);
  Serial.println("    | putResponseInMsgArray | putMessage success");
  return 1;
}