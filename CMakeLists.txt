cmake_minimum_required(VERSION 3.28)
project(DX CXX) # 建议加上语言 CXX

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 通过 vcpkg 查找 Assimp ---
# 确保你已经通过 vcpkg 安装了 Assimp (例如 .\vcpkg install assimp:x86-windows)
# 并且运行了 .\vcpkg integrate install
# 或者在配置 CMake 时通过 -DCMAKE_TOOLCHAIN_FILE=[path_to_vcpkg]/scripts/buildsystems/vcpkg.cmake 指定工具链
find_package(assimp CONFIG REQUIRED)

# --- 指定源文件 ---
# 只包含 .cpp 文件，并为窗口程序添加 WIN32
# 头文件 (.h) 不应该列在 add_executable 中
add_executable(DX WIN32
        Graphic.cpp
        # Graphic.h # 头文件不在这里列出
        common.cpp
        main.cpp
        CTransform.cpp
        # CTransform.h # 头文件不在这里列出
        CGameObject.cpp
        # CGameObject.h # 头文件不在这里列出
        CCamera.cpp
        # CCamera.h # 头文件不在这里列出
        Model.cpp
        # Model.h # 头文件不在这里列出
        Scene.cpp
        # Scene.h # 头文件不在这里列出
)

# --- 指定包含目录 (头文件搜索路径) ---
# 使用相对路径
target_include_directories(DX PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"                # 项目根目录 (用于 Graphic.h, head.h 等)
        "${CMAKE_CURRENT_SOURCE_DIR}/DX_function/DX/Include"  # DX 头文件目录
        # assimp::assimp 目标会自动处理其包含目录，通常不需要手动添加
)

# --- 指定库目录 (库文件搜索路径) ---
# 使用相对路径
# 注意：vcpkg 安装的库通常不需要手动指定链接目录，CMake 会通过工具链文件找到它们。
# 这里保留你原有的 DX 库目录设置。
target_link_directories(DX PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/DX_function/DX/Lib/x86"      # DX 库文件目录 (假设是 x86)
)

# --- 链接库 ---
# 使用 target_link_libraries 命令链接需要的库
target_link_libraries(DX PRIVATE
        d3d9        # DirectX 9 核心库
        d3dx9       # DirectX 9 辅助库
        user32      # Windows 用户界面库
        gdi32       # Windows 图形设备接口库
        winmm       # Windows 多媒体库
        assimp::assimp # <--- 添加 Assimp 库链接
)

# --- 编译定义 ---
target_compile_definitions(DX PRIVATE
        _HAS_STD_BYTE=0
        D3D_DISABLE_9EX
        # 如果 Graphic.cpp 或 main.cpp 需要 Unicode，取消下面这行的注释
        # UNICODE
        # _UNICODE
)

# --- 链接器标志 ---
# 注意: set(CMAKE_EXE_LINKER_FLAGS ...) 会影响所有后续目标，并且是全局性的。
# 对于特定目标的链接器标志，更推荐使用 target_link_options。
# "-static -static-libgcc -static-libstdc++ -static-libgfortran" 这些标志通常用于 MinGW/GCC 环境。
# 如果你使用 MSVC (Visual Studio)，这些标志可能不适用或导致问题。
# 如果你确实需要静态链接运行时库，MSVC 有不同的标志 (例如 /MT 或 /MTd)。
# 我暂时注释掉这一行，如果你确定需要并且知道它在你的编译环境中的作用，可以取消注释。
# set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++ -static-libgfortran")