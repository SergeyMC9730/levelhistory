#pragma once

class GJGameLevel;
class GeodeNetwork;

#include <string>
#include <unordered_map>
#include <functional>
#include <variant>
#include <vector>
#include <cstdint>

class LevelProvider {
public:
    enum LPFeatures {
        QueryID, QueryLevelName, 
        LimitLevelArray, SetLevelArrayPage,
        ReturnMultipleLevels, SpecificRecord,
        TmpAskedLevel
    };

    struct Cached { 
        std::unordered_map<enum LPFeatures, std::variant<std::string, int>> _params;
        std::string _result;
        uint64_t _timestamp;

        bool operator ==(const struct Cached &ref) {
            bool _member0 = _params == ref._params;
            bool _member1 = _result == ref._result;

            return _member0 && _member1;
        }
    };
protected:
    std::string _currentError;
    std::vector<struct Cached> _cachedResults;
    std::string _baseUrl;
    std::unordered_map<enum LPFeatures, std::variant<std::string, int>> _params;

    std::string url_encode(const std::string value);
    void makeLevelCopyable(GJGameLevel *level);

    bool requestCached();
    bool requestCached(std::string &result);
    int getCacheID();

    virtual void parseResult(std::string &result, std::function<void(LevelProvider *, GJGameLevel *)> onComplete, GeodeNetwork *network = nullptr) = 0;

    virtual unsigned int getTimestampMaxDiff();

    std::string encodeParams();
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

    void cleanupParameters();

    virtual std::vector<GJGameLevel *> askMultipleLevels();

    virtual void cleanupLevels(bool withRelease) = 0;

    virtual void getLevelData(int id, std::function<void(LevelProvider *, std::string, struct BasicLevelInformation)> onComplete) = 0;

    virtual std::string getErrorCodeDescription(std::string err) = 0;
    virtual std::string getErrorCode();

    virtual std::string getDescription();

    std::string getBaseURL();
    void setBaseURL(std::string url);
};