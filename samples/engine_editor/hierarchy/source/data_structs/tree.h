//
// Created by captain on 2021/10/17.
//

#ifndef UNTITLED_TREE_H
#define UNTITLED_TREE_H

#include <list>
#include <functional>

class Tree {
public:
    class Node{
    public:
        virtual ~Node(){}

        Node* parent(){return parent_;}

        std::list<Node*>& children(){return children_;}

        /// 逐级向上查找root节点
        /// \return
        Node* root(){
            Node* parent=this;
            while (true) {
                if(parent->parent()== nullptr)
                {
                    break;
                }
                parent=parent->parent();
            }
            return parent;
        }

        void AddChild(Node* child){
            //先从之前的父节点移除
            if(child->parent()!= nullptr){
                child->parent()->RemoveChild(child);
            }
            children_.push_back(child);
            child->parent_=this;
            child->depth_=this->depth_+1;
        }

        void RemoveChild(Node* child){
            children_.remove(child);
        }

        bool Empty(){
            return children_.size()>0;
        }

        unsigned short depth(){return depth_;}

    private:
        Node* parent_= nullptr;//父节点
        std::list<Node*> children_;//子节点
        unsigned short depth_=0;//树深度
    };

public:
    Tree();
    ~Tree();

    Node* root_node(){return root_node_;}

    void Post(Node* node,std::function<void(Node *)> func);

    void Find(Node* node_parent,std::function<bool(Node *)> function_check,Node** node_result);
private:
    Node* root_node_;
};


#endif //UNTITLED_TREE_H
