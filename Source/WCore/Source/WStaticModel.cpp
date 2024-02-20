#include "WAssets/WStaticModel.h"


void WStaticModel::SetModel(const WModelStruct& model)
{
    model_ = model;
}

const WModelStruct& WStaticModel::GetModel() const
{
    return model_;
}
