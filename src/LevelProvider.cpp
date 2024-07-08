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

#include <ctime>

bool LevelProvider::requestCached() {
    return getCacheID() != -1;
}

int LevelProvider::getCacheID() {
    uint64_t timestamp = time(0);

    int i = 0;

    for (struct Cached &data : _cachedResults) {
        if (_params == data._params && (timestamp - data._timestamp) < getTimestampMaxDiff()) {
            return i;
        }

        i++;
    }

    return -1;
}

bool LevelProvider::requestCached(std::string &result) {
    struct Cached toCheck = {_params, result, 0};

    for (const struct Cached &data : _cachedResults) {
        if (toCheck == data) return true;
    }

    return false;
}

unsigned int LevelProvider::getTimestampMaxDiff() {
    return 3 * 60;
}

std::string LevelProvider::encodeParams() {
    std::string result = "";

    for (auto [key, val] : _params) {
        result += std::to_string((int)key) + "_";
    }

    if (result.length() >= 1) {
        result.pop_back();
    }

    return result;
}

void LevelProvider::cleanupParameters() {
    _params.clear();
}

std::string LevelProvider::getBaseURL() {
    return _baseUrl;
}
void LevelProvider::setBaseURL(std::string url) {
    _baseUrl = url;
}