// Game.cpp
// Main game implementation for the lunar lander simulation

#include "Game.h"
#include "Entity.h"
#include "Physics.h"
#include "Terrain.h"
#include "../rendering/Renderer.h"
#include "../rendering/Renderer2D.h"
#include "../rendering/Renderer3D.h"
#include "../input/InputHandler.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>

Game::Game()
    : mGameState(GameState::READY)
    , mDifficulty(Difficulty::NORMAL)
    , m3DMode(false)
    , mScore(0.0f)
    , mElapsedTime(0.0f)
    , mFuelUsed(0.0f)
    , mLastFrameTime(0)
    , mWindowWidth(800)
    , mWindowHeight(600)
    , mIsRunning(false)
{
}

Game::~Game() {
    Shutdown();
}

bool Game::Initialize() {
    // Create core game components
    mWindowWidth = 800;
    mWindowHeight = 600;
    mLander = std::make_unique<Lander>();
    mTerrain = std::make_unique<Terrain>();
    mPhysics = std::make_unique<Physics>();
    mInputHandler = std::make_unique<InputHandler>(this);

    std::cout << "Creating renderer - 3D mode: " << (m3DMode ? "true" : "false") << std::endl; // Debug output

    // Create renderer (2D or 3D based on setting)
    if (m3DMode) {
        #ifdef USE_OPENGL
            mRenderer = std::make_unique<Renderer3D>();
            std::cout << "Created 3D renderer" << std::endl; // Debug output
        #else
            std::cout << "OpenGL support not available. Falling back to 2D renderer." << std::endl;
            m3DMode = false;
            mRenderer = std::make_unique<Renderer2D>();
            std::cout << "Created 2D renderer" << std::endl; // Debug output
        #endif
    } else {
        mRenderer = std::make_unique<Renderer2D>();
        std::cout << "Created 2D renderer" << std::endl; // Debug output
    }
    
    // Initialize renderer
    if (!mRenderer->Initialize(mWindowWidth, mWindowHeight, "Lunar Lander Simulator")) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return false; 
    } else { 
        std::cout << "Renderer initialized successfully" << std::endl; // Debug output
    }
    
    // Register entities with physics
    mPhysics->RegisterLander(mLander.get());
    mPhysics->RegisterTerrain(mTerrain.get());
    
    // Initialize terrain
    if (m3DMode) {
        mTerrain->Generate3D(mWindowWidth, mWindowWidth, mWindowHeight);
    } else {
        mTerrain->Generate2D(mWindowWidth, mWindowHeight);
    }
    
    // Reset game state
    Reset();
    
    mIsRunning = true;
    mLastFrameTime = SDL_GetTicks();
    
    return true;
}

void Game::Run() {
 // Add debugging output
 std::cout << "Starting game loop..." << std::endl;

    if (!mIsRunning) {
        std::cerr << "Game not initialized!" << std::endl;
        return;
    }
    
    // Main game loop
    while (mIsRunning) {
        // Calculate delta time
        unsigned int currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - mLastFrameTime) / 1000.0f;
        mLastFrameTime = currentTime;
        
        // Cap delta time to prevent physics issues on lag spikes
        if (deltaTime > 0.1f) {
            deltaTime = 0.1f;
        }
        
        // Process input, update game state, and render
        ProcessInput();
        Update(deltaTime);
        Render();
        
        // Small delay to prevent 100% CPU usage
        SDL_Delay(1);
    }
}

void Game::Shutdown() {
    mIsRunning = false;
    
    // Clean up components in reverse order of creation
    mInputHandler.reset();
    mRenderer.reset();
    mPhysics.reset();
    mTerrain.reset();
    mLander.reset();
    
    // Quit SDL
    SDL_Quit();
}

void Game::SetDifficulty(Difficulty difficulty) {
    mDifficulty = difficulty;
    
    // Adjust physics parameters based on difficulty
    switch (mDifficulty) {
        case Difficulty::EASY:
            mPhysics->SetGravity(1.0f);  // Lower gravity
            mLander->ApplyThrust(0.0f);  // Start with no thrust
            break;
            
        case Difficulty::NORMAL:
            mPhysics->SetGravity(1.62f); // Lunar gravity
            mLander->ApplyThrust(0.0f);  // Start with no thrust
            break;
            
        case Difficulty::HARD:
            mPhysics->SetGravity(2.0f);  // Higher gravity
            mLander->ApplyThrust(0.0f);  // Start with no thrust
            break;
    }
    
    // Reset the game with new settings
    Reset();
}

