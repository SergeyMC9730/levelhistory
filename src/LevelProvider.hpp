#pragma once

class GJGameLevel;

#include <string>
#include <unordered_map>
#include <functional>
#include <variant>
#include <vector>

class LevelProvider {
public:
    enum LPFeatures {
        QueryID, QueryLevelName, 
        LimitLevelArray, SetLevelArrayPage,
        ReturnMultipleLevels, SpecificRecord
    };
protected:
    std::string _baseUrl;

    std::unordered_map<enum LPFeatures, std::variant<std::string, int>> _params;

    std::string url_encode(const std::string value);

    std::string _currentError;

    void makeLevelCopyable(GJGameLevel *level);
public:
    struct BasicLevelInformation {
        int musicID = 0;
        int songID = 0;
        std::string _22songs = "";
        std::string _22sfxs = "";
    };

    virtual std::string getName() = 0;
    virtual void downloadLevel(std::function<void(LevelProvider *, GJGameLevel *)> onComplete) = 0;

    virtual std::unordered_map<enum LPFeatures, bool> getFeatures() = 0;

    virtual void setParameter(enum LPFeatures parameter, int value);
    virtual void setParameter(enum LPFeatures parameter, std::string value);

    virtual std::vector<GJGameLevel *> askMultipleLevels();

    virtual void cleanupLevels(bool withRelease) = 0;

    virtual void getLevelData(int id, std::function<void(LevelProvider *, std::string, struct BasicLevelInformation)> onComplete) = 0;

    virtual std::string getErrorCodeDescription(std::string err) = 0;
    virtual std::string getErrorCode();
};