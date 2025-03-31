#include <lib.hpp>
#include <sstream>

#include "romfs_structs.hpp"
#include "lib/util/modules.hpp"
#include "lua-5.1.5/src/lua.hpp"


// Type hashes for the general types (CClass, CCollectionType, CEnumType, CFlagsetType, CPointerType)
const uint64_t BASE_REFLECTION_CCLASS = UINT64_C(6091256465011624050);
const uint64_t BASE_REFLECTION_CCOLLECTIONTYPE = UINT64_C(16882394163986259376);
const uint64_t BASE_REFLECTION_CENUMTYPE = UINT64_C(18199701582522205282);
const uint64_t BASE_REFLECTION_CFLAGSETTYPE = UINT64_C(14341023533376106569);
const uint64_t BASE_REFLECTION_CPOINTERTYPE = UINT64_C(7506927479173953530);

void l_pushtablestring(lua_State* L, const char* key, const char* val) {
    lua_pushstring(L, key);
    lua_pushstring(L, val);
    lua_settable(L, -3);
}

const char* GetCRntStr(CRntString* s) {
    if (s == NULL) {
        return "";
    }
    return (const char*)s->string;
}

const char* GetCStrId(CStrId id) {
    if (id.inst == NULL) {
        return "";
    }
    return GetCRntStr(&id.inst->rnt_str);
}

inline uintptr_t GetTargetOffsetReverse(uintptr_t offset) {
    return offset - exl::util::GetMainModuleInfo().m_Total.m_Start;
}

void CTypeRef(lua_State* L, CType* type) {
    lua_newtable(L);
    lua_pushstring(L, "name");
    lua_pushstring(L, (type != NULL) ? GetCStrId(type->className) : "");
    lua_settable(L, -3);
    lua_pushstring(L, "offset");
    lua_pushstring(L, (type != NULL) ? std::to_string(GetTargetOffsetReverse((uintptr_t)&type->vtab)).c_str() : "-1");
    lua_settable(L, -3);
}

