#include "chat.h"

namespace ChatGPTuino {

ChatBox::ChatBox(int maxTokens, int numMsgs)
  {

  _maxTokens = maxTokens >= 0 ? maxTokens : MIN_TOKENS;
    
  };

// bool valid_chatBox(int maxTokens, int numMsgs){
//   if (maxTokens < 0){

//   } return false;
// }

}// close namespace