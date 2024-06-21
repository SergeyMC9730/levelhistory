#include "LevelProvider.hpp"

void LevelProvider::setParameter(LevelProvider::LPFeatures parameter, int value) {
    _params[parameter] = value;
}
void LevelProvider::setParameter(LevelProvider::LPFeatures parameter, std::string value) {
    _params[parameter] = value;
}

std::vector<GJGameLevel *> LevelProvider::askMultipleLevels() {
    return {};
}

#include <sstream>
#include <iomanip>

// https://stackoverflow.com/questions/154536/encode-decode-urls-in-c
// :pray:

std::string LevelProvider::url_encode(const std::string value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char) c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}

std::string LevelProvider::getErrorCode() {
    return _currentError;
}

#include <Geode/binding/GJGameLevel.hpp>

void LevelProvider::makeLevelCopyable(GJGameLevel *level) {
    level->m_isEditable = true;
    level->m_password = 1;
}

std::string LevelProvider::getDescription() {
    return "(No description provided)";
}