void ParseMetadata(lua_State* L, CRntDictionary<CStrId, CTypedValue<void*>>* data) {
    lua_newtable(L); // table ttMetadata
    if (data == NULL) {
        return;
    }
    
    CTypedValue<void*>* dataHead = data->values;
    CStrId* nameHead = data->keys;
    if (dataHead != NULL && data->count > 0) {
        for (uint i = 0; i < data->count; i++) {
            // ttMetadata[name]
            lua_pushstring(L, GetCStrId(nameHead[i]));
            // create table(name, value)
            lua_newtable(L);// top = ttMetadata[name]

            // data type
            CTypedValue<void*>* curr = &dataHead[i];
            // ttMetadata[name].type = className
            lua_pushstring(L, "type"); // key

            const char* className = (curr && curr->reflection) ? GetCStrId(curr->reflection->className) : "";
            uint64_t typeHash = (curr && curr->reflection) ? curr->reflection->typeHash : 0xffffffffffffffff;
            CTypeRef(L, curr->reflection); // val
            lua_settable(L, -3); // top = ttMetadata[name]

            // data value
            // ttMetadata[name].value = (switch)
            lua_pushstring(L, "value"); // key
            if (strcmp(className, "bool") == 0)
            {
                lua_pushboolean(L, (bool)(curr->value));
            }
            else if (strcmp(className, "base::global::CStrId") == 0)
            {
                CStrId str_id;
                std::memcpy(&str_id, &curr->value, sizeof(curr->value));
                lua_pushstring(L, GetCStrId(str_id));
            }
            else if (strcmp(className, "base::global::CRntString") == 0)
            {
                char** rnt_str;
                std::memcpy(&rnt_str, &curr->value, sizeof(curr->value));
                lua_pushstring(L, *rnt_str);
            }
            else if (strcmp(className, "float") == 0)
            {
                float val;
                std::memcpy(&val, &curr->value, 4);
                lua_pushstring(L, std::to_string(val).c_str());
            }
            else if (strcmp(className, "base::math::CVector2D") == 0)
            {
                float* vals;
                std::memcpy(&vals, &curr->value, sizeof(curr->value));
                lua_newtable(L);
                lua_pushstring(L, "x");
                lua_pushnumber(L, vals[0]);
                lua_settable(L, -3);
                lua_pushstring(L, "y");
                lua_pushnumber(L, vals[1]);
                lua_settable(L, -3);
            }
            else if (strcmp(className, "base::math::CVector3D") == 0)
            {
                float* vals;
                std::memcpy(&vals, &curr->value, sizeof(curr->value));
                lua_newtable(L);
                lua_pushstring(L, "x");
                lua_pushnumber(L, vals[0]);
                lua_settable(L, -3);
                lua_pushstring(L, "y");
                lua_pushnumber(L, vals[1]);
                lua_settable(L, -3);
                lua_pushstring(L, "z");
                lua_pushnumber(L, vals[2]);
                lua_settable(L, -3);
            }
            else if (strcmp(className, "base::math::CVector4D") == 0)
            {
                float* vals;
                std::memcpy(&vals, &curr->value, sizeof(curr->value));
                lua_newtable(L);
                lua_pushstring(L, "x");
                lua_pushnumber(L, vals[0]);
                lua_settable(L, -3);
                lua_pushstring(L, "y");
                lua_pushnumber(L, vals[1]);
                lua_settable(L, -3);
                lua_pushstring(L, "z");
                lua_pushnumber(L, vals[2]);
                lua_settable(L, -3);
                lua_pushstring(L, "w");
                lua_pushnumber(L, vals[3]);
                lua_settable(L, -3);
            }
            else if (strcmp(className, "unsigned") == 0)
            {
                uint32_t val;
                std::memcpy(&val, &curr->value, 4);
                lua_pushstring(L, std::to_string(val).c_str());
            }
            else if (strcmp(className, "CompleteInfo") == 0)
            {
                CompleteInfo* val;
                std::memcpy(&val, &curr->value, 8);
                
                lua_newtable(L);
                lua_pushstring(L, "unk0");
                lua_pushstring(L, std::to_string(val->unk0).c_str());
                lua_settable(L, -3);

                lua_pushstring(L, "vars");
                lua_newtable(L);

                CRntString* head = val->varNames.values;
                uint ct = val->varNames.count;
                for  (uint j = 0; j < ct; j++) {
                    lua_pushnumber(L, j+1);
                    lua_pushstring(L, GetCRntStr(&head[j]));
                    lua_settable(L, -3);
                }

                lua_settable(L, -3);
            }
            else if (typeHash == BASE_REFLECTION_CENUMTYPE) {
                // enum
                uint numVal;
                std::memcpy(&numVal, &curr->value, 4);

                // get data from class
                CEnumType* enum_cls = (CEnumType*)curr->reflection;
                EnumValue* vals_head = enum_cls->values.values;
                uint memberCount = enum_cls->values.count;
                for (uint j = 0; j < memberCount; j++) {
                    if (vals_head[j].enumValue == numVal) {
                        lua_pushstring(L, GetCStrId(vals_head[j].enumName));
                        break;
                    }
                }
            }
            else {
                lua_pushstring(L, "UNDEFINED");
            }
            lua_settable(L, -3);// top = ttMetadata[name]
            // store table(name, value) in ttMetadata[name]
            lua_settable(L, -3); // top = ttMetadata
        }
    }
}

void ParseCFunction(lua_State* L, CFunction* var) {
    lua_newtable(L); // ttable
    if (var == NULL) {
        return;
    }

    l_pushtablestring(L, "funcname", GetCStrId(var->sName)); // ttable[funcname] = funcname; top=ttable
    l_pushtablestring(L, "retType", var->return_type ? GetCStrId(var->return_type->className) : ""); // ttable["retType"]  = className ?? ""; top = ttable

    lua_pushstring(L, "params");
    lua_newtable(L); // BEGIN ttable[params]; top=ttable[params]
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
    lua_settable(L, -3);  // END ttable[params]; top=ttable

    lua_pushstring(L, "metadata");
    ParseMetadata(L, &var->metadata);
    lua_settable(L, -3);
}

