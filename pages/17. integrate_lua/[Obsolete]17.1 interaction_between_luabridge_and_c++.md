## 17.1 LuaBridge与C++交互

```bash
CLion项目文件位于 samples/integrate_lua/integrate_lua/lua_call_cpp_luabridge_component
```

LuaBridge是一个Lua绑定库，用于将C++的类、函数、数据映射到Lua中，这样就可以在Lua中创建C++实例、访问C++实例、调用C++函数。
也对Lua数据进行封装提供一系列接口给C++，在C++中访问Lua数据也变得很方便。

```bash
Github：https://github.com/vinniefalco/LuaBridge
文档：http://vinniefalco.github.io/LuaBridge/Manual.html
```

LuaBridge作为一个轻量级的绑定库，原始版本不支持以下功能：

1. 一个函数或方法的参数超过 8 个（可以自行修改 添加更多 TypeListValues 特化来增加）。
2. 重载的函数、方法或构造函数。
3. STL 容器类型和 Lua 表之间的自动转换（可以自己写转换 )
4. 从 C++ 类继承 Lua 类。
5. 将 nil 传递给需要指针或引用的 C++ 函数。
6. 标准容器，如 std::shared_ptr。 

不过在我使用LuaBridge的这一个月里，已经有人提交了一些特性。

LuaBridge还很活跃，这也是我选择它的原因之一。

本小节通过对LuaBridge提供的一系列特性，来学习介绍LuaBridge与C++交互逻辑。

### 1.项目介绍

项目将之前章节的`GameObject`-`Component`架构提取出来，用作测试，并创建了`Animator`、`Camera`两个组件。

另外实现了Lua Component逻辑，并创建了`LoginScene` 这个Lua组件。

这一小节的项目，其实就是引擎的基础架构，在这个基础上，集成LuaBridge，实现Lua Component。

![](../../imgs/integrate_lua/lua_call_cpp_luabridge_component/lua_component.jpg)


### 2.LuaBridge特性介绍

介绍对类、函数、数据的绑定。

#### 2.1 绑定类

下面是绑定`GameObject`的代码：

```c++
//绑定 GameObject
{
    luabridge::getGlobalNamespace(lua_state)
            .beginClass<GameObject>("GameObject")
            .addConstructor<void (*) ()>() //绑定 构造函数
            .addFunction("__eq", &GameObject::operator==)// 操作符重载
            .addFunction("AddComponent", &GameObject::AddComponentFromLua)//绑定 成员函数
            .addFunction("GetComponent",&GameObject::GetComponentFromLua)//绑定 成员函数
            .endClass();
}
```
上述代码执行后，就可以在lua中编写下面的代码：

```lua
---创建GameObject实例
local game_object=GameObject()

--调用GameObject函数
local component=game_object:AddComponent("Camera")

--重载操作符，判断2个GameObject是否相等
local game_object_player=GameObject()
print(game_object==game_object_player)
```

#### 2.2 绑定基类与子类

对于有继承关系的类，则需要先绑定基类，然后再绑定子类。
参考`Camera`的注册代码：

```c++
//注册基类
luabridge::getGlobalNamespace(lua_state)
        .beginClass<Component>("Component")
        .addConstructor<void (*) ()>()
        .addFunction("Awake",&Component::Awake)
        .addFunction("Update",&Component::Update)
        .addFunction("game_object",&Component::game_object)
        .addFunction("set_game_object",&Component::set_game_object)
        .endClass();
    
//注册子类
luabridge::getGlobalNamespace(lua_state)
    .deriveClass<Camera,Component>("Camera")//指明继承自 Component
    .addConstructor<void (*) ()>()//绑定 构造函数
    .addFunction("position",&Camera::position)//绑定 子类函数
    .addFunction("set_position",&Camera::set_position)
    .endClass();
```

子类中是不用绑定基类函数的。

#### 2.3 绑定操作符重载

如果需要大量的操作符重载，那可以参考`glm::vec3`的绑定过程，代码如下：

