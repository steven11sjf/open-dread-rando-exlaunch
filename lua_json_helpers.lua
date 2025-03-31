CCLASS_TYPE = "6091256465011624050"
CCOLLECTION_TYPE = "16882394163986259376"
CENUMTYPE_TYPE = "18199701582522205282"
CFLAGSET_TYPE = "14341023533376106569"
CPOINTER_TYPE = "7506927479173953530"

function Readctype(o)
    return "{\"name\": \""..o["name"].."\",\"offset\":"..o["offset"].."}"
end

function DumpTypedval(name, o)
    local s = "\""..name.."\": {"
    s = s.."\"type\": "..Readctype(o["type"])..", \"value\": "
    if o["type"]["name"] == "base::math::CVector3D" then
        s = s.."{\"x\": "..o["value"]["x"]..", \"y\": "..o["value"]["y"]..", \"z\": "..o["value"]["z"].."}"
    elseif o["type"]["name"] == "CompleteInfo" then
        s = s.."{\"unk0\": "..o["value"]["unk0"]..", \"variables\": ["
        for _, v in pairs(o["value"]["vars"]) do
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
        for k, v in pairs(o["metadata"]) do
            s=s..DumpTypedval(k, v)
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
    s = s.."\"getterType\": \""..o["getterType"].."\", "
    s = s.."\"getter\": \""..o["getter"].."\", "
    s = s.."\"setterType\": \""..o["setterType"].."\", "
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
    s = s.."],"
    s = s..DumpMetadata(o)
    s = s.."}"
    return s
end

function DumpClass(o)
    local s = ""
    s = s.."\"vctFunctions\": {"
    local vctF = o["vctFunctions"]
    for k, v in pairs(vctF) do
        s = s.."\""..k.."\": "..DumpFunc(v)..","
    end
    if string.sub(s, -1) == ',' then
        s = string.sub(s, 0, -2)
    end
    s = s.."},"

    s = s.."\"vctVariables\": {"
    for k, v in pairs(o["vctVariables"]) do
        s = s.."\""..k.."\": "..DumpVar(v)..","
    end
    if string.sub(s, -1) == ',' then
        s = string.sub(s, 0, -2)
    end
    s = s.."}"
    return s
end

function DumpCollection(o)
    local s = "\"keyType\": "..Readctype(o["keyType"])..","
    s = s.."\"valueType\": "..Readctype(o["valType"])..","
    s = s.."\"unk0\": "..o["unk0"]..","
    s = s.."\"unk1\": "..o["unk1"]..","
    s = s.."\"unk2\": "..o["unk2"]..","
    s = s.."\"unk3\": "..o["unk3"]..","
    s = s.."\"funcGetLength\": "..o["funcGetLength"]..","
    s = s.."\"funcClearMembers\": "..o["funcClearMembers"]..","
    s = s.."\"funcInsertCtor\": "..o["funcInsertCtor"]..","
    s = s.."\"funcInsertDtor\": "..o["funcInsertDtor"]..","
    s = s.."\"funcRemoveAt\": "..o["funcRemoveAt"]..","
    s = s.."\"funcGetElement\": "..o["funcGetElement"]..","
    s = s.."\"funcGetWithBoundCheck\": "..o["funcGetWithBoundCheck"]..","
    s = s.."\"funcGetCreateElCopy\": "..o["funcGetCreateElCopy"]..","
    s = s.."\"funcGetCreateElDtor\": "..o["funcGetCreateElDtor"]..","
    s = s.."\"unk4\": "..o["unk4"]..","
    s = s.."\"funcGetNextEmpty\": "..o["funcGetNextEmpty"]..","
    s = s.."\"unk6\": "..o["unk6"]..","
    s = s.."\"unk7\": "..o["unk7"]
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
    local s = "\"itemtype\": "..Readctype(o["item_type"])
    s = s..",\"enumClass\": "..Readctype(o["enum"])
    return s
end

function DumpPointer(o)
    return "\"points_to\": "..Readctype(o["pointTo"])
end

function DumpT(o)
    local s = "{\"sName\": \""..o["sName"].."\", "
    s = s.."\"type\": \""..o["type"].."\", "
    s = s.."\"size\": \""..o["size"].."\", "
    s = s.."\"parent\": "..Readctype(o["parent"])..", "
    
    s = s.."\"children\": ["
    for k, v in pairs(o["children"]) do
        s = s..Readctype(v)..","
    end
    if string.sub(s, -1) == ',' then
        s = string.sub(s, 0, -2)
    end
    s = s.."],"
    s = s..DumpMetadata(o)

    if o["type"] == CCLASS_TYPE then
        s = s..","..DumpClass(o)
    elseif o["type"] == CCOLLECTION_TYPE then
        s = s..","..DumpCollection(o)
    elseif o["type"] == CENUMTYPE_TYPE then
        s = s..","..DumpEnum(o)
    elseif o["type"] == CFLAGSET_TYPE then
        s = s..","..DumpFlagset(o)
    elseif o["type"] == CPOINTER_TYPE then
        s = s..","..DumpPointer(o)
    end
    return s.."}"
end

-- CBaseObject
local cclass = RemoteLua.DumpCType(0x01cf, 0x5960)

-- TGlobalMapIcons
local ccoll = RemoteLua.DumpCType(0x7276, 0xfa10)

-- EMarkerType
local cenum = RemoteLua.DumpCType(0x7276, 0xecd8)

-- TCoolShinesparkSituation
local cflag = RemoteLua.DumpCType(0x7265, 0x76b8)

-- CAABoxShape2DPtr
local cptr = RemoteLua.DumpCType(0x7275, 0x8cb8)

local bigFatDump = "{\"base::reflection::CBaseObject\": "..DumpT(cclass)..",\"TGlobalMapIcons\": "..DumpT(ccoll)..",\"EMarkerType\": "..DumpT(cenum)..",\"TCoolShinesparkSituation\": "..DumpT(cflag)..",\"CAABoxShape2DPtr\": "..DumpT(cptr).."}"
Game.LogWarn(0, bigFatDump)