void ParseCVariable(lua_State* L, CVariable* var) {
    lua_newtable(L);
    if (var == NULL) {
        return;
    }

    l_pushtablestring(L, "sName", GetCStrId(var->sName));
    
    lua_pushstring(L, "offset");
    lua_pushstring(L, std::to_string(var->offset).c_str());
    lua_settable(L, -3);

    l_pushtablestring(L, "type", var->typeReflection ? GetCStrId(var->typeReflection->typeData.className) : "");

    l_pushtablestring(L, "getterType", var->getterType ? GetCStrId(var->getterType->className) : "");
    l_pushtablestring(L, "setterType", var->setterType ? GetCStrId(var->setterType->className) : "");

    l_pushtablestring(L, "getter", var->getter ? GetCStrId(var->getter->sName) : "");
    l_pushtablestring(L, "setter", var->setter ? GetCStrId(var->setter->sName) : "");

    lua_pushstring(L, "metadata");
    ParseMetadata(L, &var->metadata);
    lua_settable(L, -3);
}

void ParseCClass(lua_State* L, CClass* cls) {
    lua_pushstring(L, "vctFunctions");
    lua_newtable(L); // BEGIN ttable["vctFunctions"]; top=ttable["vctFunctions"]
    CFunction* func_header = cls->vctFunctions.values;
    CStrId* funcname_header = cls->vctFunctions.keys;
    if (func_header != NULL && cls->vctFunctions.count > 0) {
        for (uint i = 0; i < cls->vctFunctions.count; i++) {
            lua_pushstring(L, GetCStrId(funcname_header[i]));
            ParseCFunction(L, &(func_header[i]));
            lua_settable(L, -3);// ttable["vctFunctions"][funcname] = ParseCFunction(); top=ttable["vctFunctions"]
        }
    }
    lua_settable(L, -3); // END ttable["vctFunctions"]; top=ttable

    lua_pushstring(L, "vctVariables");
    lua_newtable(L); // BEGIN ttable["vctVariables"]; top=ttable["vctVariables"]
    CVariable* var_header = cls->vctVariables.values;
    uint64_t* varhashes = cls->vctVariables.keys;
    if (var_header != NULL && cls->vctVariables.count > 0) {
        for  (uint i = 0; i < cls->vctVariables.count; i++) {
            lua_pushstring(L, std::to_string(varhashes[i]).c_str());
            ParseCVariable(L, &var_header[i]);
            lua_settable(L, -3); // ttable["vctVariables"][varhash] = CVariableToLua(); top=ttable["vctVariables"]
        }
    }
    lua_settable(L, -3);
}

void ParseCCollectionType(lua_State* L, CCollectionType* cls) {
    lua_pushstring(L, "keyType");
    CTypeRef(L, cls->keyType);
    lua_settable(L, -3);

    lua_pushstring(L, "valType");
    CTypeRef(L, cls->valType);
    lua_settable(L, -3);

    l_pushtablestring(L, "unk0", std::to_string(cls->unk0).c_str());
    l_pushtablestring(L, "unk1", std::to_string(cls->unk1).c_str());
    l_pushtablestring(L, "unk2", std::to_string(cls->unk2).c_str());
    l_pushtablestring(L, "unk3", std::to_string(cls->unk3).c_str());
    l_pushtablestring(L, "funcGetLength", std::to_string(cls->funcGetLength).c_str());
    l_pushtablestring(L, "funcClearMembers", std::to_string(cls->funcClearMembers).c_str());
    l_pushtablestring(L, "funcInsertCtor", std::to_string(cls->funcInsertCtor).c_str());
    l_pushtablestring(L, "funcInsertDtor", std::to_string(cls->funcInsertDtor).c_str());
    l_pushtablestring(L, "funcRemoveAt", std::to_string(cls->funcRemoveAt).c_str());
    l_pushtablestring(L, "funcGetElement", std::to_string(cls->funcGetElement).c_str());
    l_pushtablestring(L, "funcGetWithBoundCheck", std::to_string(cls->funcGetWithBoundCheck).c_str());
    l_pushtablestring(L, "funcGetCreateElCopy", std::to_string(cls->funcGetCreateElCopy).c_str());
    l_pushtablestring(L, "funcGetCreateElDtor", std::to_string(cls->funcGetCreateElDtor).c_str());
    l_pushtablestring(L, "unk4", std::to_string(cls->unk4).c_str());
    l_pushtablestring(L, "funcGetNextEmpty", std::to_string(cls->funcGetNextEmpty).c_str());
    l_pushtablestring(L, "unk6", std::to_string(cls->unk6).c_str());
    l_pushtablestring(L, "unk7", std::to_string(cls->unk7).c_str());
}

