#include "WVoxelMesh.h"
#include <algorithm>
#include <cmath>
#include <cassert>

#include "../VoxelMeshRenderBuffer.h"

Napi::Object WVoxelMesh::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func =
        DefineClass(env,
                    "VoxelMesh", {InstanceMethod("setVertices", &WVoxelMesh::SetVertices), InstanceMethod("setTexcoords", &WVoxelMesh::SetTexcoords), InstanceMethod("setTriangles", &WVoxelMesh::SetTriangles), InstanceMethod("voxelise", &WVoxelMesh::Voxelise), InstanceMethod("getVoxelPositions", &WVoxelMesh::GetVoxelPositions), InstanceMethod("getVoxelColours", &WVoxelMesh::GetVoxelColours), InstanceMethod("getVoxelCount", &WVoxelMesh::GetVoxelCount), InstanceMethod("getVoxelBounds", &WVoxelMesh::GetVoxelBounds), InstanceMethod("isVoxelAt", &WVoxelMesh::IsVoxelAt), InstanceMethod("getVoxelIndex", &WVoxelMesh::GetVoxelIndex), InstanceMethod("getNeighbours", &WVoxelMesh::GetNeighbours), InstanceMethod("getPositionsRenderBuffer", &WVoxelMesh::GetPositionsRenderBuffer), InstanceMethod("getColoursRenderBuffer", &WVoxelMesh::GetColoursRenderBuffer)});

    Napi::FunctionReference *constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("VoxelMesh", func);
    return exports;
}

