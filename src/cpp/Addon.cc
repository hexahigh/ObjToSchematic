#include <napi.h>

#include "./wrapper/WVoxelMesh.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports)
{
    return WVoxelMesh::Init(env, exports);
}

NODE_API_MODULE(addon, InitAll)