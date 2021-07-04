#include <algorithm>
#include <ctime>
#include <iostream>
#include <random>

#include "chatbot.h"
#include "chatlogic.h"
#include "graphedge.h"
#include "graphnode.h"

// constructor WITHOUT memory allocation
ChatBot::ChatBot() {
  // invalidate data handles
  _image = nullptr;
  _chatLogic = nullptr;
  _root = nullptr;
}

// constructor WITH memory allocation
ChatBot::ChatBot(std::string filename) {
  std::cout << "ChatBot Constructor" << std::endl;

  // invalidate data handles
  _chatLogic = nullptr;
  _root = nullptr;

  // load image into heap memory
  _image = new wxBitmap(filename, wxBITMAP_TYPE_PNG);
}

ChatBot::~ChatBot() {
  std::cout << "ChatBot Destructor" << std::endl;

  // deallocate heap memory
  if (_image != NULL) // Attention: wxWidgets used NULL and not nullptr
  {
    delete _image;
    _image = NULL;
  }
}

//// STUDENT CODE
//// copy constructor (shallow copy)
ChatBot::ChatBot(const ChatBot &other) {
  std::cout << "ChatBot copy constructor!"
            << "\n";

  _image = other._image;

  // rootNode is also smart pointrt
  _root = other._root;

  // chatLogic is std::unique_ptr
  _chatLogic = other._chatLogic;
}

ChatBot::ChatBot(ChatBot &&other) {
  std::cout << "ChatBot move constructor!"
            << "\n";

  _image = other._image;
  _root = other._root;
  _chatLogic = other._chatLogic;
  _chatLogic->SetChatbotHandle(this);
  // unique_ptr does not need free this ?? why should we null this smart ptr
  // explictly?

  // move constructor, you have to void the source members
  other._image = NULL;
  other._root = nullptr;
  other._chatLogic = nullptr;
}

ChatBot &ChatBot::operator=(const ChatBot &other) {
  std::cout << "ChatBot copy assignment!"
            << "\n";
  if (&other == this) {
    return *this;
  }
  _image = other._image;
  _root = other._root;
  _chatLogic = other._chatLogic;
  return *this;
}

ChatBot &ChatBot::operator=(ChatBot &&other) {
  std::cout << "ChatBot move assignment!"
            << "\n";
  if (&other == this) {
    return *this;
  }
  if (!_image)
    delete _image;
  // transfering
  _image = other._image;
  _root = other._root;
  _chatLogic = other._chatLogic;

  _chatLogic->SetChatbotHandle(this);
  // voiding
  other._image = NULL; // nullptr
  other._root = nullptr;
  other._chatLogic = nullptr;
}
////
//// EOF STUDENT CODE

void ChatBot::ReceiveMessageFromUser(std::string message) {
  // loop over all edges and keywords and compute Levenshtein distance to query
  typedef std::pair<GraphEdge *, int> EdgeDist;
  std::vector<EdgeDist> levDists; // format is <ptr,levDist>

  for (size_t i = 0; i < _current->GetNumberOfChildEdges(); ++i) {
    GraphEdge *edge = _current->GetChildEdgeAtIndex(i);
    for (auto keyword : edge->GetKeywords()) {
      EdgeDist ed{edge, ComputeLevenshteinDistance(keyword, message)};
      levDists.push_back(ed);
    }
  }

  // select best fitting edge to proceed along
  GraphNode *newNode;
  if (levDists.size() > 0) {
    // sort in ascending order of Levenshtein distance (best fit is at the top)
    std::sort(levDists.begin(), levDists.end(),
              [](const EdgeDist &a, const EdgeDist &b) {
                return a.second < b.second;
              });
    newNode = levDists.at(0).first->GetChildNode(); // after sorting the best
                                                    // edge is at first position
  } else {
    // go back to root node
    newNode = _root;
  }

  // tell current node to move chatbot to new node
  _current->MoveChatbotToNewNode(newNode);
}

void ChatBot::SetCurrnode(GraphNode *node) {
  // update pointer to current node
  _current = node;

  // select a random node answer (if several answers should exist)
  std::vector<std::string> answers = _current->GetAnswers();
  std::mt19937 generator(int(std::time(0)));
  std::uniform_int_distribution<int> dis(0, answers.size() - 1);
  std::string answer = answers.at(dis(generator));

  // send selected node answer to user
  _chatLogic->SendMessageToUser(answer);
}

int ChatBot::ComputeLevenshteinDistance(std::string s1, std::string s2) {
  // convert both strings to upper-case before comparing
  std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
  std::transform(s2.begin(), s2.end(), s2.begin(), ::toupper);

  // compute Levenshtein distance measure between both strings
  const size_t m(s1.size());
  const size_t n(s2.size());

  if (m == 0)
    return n;
  if (n == 0)
    return m;

  size_t *costs = new size_t[n + 1];

  for (size_t k = 0; k <= n; k++)
    costs[k] = k;

  size_t i = 0;
  for (std::string::const_iterator it1 = s1.begin(); it1 != s1.end();
       ++it1, ++i) {
    costs[0] = i + 1;
    size_t corner = i;

    size_t j = 0;
    for (std::string::const_iterator it2 = s2.begin(); it2 != s2.end();
         ++it2, ++j) {
      size_t upper = costs[j + 1];
      if (*it1 == *it2) {
        costs[j + 1] = corner;
      } else {
        size_t t(upper < corner ? upper : corner);
        costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
      }

      corner = upper;
    }
  }

  int result = costs[n];
  delete[] costs;

  return result;
}