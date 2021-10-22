#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <iostream>

using namespace std;

class Player
{
public:
    Player(){}

    int AddHp(int add)
    {
        hp_+=add;
        return hp_;
    }

    int hp_=0;
};

int main(int argc, char * argv[])
{
    sol::state sol_state;
    sol_state.open_libraries(sol::lib::base);
    sol::usertype<Player> usertype_player=sol_state.new_usertype<Player>("Player",sol::constructors<Player()>());
    usertype_player["AddHp"]=&Player::AddHp;
    usertype_player["hp_"]=&Player::hp_;

    Player* enemy_player=new Player();
    sol_state["enemy_player"]=enemy_player;

    sol_state.script_file("../a.lua");

    sol::function function_get_enemy_player=sol_state["get_enemy_player"];
    std::function<Player*()> std_function_get_enemy_player=function_get_enemy_player;
    Player* enemy_player_from_lua=std_function_get_enemy_player();
    std::cout<<"enemy_player_from_lua==enemy_player:"<< (enemy_player_from_lua==enemy_player) <<std::endl;

    //--------test lua extend cpp class--------
    sol::table enemy_player_table=function_get_enemy_player();
    enemy_player_table["ExtendFunction"](enemy_player_from_lua);
    return 0;
}
