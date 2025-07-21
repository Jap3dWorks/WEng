#pragma once

#include <vector>
#include <string>
#include <format>
#include <cassert>
#include <filesystem>

namespace WStringUtils {

    inline std::vector<std::string> Split(std::string in_string, const std::string & in_delimiter) {

        std::vector<std::string> result;

        size_t pos =0;
        std::string token;

        while((pos = in_string.find(in_delimiter)) != std::string::npos) {
            token = in_string.substr(0, pos);
            result.push_back(token);
            in_string.erase(0, pos + in_delimiter.length());
        }

        return result;
    }

    /**
     * Format an engine based asset path.
     * @param asset_directory, Asset engine based directory. Should start with /Content.
     * @param package_name, package name, package is like the file that contains the assets.
     *   if you introduce a file_path, package_name will use the last token without extension.
     * @param asset_name, asset name.
     */
    inline std::string AssetPath(
        std::string asset_directory,
        const std::string & package_name,
        const std::string & asset_name
        ) {

        assert(asset_directory.starts_with("/Content"));

        std::string p_name = WStringUtils::Split(
            WStringUtils::Split(package_name, ".")[0], "/"
            ).back();

        while(asset_directory.ends_with("/")) {
            asset_directory.erase(asset_directory.size() - 1, asset_directory.size());
        }

        return std::format("{}/{}.{}", asset_directory, p_name, asset_name);

    }

    /**
     * Translate a engine based path (starts with /Content) with the system path.
     * @param in_path, the path to be translated.
     */
    inline std::string SystemPath(
        std::string in_path
        )
    {
        if (in_path.starts_with("/")) {
            in_path = in_path.substr(1, in_path.length());
        }

        return std::filesystem::absolute(in_path).string();
    }
}
