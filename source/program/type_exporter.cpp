#include "type_exporter.hpp"

#include <lib.hpp>
#include <sstream>

#include "globals.hpp"
#include "bindings/reflection.hpp"
#include "lib/util/modules.hpp"
#include "lua-5.1.5/src/lua.hpp"


inline void l_pushtablestring(lua_State* L, const char* key, const char* val) {
    lua_pushstring(L, key);
    lua_pushstring(L, val);
    lua_settable(L, -3);
}

bool TryCastToCharConstPtr(lua_State *L, TypeObject* obj) {
    CType* t_cstr = Globals::ClassFromHash(CHAR_CONSTPTR);
    
    char* res = NULL;

    TypeObject dst;
    dst.type = t_cstr;
    dst.object = &res;
    if (Globals::CastObject(obj->type, obj, &dst)) {
        char*** r = (char***)dst.object;
        lua_pushstring(L, **r);//(const char*)dst.object);
    }
    else {
        lua_pushstring(L,  "Cannot cast!");
        return false;
    }
    return true;
}

bool TryCastToCharConstPtr(lua_State *L, void* obj, CType* type) {
    TypeObject o;
    o.type = type;
    o.object = obj;
    return TryCastToCharConstPtr(L, &o);
}

void GetCStrId(lua_State *L, CStrId id) {
    if (id.inst == NULL) {
        lua_pushstring(L, "NULL_CSTRID");
        return;
    }

    CType* t_charconst = Types::GetType("char const*");
    TypeObject str_in;
    str_in.object = &id;
    str_in.type = Types::GetType("base::global::CStrId");

    TypeObject* as_char = Types::CastTo(&str_in, t_charconst);
    if (!as_char) {
        lua_pushstring(L, "Conv failed 1");
        return;
    }
    char const** val = (char const**)as_char->object;
    lua_pushstring(L,  *val);

    Allocator::OnPointerDeleted(as_char->object);
    Allocator::OnPointerDeleted(as_char);
}
const char* GetCStrId(CStrId id) {
    if (id.inst == NULL) {
        return "";
    }
    else {
        if (id.inst->rnt_str.string == NULL) {
            return "NULL_CRNTSTR";
        }
        return (const char*)id.inst->rnt_str.string;
    }
}

inline uintptr_t GetTargetOffsetReverse(uintptr_t offset) {
    return offset - exl::util::GetMainModuleInfo().m_Total.m_Start;
}

void CTypeRef(lua_State* L, CType* type) {
    if (type != NULL) {
        GetCStrId(L, type->className);
    } else {
        lua_pushstring(L, "");
    }
}

void ParseTypedValue(lua_State* L, CType* type, void** data) {
    lua_newtable(L);

    const char* className = (type != NULL) ? GetCStrId(type->className) : "";
    uint64_t typeHash = (type != NULL) ? type->typeHash : 0xffffffffffffffff;
    
    lua_pushstring(L, "type"); // key
    CTypeRef(L, type); // val
    lua_settable(L, -3); // top = ttMetadata[name]
    
    lua_pushstring(L, "value"); // key
    if (strcmp(className, "bool") == 0) // no conversions
    {
        lua_pushboolean(L, (bool)(data));
    }
    else if (strcmp(className, "char") == 0) // no conversions
    {
        char val[2];
        std::memcpy(&val, &data, 1);
        val[1] = '\0';
        lua_pushstring(L, val);
    }
    else if (strcmp(className, "int") == 0) // int -> unsigned -> CStrId -> char const*
    {
        int val;
        std::memcpy(&val, &data, 4);
        lua_pushstring(L, std::to_string(val).c_str());
    }
    else if (strcmp(className, "unsigned") == 0) // unsigned -> CStrId -> char const*
    {
        uint32_t val;
        std::memcpy(&val, &data, 4);
        lua_pushstring(L, std::to_string(val).c_str());
    }
    else if (strcmp(className, "TPersistenceTargetFlagset") == 0) // no conversions
    {
        uint32_t val;
        std::memcpy(&val, &data, 4);
        lua_pushstring(L, std::to_string(val).c_str());
    }
    else if (strcmp(className, "base::global::CStrId") == 0) // CStrId -> char const*
    {
        CStrId str_id;
        std::memcpy(&str_id, &data, sizeof(data));
        lua_pushstring(L, GetCStrId(str_id));
    }
    else if (strcmp(className, "CAfterElementDeserializationWrapper") == 0) // no conversion
    {
        char*** rnt_str;
        std::memcpy(&rnt_str, &data, sizeof(data));
        lua_pushstring(L, **rnt_str);
    }
    else if (strcmp(className, "base::global::CFilePathStrId") == 0) // CFilePathStrId -> char const*
    {
        CStrId* str_id;
        std::memcpy(&str_id, &data, sizeof(data));
        lua_pushstring(L, GetCStrId(*str_id));
    }
    else if (strcmp(className, "base::global::CRntString") == 0) // CRntStr -> char const*
    {
        char** rnt_str;
        std::memcpy(&rnt_str, &data, sizeof(data));
        lua_pushstring(L, *rnt_str);
    }
    else if (strcmp(className, "base::global::CName") == 0) // no conversions
    {
        uint64_t val;
        std::memcpy(&val, &data, 8);
        lua_pushstring(L, std::to_string(val).c_str());
    }
    else if (strcmp(className, "float") == 0) // no conversions
    {
        float val;
        std::memcpy(&val, &data, 4);
        lua_pushstring(L, std::to_string(val).c_str());
    }
    else if (strcmp(className, "base::math::CVector2D") == 0) // no conversions
    {
        float vals[2];
        std::memcpy(vals, &data, sizeof(data));
        lua_newtable(L);
        lua_pushstring(L, "x");
        lua_pushnumber(L, vals[0]);
        lua_settable(L, -3);
        lua_pushstring(L, "y");
        lua_pushnumber(L, vals[1]);
        lua_settable(L, -3);
    }
    else if (strcmp(className, "base::math::CVector3D") == 0) // no conversions
    {
        float* vals;
        std::memcpy(&vals, &data, sizeof(data));
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
    else if (strcmp(className, "base::math::CVector4D") == 0) // no conversions
    {
        float* vals;
        std::memcpy(&vals, &data, sizeof(data));
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
    else if (strcmp(className, "base::color::CColor4B_SRGB") == 0) // no conversions
    {
        uint32_t val;
        std::memcpy(&val, &data, 4);
        // RGBA (guess, RGB could be any order)
        lua_newtable(L);

        lua_pushstring(L, "r");
        lua_pushinteger(L, val & 0xff);
        lua_settable(L, -3);

        lua_pushstring(L, "g");
        lua_pushinteger(L, (val >> 8) & 0xff);
        lua_settable(L, -3);
        
        lua_pushstring(L, "b");
        lua_pushinteger(L, (val >> 16) & 0xff);
        lua_settable(L, -3);
        
        lua_pushstring(L, "a");
        lua_pushinteger(L, (val >> 24) & 0xff);
        lua_settable(L, -3);
        
    }
    else if (strcmp(className, "CompleteInfo") == 0)
    {
        CompleteInfo* val;
        std::memcpy(&val, &data, 8);
        //lua_pushstring(L, ":3");
        lua_newtable(L);
        lua_pushstring(L, "unk0");
        lua_pushstring(L, std::to_string(val->unk0).c_str());
        lua_settable(L, -3);
        
        lua_pushstring(L, "unk1");
        lua_pushstring(L, std::to_string(val->unk1).c_str());
        lua_settable(L, -3);

        lua_pushstring(L, "vars");
        lua_newtable(L);

        CRntString* head = val->varNames.values;
        uint ct = val->varNames.count;

        //CType* t_crntstr = Types::GetType("base::global::CRntString");
        for  (uint j = 0; j < ct; j++) {
            lua_pushnumber(L, j+1);
            //TypeObject obj;
            //obj.type = t_crntstr;
            //obj.object = &head[j];
            lua_pushstring(L, head[j].string);//TryCastToCharConstPtr(L, &obj);
            lua_settable(L, -3);
        }

        lua_settable(L, -3);
    }
    else if (strcmp(className, "SDeathImpactDisplacement") == 0)
    {
        SDeathImpactDisplacement* obj;
        std::memcpy(&obj, &data, sizeof(data));

        lua_newtable(L);
        lua_pushstring(L, "fInitialSpeed");
        lua_pushnumber(L, obj->fInitialSpeed);
        lua_settable(L, -3);
        
        lua_pushstring(L, "fMidSpeed");
        lua_pushnumber(L, obj->fMidSpeed);
        lua_settable(L, -3);
        
        lua_pushstring(L, "fTimeToMidSpeed");
        lua_pushnumber(L, obj->fTimeToMidSpeed);
        lua_settable(L, -3);
        
        lua_pushstring(L, "fEndSpeed");
        lua_pushnumber(L, obj->fEndSpeed);
        lua_settable(L, -3);
    }
    // TODO fix CGameModelIndex
    /*else if (strcmp(className, "gameeditor::CGameModelIndex") == 0)
    {
        CRntVector<ObjectType>* obj;
        std::memcpy(&obj, &data, sizeof(data));

        lua_newtable(L);
        uint indexCount = obj->count;
        ObjectType* indexHead = obj->values;

        if (indexCount != 0 && indexHead != NULL) 
        {
            for (uint i = 0; i <  indexCount; i++)
            {
                lua_pushinteger(L, i+1);
                ParseTypedValue(L, indexHead[i].type, &indexHead[i].object);
                lua_settable(L, -3);
            }
        }
    }*/
    // TODO add CShape
    else if (typeHash == BASE_REFLECTION_CENUMTYPE) {
        // enum
        uint numVal;
        std::memcpy(&numVal, &data, 4);

        // get data from class
        CEnumType* enum_cls = (CEnumType*)type;
        EnumValue* vals_head = enum_cls->values.values;
        uint memberCount = enum_cls->values.count;
        for (uint j = 0; j < memberCount; j++) {
            if (vals_head[j].enumValue == numVal) {
                lua_pushstring(L, GetCStrId(vals_head[j].enumName));
                break;
            }
        }
    }
    else if (typeHash == BASE_REFLECTION_CFLAGSETTYPE)  {
        // flagset; just read as an int64 and postprocess
        uint64_t flags;
        std::memcpy(&flags, &data, sizeof(data));
        lua_pushstring(L, std::to_string(flags).c_str());
    }
    else {
        lua_pushstring(L, "UNDEFINED");
    }
    lua_settable(L, -3);// top = ttMetadata[name]
}

void ParseMetadata(lua_State* L, CRntDictionary<CStrId, CTypedValue<void*>>* data) {
    lua_newtable(L); // table ttMetadata
    if (data == NULL) {
        return;
    }
    
    CTypedValue<void*>* dataHead = data->values;
    CStrId* nameHead = data->keys;
    if (dataHead != NULL && nameHead != NULL && data->count > 0) {
        for (uint i = 0; i < data->count; i++) {
            // ttMetadata[name]
            lua_pushinteger(L, i+1);
            CTypedValue<void*>* curr = &dataHead[i];
            
            // data value
            // adds table to stack
            lua_newtable(L);
            l_pushtablestring(L, "name", GetCStrId(nameHead[i]));
            lua_pushstring(L, "obj");
            ParseTypedValue(L, curr->ctype, curr->value);
            lua_settable(L, -3);
            lua_settable(L, -3);
        }
    }
}

void ParseCFunction(lua_State* L, CFunction* var) {
    lua_newtable(L); // ttable
    if (var == NULL) {
        return;
    }
    l_pushtablestring(L, "vtable_offset", std::to_string(var->vtable).c_str());
    l_pushtablestring(L, "funcname", GetCStrId(var->sName)); // ttable[funcname] = funcname; top=ttable
    l_pushtablestring(L, "retType", var->return_type ? GetCStrId(var->return_type->className) : ""); // ttable["retType"]  = className ?? ""; top = ttable

    l_pushtablestring(L, "caller_value", std::to_string(var->caller_value).c_str());
    l_pushtablestring(L, "func_ptr", std::to_string(var->func_ptr).c_str());
    l_pushtablestring(L, "vtable_offset", std::to_string(var->vtable).c_str());
    l_pushtablestring(L, "unk1", std::to_string(var->unk1).c_str());
    l_pushtablestring(L, "unk2", std::to_string(var->unk2).c_str());
    l_pushtablestring(L, "unk3", std::to_string(var->unk3).c_str());
    l_pushtablestring(L, "unk4", std::to_string(var->unk4).c_str());
    l_pushtablestring(L, "unk5", std::to_string(var->unk5).c_str());
    l_pushtablestring(L, "unk6", std::to_string(var->unk6).c_str());
    l_pushtablestring(L, "unk7", std::to_string(var->unk7).c_str());
    l_pushtablestring(L, "dict2_count", std::to_string(var->params2_maybe.count).c_str());

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

    l_pushtablestring(L, "unk0", std::to_string(var->unk0).c_str());

    l_pushtablestring(L, "type", var->typeReflection ? GetCStrId(var->typeReflection->typeData.className) : "");

    l_pushtablestring(L, "getterType", var->getterType ? GetCStrId(var->getterType->className) : "");
    l_pushtablestring(L, "setterType", var->setterType ? GetCStrId(var->setterType->className) : "");

    l_pushtablestring(L, "getter", var->getter ? GetCStrId(var->getter->sName) : "");
    l_pushtablestring(L, "setter", var->setter ? GetCStrId(var->setter->sName) : "");
    l_pushtablestring(L, "fieldReflection", var->field_reflection ? GetCStrId(var->field_reflection->typeData.className) : "");

    l_pushtablestring(L, "unk01", std::to_string(var->unk01).c_str());
    l_pushtablestring(L, "unk02", std::to_string(var->unk02).c_str());
    l_pushtablestring(L, "unk2", std::to_string(var->unk2).c_str());
    l_pushtablestring(L, "unk03", std::to_string(var->unk03).c_str());
    l_pushtablestring(L, "unk3", std::to_string(var->unk3).c_str());
    l_pushtablestring(L, "unk4", std::to_string(var->unk4).c_str());
    l_pushtablestring(L, "unk04", std::to_string(var->unk04).c_str());

    l_pushtablestring(L, "dict2_count", std::to_string(var->dict2.count).c_str());

    lua_pushstring(L, "metadata");
    ParseMetadata(L, &var->metadata);
    lua_settable(L, -3);
}

void ParseCClass(lua_State* L, CClass* cls) {
    lua_pushstring(L, "funcCount");
    lua_pushinteger(L, cls->vctFunctions.count);
    lua_settable(L, -3);

    lua_pushstring(L, "vctFunctions");
    lua_newtable(L); // BEGIN ttable["vctFunctions"]; top=ttable["vctFunctions"]
    CFunction* func_header = cls->vctFunctions.values;
    if (func_header != NULL && cls->vctFunctions.count > 0 && cls->vctFunctions.count < MAX_CFUNCS) {
        for (uint i = 0; i < cls->vctFunctions.count; i++) {
            lua_pushinteger(L, i+1);
            ParseCFunction(L, &(func_header[i]));
            lua_settable(L, -3);// ttable["vctFunctions"][funcname] = ParseCFunction(); top=ttable["vctFunctions"]
        }
    }
    lua_settable(L, -3); // END ttable["vctFunctions"]; top=ttable

    lua_pushstring(L, "varCount");
    lua_pushinteger(L, cls->vctVariables.count);
    lua_settable(L, -3);

    lua_pushstring(L, "vctVariables");
    lua_newtable(L); // BEGIN ttable["vctVariables"]; top=ttable["vctVariables"]
    CVariable* var_header = cls->vctVariables.values;
    if (var_header != NULL && cls->vctVariables.count > 0 && cls->vctVariables.count <= MAX_CVARIABLES) {
        for  (uint i = 0; i < cls->vctVariables.count; i++) {
            lua_pushinteger(L, i+1);
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

    l_pushtablestring(L, "valuesCtor", std::to_string(cls->valuesCtor).c_str());

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

    l_pushtablestring(L, "unk0", std::to_string(cls->unk0).c_str());
    l_pushtablestring(L, "fun1", std::to_string(cls->fun1).c_str());
    l_pushtablestring(L, "fun2", std::to_string(cls->fun2).c_str());
    l_pushtablestring(L, "fun3", std::to_string(cls->fun3).c_str());
}

namespace TypeExporter {
    void ParseCType(lua_State *L, CType* cls) {
        lua_newtable(L);
        
        if (cls == NULL) {
            return;
        }
        
        uint64_t typeHash = cls->typeHash;

        lua_pushstring(L, "sName");
        GetCStrId(L, cls->className);
        lua_settable(L, -3);
        
        lua_pushstring(L, "type");

        CType* t_typeHash = Types::GetType(typeHash);
        if (t_typeHash) {
            GetCStrId(L, t_typeHash->className);
        } else {
            lua_pushstring(L, std::to_string(cls->typeHash).c_str());
        }
        //CTypeRef(L, t_typeHash);
        //lua_pushstring(L, std::to_string(cls->typeHash).c_str());
        lua_settable(L, -3);

        lua_pushstring(L, "size");
        lua_pushstring(L, std::to_string(cls->size).c_str());
        lua_settable(L, -3);
        
        l_pushtablestring(L, "unk0", std::to_string(cls->unk0).c_str());
        l_pushtablestring(L, "unk1", std::to_string(cls->unk1).c_str());
        l_pushtablestring(L, "funcCtor", std::to_string((uint64_t)cls->funcCtor).c_str());
        l_pushtablestring(L, "copyCtor", std::to_string((uint64_t)cls->copyCtor).c_str());
        l_pushtablestring(L, "moveCtor", std::to_string((uint64_t)cls->moveCtor).c_str());
        l_pushtablestring(L, "funcDtor", std::to_string((uint64_t)cls->funcDtor).c_str());
        l_pushtablestring(L, "copyDtor", std::to_string((uint64_t)cls->copyDtor).c_str());
        l_pushtablestring(L, "moveDtor", std::to_string((uint64_t)cls->moveDtor).c_str());
        l_pushtablestring(L, "funcCompare", std::to_string((uint64_t)cls->funcCompare).c_str());
        l_pushtablestring(L, "funcGetHashCode", std::to_string((uint64_t)cls->funcGetHashCode).c_str());
        l_pushtablestring(L, "funcGetRefInfo", std::to_string((uint64_t)cls->funcGetRefInfo).c_str());
        l_pushtablestring(L, "membersFunction", std::to_string((uint64_t)cls->membersFunction).c_str());
        l_pushtablestring(L, "unk2", std::to_string(cls->unk2).c_str());
        
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

        lua_pushstring(L, "conversions");
        lua_newtable(L);

        Conversion** conversions = cls->conversions.values;
        uint convCount = cls->conversions.count;
        for (uint i=0; i < convCount; i++) {
            CType* convTo = Types::GetType(conversions[i]->to);

            lua_pushinteger(L, i+1);
            GetCStrId(L, convTo->className);
            
            //lua_pushstring(L, std::to_string(GetTargetOffsetReverse(conversions[i]->func)).c_str());
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

    void ParseCType(lua_State *L) {
        char const* clsName = luaL_checkstring(L, 1);
        if (!clsName) {
            lua_pushnil(L);
            return;
        }
        CType *cls = Types::GetType(clsName);
        if (cls == NULL) {
            lua_pushnil(L);
            return;
        }
        ParseCType(L, cls);
    }

    void ParseHashedClasses(lua_State* L, ReflectionManager* refmgr) {
        //uint idx = luaL_checknumber(L, 1);
        lua_newtable(L); // res

        uint total = 0;
        for (uint idx = 0; idx < 5000; idx++) {
            HashedType* currT = refmgr->types.values[idx];
            while (currT != NULL) {
                lua_pushinteger(L, total+1); // res.k
                GetCStrId(L, currT->type->className);
                /*lua_newtable(L); // res[k] = v
                
                uint64_t offset = (uint64_t)GetTargetOffsetReverse((uintptr_t)&currT->type->vtab);
                //lua_pushtablestring(L, GetCStrId(currT->typeName), std::to_string(GetTargetOffsetReverse((uintptr_t)&currT->type->vtab)).c_str())
                lua_pushstring(L, "low"); // res[k]["low"]
                lua_pushinteger(L, offset & 0xffff);
                lua_settable(L, -3);

                lua_pushstring(L, "hi"); // res[k]["hi"]
                lua_pushinteger(L, (offset >> 16) & 0xffff);
                lua_settable(L, -3);*/

                lua_settable(L, -3);
                currT = currT->next;
                total++;
            }
        }

        lua_pushinteger(L, total);
    }
}