#include "bindings/types.hpp"
#include  "bindings/main_allocator.hpp"

namespace TypeFuncs {
    void* FuncCtor(CType* type) {
        if (type->funcCtor == NULL) {
            return NULL;
        }

        void* obj = Allocator::AllocateOnHeap(type->size);
        if (obj) {
            type->funcCtor(obj);
        }

        return obj;
    }

    void* CopyCtor(CType* type, void* other) {
        if (!type->copyCtor) {
            return NULL;
        }

        void* obj = Allocator::AllocateOnHeap(type->size);
        if (obj) {
            type->copyCtor(obj, other);
        }

        return obj;
    }

    void* MoveCtor(CType* type, void* other) {
        if (!type->moveCtor)  {
            return NULL;
        }

        void* obj = Allocator::AllocateOnHeap(type->size);
        if (obj) {
            type->moveCtor(obj, other);
        }

        return obj;
    }

    void FuncDtor(CType* type, void* obj) {
        if (!type->funcDtor) {
            return;
        }

        type->funcDtor(obj);
        Allocator::OnPointerDeleted(obj);
    }

    void CopyDtor(CType* type, void* obj, void* other) {
        if (!type->copyDtor) {
            return;
        }

        type->copyDtor(obj, other);
        Allocator::OnPointerDeleted(other);
    }

    void MoveDtor(CType* type, void* obj, void* other) {
        if (!type->moveDtor) {
            return;
        }

        type->moveDtor(obj, other);
        Allocator::OnPointerDeleted(other);
    }
}