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
}

void Conversation::loadConversations(string conversation_filepath) {
  std::smatch matches;
  regex r_character_image("_character_image ([^,]+), ([A-Za-z0-9_]*)");
  regex r_conversation("_conversation ([^\n]*)");
  regex r_speech("([ ]*)_speech ([^:]+): ([^\n]*)");
  regex r_choice("([ ]*)_choice ([^:]+): ([^\n]*)");
  regex r_choice_value("([ ]*)_choice_value ([^\n]*)");

  std::ifstream input_file(conversation_filepath);

  string load_current_convo = "";
  //stack<vector<ConversationElement*>*> sub_convos;
  current_indentation = 0;
  ConversationElement* last_element;

  string line;
  int line_count = 0;
  while (getline(input_file, line)) {
    //printf("Read line %s\n", line.c_str());
    if (regex_match(line, matches, r_character_image)) {
      character_images[matches[1].str()] = matches[2].str();
    } else if (regex_match(line, matches, r_conversation)) {
      //printf("Matched _conversation\n");
      //printf("Name is %s\n", matches[1].str().c_str());
      load_current_convo = matches[1].str();
      conversations[load_current_convo] = {};
      sub_conversation.empty();
      sub_conversation.push(&conversations[load_current_convo]);
      current_indentation = 2;
    } else if (regex_match(line, matches, r_speech)) {
      ConversationElement* element = new ConversationElement();
      // printf("_speech\n");
      // printf("Indentation is %lu\n", matches[1].str().size());
      // printf("Name is %s\n", matches[2].str().c_str());
      // printf("Speech is %s\n", matches[3].str().c_str());
      handleIndentation(matches[1].str().size(), line_count);
      element->type = "_speech";
      element->name = matches[2].str();
      element->text = matches[3].str();
      sub_conversation.top()->push_back(element);
      last_element = element;
    } else if (regex_match(line, matches, r_choice)) {
      ConversationElement* element = new ConversationElement();
      // printf("_speech\n");
      // printf("Indentation is %lu\n", matches[1].str().size());
      // printf("Name is %s\n", matches[2].str().c_str());
      // printf("Speech is %s\n", matches[3].str().c_str());
      handleIndentation(matches[1].str().size(), line_count);
      element->type = "_choice";
      element->name = matches[2].str();
      element->text = matches[3].str();
      sub_conversation.top()->push_back(element);
      last_element = element;
    } else if (regex_match(line, matches, r_choice)) {
      ConversationElement* element = new ConversationElement();
      // printf("_speech\n");
      // printf("Indentation is %lu\n", matches[1].str().size());
      // printf("Name is %s\n", matches[2].str().c_str());
      // printf("Speech is %s\n", matches[3].str().c_str());
      handleIndentation(matches[1].str().size(), line_count);
      element->type = "_choice";
      element->name = matches[2].str();
      element->text = matches[3].str();
      sub_conversation.top()->push_back(element);
      last_element = element;
      last_element->choices = {};
    } else if (regex_match(line, matches, r_choice_value)) {
      ConversationElement* element = new ConversationElement();
      // printf("_speech\n");
      // printf("Indentation is %lu\n", matches[1].str().size());
      // printf("Name is %s\n", matches[2].str().c_str());
      // printf("Speech is %s\n", matches[3].str().c_str());
      if (matches[1].str().size() != current_indentation + 2) {
        printf("Syntax error on line %d! _choice_value must be 2 spaces in from previous _choice", line_count);
        exit(1);
      }
      element->type = "_choice_value";
      element->text = matches[2].str();
      last_element->choices.push_back(element);
      last_element->num_choices += 1;
    }
    // printf("The size of the main list is %lu\n", conversations[load_current_convo].size());
    line_count++;
  }
}

void Conversation::handleIndentation(int new_indentation, int line_count) {
  if (new_indentation < 2) {
    printf("Syntax error on line %d! Expected speech to have indentation at least 2 spaces", line_count);
    exit(1);
  } else if (abs(new_indentation - current_indentation) > 2) {
    printf("Syntax error on line %d! Expected indentation within 2 spaces of previous indentation", line_count);
    exit(1);
  } else if (new_indentation % 2 != 0) {
    printf("Syntax error on line %d! Expected an even indentation value", line_count);
    exit(1);
  } else if (new_indentation < current_indentation) {
      sub_conversation.pop();
      current_indentation = new_indentation;
  } else if (new_indentation > current_indentation) {
    sub_conversation.top()->back()->children = {};
    sub_conversation.push(&sub_conversation.top()->back()->children);
    current_indentation = new_indentation;
  }
}

void Conversation::setCurrentConversation(string conversation_name) {
  if (conversations.count(conversation_name) != 1) {
    printf("Error! Couldn't find conversation %s\n", conversation_name.c_str());
    exit(1);
  }

  current_conversation = conversations[conversation_name];
  conversation_position.empty();
  current_element = conversations[conversation_name][0];
  sub_conversation.push(&conversations[conversation_name]);
  conversation_position.push(0);
  choice_value = 0;

  setMenus();
}

void Conversation::up() {
  if (!conversation_box->typewriterFinished()) {
    conversation_box->startTypewriter();
  } else if (current_element->type == "_choice") {
    sound.playSound("select_sound", 1);
    choice_value -= 1;
    if (choice_value < 0) {
      choice_value = current_element->num_choices - 1;
    }
  }
}

void Conversation::down() {
  if (!conversation_box->typewriterFinished()) {
    conversation_box->finishTypewriter();
  } else if (current_element->type == "_choice") {
    sound.playSound("select_sound", 1);
    choice_value += 1;
    if (choice_value > current_element->num_choices - 1) {
      choice_value = 0;
    }
  }
}

void Conversation::accept() {
  if (!conversation_box->typewriterFinished()) {
    conversation_box->finishTypewriter();
    return;
  }

  if (finished()) {
    return;
  }

  if (current_element->type == "_choice") {
    sound.playSound("accept_sound", 1);
    state->values["music_choice"] = choice_value;
    current_element = NULL;
    conversation_position = {};
    return;
  }

  if (current_element->children.size() > 0) {
    // If there is a child conversation, push it onto the stack and start walking it
    conversation_position.push(0);
    sub_conversation.push(&current_element->children);
    current_element = current_element->children[0];
  } else if (conversation_position.top() < sub_conversation.top()->size() - 1) {
    // Otherwise, if there are still elements in this sub-conversation, walk to the next one
    conversation_position.top() += 1;
    current_element = (*sub_conversation.top())[conversation_position.top()];
  } else if (conversation_position.top() >= sub_conversation.top()->size() - 1) {
    // Otherwise, pop this sub-conversation and go back to the next one.
    conversation_position.pop();
    sub_conversation.pop();
    if (conversation_position.size() > 0) {
      conversation_position.top() += 1;
    }
  }

  if (current_element != NULL) {
    setMenus();
  }
}

bool Conversation::finished() {
  return conversation_position.size() == 0;
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
    for (ConversationElementTD* element : current_element->choices) {
      printf("Found a choice: %s\n", element->text.c_str());
      lines.push_back(element->text);
    }
    choice_box->setTextLines(lines);
  }
}

void Conversation::draw() {
  if (finished() && current_element == NULL) return;

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