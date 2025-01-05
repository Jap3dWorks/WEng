#include "WSpacers.h"

#include "WEngine.h"
#include "WRender.h"
#include "WCore/WCore.h"
#include "WImporters.h"
#include "WActors/WActor.h"
#include "WAssets/WStaticModel.h"
#include "WAssets/WTextureAsset.h"
#include <exception>
#include <vector>

#include <iostream>

int main(int argc, char** argv)
{
    try
    {
        std::vector<WAsset*> geo_asset = WImportObj().Import(
            "../Content/Assets/Models/viking_room.obj", 
            "/Content/Assets/modelobj.modelobj"
        );
    
        if (geo_asset.size() < 1)
        {
            std::cout << "Failed to import geo_asset!" << std::endl;
            return 1;
        }

        if (geo_asset[0]->GetClass() != WStaticModel::GetStaticClass())
        {
            std::cout << "geo_asset is not a static model!" << std::endl;
            return 1;
        }

        std::vector<WAsset*> tex_asset = WImportTexture().Import(
            "../Content/Assets/Textures/viking_room.png", 
            "/Content/Assets/texture.texture"
        );

        if (tex_asset.size() < 1)
        {
            std::cout << "Failed to import tex_asset!" << std::endl;
            return 1;
        }

        if (tex_asset[0]->GetClass() != WTextureAsset::GetStaticClass())
        {
            std::cout << "tex_asset is not a texture!" << std::endl;
            return 1;
        }
    
        WStaticModel* static_model = static_cast<WStaticModel*>(geo_asset[0]);
        const WModelStruct& ModelData = static_model->GetModel();

        WTextureAsset* texture_asset = static_cast<WTextureAsset*>(tex_asset[0]);
        texture_asset->GetTexture();

        std::cout << "Test WSpacers!" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cout << "Something goes really bad!" << std::endl;
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

