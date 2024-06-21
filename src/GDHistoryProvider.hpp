#pragma once

#include "LevelProvider.hpp"
#include <nlohmann/json.hpp>

class GDHistoryProvider : public LevelProvider {
protected:
    std::vector<GJGameLevel *> _serverResponseParsed;

    // GJGameLevel *parseFromJson(nlohmann::json j);
public:
    GDHistoryProvider();

    std::string getName() override;
    void downloadLevel(std::function<void(LevelProvider *, GJGameLevel *)> onComplete) override;

    std::unordered_map<LPFeatures, bool> getFeatures() override;

    std::vector<GJGameLevel *> askMultipleLevels() override;
    void cleanupLevels(bool withRelease) override;

    void getLevelData(int id, std::function<void(LevelProvider *, std::string, struct BasicLevelInformation)> onComplete) override;

    virtual std::string getErrorCodeDescription(std::string err) override;

    std::string getDescription() override;
};