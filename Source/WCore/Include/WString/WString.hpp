#pragma once

#include "WCore/WCoreMacros.hpp"

#include <vector>
#include <string>
#include <format>
#include <cassert>
#include <filesystem>
#include <string_view>

namespace wstr {

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
        std::string asset_directory, // TODO use string_view if it is possible.
        std::string const & package_name,
        std::string const & asset_name
        ) {
        // TODO can I use string_view here?
        assert(asset_directory.starts_with("/Content"));

        std::string p_name = wstr::Split(
            wstr::Split(package_name, ".")[0], "/"
            ).back();

        while(asset_directory.ends_with("/")) {
            asset_directory.erase(asset_directory.size() - 1, asset_directory.size());
        }

        return std::format("{}/{}.{}", asset_directory, p_name, asset_name);

    }

    /**
     * Return splitted asset path.
     * delimiters are [/ \ : .]
     * No string copies are performed.
     */
    WNODISCARD inline
    std::vector<std::string_view> SplitAssetPath(std::string_view in_asset_path) {
        std::vector<std::string_view> result;
        constexpr std::string_view delimiters = "/\\:.";

        std::size_t start = 0;
        while (start < in_asset_path.size()) {
            auto end = in_asset_path.find_first_of(delimiters, start);
            if (end == std::string_view::npos) {
                result.push_back(in_asset_path.substr(start));
                break;
            }
            if (end > start) {
                result.push_back(in_asset_path.substr(start, end - start));
            }
            start = end + 1;
        }

        return result;
    }


    /**
     * Translate a engine based path (starts with /Content) with the system path.
     * @param in_path, the path to be translated.
     */
    WNODISCARD inline
    std::string SystemPath(
        std::string in_path
        )
    {
        if (in_path.starts_with("/")) {
            in_path = in_path.substr(1, in_path.length());
        }

        return std::filesystem::absolute(in_path).string();
    }

    /**
     * @brief converts a vector of std::string_view into a vector of const char *.
     * @DEPRECATED this is not reliable.
     */
    [[deprecated("Not a secure function.")]] inline void ToConstCharPtrs(
        const std::vector<std::string_view>& in_vw,
        std::vector<const char *> & out_char_ptr
        ) {
        out_char_ptr.clear();
        out_char_ptr.reserve(in_vw.size());

        for (const std::string_view & v : in_vw) {
            out_char_ptr.push_back(v.data());  // CAUTION not null character!
        }

    }

    /**
     * @brief Returns a string_view located at the segment of the in_path that represents
     * the basename without extension.
     */
    WNODISCARD inline std::string_view CleanBasename(std::string_view in_path) {
        auto last_sep = in_path.find_last_of("/\\");
        auto filename = (last_sep == std::string_view::npos)
            ? in_path
            : in_path.substr(last_sep + 1);

        // Find the last dot for the extension
        auto dot = filename.rfind('.');
        if (dot == std::string_view::npos) {
            return filename;               // No extension, whole filename is stem
        }
        return filename.substr(0, dot);    // Stem without extension
    }
    
}