void ParseCEnumType(lua_State* L, CEnumType* cls) {
    lua_pushstring(L, "value_type");
    lua_pushstring(L, cls->item_type ? GetCStrId(cls->item_type->className) : "");
    lua_settable(L, -3);

    lua_pushstring(L, "values");
    lua_newtable(L);
    EnumValue* head = cls->values.values;
    uint count = cls->values.count;
    if (head != NULL && count > 0) {
        for (uint i = 0; i < count; i++) {
            lua_pushstring(L, GetCStrId(head[i].enumName));
            lua_pushstring(L, std::to_string(head[i].enumValue).c_str());
            lua_settable(L, -3);
        }
    }
    lua_settable(L, -3);
}

void ParseCFlagsetType(lua_State* L, CFlagsetType* cls) {
    lua_pushstring(L, "item_type");
    CTypeRef(L, cls->itemType);
    lua_settable(L, -3);

    lua_pushstring(L, "enum");
    CTypeRef(L, &cls->flagset->typeData);
    lua_settable(L, -3);
}

void ParseCPointerType(lua_State* L, CPointerType* cls) {
    lua_pushstring(L, "pointTo");
    CTypeRef(L, cls->pointingType);
    lua_settable(L, -3);
}

void ParseCType(lua_State *L, CType* cls) {
    lua_newtable(L);
    
    if (cls == NULL) {
        return;
    }
    
    uint64_t typeHash = cls->typeHash;

    l_pushtablestring(L, "sName", GetCStrId(cls->className));
    
    lua_pushstring(L, "type");
    lua_pushstring(L, std::to_string(cls->typeHash).c_str());
    lua_settable(L, -3);

    lua_pushstring(L, "size");
    lua_pushstring(L, std::to_string(cls->size).c_str());
    lua_settable(L, -3);
    
    lua_pushstring(L, "parent");
    CTypeRef(L, cls->parentClass);
    lua_settable(L, -3);

    lua_pushstring(L, "children");
    lua_newtable(L);

    CType** childTypes = cls->childClasses.values;
    uint childCount = cls->childClasses.count;
    for (uint i = 0; i < childCount; i++) {
        lua_pushnumber(L, i+1);
        CTypeRef(L, childTypes[i]);
        lua_settable(L, -3);
    }
    
    lua_settable(L, -3);

    lua_pushstring(L, "metadata");
    ParseMetadata(L, cls->metadata);
    lua_settable(L, -3);

    if (typeHash == BASE_REFLECTION_CCLASS) {
        ParseCClass(L, (CClass*)cls);
    }
    else if (typeHash == BASE_REFLECTION_CCOLLECTIONTYPE)
    {
        ParseCCollectionType(L, (CCollectionType*)cls);
    }
    else if (typeHash == BASE_REFLECTION_CENUMTYPE) {
        ParseCEnumType(L, (CEnumType*)cls);
    }
    else if (typeHash == BASE_REFLECTION_CFLAGSETTYPE) {
        ParseCFlagsetType(L, (CFlagsetType*)cls);
    }
    else if (typeHash == BASE_REFLECTION_CPOINTERTYPE) {
        ParseCPointerType(L, (CPointerType*)cls);
    }
}