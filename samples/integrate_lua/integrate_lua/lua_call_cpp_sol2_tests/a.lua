print("-------run lua--------")
local player = Player.new()
print(player)
print("player.hp_:" .. player.hp_)
print("player.AddHp(4):" .. player:AddHp(4))
print("player.hp_:" .. player.hp_)

print("--------test lua replace cpp class function--------")
Player.AddHp=function(add)
    print("Player:ExtendFunction AddHp self:" .. tostring(self))
    self.hp_=self.hp_ + add;
    return self.hp_;
end

print("--------test cpp pass obj to lua--------")

print(enemy_player)
print("enemy_player.hp_:" .. enemy_player.hp_)
print("enemy_player.AddHp(5):" .. enemy_player:AddHp(5))
print("enemy_player.hp_:" .. enemy_player.hp_)

print("--------test lua return obj to cpp--------")
function get_enemy_player()
    return enemy_player
end

print("--------test lua extend cpp class--------")
function Player:ExtendFunction()
    print("Player:ExtendFunction self:" .. tostring(self))
end