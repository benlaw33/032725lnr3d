// Renderer3D.cpp
// Implementation of the 3D renderer using OpenGL and SDL2

#include "Renderer3D.h"
#include "../core/Entity.h"
#include "../core/Terrain.h"
#include "../core/Game.h"
#include <iostream>
#include <cmath>

// Include OpenGL headers
#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

// Simple vertex and fragment shaders for basic lighting
const char* vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aNormal;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    out vec3 FragPos;
    out vec3 Normal;
    
    void main() {
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal;
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 FragPos;
    in vec3 Normal;
    
    uniform vec3 lightPos;
    uniform vec3 ambientLight;
    uniform vec3 objectColor;
    
    out vec4 FragColor;
    
    void main() {
        // Ambient light
        vec3 ambient = ambientLight * objectColor;
        
        // Diffuse light
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * vec3(1.0, 1.0, 1.0) * objectColor;
        
        // Combine lights
        vec3 result = ambient + diffuse;
        FragColor = vec4(result, 1.0);
    }
)";

Renderer3D::Renderer3D()
    : mWindow(nullptr)
    , mGLContext(nullptr)
    , mWidth(800)
    , mHeight(600)
    , mInitialized(false)
    , mShaderProgram(0)
    , mModelMatrixLocation(0)
    , mViewMatrixLocation(0)
    , mProjectionMatrixLocation(0)
    , mLightPositionLocation(0)
    , mAmbientLightLocation(0)
    , mLanderModel(0)
    , mLanderVertexCount(0)
{
    // Initialize camera position
    mCameraPosition[0] = 0.0f;
    mCameraPosition[1] = 100.0f;
    mCameraPosition[2] = 200.0f;
    
    // Initialize camera target
    mCameraTarget[0] = 0.0f;
    mCameraTarget[1] = 0.0f;
    mCameraTarget[2] = 0.0f;
    
    // Initialize camera up vector
    mCameraUp[0] = 0.0f;
    mCameraUp[1] = 1.0f;
    mCameraUp[2] = 0.0f;
    
    // Initialize light position
    mLightPosition[0] = 500.0f;
    mLightPosition[1] = 1000.0f;
    mLightPosition[2] = 500.0f;
    
    // Initialize ambient light
    mAmbientLight[0] = 0.3f;
    mAmbientLight[1] = 0.3f;
    mAmbientLight[2] = 0.3f;
}

Renderer3D::~Renderer3D() {
    Shutdown();
}

