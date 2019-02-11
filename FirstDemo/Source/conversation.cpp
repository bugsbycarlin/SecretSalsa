/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "conversation.h"

using namespace std;
using namespace Honey;

Conversation::Conversation(State* state) {
  this->state = state;

  character_box = new Menu(
    hot_config.getInt("menu", "character_x"),
    hot_config.getInt("menu", "character_y"),
    hot_config.getInt("menu", "character_width"),
    hot_config.getInt("menu", "character_height"),
    "Art/",
    hot_config.getString("menu", "character_image_root"),
    hot_config.getInt("menu", "character_margin_x"),
    hot_config.getInt("menu", "character_margin_y"),
    hot_config.getInt("menu", "character_num_lines"),
    hot_config.getInt("menu", "character_wrap_length"),
    hot_config.getBool("menu", "character_typewriter"),
    hot_config.getFloat("menu", "character_typewriter_delay"),
    hot_config.getString("menu", "character_font_path"),
    hot_config.getInt("menu", "character_font_size"),
    hot_config.getString("menu", "character_font_color")
  );

  conversation_box = new Menu(
    hot_config.getInt("menu", "conversation_x"),
    hot_config.getInt("menu", "conversation_y"),
    hot_config.getInt("menu", "conversation_width"),
    hot_config.getInt("menu", "conversation_height"),
    "Art/",
    hot_config.getString("menu", "conversation_image_root"),
    hot_config.getInt("menu", "conversation_margin_x"),
    hot_config.getInt("menu", "conversation_margin_y"),
    hot_config.getInt("menu", "conversation_num_lines"),
    hot_config.getInt("menu", "conversation_wrap_length"),
    hot_config.getBool("menu", "conversation_typewriter"),
    hot_config.getFloat("menu", "conversation_typewriter_delay"),
    hot_config.getString("menu", "conversation_font_path"),
    hot_config.getInt("menu", "conversation_font_size"),
    hot_config.getString("menu", "conversation_font_color")
  );

  choice_box = new Menu(
    hot_config.getInt("menu", "choice_x"),
    hot_config.getInt("menu", "choice_y"),
    hot_config.getInt("menu", "choice_width"),
    hot_config.getInt("menu", "choice_height"),
    "Art/",
    hot_config.getString("menu", "choice_image_root"),
    hot_config.getInt("menu", "choice_margin_x"),
    hot_config.getInt("menu", "choice_margin_y"),
    hot_config.getInt("menu", "choice_num_lines"),
    hot_config.getInt("menu", "choice_wrap_length"),
    hot_config.getBool("menu", "choice_typewriter"),
    hot_config.getFloat("menu", "choice_typewriter_delay"),
    hot_config.getString("menu", "choice_font_path"),
    hot_config.getInt("menu", "choice_font_size"),
    hot_config.getString("menu", "choice_font_color")
  );

  finished = false;
  finish_value = "";
}

