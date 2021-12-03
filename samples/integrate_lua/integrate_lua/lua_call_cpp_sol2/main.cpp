#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

using namespace std;

class Player
{
public:
    int AddHp(int add)
    {
        hp_+=add;
        return hp_;
    }

    int hp_=0;

    void CreateMesh(float data[],int size){
        float x=data[0];
    }
};

int main(int argc, char * argv[])
{
    sol::state sol_state;
    sol_state.open_libraries(sol::lib::base);
    //注册构造函数到lua
    sol::usertype<Player> usertype_player=sol_state.new_usertype<Player>("Player",sol::constructors<Player()>());
    //注册成员函数和变量到lua
    usertype_player["AddHp"]=&Player::AddHp;
    usertype_player["hp_"]=&Player::hp_;
    usertype_player["CreateMesh"]=&Player::CreateMesh;



    auto result= sol_state.script_file("../a.lua");
    if(result.valid()==false){
        sol::error err = result;
        std::cerr<<"\n---- LOAD LUA ERROR ----\n"<<err.what()<<std::endl;
    }

    return 0;
}
