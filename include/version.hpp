#ifndef KUMIGAME_VERSION_HPP
#define KUMIGAME_VERSION_HPP

#include <cstdint>
#include <string>

#ifndef RELEASE_TYPE
#define RELEASE_TYPE "internal"
#endif

#ifndef VERSION_MAJOR
#define VERSION_MAJOR 0
#endif

#ifndef VERSION_MINOR
#define VERSION_MINOR 0
#endif

#ifndef VERSION_PATCH
#define VERSION_PATCH 0
#endif

struct Version
{
public:
    /**
     * @brief Construct a new Version object.
     *
     * @param major Max value of 255.
     * @param minor Max value of 255.
     * @param patch Max value of 65535.
     */
    Version(uint8_t major, uint8_t minor, uint16_t patch);

    uint8_t getMajor() const;
    uint8_t getMinor() const;
    uint16_t getPatch() const;
    std::string toString() const;
    std::string toLongString() const;

private:
    uint32_t version;
};

#endif //KUMIGAME_VERSION_HPP
