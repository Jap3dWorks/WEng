#include "WSpacers.h"
#include "WEngine.h"
#include "WRender.h"
#include "WCore/WCore.h"
#include "WImporters.h"
#include "WActors/WActor.h"
#include <vector>

#include <iostream>

int main(int argc, char** argv)
{
    WImportObj importer;

    std::vector<WAsset*> actor = importer.Import(
        "file.obj", 
        "/Content/Assets/modelobj.wobj"
    );

    // WSpacers::WEngine engine;
    // engine.run();
    
    std::cout << "Test WSpacers!" << std::endl;
    return 0;
}
