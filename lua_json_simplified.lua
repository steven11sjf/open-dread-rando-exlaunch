CTypes = {
    CClass = "base::reflection::CClass",
    CCollectionType = "base::reflection::CCollectionType",
    CEnumType = "base::reflection::CEnumType",
    CFlagsetType = "base::reflection::CFlagsetType",
    CPointerType = "base::reflection::CPointerType"
}

function Readctype(o)
    return o
end

function DumpTypedval(name, o)
    local s = "\""..name.."\": {"
    s = s.."\"type\": \""..Readctype(o["type"]).."\", \"value\": "
    if o["type"] == "base::math::CVector3D" then
        s = s.."{\"x\": "..o["value"]["x"]..", \"y\": "..o["value"]["y"]..", \"z\": "..o["value"]["z"].."}"
    elseif o["type"] == "CompleteInfo" then
        s = s.."{\"unk0\": "..o["value"]["unk0"]..",\"unk1\": "..o["value"]["unk1"]..", \"variables\": ["
        for _, v in ipairs(o["value"]["vars"]) do
            s = s.."\""..tostring(v).."\","
        end
        if string.sub(s, -1) == ',' then
            s = string.sub(s, 0, -2)
        end
        s = s.."]}"
    else
        s = s.."\""..tostring(o["value"]).."\""
    end

    s = s.."},"
    return s
end

function DumpMetadata(o)
    local s = "\"metadata\": {"
    if o["metadata"] ~= nil then
        for _, v in ipairs(o["metadata"]) do
            s=s..DumpTypedval(v["name"], v["obj"])
        end

        if string.sub(s, -1)  == ',' then
            s = string.sub(s, 0, -2)
        end
    end
    s = s.."}"
    return s
end

function DumpVar(o)
    local s = "{\"sName\": \""..o["sName"].."\", "
    s = s.."\"offset\": \""..o["offset"].."\", "
    s = s.."\"type\": \""..o["type"].."\", "
    s = s.."\"getter\": \""..o["getter"].."\", "
    s = s.."\"setter\": \""..o["setter"].."\","
    s = s..DumpMetadata(o)
    s = s.."}"
    return s
end

function DumpFunc(o)
    local s = "{\"funcname\": \""..o["funcname"].."\", "
    s = s.."\"retType\": \""..o["retType"].."\", "
    s = s.."\"params\": ["
    for i, v in ipairs(o["params"]) do
        s = s.."\""..v.."\","
    end
    if string.sub(s, -1) == ',' then
        s = string.sub(s, 0, -2)
    end
    s = s.."]"
    s = s..DumpMetadata(o)
    s = s.."}"
    return s
end

function DumpClass(o)
    local s = ""
    s = s.."\"vctFunctions\": ["
    local vctF = o["vctFunctions"]
    for k, v in ipairs(vctF) do
        s = s..DumpFunc(v)..","
    end
    if string.sub(s, -1) == ',' then
        s = string.sub(s, 0, -2)
    end
    s = s.."],"

    s = s.."\"vctVariables\": ["
    for k, v in ipairs(o["vctVariables"]) do
        s = s..DumpVar(v)..","
    end
    if string.sub(s, -1) == ',' then
        s = string.sub(s, 0, -2)
    end
    s = s.."]"
    return s
end

function DumpCollection(o)
    local s = "\"keyType\": \""..Readctype(o["keyType"]).."\","
    s = s.."\"valueType\": \""..Readctype(o["valType"]).."\","
    return s
end

function DumpEnum(o)
    local s = "\"itemtype\": \""..o["value_type"].."\",\"values\": {"
    for k, v in pairs(o["values"]) do
        s = s.."\""..k.."\": "..tostring(v)..","
    end
    if string.sub(s, -1) == ',' then
        s = string.sub(s, 0, -2)
    end
    s = s.."}"
    return s
end

function DumpFlagset(o)
    local s = "\"itemtype\": \""..Readctype(o["item_type"])
    s = s.."\",\"enumClass\": \""..Readctype(o["enum"]).."\""
    return s
end

function DumpPointer(o)
    local s = "\"points_to\": \""..Readctype(o["pointTo"])
    return s
end

function DumpT(o)
    local s = "{\"sName\": \""..o["sName"].."\", "
    s = s.."\"type\": \""..o["type"].."\", "
    s = s.."\"size\": "..o["size"]..", "
    s = s.."\"parent\": \""..Readctype(o["parent"]).."\", "
    
    s = s.."\"children\": ["
    for _, v in ipairs(o["children"]) do
        s = s.."\""..Readctype(v).."\","
    end
    if string.sub(s, -1) == ',' then
        s = string.sub(s, 0, -2)
    end
    s = s.."],"

    s = s.."\"conversions\": ["
    for _, v in ipairs(o["conversions"]) do
        s = s.."\""..v.."\","
    end
    if string.sub(s, -1) == "," then
        s = string.sub(s, 0, -2)
    end
    s = s.."],"
    s = s..DumpMetadata(o)

    if o["type"] == CTypes.CClass then
        s = s..","..DumpClass(o)
    elseif o["type"] == CTypes.CCollectionType then
        s = s..","..DumpCollection(o)
    elseif o["type"] == CTypes.CEnumType then
        s = s..","..DumpEnum(o)
    elseif o["type"] == CTypes.CFlagsetType then
        s = s..","..DumpFlagset(o)
    elseif o["type"] == CTypes.CPointerType then
        s = s..","..DumpPointer(o)
    end
    return s.."}"
end

function DumpToRemote(o)
    RemoteLua.SendClassString(DumpT(RemoteLua.GetClassByName(o)))
end

function AlertDumpToRemote(n, o)
    Game.LogWarn(0, n..":  "..o)
    Game.AddGUISF(0.25, DumpToRemote, 's', o)
end
function DumpHashes(start)
    local alltypes, tcount=RemoteLua.ParseHashedClasses()
    local i = 1
    for k, v in ipairs(alltypes) do
        if k == start + 1000 then
            return
        end
        if k >= start then
            Game.AddGUISF(5*i, AlertDumpToRemote, 'is', k, v)
            i=i+1
        end
    end
end



--[[
-- Run in Remote Dread Lua Connector branch!
return DumpHashes()
--]]