void Conversation::loadConversations(string conversation_filepath) {
  std::smatch matches;
  regex r_character_image("_character_image ([^,]+), ([A-Za-z0-9_]*)");
  regex r_character_speech("_character_speech ([^,]+), ([A-Za-z0-9_]*)");
  regex r_conversation("_conversation ([^\n]*)");
  regex r_speech("([ ]*)_speech ([^:]+): ([^\n]*)");
  regex r_choice("([ ]*)_choice ([^:]+): ([^\n]*)");
  regex r_choice_value("([ ]*)_choice_value ([^\n]*)");
  regex r_action("([ ]*)_action ([^\n]*)");
  regex r_next("([ ]*)_next ([^\n]*)");
  regex r_finish("([ ]*)_finish ([^\n]*)");

  std::ifstream input_file(conversation_filepath);

  string load_current_convo = "";

  int last_indentation = 0;
  ConversationElement* last_element;

  string line;
  int line_count = 0;
  while (getline(input_file, line)) {
    if (regex_match(line, matches, r_character_image)) {
      character_images[matches[1].str()] = matches[2].str();
    } if (regex_match(line, matches, r_character_speech)) {
      character_speech[matches[1].str()] = matches[2].str();
    } else if (regex_match(line, matches, r_conversation)) {
      load_current_convo = matches[1].str();
      ConversationElement* element = new ConversationElement();
      element->type = "_root";
      element->text = load_current_convo;
      element->parent = NULL;
      conversation_trees[load_current_convo] = element;
      last_indentation = 0;
      last_element = element;
    } else if (regex_match(line, matches, r_speech)) {
      ConversationElement* new_element = new ConversationElement();
      new_element->type = "_speech";
      new_element->name = matches[2].str();
      new_element->text = matches[3].str();
      int new_indentation = matches[1].str().size();
      checkAndAdd(line, line_count, last_indentation, new_indentation, new_element, last_element);
      last_element = new_element;
      last_indentation = new_indentation;
    } else if (regex_match(line, matches, r_choice)) {
      ConversationElement* new_element = new ConversationElement();
      new_element->type = "_choice";
      new_element->name = matches[2].str();
      new_element->text = matches[3].str();
      int new_indentation = matches[1].str().size();
      checkAndAdd(line, line_count, last_indentation, new_indentation, new_element, last_element);
      last_element = new_element;
      last_indentation = new_indentation;
    } else if (regex_match(line, matches, r_choice_value)) {
      ConversationElement* new_element = new ConversationElement();
      new_element->type = "_choice_value";
      new_element->text = matches[2].str();
      int new_indentation = matches[1].str().size();
      // if (new_indentation != last_indentation + 2) {
      //   printf("Syntax error on line %d! _choice_value must be 2 spaces in from previous _choice\n", line_count);
      //   printf("%s\n", line.c_str());
      //   exit(1);
      // }
      checkAndAdd(line, line_count, last_indentation, new_indentation, new_element, last_element);
      last_element = new_element;
      last_indentation = new_indentation;
    } else if (regex_match(line, matches, r_action)) {
      ConversationElement* new_element = new ConversationElement();
      new_element->type = "_action";
      new_element->text = matches[2].str();
      int new_indentation = matches[1].str().size();
      checkAndAdd(line, line_count, last_indentation, new_indentation, new_element, last_element);
      last_element = new_element;
      last_indentation = new_indentation;
    } else if (regex_match(line, matches, r_next)) {
      ConversationElement* new_element = new ConversationElement();
      new_element->type = "_next";
      new_element->text = matches[2].str();
      int new_indentation = matches[1].str().size();
      checkAndAdd(line, line_count, last_indentation, new_indentation, new_element, last_element);
      last_element = new_element;
      last_indentation = new_indentation;
    } else if (regex_match(line, matches, r_finish)) {
      ConversationElement* new_element = new ConversationElement();
      new_element->type = "_finish";
      new_element->text = matches[2].str();
      int new_indentation = matches[1].str().size();
      checkAndAdd(line, line_count, last_indentation, new_indentation, new_element, last_element);
      last_element = new_element;
      last_indentation = new_indentation;
    } 
    line_count++;
  }

  printf("Here I am, dawg\n");
  for (pair<string, ConversationElement*> convo : conversation_trees) {
    string key = (string) convo.first;
    printf("The conversation in question is called %s\n", key.c_str());

    ConversationElement* element = convo.second;
    walk(element, 0);
  }
}

void Conversation::walk(ConversationElement* element, int indentation) {
  string indentation_string(indentation, ' ');
  printf("%s%s: %s\n", indentation_string.c_str(), element->type.c_str(), element->text.c_str());
  for (ConversationElement* child : element->children) {
    walk(child, indentation + 2);
  }
}

void Conversation::checkAndAdd(
  string line,
  int line_count,
  int last_indentation,
  int new_indentation,
  ConversationElement* new_element,
  ConversationElement* last_element) {
  
  if (new_indentation < 2) {
    printf("Syntax error on line %d! Expected element to have indentation at least 2 spaces\n", line_count);
    printf("%s\n", line.c_str());
    exit(1);
  } else if (abs(new_indentation - last_indentation) > 2) {
    printf("Syntax error on line %d! Expected indentation within 2 spaces of previous indentation\n", line_count);
    printf("%s\n", line.c_str());
    exit(1);
  } else if (new_indentation % 2 != 0) {
    printf("Syntax error on line %d! Expected an even indentation value\n", line_count);
    printf("%s\n", line.c_str());
    exit(1);
  }

  if (new_indentation == last_indentation) {
    new_element->parent = last_element->parent;
    last_element->parent->children.push_back(new_element);
  } else if (new_indentation > last_indentation) {
    new_element->parent = last_element;
    last_element->children.push_back(new_element);
  } else if (new_indentation < last_indentation) {
    // assume the new indentation is equal to the parent level. might be a bad assumption.
    new_element->parent = last_element->parent->parent;
    last_element->parent->parent->children.push_back(new_element);
  }
}

void Conversation::setCurrentConversation(string conversation_name) {
  printf("Setting conversation to %s\n", conversation_name.c_str());
  if (conversation_trees.count(conversation_name) != 1) {
    printf("Error! Couldn't find conversation %s\n", conversation_name.c_str());
    exit(1);
  }

  current_conversation_name = conversation_name;
  current_conversation_root = conversation_trees[conversation_name];
  current_element = current_conversation_root->children[0];
  
  conversation_position.empty();
  conversation_position.push(0);

  choice_value = 0;

  finished = false;
  finish_value = "";

  setMenus();
}

string Conversation::getCurrentConversation() {
  return current_conversation_name;
}

void Conversation::up() {
  if (!conversation_box->typewriterFinished()) {
    conversation_box->startTypewriter();
  } else if (current_element->type == "_choice") {
    sound.playSound("select_sound", 1);
    choice_value -= 1;
    if (choice_value < 0) {
      choice_value = current_element->children.size() - 1;
    }
  }
}

