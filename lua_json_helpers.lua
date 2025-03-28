function dumpmetadata(o)
    local s = "\"metadata\": {"
    if o["metadata"] ~= nil then
        for k, v in pairs(o["metadata"]) do
            s = s.."\""..k.."\": {"
            for k2, v2 in pairs(v) do
                s = s.."\""..k2.."\": \""..tostring(v2).."\","
            end
            if string.sub(s, -1)  == ',' then
                s = string.sub(s, 0, -2)
            end
            s = s.."},"
        end

        if string.sub(s, -1)  == ',' then
            s = string.sub(s, 0, -2)
        end
    else
        s = s.."WTF"
    end
    s = s.."}"
    return s
end

function dumpvar(o)
    local s = "{\"sName\": \""..o["sName"].."\", "
    s = s.."\"offset\": \""..o["offset"].."\", "
    s = s.."\"type\": \""..o["type"].."\", "
    s = s.."\"getterType\": \""..o["getterType"].."\", "
    s = s.."\"getter\": \""..o["getter"].."\", "
    s = s.."\"setterType\": \""..o["setterType"].."\", "
    s = s.."\"setter\": \""..o["setter"].."\","
    s = s..dumpmetadata(o)
    s = s.."}"
    return s
end

function dumpfunc(o)
    local s = "{\"funcname\": \""..o["funcname"].."\", "
    s = s.."\"retType\": \""..o["retType"].."\", "
    s = s.."\"params\": ["
    for i, v in ipairs(o["params"]) do
        s = s.."\""..v.."\","
    end
    if string.sub(s, -1) == ',' then
        s = string.sub(s, 0, -2)
    end
    s = s.."],"
    s = s..dumpmetadata(o)
    s = s.."}"
    return s
end

function dump(o)
    local s = "{\"sName\": \""..o["sName"].."\", "
    s = s.."\"type\": \""..o["type"].."\", "
    s = s.."\"size\": \""..o["size"].."\", "
    s = s.."\"parent\": \""..o["parent"].."\", "
    
    s = s.."\"vctFunctions\": {"
    local vctF = o["vctFunctions"]
    for k, v in pairs(vctF) do
        s = s.."\""..k.."\": "..dumpfunc(v)..","
    end
    if string.sub(s, -1) == ',' then
        s = string.sub(s, 0, -2)
    end
    s = s.."},"

    s = s.."\"vctVariables\": {"
    for k, v in pairs(o["vctVariables"]) do
        s = s.."\""..k.."\": "..dumpvar(v)..","
    end
    if string.sub(s, -1) == ',' then
        s = string.sub(s, 0, -2)
    end
    s = s.."},"
    s = s..dumpmetadata(o).."}"
    return s
end

local res = RemoteLua.GetClassInfo(0x7275, 0xe4e8)

Game.LogWarn(0, dump(res))