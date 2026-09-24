#include "WCollision/Systems/CollisionSystems.hpp"
#include "WSystem/WSystem.hpp"
#include "WAssets/Level.hpp"


bool wcl::system::collision::INIT_SetupLevelCollision(wsm::SystemParameters const & parameters) {
    // Create CollisionDataComponent in primary Level if not exists.
    
    return false;
}

bool wcl::system::collision::PRE_CheckCollision(wsm::SystemParameters const & parameters) {
    // Run collision detection algorithms
    // should trigger collision events here?
    // Future implementations can use multithread design
    // When the time comes a POST_CollecCollision system will be required.

    return false;
}

bool wcl::system::collision::MID_TriggerCollisionEvents(wsm::SystemParameters const & parameters) {
    // Run detected collision events

    return false;
}

bool wcl::system::collision::END_CleanCollisionData(wsm::SystemParameters const & parameters) {
    // Clean or save collision data

    return false;
}
