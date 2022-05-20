## 17.1 Lua与C++交互

sol2特性
1. lua可以对cpp class添加函数
2. 不能覆盖函数
3. set_function加lambda在vs下面会有一大堆提示信息，加上overload就不会有了……，github上新版本已经修复等release。
4. 调用cpp的构造函数是需要调用new的。
   ```c++
    sol::table engine_table=sol_state["Engine"];
    sol::table player_table=engine_table["Player"];
    sol::protected_function  player_new_function=player_table["new"];
    auto result=player_new_function();
    if(result.valid()==false){
        sol::error err=result;
        std::cerr<<err.what()<<std::endl;
    }
    sol::table player_instance_table=result;
    player_instance_table["hp_"]=4;
    Player* player_create_from_lua=player_instance_table.as<Player*>();
    int hp=player_create_from_lua->hp_;
   ```
5. 可选两种构造函数方式，直接带() ，或者使用.new()

    1. 默认就是.new()
        ```c++
        sol_state.new_usertype<Player>("Player",sol::constructors<Player(),Player(int)>());
        ```

        这种在lua的Component需要也添加.new() 函数，才能和cpp component行为一致。
        ```lua
        setmetatable(LoginScene,{["new"]=function(table,param)
            local instance=setmetatable({},{__index=table})
            return instance
        end})
        ```

    2. 主动指定构造函数为sol::call_constructor,使用__call metafunction。
        ```c++
        sol_state.new_usertype<Player>("Player",sol::call_constructor,sol::constructors<Player(),Player(int)>())
        ```

        这种在lua的Component需要也添加 __call，才能和cpp component行为一致。
        ```lua
        setmetatable(LoginScene,{["__call"]=function(table,param)
            local instance=setmetatable({},{__index=table})
            return instance
        end})
        ```