void Game::SetRenderingMode(bool use3D) {
    // Only change if needed
    if (m3DMode != use3D) {
        m3DMode = use3D;
        
        // Reinitialize if the game is already running
        if (mIsRunning) {
            Shutdown();
            Initialize();
        }
    }
}

void Game::Reset() {
    // Reset game state
    mGameState = GameState::FLYING;
    mScore = 0.0f;
    mElapsedTime = 0.0f;
    mFuelUsed = 0.0f;
    
    // Reset lander
    if (mLander) {
        mLander->Reset();
        mLander->SetActive(true);  // Explicitly set active flag
        
        // Set initial position based on mode
        if (m3DMode) {
            mLander->SetPosition(mWindowWidth / 2,mWindowHeight / 3, mWindowWidth / 2);
        } else {
            mLander->SetPosition(mWindowWidth / 2, 100);
        }

        std::cout << "Lander reset: Active=" << (mLander->IsActive() ? "true" : "false") 
              << ", Position=(" << mLander->GetPosition()[0] << "," 
              << mLander->GetPosition()[1] << ")" << std::endl;
    }
    
    // Reset terrain (regenerate)
    if (mTerrain) {
        if (m3DMode) {
            mTerrain->Generate3D(mWindowWidth, mWindowWidth, mWindowHeight);
        } else {
            mTerrain->Generate2D(mWindowWidth, mWindowHeight);
        }
    }
    // Add debugging output
    std::cout << "Game reset. Lander position: " << mLander->GetPosition()[0] 
              << ", " << mLander->GetPosition()[1] << std::endl;
}

void Game::ProcessInput() {
    // Use the input handler to process input
    if (mInputHandler) {
        mInputHandler->ProcessInput();
        
        // Handle input based on game state
        if (mGameState == GameState::READY) {
            // Check for game start
            if (mInputHandler->IsStartActive()) {
                mGameState = GameState::FLYING;
            }
        } else if (mGameState == GameState::FLYING) {
            // Apply thrust if active
            if (mInputHandler->IsThrustActive()) {
                mLander->ApplyThrust(1.0f);
                
                // Track fuel usage
                float originalFuel = mLander->GetFuel();
                float newFuel = originalFuel;
                if (originalFuel > newFuel) {
                    mFuelUsed += originalFuel - newFuel;
                }
            } else {
                mLander->ApplyThrust(0.0f);
            }
            
            // Handle rotation
            if (mInputHandler->IsRotateLeftActive()) {
                mLander->RotateLeft(2.0f);
            }
            
            if (mInputHandler->IsRotateRightActive()) {
                mLander->RotateRight(2.0f);
            }
        } else if (mGameState == GameState::LANDED || mGameState == GameState::CRASHED) {
            // Check for game reset
            if (mInputHandler->IsResetActive()) {
                Reset();
            }
        }
        
        // Check for quit
        if (mInputHandler->IsQuitActive()) {
            mIsRunning = false;
        }
    }
}

