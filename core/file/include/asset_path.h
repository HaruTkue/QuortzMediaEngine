#pragma once
#include <filesystem>

//asset Route path
void SetAssetRoutePath(const std::filesystem::path & path);
//now root asset path
std::filesystem::path GetAssetRootPath();


//アセット種別
enum class AssetType{
    Shader = 0,
    Texture,
    Model ,
    AssetTypeMax,
};
//種別指定 path get
std::filesystem::path GetAssetPath(AssetType type , const std::filesystem::path & fileName);