```c++
luabridge::getGlobalNamespace(lua_state)
    .beginNamespace("glm")//指定NameSpace
    .beginClass<glm::vec3>("vec3")//绑定类
    .addConstructor<void(*)(const float&, const float&, const float&)>()//绑定 构造函数
    .addData("x", &glm::vec3::x)//绑定 成员变量
    .addData("y", &glm::vec3::y)
    .addData("z", &glm::vec3::z)
    .addData("r", &glm::vec3::r)
    .addData("g", &glm::vec3::g)
    .addData("b", &glm::vec3::b)
    .addFunction ("__tostring", std::function <std::string (const glm::vec3*)> ([] (const glm::vec3* vec) {return glm::to_string(*vec);}))//指定Lua Meta Function
    .addFunction ("__add", std::function <glm::vec3 (const glm::vec3*,const glm::vec3*)> ([] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)+(*vec_b);}))
    .addFunction ("__sub", std::function <glm::vec3 (const glm::vec3*,const glm::vec3*)> ([] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)-(*vec_b);}))
    .addFunction ("__mul", std::function <glm::vec3 (const glm::vec3*,const float)> ([] (const glm::vec3* vec,const float a) {return (*vec)*a;}))
    .addFunction ("__div", std::function <glm::vec3 (const glm::vec3*,const float)> ([] (const glm::vec3* vec,const float a) {return (*vec)/a;}))
    .addFunction ("__unm", std::function <glm::vec3 (const glm::vec3*)> ([] (const glm::vec3* vec) {return (*vec)*-1;}))
    .addFunction ("__eq", std::function <bool (const glm::vec3*,const glm::vec3*)> ([] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)==(*vec_b);}))
    .endClass();
```

上述代码中，以`__`为前缀的，就是Lua提供的操作符重载元方法，只要对其赋值即可实现操作符重载。

上述代码执行后，就可以在lua中编写下面的代码：

```lua
print("glm.vec3(4,5,6)+glm.vec3(4,5,6): " .. tostring(glm.vec3(4,5,6)+glm.vec3(4,5,6)))
print("glm.vec3(4,5,6)-glm.vec3(4,5,6): " .. tostring(glm.vec3(4,5,6)-glm.vec3(4,5,6)))
print("glm.vec3(4,5,6)*3: " .. tostring(glm.vec3(4,5,6)*3))
print("glm.vec3(4,5,6)/3: " .. tostring(glm.vec3(4,5,6)/3))
```

#### 2.4 绑定普通函数

简单的直接绑定，例如：

```c++
GameObject* game_object_;
void CompareGameObject(GameObject* a,GameObject* b){
    std::cout<<"CompareGameObject a==b: "<<(a==b)<<std::endl;
}

luabridge::getGlobalNamespace(lua_state)
    .addFunction("CompareGameObject", &CompareGameObject);
```

上述代码执行后，就可以在lua中编写下面的代码：

```lua
print("----------- simple function ------------")
local go=GameObject()
CompareGameObject(go,go)
```


对付复杂函数的绑定，需要写lambda表达式的，可以参考对 glm 提供的API的绑定。

```c++
luabridge::getGlobalNamespace(lua_state)
    .beginNamespace("glm")
    .addFunction("to_string",std::function <std::string (const glm::mat4*)> ([] (const glm::mat4* m) {return glm::to_string((*m));}))
    .addFunction("to_string",std::function <std::string (const glm::vec3*)> ([] (const glm::vec3* v) {return glm::to_string((*v));}))//同名覆盖，不支持函数重载。应该使用 __tostring注册。
    .addFunction("rotate",std::function <glm::mat4 (const glm::mat4*,const float,const glm::vec3*)> ([] (const glm::mat4* m,const float f,const glm::vec3* v) {return glm::rotate(*m,f,*v);}))
    .addFunction("radians",std::function <float (const float)> ([] (const float f) {return glm::radians(f);}));
```

#### 2.5 绑定常量

绑定常量需要指定NameSpace。

```c++
const int const_value=12;
        
luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Test")
        .addConstant("const_value",const_value)
        .endNamespace();
```

上述代码执行后，就可以在lua中编写下面的代码：

```lua
print("glm.vec3(4,5,6)+glm.vec3(4,5,6): " .. tostring(glm.vec3(4,5,6)+glm.vec3(4,5,6)))
print("glm.vec3(4,5,6)-glm.vec3(4,5,6): " .. tostring(glm.vec3(4,5,6)-glm.vec3(4,5,6)))
print("glm.vec3(4,5,6)*3: " .. tostring(glm.vec3(4,5,6)*3))
print("glm.vec3(4,5,6)/3: " .. tostring(glm.vec3(4,5,6)/3))
```

