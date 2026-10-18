#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

// Track if an early click is buffered in memory
bool g_isClickBuffered = false;

class $modify(MyGJBaseGameLayer, GJBaseGameLayer) {
    void handleButton(bool push, int button, bool isPlayer1) {
        // Intercept if Player 1 pushes the jump button
        if (push && button == 1 && isPlayer1) {
            auto player = m_player1;
            
            // If player clicks early (the engine blocks jumps right now)
            if (player && !player->m_canJump) {
                g_isClickBuffered = true; // Save it to frame-buffer
                return; // Silence the input so it doesn't cause a premature crash
            }
        }
        GJBaseGameLayer::handleButton(push, button, isPlayer1);
    }
};

class $modify(MyPlayerObject, PlayerObject) {
    void update(float dt) {
        // Run standard game physics step first
        PlayerObject::update(dt);
        
        if (g_isClickBuffered) {
            // Once the physics register that player hit the ground / real spot
            if (this->m_canJump) {
                g_isClickBuffered = false; // Empty the buffer slot
                
                // Force fire the click event perfectly on time
                auto gameLayer = GJBaseGameLayer::get();
                if (gameLayer) {
                    gameLayer->handleButton(true, 1, true); 
                }
            }
        }
    }
};
