#include <lib.hpp>
#include <sstream>

#include "romfs_structs.hpp"
#include "lib/util/modules.hpp"
#include "lua-5.1.5/src/lua.hpp"

void l_pushtablestring(lua_State* L, const char* key, const char* val) {
    lua_pushstring(L, key);
    lua_pushstring(L, val);
    lua_settable(L, -3);
}

void l_pushhexstring(lua_State* L, uint64_t val) {
    std::string _str;
    std::stringstream ss(_str);
    ss << "0x" << std::hex << val;
    lua_pushstring(L, ss.str().c_str());
}

const char* GetCStrId(CStrId id) {
    if (id.inst == NULL || id.inst->string == NULL) {
        return "";
    }
    return (const char*)id.inst->string;
}

inline uintptr_t GetTargetOffsetReverse(uintptr_t offset) {
    return offset - exl::util::GetMainModuleInfo().m_Total.m_Start;
}

void MetadataAsLuaTable(lua_State* L, CRntDictionary<CStrId, CTypedValue<void*>>* data) {
    if (data == NULL) {
        return;
    }
    
    lua_newtable(L); // table ttMetadata
    CTypedValue<void*>* dataHead = data->values;
    CStrId* nameHead = data->keys;
    if (dataHead != NULL && data->count > 0) {
        for (uint i = 0; i < data->count; i++) {
            // ttMetadata[name]
            lua_pushstring(L, GetCStrId(nameHead[i]));
            // create table(name, value)
            lua_newtable(L);

            // data type
            CTypedValue<void*>* curr = &dataHead[i];
            // ttMetadata[name].type = className
            lua_pushstring(L, "type");

            const char* className = (curr && curr->reflection) ? GetCStrId(curr->reflection->typeData.className) : "";
            lua_pushstring(L, className);
            lua_settable(L, -3);

            // data value
            // ttMetadata[name].value = (switch)
            lua_pushstring(L, "value");
            if (strcmp(className, "bool") == 0)
            {
                lua_pushboolean(L, (bool)(curr->value));
            }
            else if (strcmp(className, "base::global::CStrId") == 0)
            {
                CStrId *str_id;
                std::memcpy(&str_id, &curr->value, sizeof(curr->value));
                lua_pushstring(L, GetCStrId(*str_id));
            }
            else if (strcmp(className, "base::global::CRntString") == 0)
            {
                char** rnt_str;
                std::memcpy(&rnt_str, &curr->value, sizeof(curr->value));
                lua_pushstring(L, *rnt_str);
            }
            else {
                lua_pushstring(L, "UNDEFINED");
            }
            lua_settable(L, -3);
            // store table(name, value) in ttMetadata[name]
            lua_settable(L, -3);
        }
    }
}

void CFunctionAsJson(lua_State* L, CFunction* var) {
    if (var == NULL) {
        return;
    }

    lua_newtable(L);
    l_pushtablestring(L, "funcname", GetCStrId(var->sName));
    l_pushtablestring(L, "retType", var->return_type ? GetCStrId(var->return_type->className) : "");

    lua_pushstring(L, "params");
    lua_newtable(L);
    uint numParams = var->params.count;
    CClass** params_head = var->params.values;
    if (params_head != NULL && var->params.count > 0)
    {
        for (uint i = 0; i < numParams; i++) {
            CClass* pClass = params_head[i];
            lua_pushinteger(L, i+1);
            lua_pushstring(L, GetCStrId(pClass->typeData.className));
            lua_settable(L, -3);
        }
    }
    lua_settable(L, -3);

    lua_pushstring(L, "metadata");
    MetadataAsLuaTable(L, &var->metadata);
    lua_settable(L, -3);
}

void CVariableToLua(lua_State* L, CVariable* var) {
    lua_newtable(L);
    if (var == NULL) {
        return;
    }

    l_pushtablestring(L, "sName", GetCStrId(var->sName));
    
    lua_pushstring(L, "offset");
    l_pushhexstring(L, (uint64_t)var->offset);
    lua_settable(L, -3);

    l_pushtablestring(L, "type", var->typeReflection ? GetCStrId(var->typeReflection->typeData.className) : "");

    l_pushtablestring(L, "getterType", var->getterType ? GetCStrId(var->getterType->className) : "");
    l_pushtablestring(L, "setterType", var->setterType ? GetCStrId(var->setterType->className) : "");

    l_pushtablestring(L, "getter", var->getter ? GetCStrId(var->getter->sName) : "");
    l_pushtablestring(L, "setter", var->setter ? GetCStrId(var->setter->sName) : "");

    lua_pushstring(L, "metadata");
    MetadataAsLuaTable(L, &var->metadata);
    lua_settable(L, -3);
}

const char* ClassInfoString(lua_State* L, CClass* cls) {
    if (cls == NULL) {
        return "UNDEFINED";
    }

    lua_newtable(L);
    l_pushtablestring(L, "sName", GetCStrId(cls->typeData.className));
    
    lua_pushstring(L, "type");
    l_pushhexstring(L, cls->typeData.typeHash);
    lua_settable(L, -3);

    lua_pushstring(L, "size");
    l_pushhexstring(L, cls->typeData.size);
    lua_settable(L, -3);
    
    l_pushtablestring(L, "parent", cls->typeData.parentClass ? GetCStrId(cls->typeData.parentClass->typeData.className) : "");

    lua_pushstring(L, "vctFunctions");
    lua_newtable(L);
    CFunction* func_header = cls->vctFunctions.values;
    CStrId* funcname_header = cls->vctFunctions.keys;
    if (func_header != NULL && cls->vctFunctions.count > 0) {
        for (uint i = 0; i < cls->vctFunctions.count; i++) {
            lua_pushstring(L, GetCStrId(funcname_header[i]));
            CFunctionAsJson(L, &(func_header[i]));
            lua_settable(L, -3);
        }
    }
    lua_settable(L, -3);

    lua_pushstring(L, "vctVariables");
    lua_newtable(L);
    CVariable* var_header = cls->vctVariables.values;
    uint64_t* varhashes = cls->vctVariables.keys;
    if (var_header != NULL && cls->vctVariables.count > 0) {
        for  (uint i = 0; i < cls->vctVariables.count; i++) {
            l_pushhexstring(L, varhashes[i]);
            CVariableToLua(L, &var_header[i]);
            lua_settable(L, -3);
        }
    }
    lua_settable(L, -3);

    lua_pushstring(L, "metadata");
    MetadataAsLuaTable(L, cls->typeData.metadata);
    lua_settable(L, -3);

    return "";
}