#### 2.6 绑定枚举

有以下枚举：

```c++
typedef enum KeyAction{
    UP=0,
    DOWN=1,
    REPEAT=2
}KeyAction;
```

下面函数返回了枚举中的一项：

```
KeyAction GetKeyActionUp(){
    return KeyAction::UP;
}
```

如果想在lua中调用`GetKeyActionUp`，那么需要注册这个函数，以及返回的枚举。

下面代码注册枚举类型到Lua：

```c++
// 注册枚举
template <typename T>
struct EnumWrapper {
    static typename std::enable_if<std::is_enum<T>::value, void>::type push(lua_State* L, T value){
        lua_pushnumber (L, static_cast<std::size_t> (value));
    }
    static typename std::enable_if<std::is_enum<T>::value, T>::type get(lua_State* L, int index){
        return static_cast <T> (lua_tointeger (L, index));
    }
};

namespace luabridge {
    template <>
    struct Stack<KeyAction> : EnumWrapper<KeyAction>{};
} // namespace luabridge
```

然后注册函数：

```c++
luabridge::getGlobalNamespace(lua_state)
    .addFunction("GetKeyActionUp",&GetKeyActionUp)
```

上述代码执行后，就可以在lua中编写下面的代码：

```lua
print(GetKeyActionUp())
```

但是还没完，上面是将`KeyAction`这个类型注册到Lua。

然后在lua调用c++函数时，对枚举转换为`number`传入到lua，从而在lua中可以获取到函数返回值。

但是当在lua直接访问`KeyAction`的枚举项，就会报错。

因为lua中识别不了`KeyAction::UP`这种形式，`UP`这个项，并没有注册。

所以还需要将枚举中的项，一一注册。

```c++
luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("KeyAction")
        .addConstant<std::size_t>("UP",KeyAction::UP)
        .addConstant<std::size_t>("DOWN",KeyAction::DOWN)
        .endNamespace();
```

上述代码执行后，就可以在lua中编写下面的代码：

```lua
print(GetKeyActionUp())
print(KeyAction.UP)
print(GetKeyActionUp()==KeyAction.UP)
```

#### 2.7 C++调用Lua全局函数

以调用`main.lua`中的`main()`为例：

```c++
//加上大括号，为了LuaRef在lua_close之前自动析构。
{
    luabridge::LuaRef main_function = luabridge::getGlobal(lua_state, "main");
    try {
        main_function();
    } catch (const luabridge::LuaException& e) {
        std::cout<<"lua error: "<<e.what()<<std::endl;
    }
}
```

#### 2.8 C++调用Lua类成员函数

以Lua组件`LoginScene`为例：

```lua
--file:example/login_scene.lua line:1

LoginScene={
    game_object_
}

function LoginScene:game_object()
    return self.game_object_
end

function LoginScene:set_game_object(game_object)
    self.game_object_=game_object
end

function LoginScene:Awake()
    print("LoginScene Awake")
end

function LoginScene:Update()
    print("LoginScene Update")
end

setmetatable(LoginScene,{["__call"]=function(table,param)
    local instance=setmetatable({},{__index=table})
    return instance
end})
```

在`GameObject::AddComponentFromLua`根据传入的组件名，创建组件实例并添加，然后调用组件成员函数。

```c++
//file:source/game_object.h line:83

/// 根据传入的组件名，创建组件实例
/// \param component_type_name
/// \return
luabridge::LuaRef AddComponentFromLua(std::string component_type_name) {
    luabridge::LuaRef component_type=luabridge::getGlobal(lua_state,component_type_name.c_str());
    auto new_table=component_type();//luabridge对c++的class注册为table，并实现了__call，所以可以直接带括号。

    LuaRef function_ref=new_table["set_game_object"];
    if(function_ref.isFunction()){
        function_ref(new_table,this);
    }
    ......

    return new_table;
}
```

`luabridge::getGlobal`以组件名获取到全局Table后，直接以括号就可以创建实例对象`new_table`。

然后以`new_table["set_game_object"]`就可以获取到成员函数`set_game_object`的引用`function_ref`。

然后直接调用函数即可。

记得要传入`self`，即要传入`new_table`。
