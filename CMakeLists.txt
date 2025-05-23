cmake_minimum_required(VERSION 3.28)
project(DX CXX) # 建议加上语言 CXX

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 指定源文件 ---
# 只包含 .cpp 文件，并为窗口程序添加 WIN32
add_executable(DX WIN32
        Graphic.cpp
        Graphic.h
        common.cpp
        main.cpp
        CTransform.cpp
        CTransform.h
        CGameObject.cpp
        CGameObject.h
        CCamera.cpp
        CCamera.h
        Model.cpp
        Model.h
        Scene.cpp
        Scene.h
)

# --- 指定包含目录 (头文件搜索路径) ---
# 使用相对路径
target_include_directories(DX PRIVATE
        "${CMAKE_SOURCE_DIR}"                       # 项目根目录 (用于 Graphic.h, head.h)
        "${CMAKE_SOURCE_DIR}/DX_function/DX/Include"  # DX 头文件目录
)

# --- 指定库目录 (库文件搜索路径) ---
# 使用相对路径
target_link_directories(DX PRIVATE
        "${CMAKE_SOURCE_DIR}/DX_function/DX/Lib"      # DX 库文件目录
)

# --- 链接库 ---
# 使用 target_link_libraries 命令链接需要的库
target_link_libraries(DX PRIVATE
        d3d9        # DirectX 9 核心库
        d3dx9       # DirectX 9 辅助库
        user32      # Windows 用户界面库
        gdi32       # Windows 图形设备接口库
        winmm       # Windows 多媒体库
        # 如果需要，添加其他库
)

target_compile_definitions(DX PRIVATE _HAS_STD_BYTE=0
                                D3D_DISABLE_9EX
                                ) # 防止编译错误
# --- (可选) 定义宏 ---
# 如果 Graphic.cpp 或 main.cpp 需要 Unicode，取消下面这行的注释
# target_compile_definitions(DX PRIVATE UNICODE _UNICODE)

set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++ -static-libgfortran")
