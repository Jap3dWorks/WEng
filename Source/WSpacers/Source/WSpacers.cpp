#include "MonkeyLevel.hpp"
#include "GltfLevel.hpp"

#include "PlaneLevel.hpp"
#include "WEngine/WEngineDefaults.hpp"
#include "WLog.hpp"
#include "WCoreTypes/WEngineStructs.hpp"

#include <exception>
#include <glm/ext/matrix_transform.hpp>

bool Run(WEngine & engine)
{
    engine.Run();
    return true;
}

int main(int argc, char** argv)
{
    WFLOG("Initialize WSpacers App.");
    
    try
    {
        WFLOG("Create Engine Instance.");

        WEngine engine = weng::defaults::DefaultEngine();

        
        wid::WAssetId monkey_level_id = spacers::monkey::CreateMonkeyLevel(engine);
        wid::WAssetId gltflevel = spacers::gltflevel::CreateLevel(engine);
        wid::WAssetId planelevel = spacers::plane::CreateLevel(engine);

        // engine.StartupLevel(monkey_level_id);
        // engine.StartupLevel(planelevel);
        engine.StartupLevel(gltflevel);

        WFLOG("[INFO] Initialize While Loop");

        Run(engine);

        WFLOG("Clossing app ...");
    }
    catch(const std::exception& e)
    {
        WFLOG("[ERROR] {}", e.what());
        throw;
    }

    WFLOG("WSpacers App Ends");

    return 0;

}

