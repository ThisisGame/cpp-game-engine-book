print("-------run lua--------")

LoginScene={}
function LoginScene:Awake()
    print("LoginScene Awake")
end

function LoginScene:Update()
    print("LoginScene Update")
end

function main()
    go = GameObject()
    go:AddComponent(Animator)
    --go:AddComponent(Camera)
    -- print(go)

    local loginScene=go:AddComponent(LoginScene)
    print(loginScene)
end
