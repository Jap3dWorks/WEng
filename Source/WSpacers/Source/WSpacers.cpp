#include "WSpacers.h"

#include "WEngine.h"
#include "WRender.h"
#include "WCore/WCore.h"
#include "WImporters.h"
#include "WActors/WActor.h"
#include "WAssets/WStaticModel.h"
#include <vector>

#include <iostream>

int main(int argc, char** argv)
{
    WImportObj importer;

    std::vector<WAsset*> actor = importer.Import(
        "../Content/Assets/Models/viking_room.obj", 
        "/Content/Assets/modelobj.modelobj"
    );
    
    if (actor.size() < 1)
    {
        std::cout << "Failed to import actor!" << std::endl;
        return 1;
    }

    if (actor[0]->GetClass() != WStaticModel::GetStaticClass())
    {
        std::cout << "Actor is not a static model!" << std::endl;
        return 1;
    }
    
    WStaticModel* static_model = static_cast<WStaticModel*>(actor[0]);
    static_model->GetModel().meshes[0];

    std::cout << "Test WSpacers!" << std::endl;
    return 0;
}

