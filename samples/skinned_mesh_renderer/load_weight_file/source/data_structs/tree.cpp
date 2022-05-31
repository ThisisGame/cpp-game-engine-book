//
// Created by captain on 2021/10/17.
//

#include "tree.h"


Tree::Tree() {
    root_node_=new Node();
}

void Tree::Post(Node* node,std::function<void(Node * )> func) {
    for (auto child:node->children()) {
        Post(child,func);
    }
    if(node!=root_node_){
        func(node);
    }
}

Tree::~Tree()=default;

void Tree::Find(Node* node_parent,std::function<bool(Node *)> function_check,Node** node_result= nullptr) {
    for (auto child:node_parent->children()) {
        Find(child,function_check,node_result);
        if(*node_result!= nullptr){
            break;
        }
    }
    if(*node_result!= nullptr){
        return;
    }
    if(function_check(node_parent)){
        (*node_result)=node_parent;
    }
}
