#include "version.hpp"
#include <fmt/format.h>
#include <string>

Version::Version(uint8_t major, uint8_t minor, uint16_t patch)
{
    version = (major << 24) | (minor << 16) | patch;
}

uint8_t Version::getMajor() const
{
    return version >> 24;
}

uint8_t Version::getMinor() const
{
    return (version >> 16) & 255;
}

uint16_t Version::getPatch() const
{
    return version & 65535;
}

std::string Version::toString() const
{
    return fmt::format("{}.{}.{}", getMajor(), getMinor(), getPatch());
}

std::string Version::toLongString() const
{
    return fmt::format("v{}-{}", toString(), RELEASE_TYPE);
}
