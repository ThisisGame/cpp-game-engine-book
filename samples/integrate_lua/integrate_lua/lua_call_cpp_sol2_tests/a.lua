print("-------run lua--------")


print("--------test cpp pass obj to lua--------")

print(enemy_player)
print(enemy_player)
--print("enemy_player.hp_:" .. enemy_player.hp_)
--print("enemy_player.AddHp(5):" .. enemy_player:AddHp(5))
--print("enemy_player.hp_:" .. enemy_player.hp_)

local a=enemy_player+enemy_player
local a=(enemy_player==enemy_player)

--print("--------test lua return obj to cpp--------")
--function get_enemy_player()
--    return enemy_player
--end
--
--print("--------test lua extend cpp class--------")
--function Engine.Player:ExtendFunction()
--    print("Player:ExtendFunction self:" .. tostring(self))
--end
--
--function Engine.Player:AddHp(add)
--    print("Player:AddHp self:" .. tostring(self))
--end
--
----print("--------test lua replace cpp class function--------")
----Player.AddHp=function(this,add)
----    print("Player:ExtendFunction AddHp self:" .. tostring(this))
----    this.hp_=this.hp_ + add;
----    return this.hp_;
----end
--
--local player = Engine.Player.new()
--print(player)
--print("player.hp_:" .. player.hp_)
--print("player.AddHp(4):" .. player:AddHp(4))
--print("player.hp_:" .. player.hp_)
--player:ExtendFunction()