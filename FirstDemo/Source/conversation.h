/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <array>
#include <fstream>
#include <regex>
#include <stack>
#include <string>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "honey.h"

#include "state.h"

using namespace Honey;
using namespace std;

class Conversation {
 public:
  Conversation(State* state);

  void loadConversations(string conversation_filepath);
  void setCurrentConversation(string conversation_name);
  
  void up();
  void down();
  void accept();

  bool finished();

  void draw();

  ~Conversation();

 private:
  typedef struct ConversationElementTD {
    string type;
    string name;
    string text;
    string action;
    int num_choices;
    vector<ConversationElementTD*> children;
    vector<ConversationElementTD*> choices;
  } ConversationElement;

  void setMenus();
  void handleIndentation(int new_indentation, int line_count);

  map<string, vector<ConversationElement*>> conversations;
  vector<ConversationElement*> current_conversation;
  ConversationElement* current_element;
  stack<vector<ConversationElement*>*> sub_conversation;
  stack<int> conversation_position;
  int choice_value;

  map<string, string> character_images;

  int current_indentation;

  State* state;

  Menu* character_box;
  Menu* conversation_box;
  Menu* choice_box;
};