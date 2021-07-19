#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <tuple>
#include <vector>

#include "chatbot.h"
#include "chatlogic.h"
#include "graphedge.h"
#include "graphnode.h"

ChatLogic::ChatLogic() {
  //// STUDENT CODE
  ////

  // create instance of chatbot
  // _chatBot = new ChatBot("../images/chatbot.png");

  // add pointer to chatlogic so that chatbot answers can be passed on to the
  // GUI
  // _chatBot->SetChatLogicHandle(this);

  ////
  //// EOF STUDENT CODE
}

ChatLogic::~ChatLogic() {
  //// STUDENT CODE
  ////

  // delete chatbot instance
  // delete _chatBot;

  // delete all nodes
  // for (auto it = std::begin(_nodes); it != std::end(_nodes); ++it) {
  // delete *it;
  // }

  // delete all edges
  // for (auto it = std::begin(_edges); it != std::end(_edges); ++it) {
  // delete *it;
  // }

  ////
  //// EOF STUDENT CODE
}

template <typename T>
void ChatLogic::AddAllTokensToElement(std::string tokenID, tokenlist &tokens,
                                      T &element) {
  // find all occurences for current node
  auto token = tokens.begin();
  while (true) {
    token = std::find_if(
        token, tokens.end(),
        [&tokenID](const std::pair<std::string, std::string> &pair) {
          return pair.first == tokenID;
          ;
        });
    if (token != tokens.end()) {
      element.AddToken(token->second); // add new keyword to edge
      token++;                         // increment iterator to next element
    } else {
      break; // quit infinite while-loop
    }
  }
}

