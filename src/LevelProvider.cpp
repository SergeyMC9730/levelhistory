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