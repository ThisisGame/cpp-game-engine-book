//
// Created by captain on 2021/6/9.
//

#ifndef UNTITLED_GAME_OBJECT_H
#define UNTITLED_GAME_OBJECT_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <list>
#include <functional>
#include "component.h"
#include "data_structs/tree.h"

class GameObject:public Tree::Node {
public:
    GameObject(const char *name);
    ~GameObject();

    const char * name(){return name_;}
    void set_name(const char *name){ name_=name;}

    unsigned char layer(){return layer_;}
    void set_layer(unsigned char layer){layer_=layer;}

    /// 是否激活，受到父节点影响
    /// \return
    bool active(){
        if(active_self_==false){//自己本身没有激活，直接返回false。
            return false;
        }
        //向上检查父节点是否没有激活
        Node* node=this;
        while (true) {
            node=node->parent();
            if(node == nullptr) {
                break;
            }
            GameObject* game_object=dynamic_cast<GameObject*>(node);
            if(game_object->active_self()==false){
                return false;
            }
        }
        return true;
    }

    bool active_self(){return active_self_;}
    void set_active_self(bool active_self){active_self_=active_self;}

    /// 设置父节点
    /// \param parent
    /// \return
    bool SetParent(GameObject* parent);

    /// 全局查找GameObject
    /// \param name
    /// \return
    static GameObject* Find(const char *name);
public:
    /// 添加组件，仅用于C++中添加组件。
    /// \tparam T 组件类型
    /// \return 组件实例
    template <class T=Component>
    T* AddComponent(){
        T* component=new T();
        AttachComponent(component);
        component->Awake();
        return dynamic_cast<T*>(component);
    }

    /// 附加组件实例
    /// \param component_instance_table
    void AttachComponent(Component* component);



    /// 获取组件，仅用于C++中。
    /// \tparam T 组件类型
    /// \return 组件实例
    template <class T=Component>
    T* GetComponent(){
        //获取类名
        type t=type::get<T>();
        std::string component_type_name=t.get_name().to_string();
        std::vector<Component*> component_vec;

        if(components_map_.find(component_type_name)!=components_map_.end()){
            component_vec=components_map_[component_type_name];
        }
        if(component_vec.size()==0){
            //没有找到组件,就去查找子类组件
            auto derived_classes = t.get_derived_classes();
            for(auto derived_class:derived_classes){
                std::string derived_class_type_name=derived_class.get_name().to_string();
                if(components_map_.find(derived_class_type_name)!=components_map_.end()){
                    component_vec=components_map_[derived_class_type_name];
                    if(component_vec.size()!=0){
                        break;
                    }
                }
            }
        }
        if(component_vec.size()==0){
            return nullptr;
        }
        return dynamic_cast<T*>(component_vec[0]);
    }

    /// 遍历组件
    /// \param func
    void ForeachComponent(std::function<void(Component*)> func);

    /// 遍历GameObject
    /// \param func
    static void Foreach(std::function<bool(GameObject* game_object)> func);

    /// 返回GameObject树结构
    /// \return
    static Tree& game_object_tree(){return game_object_tree_;}
private:
    const char * name_;

    unsigned char layer_;//将物体分不同的层，用于相机分层、物理碰撞分层等。

    bool active_self_=true;//自身是否激活

    std::unordered_map<std::string,std::vector<Component*>> components_map_;

    static Tree game_object_tree_;//用树存储所有的GameObject。

    static std::list<GameObject*> game_object_list_;//存储所有的GameObject。
};


#endif //UNTITLED_GAME_OBJECT_H
