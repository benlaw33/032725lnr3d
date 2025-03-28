// Entity.cpp
// Implementation of Entity and Lander classes

#include "Entity.h"
#include <algorithm>
#include <iostream>

// The Renderer.h include is problematic due to circular dependencies
// Instead, we'll use forward declarations and implement the minimal required functionality

// Initialize static ID counter
int Entity::sNextID = 0;

// Base Entity implementation
Entity::Entity() 
    : mActive(true)
    , mID(sNextID++)
    , mName("Entity")
{
    // Initialize position, rotation, and scale
    mPosition[0] = mPosition[1] = mPosition[2] = 0.0f;
    mRotation[0] = mRotation[1] = mRotation[2] = 0.0f;
    mScale[0] = mScale[1] = mScale[2] = 1.0f;
}

void Entity::SetPosition(float x, float y, float z) {
    mPosition[0] = x;
    mPosition[1] = y;
    mPosition[2] = z;
}

void Entity::SetRotation(float x, float y, float z) {
    mRotation[0] = x;
    mRotation[1] = y;
    mRotation[2] = z;
}

void Entity::SetScale(float x, float y, float z) {
    mScale[0] = x;
    mScale[1] = y;
    mScale[2] = z;
}

// Lander implementation
Lander::Lander()
    : Entity()
    , mWidth(20.0f)
    , mHeight(30.0f)
    , mDepth(20.0f)  // For 3D
    , mMass(10000.0f)  // 10 metric tons
    , mThrustLevel(0.0f)
    , mThrustActive(false)
    , mMaxThrustForce(50000.0f)  // 50 kN
    , mFuel(1000.0f)
    , mMaxFuel(1000.0f)
    , mFuelConsumptionRate(10.0f)  // Units per second
    , mLanded(false)
    , mCrashed(false)
{
    mName = "Lander";
    
    // Initialize velocity and acceleration
    mVelocity[0] = mVelocity[1] = mVelocity[2] = 0.0f;
    mAcceleration[0] = mAcceleration[1] = mAcceleration[2] = 0.0f;
}

void Lander::Update(float deltaTime) {
    // Apply physics updates (will be handled by Physics system)
    // This is just a placeholder for the entity-specific update logic
    
    // Update position based on velocity
    mPosition[0] += mVelocity[0] * deltaTime;
    mPosition[1] += mVelocity[1] * deltaTime;
    mPosition[2] += mVelocity[2] * deltaTime;
    
    // Handle fuel consumption if thrust is active
    if (mThrustActive && mFuel > 0) {
        mFuel -= mFuelConsumptionRate * mThrustLevel * deltaTime;
        mFuel = std::max(0.0f, mFuel);
        
        if (mFuel <= 0) {
            mThrustActive = false;
        }
    }
}

void Lander::Render(Renderer* renderer) {
    // This implementation would normally delegate to the renderer
    // Due to circular dependency, we'll let the Game class handle this
    // The renderer will call appropriate methods to render the lander
    
    // Note: In a full implementation, this would call:
    // renderer->RenderLander(this);
    // But we can't do that here due to header include order
}

void Lander::ApplyThrust(float amount) {
    if (mFuel <= 0) {
        mThrustActive = false;
        mThrustLevel = 0.0f;
        return;
    }
    
    mThrustLevel = std::max(0.0f, std::min(1.0f, amount));
    mThrustActive = mThrustLevel > 0.0f;
}

void Lander::RotateLeft(float amount) {
    mRotation[2] += amount;
    // Normalize rotation to 0-360 degrees
    while (mRotation[2] >= 360.0f) mRotation[2] -= 360.0f;
}

void Lander::RotateRight(float amount) {
    mRotation[2] -= amount;
    // Normalize rotation to 0-360 degrees
    while (mRotation[2] < 0.0f) mRotation[2] += 360.0f;
}

void Lander::Reset() {
    // Reset position
    SetPosition(0.0f, 100.0f, 0.0f);
    
    // Reset rotation
    SetRotation(0.0f, 0.0f, 0.0f);
    
    // Reset velocity and acceleration
    mVelocity[0] = mVelocity[1] = mVelocity[2] = 0.0f;
    mAcceleration[0] = mAcceleration[1] = mAcceleration[2] = 0.0f;
    
    // Reset thrust
    mThrustLevel = 0.0f;
    mThrustActive = false;
    
    // Reset fuel
    mFuel = mMaxFuel;
    
    // Reset landing status
    mLanded = false;
    mCrashed = false;
}
