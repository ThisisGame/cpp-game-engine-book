print("-------run lua--------")
local player = Player()
print(player)
print("player.hp_:" .. player.hp_)
print("player.AddHp(4):" .. player:AddHp(4))
print("player.hp_:" .. player.hp_)

function fail ()
  player:he()
  error ("A problem occurred")
end