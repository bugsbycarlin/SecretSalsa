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
  string getCurrentConversation();
  
  void up();
  void down();
  void accept();

  void draw();

  ~Conversation();

  bool finished;
  string finish_value;

 private:
  typedef struct ConversationElementTD {
    string type;
    string name;
    string text;
    vector<ConversationElementTD*> children;
    ConversationElementTD* parent;
  } ConversationElement;

  void setMenus();

  void checkAndAdd(
    string line,
    int line_count,
    int last_indentation,
    int new_indentation,
    ConversationElement* new_element,
    ConversationElement* last_element
  );

  void walk(ConversationElement* element, int indentation);
  void next();

  int choice_value;

  map<string, ConversationElement*> conversation_trees;

  string current_conversation_name;
  ConversationElement* current_conversation_root;
  ConversationElement* current_element;
  stack<int> conversation_position;

  map<string, string> character_images;
  map<string, string> character_speech;

  State* state;

  Menu* character_box;
  Menu* conversation_box;
  Menu* choice_box;
};