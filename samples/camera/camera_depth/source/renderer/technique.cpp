//
// Created by captainchen on 2021/5/14.
//

#include "technique.h"
#include "pass.h"

Technique::Technique() {

}

Technique::~Technique() {

}

void Technique::Parse(rapidxml::xml_node<>* technique_node) {
    //根节点
    rapidxml::xml_node<>* root_node=technique_node;
    if(root_node== nullptr){
        return;
    }

    //pass 节点
    rapidxml::xml_node<>* pass_node=root_node->first_node("pass");
    while (pass_node!= nullptr){
        if(pass_node== nullptr){
            break;
        }

        Pass* pass=new Pass();
        pass->Parse(pass_node);
        pass_vec_.push_back(pass);

        pass_node=pass_node->next_sibling("pass");
    }
}