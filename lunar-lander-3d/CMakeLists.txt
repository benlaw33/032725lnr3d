cmake_minimum_required(VERSION 3.10)
project(LunarLanderSimulator)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Special handling for Apple Silicon
if(APPLE)
    # Check if we're on ARM64 architecture
    execute_process(
        COMMAND uname -m
        OUTPUT_VARIABLE ARCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    if(${ARCH} STREQUAL "arm64")
        # Apple Silicon specific flags
        add_definitions(-DSDL_DISABLE_IMMINTRIN_H=1)
        add_definitions(-DSDL_DISABLE_MMX_H=1)
        add_definitions(-DSDL_DISABLE_MM3DNOW_H=1)
        set(CMAKE_OSX_ARCHITECTURES "arm64")
    endif()
endif()

# Find SDL2 package
find_package(SDL2 REQUIRED)

# Include directories
include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
)

# Source files
set(SOURCES
    src/main.cpp
    
    # Core files
    src/core/Entity.cpp
    src/core/Game.cpp
    src/core/Physics.cpp
    src/core/Terrain.cpp
    
    # Rendering files
    src/rendering/Renderer2D.cpp
    
    # Input files
    src/input/InputHandler.cpp
)

# Add the 3D renderer if OpenGL is found
find_package(OpenGL)
if(OPENGL_FOUND)
    add_definitions(-DUSE_OPENGL=1)
    list(APPEND SOURCES src/rendering/Renderer3D.cpp)
    include_directories(${OPENGL_INCLUDE_DIRS})
endif()

# Create executable
add_executable(LunarLander ${SOURCES})

# Link libraries
target_link_libraries(LunarLander
    ${SDL2_LIBRARIES}
)

# Link OpenGL if found
if(OPENGL_FOUND)
    target_link_libraries(LunarLander
        ${OPENGL_LIBRARIES}
    )
endif()

# Generate a Game.cpp implementation based on Phase 2 code
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "// Auto-generated Game.cpp" > ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "// This is a template that you should fill with your actual implementation" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "#include \"Game.h\"" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "#include \"Entity.h\"" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "#include \"Physics.h\"" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "#include \"Terrain.h\"" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "#include \"../rendering/Renderer.h\"" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "#include \"../rendering/Renderer2D.h\"" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "#include \"../input/InputHandler.h\"" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "#include <iostream>" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo " " >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "Game::Game() {}" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "Game::~Game() {}" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "bool Game::Initialize() { return true; }" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "void Game::Run() {}" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "void Game::Shutdown() {}" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "void Game::Reset() {}" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "void Game::ProcessInput() {}" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "void Game::Update(float deltaTime) {}" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "void Game::Render() {}" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "void Game::SetDifficulty(Difficulty difficulty) {}" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "void Game::SetRenderingMode(bool use3D) {}" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "void Game::OnKeyDown(int keyCode) {}" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "void Game::OnKeyUp(int keyCode) {}" >> ${CMAKE_SOURCE_DIR}/src/core/Game.cpp
    DEPENDS src/core/Game.h
)

# Copy any needed asset files
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets)