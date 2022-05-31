function clone(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end
        local new_table = {}
        lookup_table[object] = new_table
        for key, value in pairs(object) do
            new_table[_copy(key)] = _copy(value)
        end
        return setmetatable(new_table, getmetatable(object))
    end
    return _copy(object)
end

--Create an class.
function class(classname, super)
    local superType = type(super)
    local cls

    if superType ~= "table" then
        superType = nil
        super = nil
    end

    -- inherited from Lua Object
    if super then
        cls = clone(super)
        cls.super = super
    else
        cls = {ctor = function() end}
    end

    cls.__cname = classname
    cls.__index = cls

    function cls.new(...)
        local instance = setmetatable({}, cls)
        instance.class = cls
        instance:ctor(...)
        return instance
    end

    return cls
end

--- 判断继承关系
function is_sub_class_of(sub,super)
    if sub.__index==nil or sub.__index.super==nil then
        return false
    end
    if sub.__index.super==super then
        return true
    else
        return is_sub_class_of(sub.__index.super,super)
    end
end

--- 合并列表
function combine_list(t1,t2)
    for k,v in pairs(t2) do
        table.insert(t1,v)
    end
end

--- 替换字符
function string_replace(src,find,new)
    find=string.gsub(find,"[%(%)%.%%%+%-%*%?%[%]%^%$]", function(c) return "%" .. c end)
    return string.gsub(src,find,new)
end

--- 打印table内容
function table_tostring (t)
    if t==nil then
        return "nil"
    end
    local tmpData={}
    for key,value in pairs(t) do
        -- if type(value)=="table" then
        --     value=table.tostring(value)
        -- end
        table.insert(tmpData,string.format("%s=%s[%s]",key,tostring(value),type(value)))
    end
    return(string.format("{%s}",table.concat(tmpData,",")))
end

--- 判断字符串以 xx 开头
--- @param varStr string 需要判断的字符串
--- @param varStart string xx
function string_startswith(str,start_str)
    return string.sub(str,1,string.len(start_str))==start_str
end

--- 判断字符串以 xx 结尾
function string_endswith(str,end_str)
    return end_str=='' or string.sub(str,-string.len(end_str))==end_str
end