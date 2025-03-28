// Renderer2D.cpp
// Implementation of the 2D renderer using SDL2

#include "Renderer2D.h"
#include "../core/Entity.h"
#include "../core/Terrain.h"
#include "../core/Game.h"
#include <iostream>

Renderer2D::Renderer2D()
    : mWindow(nullptr)
    , mRenderer(nullptr)
    , mWidth(800)
    , mHeight(600)
    , mInitialized(false)
    , mPixelsPerMeter(20.0f)
{
}

Renderer2D::~Renderer2D() {
    Shutdown();
}

bool Renderer2D::Initialize(int width, int height, const std::string& title) {
    // Store dimensions
    mWidth = width;
    mHeight = height;
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Create window
    mWindow = SDL_CreateWindow(
        title.c_str(),          // Window title
        SDL_WINDOWPOS_CENTERED, // x position
        SDL_WINDOWPOS_CENTERED, // y position
        mWidth,                 // width
        mHeight,                // height
        0                       // flags
    );
    
    if (!mWindow) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Create renderer
    mRenderer = SDL_CreateRenderer(
        mWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    if (!mRenderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    mInitialized = true;
    return true;
}

void Renderer2D::Shutdown() {
    if (mRenderer) {
        SDL_DestroyRenderer(mRenderer);
        mRenderer = nullptr;
    }
    
    if (mWindow) {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }
    
    mInitialized = false;
}

void Renderer2D::Clear() {
    if (!mInitialized) return;
    
    // Clear screen with black background
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
    SDL_RenderClear(mRenderer);
}

void Renderer2D::Present() {
    if (!mInitialized) return;
    
    SDL_RenderPresent(mRenderer);
}

void Renderer2D::RenderLander(Lander* lander) {
    if (!mInitialized || !lander) {
        std::cout << "Failed to render lander: " << 
            (!mInitialized ? "Renderer not initialized" : "Lander is null") << std::endl;
        return;
    }
    std::cout << "Drawing lander at position: " << lander->GetPosition()[0] << ", " 
    << lander->GetPosition()[1] << std::endl;

    // Get lander properties
    const float* position = lander->GetPosition();
    float width = lander->GetWidth();
    float height = lander->GetHeight();

    // CRITICAL FIX: Convert physics coordinates to screen coordinates
    // In physics, Y increases upward, but in screen coordinates, Y increases downward
    float screenX = position[0];
    float screenY = mHeight - position[1]; // This is the key fix!

    std::cout << "Drawing lander at physics pos: " << position[0] << "," << position[1] 
    << " screen pos: " << screenX << "," << screenY << std::endl;

    // IMPORTANT: Make lander bigger and use a bright color so it's visible
    width = 40.0f;  // Increase width
    height = 60.0f; // Increase height

    
    // Draw lander body as a rectangle
    DrawRect(
        position[0] - width / 2, 
        position[1] - height / 2, 
        width, 
        height, 
        255, 0, 0
    );
    
    // Draw thrust flame if active
    if (lander->IsThrustActive()) {
        DrawRect(
            position[0] - width / 4,
            position[1] + height / 2,
            width / 2,
            height / 3,
            255, 165, 0
        );
    }
    std::cout << "Lander drawn with width: " << width << ", height: " << height << std::endl;
}

void Renderer2D::RenderTerrain(Terrain* terrain) {
    if (!mInitialized || !terrain) return;
    
    // Get terrain segments
    const std::vector<TerrainSegment>& segments = terrain->GetSegments2D();
    
    // Draw each terrain segment
    for (const auto& segment : segments) {
        // Use white for normal terrain and green for landing pads
        if (segment.isLandingPad) {
            DrawLine(segment.x1, segment.y1, segment.x2, segment.y2, 0, 255, 0);
        } else {
            DrawLine(segment.x1, segment.y1, segment.x2, segment.y2, 200, 200, 200);
        }
    }
}

void Renderer2D::RenderTelemetry(Game* game) {
    if (!mInitialized || !game) return;
    
    Lander* lander = game->GetLander();
    if (!lander) return;
    
    // Get lander properties
    const float* position = lander->GetPosition();
    const float* velocity = lander->GetVelocity();
    float fuel = lander->GetFuel();
    float maxFuel = lander->GetMaxFuel();
    
    // Background for telemetry panel
    DrawRect(10, 10, 200, 100, 50, 50, 50, 200);
    
    // Altitude indicator (green bar)
    float altitude = (mHeight - 50) - (position[1] + lander->GetHeight() / 2);
    float maxAltitude = mHeight - 150;
    float altitudePct = altitude / maxAltitude;
    
    DrawRect(20, 20, static_cast<int>(altitudePct * 180), 20, 0, 255, 0);
    
    // Velocity indicator (blue bar for positive, red for negative)
    float velocityPct = std::abs(velocity[1]) / (2.0f * 3 / mPixelsPerMeter);
    if (velocityPct > 1.0f) velocityPct = 1.0f;
    
    if (velocity[1] <= 0) {
        DrawRect(20, 50, static_cast<int>(velocityPct * 180), 20, 0, 0, 255);
    } else {
        DrawRect(20, 50, static_cast<int>(velocityPct * 180), 20, 255, 0, 0);
    }
    
    // Fuel indicator (yellow bar)
    float fuelPct = fuel / maxFuel;
    
    DrawRect(20, 80, static_cast<int>(fuelPct * 180), 20, 255, 255, 0);
}

void Renderer2D::RenderGameState(Game* game) {
    if (!mInitialized || !game) return;
    
    // Get game state
    GameState state = game->GetGameState();
    
    // Draw state message
    switch (state) {
        case GameState::READY:
            // Draw "Press SPACE to start" message
            DrawRect(mWidth / 2 - 100, mHeight / 2, 200, 30, 255, 255, 255);
            break;
            
        case GameState::LANDED:
            // Draw "Landed Successfully!" message
            DrawRect(mWidth / 2 - 100, mHeight / 2, 200, 30, 0, 255, 0);
            break;
            
        case GameState::CRASHED:
            // Draw "Crashed!" message
            DrawRect(mWidth / 2 - 100, mHeight / 2, 200, 30, 255, 0, 0);
            break;
            
        default:
            // No message for other states
            break;
    }
}

void Renderer2D::DrawRect(float x, float y, float width, float height, 
                          Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    if (!mInitialized) return;
    
    SDL_SetRenderDrawColor(mRenderer, r, g, b, a);
    
    SDL_Rect rect {
        static_cast<int>(x),
        static_cast<int>(y),
        static_cast<int>(width),
        static_cast<int>(height)
    };
    
    SDL_RenderFillRect(mRenderer, &rect);
}

void Renderer2D::DrawLine(float x1, float y1, float x2, float y2, 
                          Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    if (!mInitialized) return;
    
    SDL_SetRenderDrawColor(mRenderer, r, g, b, a);
    
    SDL_RenderDrawLine(
        mRenderer,
        static_cast<int>(x1),
        static_cast<int>(y1),
        static_cast<int>(x2),
        static_cast<int>(y2)
    );
}