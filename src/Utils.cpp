#include "stdafx.hpp"
#include "Utils.hpp"

std::string SanitizeGeneral(const std::string& aInput)
{
    // https://github.com/WopsS/RED4ext.SDK/blob/794feb2023121f3a0ea6fe2a593830d81c6e9026/include/RED4ext/Dump/Reflection.hpp#L29
    static std::regex invalidCharacters(R"(-|'|\(|\)|\]|\[|/|\.|\s|:)");
    return std::regex_replace(aInput, invalidCharacters, "_");
}
