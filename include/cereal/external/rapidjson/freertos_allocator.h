#ifndef FREERTOS_ALLOCATOR_H_
#define FREERTOS_ALLOCATOR_H_

#include "FreeRTOS.h"

CEREAL_RAPIDJSON_NAMESPACE_BEGIN

class FrtosAllocator
{
public:
    static const bool kNeedFree = true;
    void *Malloc(size_t size)
    {
        if (size) //  behavior of malloc(0) is implementation defined.
            return pvPortMalloc(size);
        else
            return NULL; // standardize to returning NULL.
    }
    void *Realloc(void *originalPtr, size_t originalSize, size_t newSize)
    {
        (void) originalSize;
        if (newSize == 0) {
            vPortFree(originalPtr);
            return NULL;
        }
        void *newPtr = pvPortMalloc(newSize);
        if (newPtr) {
            memcpy(newPtr, originalPtr, originalSize);
            vPortFree(originalPtr);
        }
        return newPtr;
    }
    static void Free(void *ptr) { vPortFree(ptr); }
};

CEREAL_RAPIDJSON_NAMESPACE_END

#endif // FREERTOS_ALLOCATOR_H_