WVoxelMesh::WVoxelMesh(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<WVoxelMesh>(info), VoxelMesh()
{
}

Napi::Value WVoxelMesh::SetVertices(const Napi::CallbackInfo &info)
{
    if (info.Length() != 1)
    {
        Napi::Error::New(info.Env(), "Expected exactly one argument")
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    if (!info[0].IsArrayBuffer())
    {
        Napi::Error::New(info.Env(), "Expected an ArrayBuffer")
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    Napi::ArrayBuffer buf = info[0].As<Napi::ArrayBuffer>();

    const float *data = reinterpret_cast<float *>(buf.Data());
    const size_t length = buf.ByteLength() / sizeof(float);
    VoxelMesh::InternalSetVertices(data, length);

    return info.Env().Undefined();
}

Napi::Value WVoxelMesh::SetTexcoords(const Napi::CallbackInfo &info)
{
    if (info.Length() != 1)
    {
        Napi::Error::New(info.Env(), "Expected exactly one argument")
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    if (!info[0].IsArrayBuffer())
    {
        Napi::Error::New(info.Env(), "Expected an ArrayBuffer")
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    Napi::ArrayBuffer buf = info[0].As<Napi::ArrayBuffer>();

    const float *data = reinterpret_cast<float *>(buf.Data());
    const size_t length = buf.ByteLength() / sizeof(float);
    VoxelMesh::InternalSetTexcoords(data, length);

    return info.Env().Undefined();
}

Napi::Value WVoxelMesh::IsVoxelAt(const Napi::CallbackInfo &info)
{
    const int32_t x = info[0].As<Napi::Number>().Int32Value();
    const int32_t y = info[1].As<Napi::Number>().Int32Value();
    const int32_t z = info[2].As<Napi::Number>().Int32Value();

    //const bool is_voxel = this->voxel_hash_.find(Vec3_i32(x, y, z).hash()) != this->voxel_hash_.end();
    const bool is_voxel = false;
    return Napi::Boolean::New(info.Env(), is_voxel);
}

Napi::Value WVoxelMesh::GetVoxelIndex(const Napi::CallbackInfo &info)
{
    const int32_t x = info[0].As<Napi::Number>().Int32Value();
    const int32_t y = info[1].As<Napi::Number>().Int32Value();
    const int32_t z = info[2].As<Napi::Number>().Int32Value();

    //const auto &tmp = this->voxel_hash_.find(Vec3_i32(x, y, z).hash());
    //const bool is_new_voxel = tmp == this->voxel_hash_.end();

    //const int32_t voxel_index = is_new_voxel ? -1 : tmp->second;
    const int32_t voxel_index = -1;

    return Napi::Number::New(info.Env(), voxel_index);
}

Napi::Value WVoxelMesh::GetNeighbours(const Napi::CallbackInfo &info)
{
    const int32_t x = info[0].As<Napi::Number>().Int32Value();
    const int32_t y = info[1].As<Napi::Number>().Int32Value();
    const int32_t z = info[2].As<Napi::Number>().Int32Value();

    //const auto &tmp = this->neighbour_map_.find(Vec3_i32(x, y, z).hash());
    //const bool is_new_voxel = tmp == this->neighbour_map_.end();

    //const int32_t neighbour_value = is_new_voxel ? -1 : tmp->second;
    const int32_t neighbour_value = -1;


    return Napi::Number::New(info.Env(), neighbour_value);
}

Napi::Value WVoxelMesh::SetTriangles(const Napi::CallbackInfo &info)
{
    if (info.Length() != 1)
    {
        Napi::Error::New(info.Env(), "Expected exactly one argument")
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    if (!info[0].IsArrayBuffer())
    {
        Napi::Error::New(info.Env(), "Expected an ArrayBuffer")
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    Napi::ArrayBuffer buf = info[0].As<Napi::ArrayBuffer>();

    const int32_t *data = reinterpret_cast<int32_t *>(buf.Data());
    const size_t length = buf.ByteLength() / sizeof(int32_t);
    InternalSetTriangles(data, length);

    return info.Env().Undefined();
}

Napi::Value WVoxelMesh::Voxelise(const Napi::CallbackInfo &info)
{
    const int32_t desired_height = info[0].As<Napi::Number>().Uint32Value();

    InternalVoxelise(desired_height);

    return info.Env().Undefined();
}

Napi::Value WVoxelMesh::GetVoxelBounds(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    std::vector<int32_t> bounds_encoded = {
        bounds_min.x, bounds_min.y, bounds_min.z, bounds_max.x, bounds_max.y, bounds_max.z};

    const auto buffer = Napi::ArrayBuffer::New(env, bounds_encoded.data(), bounds_encoded.size() * sizeof(int32_t));
    return Napi::Int32Array::New(env, bounds_encoded.size(), buffer, 0);
}

Napi::Value WVoxelMesh::GetVoxelCount(const Napi::CallbackInfo &info)
{
    return Napi::Number::New(info.Env(), this->voxel_positions_.size() / 3);
}

Napi::Value WVoxelMesh::GetVoxelPositions(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    const auto buffer = Napi::ArrayBuffer::New(env, this->voxel_positions_.data(), this->voxel_positions_.size() * sizeof(int32_t));
    return Napi::Int32Array::New(env, this->voxel_positions_.size(), buffer, 0);
}

Napi::Value WVoxelMesh::GetVoxelColours(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    const auto buffer = Napi::ArrayBuffer::New(env, this->voxel_colours_.data(), this->voxel_colours_.size() * sizeof(float));
    return Napi::Float32Array::New(env, voxel_colours_.size(), buffer, 0);
}

Napi::Value WVoxelMesh::GetPositionsRenderBuffer(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    const int32_t start_index = info[0].As<Napi::Number>().Int32Value();
    const int32_t end_index = info[1].As<Napi::Number>().Int32Value();

    std::vector<float> positions = this->render_buffer_->GetPositions(start_index, end_index);

    const auto buffer = Napi::ArrayBuffer::New(env, positions.data(), positions.size() * sizeof(float));
    return Napi::Float32Array::New(env, positions.size(), buffer, 0);
}

Napi::Value WVoxelMesh::GetColoursRenderBuffer(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    const int32_t start_index = info[0].As<Napi::Number>().Int32Value();
    const int32_t end_index = info[1].As<Napi::Number>().Int32Value();

    std::vector<float> colours = this->render_buffer_->GetColours(start_index, end_index);

    const auto buffer = Napi::ArrayBuffer::New(env, colours.data(), colours.size() * sizeof(float));
    return Napi::Float32Array::New(env, colours.size(), buffer, 0);
}