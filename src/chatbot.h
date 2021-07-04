#ifndef CHATBOT_H_
#define CHATBOT_H_

#include <string>
#include <wx/bitmap.h>

class GraphNode; // forward declaration
class ChatLogic; // forward declaration

class ChatBot {
private:
  // data handles (owned)
  wxBitmap *_image; // avatar image

  // data handles (not owned)
  GraphNode *_current;
  GraphNode *_root;
  ChatLogic *_chatLogic;

  // proprietary functions
  int ComputeLevenshteinDistance(std::string s1, std::string s2);

public:
  // constructors / destructors
  ChatBot();                     // constructor WITHOUT memory allocation
  ChatBot(std::string filename); // constructor WITH memory allocation
  ~ChatBot();

  //// STUDENT CODE
  ////
  // copy constructor
  ChatBot(const ChatBot &other);
  // move constructor
  ChatBot(ChatBot &&other);
  // copy assignment
  ChatBot &operator=(const ChatBot &other);
  // move assignment
  ChatBot &operator=(ChatBot &&other);

  ////
  //// EOF STUDENT CODE

  // getters / setters
  void SetCurrnode(GraphNode *node);
  void SetRootNode(GraphNode *rootNode) { _root = rootNode; }
  void SetChatLogicHandle(ChatLogic *chatLogic) { _chatLogic = chatLogic; }
  wxBitmap *GetImageHandle() { return _image; }

  // communication
  void ReceiveMessageFromUser(std::string message);
};

#endif /* CHATBOT_H_ */