void ChatLogic::LoadAnswerGraphFromFile(std::string filename) {
  // load file with answer graph elements
  std::ifstream file(filename);

  // check for file availability and process it line by line
  if (file) {
    // loop over all lines in the file
    std::string lineStr;
    while (getline(file, lineStr)) {
      // extract all tokens from current line
      tokenlist tokens;
      while (lineStr.size() > 0) {
        // extract next token
        int posTokenFront = lineStr.find("<");
        int posTokenBack = lineStr.find(">");
        if (posTokenFront < 0 || posTokenBack < 0)
          break; // quit loop if no complete token has been found
        std::string tokenStr =
            lineStr.substr(posTokenFront + 1, posTokenBack - 1);

        // extract token type and info
        int posTokenInfo = tokenStr.find(":");
        if (posTokenInfo != std::string::npos) {
          std::string tokenType = tokenStr.substr(0, posTokenInfo);
          std::string tokenInfo =
              tokenStr.substr(posTokenInfo + 1, tokenStr.size() - 1);

          // add token to vector
          tokens.emplace_back(std::make_pair(tokenType, tokenInfo));
        }

        // remove token from current line
        lineStr = lineStr.substr(posTokenBack + 1, lineStr.size());
      }

      // process tokens for current line
      auto type =
          std::find_if(tokens.begin(), tokens.end(),
                       [](const std::pair<std::string, std::string> &pair) {
                         return pair.first == "TYPE";
                       });
      if (type != tokens.end()) {
        // check for id
        auto idToken =
            std::find_if(tokens.begin(), tokens.end(),
                         [](const std::pair<std::string, std::string> &pair) {
                           return pair.first == "ID";
                         });
        if (idToken != tokens.end()) {
          // extract id from token
          int id = std::stoi(idToken->second);

          // node-based processing
          if (type->second == "NODE") {
            //// STUDENT CODE
            ////

            // check if node with this ID exists already
            auto newNode = std::find_if(
                _nodes.begin(), _nodes.end(),
                // 람다 표현식이라는 방식.. 이름 없이 간략하게
                // 함수를 사용한다고 보면 댐 어렵네..
                //  &node인 이유 : node는 _node vector 안에
                // 있는 node 한 개 즉, node = unique pointer type
                // 이므로 함수 arg로 사용할 때 &node로 써야함
                // const 사용하는 이유 : ChatBot::SetCurrentNode에서
                // node가 currentnode로 카피 되는데 이때 node포인터가
                // 바뀔 수 있음 우리가 아래 사용할 node는 바뀌면 안되니까
                // const로 막아준다.
                //  [&id](GraphNode *node) { return node->GetID() == id; });
                [&id](const std::unique_ptr<GraphNode> &node) {
                  return node->GetID() == id;
                });

            // create new element if ID does not yet exist
            if (newNode == _nodes.end()) {
              _nodes.emplace_back(new GraphNode(id));
              newNode = _nodes.end() - 1; // get iterator to last element

              // add all answers to current node
              AddAllTokensToElement("ANSWER", tokens, **newNode);
            }

            ////
            //// EOF STUDENT CODE
          }

          // edge-based processing
          if (type->second == "EDGE") {
            //// STUDENT CODE
            ////

            // find tokens for incoming (parent) and outgoing (child) node
            auto parentToken = std::find_if(
                tokens.begin(), tokens.end(),
                [](const std::pair<std::string, std::string> &pair) {
                  return pair.first == "PARENT";
                });
            auto childToken = std::find_if(
                tokens.begin(), tokens.end(),
                [](const std::pair<std::string, std::string> &pair) {
                  return pair.first == "CHILD";
                });

            if (parentToken != tokens.end() && childToken != tokens.end()) {
              // get iterator on incoming and outgoing node via ID search
              auto parentNode = std::find_if(
                  _nodes.begin(), _nodes.end(),
                  // 람다 방식..이름 없이 간략하게 함수를
                  // 사용한다고 보면 댐 어렵네..
                  // &node인 이유 : node는 _node안에 있는
                  // const 사용하는 이유 : ChatBot::SetCurrentNode에서
                  // node가 currentnode로 카피 되는데 이때 node포인터가
                  // 바뀔 수 있음 우리가 아래 사용할 node는 바뀌면 안되니까
                  // const로 막아준다.
                  [&parentToken](const std::unique_ptr<GraphNode> &node) {
                    return node->GetID() == std::stoi(parentToken->second);
                  });
              auto childNode = std::find_if(
                  _nodes.begin(), _nodes.end(),
                  // 람다 방식..이름 없이 간략하게 함수를
                  // 사용한다고 보면 댐 어렵네..
                  // &node인 이유 : node는 _node안에 있는
                  // const 사용하는 이유 : ChatBot::SetCurrentNode에서
                  // node가 currentnode로 카피 되는데 이때 node포인터가
                  // 바뀔 수 있음 우리가 아래 사용할 node는 바뀌면 안되니까
                  // const로 막아준다.
                  [&childToken](const std::unique_ptr<GraphNode> &node) {
                    return node->GetID() == std::stoi(childToken->second);
                  });

              // create new edge
              // GraphEdge *edge = new GraphEdge(id);
              std::unique_ptr<GraphEdge> edge = std::make_unique<GraphEdge>(id);

              // get() is return raw pointer
              edge->SetChildNode((*childNode).get());
              edge->SetParentNode((*parentNode).get());
              // _edges = ~ChatLogic() 에서 edge들 deallocation 할 때 사용하려고
              // 만든거 즉 deallocation smart pointer가 알아서 하니까 얘는 안씀
              // _edges.emplace_back(*edge);

              // find all keywords for current node
              AddAllTokensToElement("KEYWORD", tokens, *edge);

              // (*childNode)->AddEdgeToParentNode(edge.get());
              // GraphEdge *edge = _currentNode->GetChildEdgeAtIndex(i); 여기
              // 때문에 AddEdgeToParentNode 에서는 smart_pointer사용 못 함
              (*childNode)->AddEdgeToParentNode(edge.get());
              (*parentNode)->AddEdgeToChildNode(std::move(edge));
            }

            ////
            //// EOF STUDENT CODE
          }
        } else {
          std::cout << "Error: ID missing. Line is ignored!" << std::endl;
        }
      }
    } // eof loop over all lines in the file

    file.close();

  } // eof check for file availability
  else {
    std::cout << "File could not be opened!" << std::endl;
    return;
  }

  //// STUDENT CODE
  ////

  // identify root node
  GraphNode *rootNode = nullptr;
  for (auto it = std::begin(_nodes); it != std::end(_nodes); ++it) {
    // search for nodes which have no incoming edges
    // get() is return raw pointer
    if ((*it)->GetNumberOfParents() == 0) {
      if (rootNode == nullptr) {
        // rootNode = *it; // assign current node to root
        // rootNode = it->get();   // 포인터 접근법 1. ->func()
        rootNode = (*it).get(); // 포인터 접근법 2. (*pt).func()
      } else {
        std::cout << "ERROR : Multiple root nodes detected" << std::endl;
      }
    }
  }

  // kjs
  // create local chatbot instance in stack (not heap)
  ChatBot _localchatBot = ChatBot("../images/chatbot.png");

  _localchatBot.SetChatLogicHandle(this);

  // add chatbot to graph root node
  _localchatBot.SetRootNode(rootNode);

  _chatBot = &_localchatBot;

  // 여기에 std::move안하니까 move constructor가 안불리네...
  rootNode->MoveChatbotHere(std::move(_localchatBot));

  ////
  //// EOF STUDENT CODE
}

void ChatLogic::SetPanelDialogHandle(ChatBotPanelDialog *panelDialog) {
  _panelDialog = panelDialog;
}

void ChatLogic::SetChatbotHandle(ChatBot *chatbot) { _chatBot = chatbot; }

void ChatLogic::SendMessageToChatbot(std::string message) {
  _chatBot->ReceiveMessageFromUser(message);
}

void ChatLogic::SendMessageToUser(std::string message) {
  _panelDialog->PrintChatbotResponse(message);
}

wxBitmap *ChatLogic::GetImageFromChatbot() {
  return _chatBot->GetImageHandle();
}
