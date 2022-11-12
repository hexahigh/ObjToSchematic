#pragma once

#include <napi.h>

#include "../VoxelMesh.h"

class WVoxelMesh : public Napi::ObjectWrap<WVoxelMesh>, VoxelMesh
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    WVoxelMesh(const Napi::CallbackInfo &info);

private:
    Napi::Value SetVertices(const Napi::CallbackInfo &info);
    Napi::Value SetTexcoords(const Napi::CallbackInfo &info);
    Napi::Value SetTriangles(const Napi::CallbackInfo &info);
    Napi::Value Voxelise(const Napi::CallbackInfo &info);
    Napi::Value GetVoxelPositions(const Napi::CallbackInfo &info);
    Napi::Value GetVoxelColours(const Napi::CallbackInfo &info);
    Napi::Value GetVoxelCount(const Napi::CallbackInfo &info);
    Napi::Value GetVoxelBounds(const Napi::CallbackInfo &info);
    Napi::Value IsVoxelAt(const Napi::CallbackInfo &info);
    Napi::Value GetVoxelIndex(const Napi::CallbackInfo &info);
    Napi::Value GetNeighbours(const Napi::CallbackInfo &info);

    // Render buffers
    Napi::Value GetPositionsRenderBuffer(const Napi::CallbackInfo &info);
    Napi::Value GetColoursRenderBuffer(const Napi::CallbackInfo &info);
};