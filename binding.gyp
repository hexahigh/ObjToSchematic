{
    "targets": [
        {
            "target_name": "addon",
            "cflags!": ["-fno-exceptions"],
            "cflags_cc!": ["-fno-exceptions"],
            "sources": ["./src/cpp/Addon.cc", "./src/cpp/VoxelMesh.cc", "./src/cpp/wrapper/WVoxelMesh.cc", "./src/cpp/util/Geometry.cc", "./src/cpp/VoxelMeshRenderBuffer.cc", "./src/cpp/util/Timer.cc", "./src/cpp/Texture.cc"],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")"
            ],
            'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
        }
    ]
}
