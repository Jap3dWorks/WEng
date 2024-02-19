#include "WAssets/WStaticModel.h"


void WStaticModel::SetModel(const WModel& model)
{
    model_ = model;
}

const WModel& WStaticModel::GetModel() const
{
    return model_;
}
