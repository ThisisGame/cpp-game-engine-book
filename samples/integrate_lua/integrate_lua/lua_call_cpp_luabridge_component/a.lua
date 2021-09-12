print("-------run lua--------")

LoginScene=setmeta
function LoginScene:Awake()
    print("LoginScene Awake")
end

go = GameObject()
-- go:AddComponent(Animator)
-- go:AddComponent(Camera)
-- print(go)

local loginScene=go:AddComponent(LoginScene)
print(loginScene)