void Conversation::down() {
  if (!conversation_box->typewriterFinished()) {
    conversation_box->finishTypewriter();
  } else if (current_element->type == "_choice") {
    sound.playSound("select_sound", 1);
    choice_value += 1;
    if (choice_value > current_element->children.size() - 1) {
      choice_value = 0;
    }
  }
}

void Conversation::accept() {
  if (!conversation_box->typewriterFinished()) {
    conversation_box->finishTypewriter();
    return;
  }

  if (finished) {
    return;
  }

  if (current_element->type == "_choice") {
    sound.playSound("accept_sound", 1);
  }

  next();
}

void Conversation::next() {
  printf("Called next. I am in node %s %s\n", current_element->type.c_str(), current_element->text.c_str());

  if (current_element->type == "_choice") {
    current_element = current_element->children[choice_value]->children[0];
    conversation_position.push(0);
    conversation_position.push(0);
  } else if (current_element->children.size() > 0) {
    // If there is a child conversation, start walking them.
    printf("Moving to children of element.\n");
    conversation_position.push(0);
    current_element = current_element->children[0];
  } else if (current_element->parent->children.size() > conversation_position.top()) {
    // Otherwise, if there are still siblings, walk them
    printf("Moving to next sibling.\n");
    conversation_position.top() += 1;
    current_element = current_element->parent->children[conversation_position.top()];
  } else if (conversation_position.top() >= current_element->parent->children.size()) {
    if (current_element->parent->type != "_choice_value") {
      // Otherwise, pop this element and go back to the parent, and go next from that.
      current_element = current_element->parent;
      conversation_position.pop();
      conversation_position.top() += 1;
      next();
    } else {
      // Go past the choice
      current_element = current_element->parent->parent;
      conversation_position.pop();
      conversation_position.pop();
      conversation_position.top() += 1;
      next();
    }
  }

  printf("Now I am in node %s %s\n", current_element->type.c_str(), current_element->text.c_str());

  if (current_element->type == "_action") {
    printf("Action is %s\n", current_element->text.c_str());
    std::smatch matches;
    regex r_action_parts("([^ ]+) = ([^ ]+)");
    if (regex_match(current_element->text, matches, r_action_parts)) {
      string key = matches[1].str();
      string value = matches[2].str();
      printf("Setting %s to %s.\n", key.c_str(), value.c_str());
      state->storeString(key, value);
    } else {
      printf("Malformed action!\n");
    }
    next();
  } else if (current_element->type == "_choice_value") {
    printf("I'm in a choice value. I don't think I'm supposed to be here.\n");
    next();
  } else if (current_element->type == "_next") {
    setCurrentConversation(current_element->text);
  } else if (current_element->type == "_finish") {
    finish_value = current_element->text;
    finished = true;
  } else if (current_element->type == "_choice") {
    setMenus();
  } else if (current_element->type == "_speech") {
    setMenus();
  }
}

void Conversation::setMenus() {
  conversation_box->setText(current_element->text);
  conversation_box->startTypewriter();

  if (current_element->name != "") {
    character_box->setText({current_element->name});
  }

  if (current_element->type == "_choice") {
    vector<string> lines = {};
    printf("Checking choices\n");
    for (ConversationElementTD* element : current_element->children) {
      printf("Found a choice: %s\n", element->text.c_str());
      lines.push_back(element->text);
    }
    choice_box->setTextLines(lines);
  }

  if (current_element->type == "_speech" || (current_element->type == "_choice" && current_element->name != "")) {
    int speech_number = math_utils.randomInt(1,3);
    printf("Speech number is %d\n", speech_number);
    sound.stopSound();
    sound.playSound(character_speech[current_element->name] + "_" + to_string(speech_number), 1);
  }
}

void Conversation::draw() {
  if (finished) return;

  character_box->draw();
  conversation_box->draw();
  graphics.setColor("#ffffff", 1.0);
  
  if (current_element->type == "_choice" && conversation_box->typewriterFinished()) {
    choice_box->draw();
    graphics.drawImage(
      "choice_arrow",
      hot_config.getInt("menu", "choice_x") + hot_config.getInt("menu", "choice_margin_x") - 30,
      hot_config.getInt("menu", "choice_y") + hot_config.getInt("menu", "choice_margin_y") + 4 + 28 * choice_value
    );
  }

  graphics.setColor("#ffffff", 1.0);
  if (current_element->name != "" 
    && character_images.count(current_element->name) == 1
    && graphics.checkImage(character_images[current_element->name])) {
    graphics.drawImage(
      character_images[current_element->name],
      hot_config.getInt("menu", "profile_x"),
      hot_config.getInt("menu", "profile_y")
    );
  }
}

Conversation::~Conversation() {
  delete character_box;
  delete conversation_box;
}