bool Renderer3D::Initialize(int width, int height, const std::string& title) {
    // Store dimensions
    mWidth = width;
    mHeight = height;
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    // Create window with OpenGL context
    mWindow = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        mWidth,
        mHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
    
    if (!mWindow) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Create OpenGL context
    mGLContext = SDL_GL_CreateContext(mWindow);
    if (!mGLContext) {
        std::cerr << "OpenGL context creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Set vsync
    SDL_GL_SetSwapInterval(1);
    
    // Initialize OpenGL
    if (!InitializeOpenGL()) {
        std::cerr << "OpenGL initialization failed!" << std::endl;
        return false;
    }
    
    // Load shaders
    if (!LoadShaders()) {
        std::cerr << "Shader loading failed!" << std::endl;
        return false;
    }
    
    // Load models
    if (!LoadModels()) {
        std::cerr << "Model loading failed!" << std::endl;
        return false;
    }
    
    mInitialized = true;
    return true;
}

bool Renderer3D::InitializeOpenGL() {
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    
    // Enable backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // Set clear color
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    
    // Create projection matrix
    float aspectRatio = (float)mWidth / (float)mHeight;
    mProjectionMatrix = CreateProjectionMatrix(45.0f, aspectRatio, 0.1f, 1000.0f);
    
    return true;
}

bool Renderer3D::LoadShaders() {
    // For a real implementation, this would load and compile shaders
    // Since we're using a simplified approach, we'll just create a dummy shader program
    mShaderProgram = 1; // Dummy value, actual implementation would use glCreateProgram()
    
    // Set uniform locations (in a real implementation, these would be real locations)
    mModelMatrixLocation = 1;
    mViewMatrixLocation = 2;
    mProjectionMatrixLocation = 3;
    mLightPositionLocation = 4;
    mAmbientLightLocation = 5;
    
    return true;
}

bool Renderer3D::LoadModels() {
    // For a real implementation, this would load 3D models from files
    // For simplicity, we'll just create placeholder values
    
    // Create a simple cube model for the lander
    mLanderModel = 1; // Dummy value
    mLanderVertexCount = 36; // A cube has 36 vertices (6 faces, 2 triangles per face, 3 vertices per triangle)
    
    return true;
}

void Renderer3D::Shutdown() {
    // Clean up OpenGL resources
    if (mShaderProgram) {
        // In a real implementation: glDeleteProgram(mShaderProgram);
        mShaderProgram = 0;
    }
    
    if (mLanderModel) {
        // In a real implementation: glDeleteVertexArrays(1, &mLanderModel);
        mLanderModel = 0;
    }
    
    // Delete OpenGL context
    if (mGLContext) {
        SDL_GL_DeleteContext(mGLContext);
        mGLContext = nullptr;
    }
    
    // Destroy window
    if (mWindow) {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }
    
    mInitialized = false;
}

void Renderer3D::Clear() {
    if (!mInitialized) return;
    
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer3D::Present() {
    if (!mInitialized) return;
    
    // Swap buffers
    SDL_GL_SwapWindow(mWindow);
}

void Renderer3D::RenderLander(Lander* lander) {
    if (!mInitialized || !lander) return;
    
    // Get lander properties
    const float* position = lander->GetPosition();
    const float* rotation = lander->GetRotation();
    const float* scale = lander->GetScale();
    
    // In a real implementation, this would use modern OpenGL to render the lander model
    // For this example, we'll use legacy OpenGL functions
    
    // Save current matrix
    glPushMatrix();
    
    // Apply transformations
    glTranslatef(position[0], position[1], position[2]);
    glRotatef(rotation[0], 1.0f, 0.0f, 0.0f);
    glRotatef(rotation[1], 0.0f, 1.0f, 0.0f);
    glRotatef(rotation[2], 0.0f, 0.0f, 1.0f);
    glScalef(scale[0], scale[1], scale[2]);
    
    // Render a simplified lander
    // In practice, you would bind proper shader, set uniforms, and render a real model
    glBegin(GL_QUADS);
    
    // Lander body (white)
    glColor3f(1.0f, 1.0f, 1.0f);
    
    float width = lander->GetWidth() / 2.0f;
    float height = lander->GetHeight() / 2.0f;
    float depth = lander->GetDepth() / 2.0f;
    
    // Front face
    glVertex3f(-width, -height, depth);
    glVertex3f(width, -height, depth);
    glVertex3f(width, height, depth);
    glVertex3f(-width, height, depth);
    
    // Back face
    glVertex3f(-width, -height, -depth);
    glVertex3f(-width, height, -depth);
    glVertex3f(width, height, -depth);
    glVertex3f(width, -height, -depth);
    
    // Left face
    glVertex3f(-width, -height, depth);
    glVertex3f(-width, height, depth);
    glVertex3f(-width, height, -depth);
    glVertex3f(-width, -height, -depth);
    
    // Right face
    glVertex3f(width, -height, depth);
    glVertex3f(width, -height, -depth);
    glVertex3f(width, height, -depth);
    glVertex3f(width, height, depth);
    
    // Top face
    glVertex3f(-width, height, depth);
    glVertex3f(width, height, depth);
    glVertex3f(width, height, -depth);
    glVertex3f(-width, height, -depth);
    
    // Bottom face
    glVertex3f(-width, -height, depth);
    glVertex3f(-width, -height, -depth);
    glVertex3f(width, -height, -depth);
    glVertex3f(width, -height, depth);
    
    glEnd();
    
    // Draw thrust flame if active
    if (lander->IsThrustActive()) {
        glBegin(GL_TRIANGLES);
        
        // Flame (orange)
        glColor3f(1.0f, 0.5f, 0.0f);
        
        // Calculate thrust direction based on rotation
        float rotRad = rotation[2] * M_PI / 180.0f;
        float thrustDirX = sin(rotRad);
        float thrustDirY = -cos(rotRad);
        
        // Flame position
        float flameBaseX = 0.0f;
        float flameBaseY = -height;
        float flameLength = height * lander->GetThrustLevel();
        
        // Draw flame
        glVertex3f(flameBaseX - width/4, flameBaseY, depth/2);
        glVertex3f(flameBaseX + width/4, flameBaseY, depth/2);
        glVertex3f(flameBaseX + thrustDirX * flameLength, 
                   flameBaseY + thrustDirY * flameLength, 0.0f);
        
        glVertex3f(flameBaseX - width/4, flameBaseY, -depth/2);
        glVertex3f(flameBaseX + width/4, flameBaseY, -depth/2);
        glVertex3f(flameBaseX + thrustDirX * flameLength, 
                   flameBaseY + thrustDirY * flameLength, 0.0f);
        
        glEnd();
    }
    
    // Restore matrix
    glPopMatrix();
}

void Renderer3D::RenderTerrain(Terrain* terrain) {
    if (!mInitialized || !terrain) return;
    
    // Get terrain triangles
    const std::vector<TerrainTriangle>& triangles = terrain->GetTriangles3D();
    
    // Begin rendering terrain
    glBegin(GL_TRIANGLES);
    
    // Render each triangle
    for (const auto& triangle : triangles) {
        // Set color based on whether it's a landing pad
        if (triangle.isLandingPad) {
            glColor3f(0.0f, 0.8f, 0.0f); // Green for landing pads
        } else {
            glColor3f(0.5f, 0.5f, 0.5f); // Grey for regular terrain
        }
        
        // Set normal
        glNormal3fv(triangle.normal);
        
        // Draw triangle vertices
        glVertex3f(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2]);
        glVertex3f(triangle.vertices[3], triangle.vertices[4], triangle.vertices[5]);
        glVertex3f(triangle.vertices[6], triangle.vertices[7], triangle.vertices[8]);
    }
    
    glEnd();
}

void Renderer3D::RenderTelemetry(Game* game) {
    if (!mInitialized || !game) return;
    
    // Get lander
    Lander* lander = game->GetLander();
    if (!lander) return;
    
    // Get lander properties
    const float* position = lander->GetPosition();
    const float* velocity = lander->GetVelocity();
    float fuel = lander->GetFuel();
    float maxFuel = lander->GetMaxFuel();
    
    // Set up orthographic projection for 2D UI
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, mWidth, mHeight, 0.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Disable depth testing for UI
    glDisable(GL_DEPTH_TEST);
    
    // Draw background for telemetry panel
    glBegin(GL_QUADS);
    glColor4f(0.2f, 0.2f, 0.2f, 0.8f);
    glVertex2f(10, 10);
    glVertex2f(210, 10);
    glVertex2f(210, 110);
    glVertex2f(10, 110);
    glEnd();
    
    // Draw altitude bar
    float altitude = position[1];
    float maxAltitude = 500.0f;
    float altitudePct = altitude / maxAltitude;
    if (altitudePct > 1.0f) altitudePct = 1.0f;
    
    glBegin(GL_QUADS);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(20, 20);
    glVertex2f(20 + altitudePct * 180.0f, 20);
    glVertex2f(20 + altitudePct * 180.0f, 40);
    glVertex2f(20, 40);
    glEnd();
    
    // Draw velocity indicator
    float velocityPct = std::abs(velocity[1]) / 10.0f;
    if (velocityPct > 1.0f) velocityPct = 1.0f;
    
    glBegin(GL_QUADS);
    if (velocity[1] <= 0) {
        glColor3f(0.0f, 0.0f, 1.0f); // Blue for downward
    } else {
        glColor3f(1.0f, 0.0f, 0.0f); // Red for upward
    }
    glVertex2f(20, 50);
    glVertex2f(20 + velocityPct * 180.0f, 50);
    glVertex2f(20 + velocityPct * 180.0f, 70);
    glVertex2f(20, 70);
    glEnd();
    
    // Draw fuel indicator
    float fuelPct = fuel / maxFuel;
    
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex2f(20, 80);
    glVertex2f(20 + fuelPct * 180.0f, 80);
    glVertex2f(20 + fuelPct * 180.0f, 100);
    glVertex2f(20, 100);
    glEnd();
    
    // Restore projection matrix
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    // Re-enable depth testing
    glEnable(GL_DEPTH_TEST);
}

void Renderer3D::RenderGameState(Game* game) {
    if (!mInitialized || !game) return;
    
    // Get game state
    GameState state = game->GetGameState();
    
    // Set up orthographic projection for 2D UI
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, mWidth, mHeight, 0.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Disable depth testing for UI
    glDisable(GL_DEPTH_TEST);
    
    // Draw state message
    glBegin(GL_QUADS);
    
    switch (state) {
        case GameState::READY:
            // Draw "Press SPACE to start" message
            glColor3f(1.0f, 1.0f, 1.0f);
            glVertex2f(mWidth / 2 - 100, mHeight / 2);
            glVertex2f(mWidth / 2 + 100, mHeight / 2);
            glVertex2f(mWidth / 2 + 100, mHeight / 2 + 30);
            glVertex2f(mWidth / 2 - 100, mHeight / 2 + 30);
            break;
            
        case GameState::LANDED:
            // Draw "Landed Successfully!" message
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex2f(mWidth / 2 - 100, mHeight / 2);
            glVertex2f(mWidth / 2 + 100, mHeight / 2);
            glVertex2f(mWidth / 2 + 100, mHeight / 2 + 30);
            glVertex2f(mWidth / 2 - 100, mHeight / 2 + 30);
            break;
            
        case GameState::CRASHED:
            // Draw "Crashed!" message
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(mWidth / 2 - 100, mHeight / 2);
            glVertex2f(mWidth / 2 + 100, mHeight / 2);
            glVertex2f(mWidth / 2 + 100, mHeight / 2 + 30);
            glVertex2f(mWidth / 2 - 100, mHeight / 2 + 30);
            break;
            
        default:
            // No message for other states
            break;
    }
    
    glEnd();
    
    // Restore projection matrix
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    // Re-enable depth testing
    glEnable(GL_DEPTH_TEST);
}

void Renderer3D::SetCameraPosition(float x, float y, float z) {
    mCameraPosition[0] = x;
    mCameraPosition[1] = y;
    mCameraPosition[2] = z;
    
    // Update view matrix
    mViewMatrix = CreateViewMatrix();
}

void Renderer3D::SetCameraTarget(float x, float y, float z) {
    mCameraTarget[0] = x;
    mCameraTarget[1] = y;
    mCameraTarget[2] = z;
    
    // Update view matrix
    mViewMatrix = CreateViewMatrix();
}

void Renderer3D::SetCameraUp(float x, float y, float z) {
    mCameraUp[0] = x;
    mCameraUp[1] = y;
    mCameraUp[2] = z;
    
    // Update view matrix
    mViewMatrix = CreateViewMatrix();
}

void Renderer3D::SetLightPosition(float x, float y, float z) {
    mLightPosition[0] = x;
    mLightPosition[1] = y;
    mLightPosition[2] = z;
}

void Renderer3D::SetAmbientLight(float r, float g, float b) {
    mAmbientLight[0] = r;
    mAmbientLight[1] = g;
    mAmbientLight[2] = b;
}

GLuint Renderer3D::LoadShader(const char* vertexShaderSource, const char* fragmentShaderSource) {
    // This would compile and link shaders in a real implementation
    // For now, just return a dummy shader program ID
    return 1;
}

Matrix4x4 Renderer3D::CreateProjectionMatrix(float fov, float aspect, float near, float far) {
    Matrix4x4 result;
    
    float tanHalfFovy = tan(fov / 2.0f);
    
    result.values[0] = 1.0f / (aspect * tanHalfFovy);
    result.values[1] = 0.0f;
    result.values[2] = 0.0f;
    result.values[3] = 0.0f;
    
    result.values[4] = 0.0f;
    result.values[5] = 1.0f / tanHalfFovy;
    result.values[6] = 0.0f;
    result.values[7] = 0.0f;
    
    result.values[8] = 0.0f;
    result.values[9] = 0.0f;
    result.values[10] = -(far + near) / (far - near);
    result.values[11] = -1.0f;
    
    result.values[12] = 0.0f;
    result.values[13] = 0.0f;
    result.values[14] = -(2.0f * far * near) / (far - near);
    result.values[15] = 0.0f;
    
    return result;
}

Matrix4x4 Renderer3D::CreateViewMatrix() {
    Matrix4x4 result;
    
    // This would normally compute a view matrix from camera parameters
    // For simplicity, we're just returning an identity matrix
    for (int i = 0; i < 16; i++) {
        result.values[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    }
    
    return result;
}

Matrix4x4 Renderer3D::CreateModelMatrix(float* position, float* rotation, float* scale) {
    Matrix4x4 result;
    
    // This would normally compute a model matrix from transform parameters
    // For simplicity, we're just returning an identity matrix
    for (int i = 0; i < 16; i++) {
        result.values[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    }
    
    return result;
}

void Renderer3D::MultiplyMatrices(Matrix4x4& result, const Matrix4x4& a, const Matrix4x4& b) {
    // This would normally multiply two matrices
    // For simplicity, we're just copying the first matrix
    for (int i = 0; i < 16; i++) {
        result.values[i] = a.values[i];
    }
}