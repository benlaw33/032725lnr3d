// Renderer3D.h
// 3D rendering implementation using OpenGL and SDL2

#pragma once

#include "Renderer.h"
#include <SDL2/SDL.h>
#include <string>
#include <vector>

// Forward declaration for SDL_GLContext (it's an opaque type)
typedef void* SDL_GLContext;

// Placeholder - in a real implementation, include the appropriate OpenGL headers
// or use a library like GLEW or GLAD
typedef unsigned int GLuint;
struct Matrix4x4 {
    float values[16];
};

class Renderer3D : public Renderer {
public:
    Renderer3D();
    virtual ~Renderer3D();
    
    // Implement Renderer interface
    bool Initialize(int width, int height, const std::string& title) override;
    void Shutdown() override;
    void Clear() override;
    void Present() override;
    
    void RenderLander(Lander* lander) override;
    void RenderTerrain(Terrain* terrain) override;
    
    void RenderTelemetry(Game* game) override;
    void RenderGameState(Game* game) override;
    
    int GetWidth() const override { return mWidth; }
    int GetHeight() const override { return mHeight; }
    bool IsInitialized() const override { return mInitialized; }
    
    // 3D camera methods
    void SetCameraPosition(float x, float y, float z) override;
    void SetCameraTarget(float x, float y, float z) override;
    void SetCameraUp(float x, float y, float z) override;
    
    // 3D lighting methods
    void SetLightPosition(float x, float y, float z) override;
    void SetAmbientLight(float r, float g, float b) override;
    
private:
    // Initialize OpenGL
    bool InitializeOpenGL();
    
    // Load and compile shaders
    bool LoadShaders();
    
    // Load models
    bool LoadModels();
    
    // Helper methods for 3D rendering
    void SetupMVP();
    void RenderModel(GLuint modelVAO, int vertexCount, float* position, float* rotation, float* scale);
    
    // OpenGL shader methods
    GLuint LoadShader(const char* vertexShaderSource, const char* fragmentShaderSource);
    
    // 3D math helpers
    Matrix4x4 CreateProjectionMatrix(float fov, float aspect, float near, float far);
    Matrix4x4 CreateViewMatrix();
    Matrix4x4 CreateModelMatrix(float* position, float* rotation, float* scale);
    void MultiplyMatrices(Matrix4x4& result, const Matrix4x4& a, const Matrix4x4& b);
    
    // SDL and OpenGL context
    SDL_Window* mWindow;
    SDL_GLContext mGLContext;  // Now properly forward declared
    
    // Renderer properties
    int mWidth;
    int mHeight;
    bool mInitialized;
    
    // OpenGL shader program
    GLuint mShaderProgram;
    
    // Uniform locations
    GLuint mModelMatrixLocation;
    GLuint mViewMatrixLocation;
    GLuint mProjectionMatrixLocation;
    GLuint mLightPositionLocation;
    GLuint mAmbientLightLocation;
    
    // Models
    GLuint mLanderModel;
    int mLanderVertexCount;
    
    // Camera properties
    float mCameraPosition[3];
    float mCameraTarget[3];
    float mCameraUp[3];
    
    // Light properties
    float mLightPosition[3];
    float mAmbientLight[3];
    
    // Matrices
    Matrix4x4 mProjectionMatrix;
    Matrix4x4 mViewMatrix;
};