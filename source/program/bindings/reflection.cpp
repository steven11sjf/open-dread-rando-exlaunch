#include "bindings/reflection.hpp"
#include "bindings/types.hpp"
#include "globals.hpp"
#include <string.h>


namespace Types {
    CType* GetType(uint64_t hash) {
        return Globals::ClassFromHash(hash);
    }

    CType* GetType(char const* className) {
        if (!className) {
            return NULL;
        }

        uint64_t hash = Globals::CRC64(className, strlen(className));
        return GetType(hash);
    }

    TypeObject* CastTo(TypeObject* obj, CType* target) {
        if (!Globals::CanCastTo(obj->type, target)) {
            return NULL;
        }

        TypeObject* res = (TypeObject*)Allocator::AllocateOnHeap(0x10);
        res->type = target;
        res->object = TypeFuncs::FuncCtor(target);
        if (res->object == NULL) {
            return NULL;
        }

        Globals::CastObject(obj->type, obj, res);
        return res;
    }
}