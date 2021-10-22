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
};

int main(int argc, char * argv[])
{
    sol::state sol_state;
    sol_state.open_libraries(sol::lib::base);
    sol::usertype<Player> usertype_player=sol_state.new_usertype<Player>("Player",sol::constructors<Player()>());
    usertype_player["AddHp"]=&Player::AddHp;
    usertype_player["hp_"]=&Player::hp_;

    sol_state.script_file("../a.lua");

    return 0;
}