void Game::Update(float deltaTime) {
    // Only update physics when flying
    if (mGameState == GameState::FLYING) {
        // Update physics
        if (mPhysics) {
            mPhysics->Update(deltaTime);
        }
        
        // Update lander
        if (mLander) {
            mLander->Update(deltaTime);
            
            // Check if landed or crashed
            if (mLander->IsLanded()) {
                mGameState = GameState::LANDED;
                
                // Calculate score based on fuel remaining and landing position
                float fuelRemaining = mLander->GetFuel() / mLander->GetMaxFuel();
                mScore = fuelRemaining * 1000.0f;
                
                std::cout << "Landing successful! Score: " << mScore << std::endl;
            } else if (mLander->IsCrashed()) {
                mGameState = GameState::CRASHED;
                mScore = 0.0f;
                
                std::cout << "Crash landing! Score: " << mScore << std::endl;
            }
        }
        
        // Update elapsed time
        mElapsedTime += deltaTime;
    }

   // Add fall time debugging code - declare static variables once outside the conditionals
   static float fallStartTime = 0;
   static bool fallTimerStarted = false;
   
   if (mGameState == GameState::FLYING) {
       if (!fallTimerStarted) {
           fallStartTime = mElapsedTime;
           fallTimerStarted = true;
           std::cout << "=== FALL TEST STARTED ===" << std::endl;
       }
       
       // Convert height to meters (assuming 20 pixels = 1 meter)
       float currentHeight = mLander->GetPosition()[1] / 20.0f;
       std::cout << "Height: " << currentHeight << "m, Time: " 
               << (mElapsedTime - fallStartTime) << "s" << std::endl;
               
       // Check if landed or crashed
       if (mLander->IsLanded() || mLander->IsCrashed()) {
           std::cout << "=== FALL TEST ENDED ===\nTotal fall time: " 
                   << (mElapsedTime - fallStartTime) << "s" << std::endl;
           fallTimerStarted = false;
       }
   }
   else {
       // Reset fall timer when not flying
       fallTimerStarted = false;
   }  

    // Update terrain (generally static, but may have animations)
    if (mTerrain) {
        mTerrain->Update(deltaTime);
    }
    
    // If 3D mode, update camera to follow lander
    if (m3DMode && mRenderer && mLander) {
        const float* landerPos = mLander->GetPosition();
        
        // Position camera based on lander position
        mRenderer->SetCameraPosition(
            landerPos[0] - 100.0f, 
            landerPos[1] - 100.0f, 
            landerPos[2] + 100.0f
        );
        
        // Target camera at lander
        mRenderer->SetCameraTarget(landerPos[0], landerPos[1], landerPos[2]);
        
        // Set camera up vector
        mRenderer->SetCameraUp(0.0f, 1.0f, 0.0f);
        
        // Set light position above terrain
        mRenderer->SetLightPosition(
            mWindowWidth / 2, 
            mWindowHeight + 500.0f, 
            mWindowWidth / 2
        );
        
        // Set ambient light
        mRenderer->SetAmbientLight(0.3f, 0.3f, 0.3f);
    }
}

void Game::Render() {
    // Clear the screen
    if (mRenderer) {
        std::cout << "Rendering frame..." << std::endl; // Debug output
        mRenderer->Clear();
        
        // Render terrain
        if (mTerrain) {
            std::cout << "Rendering terrain" << std::endl; // Debug output
            mTerrain->Render(mRenderer.get());
        }
        
        // Render lander
        if (mLander) {
            std::cout << "About to render lander at position: " << mLander->GetPosition()[0] 
                      << ", " << mLander->GetPosition()[1] << std::endl;
            
            // Call RenderLander directly instead of through Lander::Render
            mRenderer->RenderLander(mLander.get());
            
            std::cout << "Lander render complete" << std::endl;
        }
        
        // Render UI elements
        mRenderer->RenderTelemetry(this);
        mRenderer->RenderGameState(this);
        
        // Present rendered frame
        mRenderer->Present();
    } else { 
        std::cout << "Renderer is null!" << std::endl; // Debug output
    }
}

void Game::OnKeyDown(int keyCode) {
    // Handle key press events
    switch (keyCode) {
        case SDLK_r:
            // Reset game
            Reset();
            break;
            
        case SDLK_ESCAPE:
            // Quit game
            mIsRunning = false;
            break;
            
        case SDLK_1:
            // Set easy difficulty
            SetDifficulty(Difficulty::EASY);
            break;
            
        case SDLK_2:
            // Set normal difficulty
            SetDifficulty(Difficulty::NORMAL);
            break;
            
        case SDLK_3:
            // Set hard difficulty
            SetDifficulty(Difficulty::HARD);
            break;
            
        case SDLK_TAB:
            // Toggle between 2D and 3D mode
            SetRenderingMode(!m3DMode);
            break;
    }
}

void Game::OnKeyUp(int keyCode) {
    // Handle key release events
    // Currently no additional functionality needed here
}