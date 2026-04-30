#include "../include/asset_path.h"
#include <array>
#include <string_view>

namespace{
    std::filesystem::path g_aseetRoot = "asset/";
    std::string_view ToSubDirectoryName(AssetType type){
        constexpr std::array<std::string_view , int(AssetType::AssetTypeMax)> kAssetDirs ={
            "shaders" , "texture" , "models",
        };
        return kAssetDirs[int(type)];
    }
}
void SetAssetRootPath(const std::filesystem::path& path){
    auto fullPath = std::filesystem::absolute(path);
    g_aseetRoot = std::filesystem::canonical(fullPath);
}
std::filesystem::path GetAssetRootPath(){
    return g_aseetRoot;
}
std::filesystem::path GetAssetPath(AssetType type , const std::filesystem::path& fileName){
    //return GetAssetRootPath() / ToSubDirectoryNames(type) / fileName;
}