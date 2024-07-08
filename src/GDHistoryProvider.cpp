#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <nlohmann/json.hpp>
#include <hjfod.gmd-api/include/GMD.hpp>
#include "GeodeNetwork.hpp"
#include <ctime>

using namespace geode::prelude;

#include "GDHistoryProvider.hpp"

#define PARSE_STRING(lvalue, rvalue) if (!rvalue.is_null()) lvalue = rvalue.get<std::string>().c_str()
#define PARSE_INT(lvalue, rvalue) if (!rvalue.is_null()) lvalue = rvalue.get<int>()
#define PARSE_BOOL(lvalue, rvalue) if (!rvalue.is_null()) lvalue = (int)(rvalue.get<bool>())

std::string GDHistoryProvider::getName() {
    return "GDHistory";
}
void GDHistoryProvider::downloadLevel(std::function<void(LevelProvider *, GJGameLevel *)> onComplete) {
    auto savedParams = _params;
    
    cleanupLevels(false);

    int cid = getCacheID();

    if (cid != -1) {
        struct Cached *cached = _cachedResults.data() + cid;

        parseResult(cached->_result, onComplete);

        return;
    }

    GeodeNetwork *network = new GeodeNetwork();

    if (_params.count(LPFeatures::QueryID)) {
        network->setURL(fmt::format("{}/api/v1/level/{}", _baseUrl, std::get<int>(_params[LPFeatures::QueryID])));
        network->setErrorCallback([this, network, onComplete](GeodeNetwork *) {
            std::string catgirl = network->getResponse();

            if (catgirl.find("Server Error") != std::string::npos) {
                this->_currentError = "-7";
                onComplete(this, nullptr);

                delete network;

                return;
            }

            this->_currentError = "-1";
            onComplete(this, nullptr);

            delete network;

            return;
        });

        network->setOkCallback([this, network, onComplete, savedParams](GeodeNetwork *) {
            std::string catgirl = network->getResponse();

            if (catgirl.find("Server Error") != std::string::npos) {
                this->_currentError = "-7";
                onComplete(this, nullptr);

                delete network;

                return;
            }

            this->_params = savedParams;
            this->parseResult(catgirl, onComplete, network);

            struct Cached cached = {
                _params, catgirl, (uint64_t)time(0)
            };

            this->_cachedResults.push_back(cached);

            return;
        });
        
        network->send();
    } else if (_params.count(LPFeatures::QueryLevelName)) {
        std::string a = std::get<std::string>(_params[LPFeatures::QueryLevelName]);
        int b = 0;
        int c = 0;

        if (_params.count(LPFeatures::LimitLevelArray)) {
            b = std::get<int>(_params[LPFeatures::LimitLevelArray]);
        }
        if (_params.count(LPFeatures::SetLevelArrayPage)) {
            c = std::get<int>(_params[LPFeatures::SetLevelArrayPage]);
        }

        if (b == 0) {
            b = 100;
        }

        network->setURL(fmt::format("{}/api/v1/search/level/advanced/?query={}&limit={}&offset={}", _baseUrl, url_encode(a), b, b * c));
        
        network->setErrorCallback([this, network, onComplete](GeodeNetwork *) {
            std::string error = network->getResponse();

            if (error.find("Server Error") != std::string::npos) {
                this->_currentError = "-7";
                onComplete(this, nullptr);

                delete network;

                return;
            }

            this->_currentError = "-1";
            onComplete(this, nullptr);

            delete network;

            return;
        });

        network->setOkCallback([this, network, onComplete, savedParams](GeodeNetwork *) {
            std::string catgirl = network->getResponse();

            if (catgirl.find("Server Error") != std::string::npos) {
                this->_currentError = "-7";
                onComplete(this, nullptr);

                delete network;

                return;
            }

            this->_params = savedParams;
            this->parseResult(catgirl, onComplete, network);

            struct Cached cached = {
                _params, catgirl, (uint64_t)time(0)
            };

            this->_cachedResults.push_back(cached);

            return;
        });
        
        network->send();
    } else {
        delete network;
    }

    _params.clear();

    return;
}

GDHistoryProvider::GDHistoryProvider() {
    _baseUrl = "https://history.geometrydash.eu";
}

std::unordered_map<GDHistoryProvider::LPFeatures, bool> GDHistoryProvider::getFeatures() {
    std::unordered_map<enum LPFeatures, bool> features;

    features[LPFeatures::QueryID] = true;
    features[LPFeatures::QueryLevelName] = true;
    features[LPFeatures::LimitLevelArray] = true;
    features[LPFeatures::SetLevelArrayPage] = true;
    features[LPFeatures::ReturnMultipleLevels] = true;

    return features;
}

std::vector<GJGameLevel *> GDHistoryProvider::askMultipleLevels() {
    return _serverResponseParsed;
}
void GDHistoryProvider::cleanupLevels(bool withRelease) {
    if (!withRelease) {
        _serverResponseParsed.clear();

        return;
    }
    
    for (auto level : _serverResponseParsed) {
        if (level != nullptr) {
            level->release();
        }
    }

    _serverResponseParsed.clear();
}

#include <fstream>
#include <filesystem>

void GDHistoryProvider::getLevelData(int id, std::function<void(LevelProvider *, std::string, struct LevelProvider::BasicLevelInformation)> onComplete) {
    int recid = 0;

    log::info("(GDHistoryProvider) preparing to download {}", id);

    // cleanupParameters();

    if (_params.count(LPFeatures::SpecificRecord)) {
        recid = std::get<int>(_params[LPFeatures::SpecificRecord]);
    }

    if (recid == 0) {
        log::info("(GDHistoryProvider) record for {} not found, getting it", id);

        setParameter(LPFeatures::QueryID, id);

        std::vector<GJGameLevel *> old_vec = this->_serverResponseParsed;
        cleanupLevels(false);

        downloadLevel([this, id, onComplete, old_vec](LevelProvider *, GJGameLevel *level) {
            struct LevelProvider::BasicLevelInformation info;

            this->_params.clear();

            if (!level) {
                this->cleanupLevels(true);
                this->_serverResponseParsed = old_vec;

                // this->_currentError = "-4";
                onComplete(this, this->getErrorCode(), info);

                return;
            }

            GJGameLevel *propLevel = nullptr;
            auto levels = this->askMultipleLevels();

            for (auto lvl : levels) {
                if (lvl->m_isEditable) {
                    propLevel = lvl;
                    
                    break;
                }
            }

            if (!propLevel) {
                this->cleanupLevels(true);
                this->_serverResponseParsed = old_vec;

                this->_currentError = "-5";
                onComplete(this, "-5", info);

                return;
            }

            if (propLevel->m_demonVotes <= 0) {
                // log::info("votes: {}", propLevel->m_demonVotes);

                this->cleanupLevels(true);
                this->_serverResponseParsed = old_vec;

                this->_currentError = "-3";
                onComplete(this, "-3", info);

                return;
            }

            this->setParameter(LPFeatures::SpecificRecord, propLevel->m_demonVotes);

            this->cleanupLevels(true);
            this->_serverResponseParsed = old_vec;

            this->getLevelData(id, onComplete);
        });

        return;
    }

    log::info("(GDHistoryProvider) downloading level {}", id);

    setParameter(LPFeatures::TmpAskedLevel, id);

    auto savedParams = _params;

    int cid = getCacheID();
    if (cid != -1) {
        log::info("(GDHistoryProvider) trying to use cached verison of level {}", id);
    
        struct Cached *cached = _cachedResults.data() + cid;

        parseResult(cached->_result, onComplete);

        return;
    }

    GeodeNetwork *network = new GeodeNetwork();

    network->setURL(fmt::format("{}/level/{}/{}/download", _baseUrl, id, recid));

    network->setErrorCallback([this, onComplete, network](GeodeNetwork *) {
        struct LevelProvider::BasicLevelInformation info = {};

        onComplete(this, "-1", info);

        delete network;
    });

    network->setOkCallback([this, onComplete, network, savedParams](GeodeNetwork *) {
        struct LevelProvider::BasicLevelInformation info;

        std::string catgirl = network->getResponse();

        this->_params = savedParams;
        this->parseResult(catgirl, onComplete, network);

        struct Cached cached = {
            _params, catgirl, (uint64_t)time(0)
        };

        this->_cachedResults.push_back(cached);
    });

    network->send();

    _params.clear();
}

std::string GDHistoryProvider::getErrorCodeDescription(std::string err) {
    std::string res = "unknown error";

    std::unordered_map<std::string, std::string> errors = {
        {"-1", "http error."},
        {"-2", "gmd api error. (dumped into console)"},
        {"-3", "invalid record id."},
        {"-4", "level not found."},
        {"-5", "level data cannot be downloaded for this level. Note that this issue will be fixed if level would have downloadable link for it in the future."},
        {"-6", "insufficient rights to download this level."},
        {"-7", "gd history's api is down."},
        {"-8", "api did not return record data."},
        {"-9", "api returned record data with invalid data type."},
        {"-10", "api returned an record array with zero levels in it."}
    };

    if (errors.count(err)) {
        res = errors[err];
    }

    return res;
}

std::string GDHistoryProvider::getDescription() {
    return std::string("A Geometry Dash preservation project. ") + _baseUrl;
}

void GDHistoryProvider::parseResult(std::string &result, std::function<void(LevelProvider *, GJGameLevel *)> onComplete, GeodeNetwork *network) {
    // log::info("(GDHistoryProvider) result={}", result);
    
    if (_params.count(LPFeatures::QueryID) && !_params.count(LPFeatures::TmpAskedLevel)) {
        nlohmann::json data = nlohmann::json::parse(result);

        if (!data.contains("records")) {
            this->_currentError = "-8";
            onComplete(this, nullptr);

            if (network != nullptr) delete network;

            return;
        }

        nlohmann::json records = data.at("records");
        if (!records.is_array()) {
            this->_currentError = "-9";
            onComplete(this, nullptr);
                    
            if (network != nullptr) delete network;

            return;
        }

        int levels = records.size();

        if (levels == 0) {
            this->_currentError = "-10";
            onComplete(this, nullptr);

            if (network != nullptr) delete network;

            return;
        }

        for (int i = 0; i < levels; i++) {
            nlohmann::json leveljson = records[i];
            GJGameLevel *level = GJGameLevel::create();

            PARSE_STRING(level->m_levelName, data["cache_level_name"]);
            PARSE_STRING(level->m_levelDesc, leveljson["level_description"]);
            PARSE_STRING(level->m_uploadDate, leveljson["real_date"]);
            PARSE_STRING(level->m_creatorName, data["cache_username"]);
            PARSE_STRING(level->m_songIDs, data["song_ids"]);
            PARSE_STRING(level->m_sfxIDs, data["sfx_ids"]);
            PARSE_INT(level->m_audioTrack, leveljson["official_song"]);
            PARSE_INT(level->m_gameVersion, leveljson["game_version"]);
            PARSE_INT(level->m_ratings, leveljson["rating"]);
            PARSE_INT(level->m_ratingsSum, leveljson["rating_sum"]);
            PARSE_INT(level->m_downloads, leveljson["downloads"]);
            PARSE_INT(level->m_likes, leveljson["likes"]);
            PARSE_INT(level->m_levelLength, leveljson["length"]);
            PARSE_INT(level->m_userID, data["cache_user_id"]);
            PARSE_INT(level->m_coins, leveljson["coins"]);
            PARSE_INT(level->m_coinsVerified, leveljson["coins_verified"]);
            PARSE_INT(level->m_rateStars, leveljson["stars"]);
            PARSE_INT(level->m_accountID, leveljson["account_id"]);
            PARSE_INT(level->m_levelID, data["online_id"]);
            PARSE_BOOL(level->m_demon, leveljson["demon"]);
            PARSE_BOOL(level->m_autoLevel, leveljson["auto"]);
            PARSE_BOOL(level->m_isEditable, leveljson["level_string_available"]);
            PARSE_INT(level->m_demonDifficulty, leveljson["demon_type"]);
            PARSE_INT(level->m_demonVotes, leveljson["id"]);
            PARSE_INT(level->m_featured, leveljson["feature_score"]);
        
            if (!leveljson["song"].is_null()) {
                PARSE_INT(level->m_songID, leveljson["song"]["online_id"]);
            }
            PARSE_INT(level->m_isEpic, leveljson["epic"]);

            bool has_leveldata = false;
            PARSE_BOOL(has_leveldata, leveljson["level_string_available"]);
            if (has_leveldata) {
                level->m_dislikes = 1;
                level->m_likes++;
            } else {
                level->m_dislikes = 0;
            }

            this->makeLevelCopyable(level);

            level->retain();
                        
            this->_serverResponseParsed.push_back(level);
        }

        if (this->_params.count(LPFeatures::LimitLevelArray) ) {
            int max = std::get<int>(this->_params[LPFeatures::LimitLevelArray]);

            if (max > 0) {
                std::vector<GJGameLevel *> new_vec;

                for (int i = 0; i < max && i < _serverResponseParsed.size(); i++) {
                    new_vec.push_back(this->_serverResponseParsed.at(i));
                }

                this->_serverResponseParsed = new_vec;
            }
        }
                    
        onComplete(this, this->_serverResponseParsed.at(0));

        if (network != nullptr) delete network;
    } else if (_params.count(LPFeatures::QueryLevelName) && !_params.count(LPFeatures::TmpAskedLevel)) {

        nlohmann::json data = nlohmann::json::parse(result);

        if (!data.contains("hits")) {
            this->_currentError = "-8";
            onComplete(this, nullptr);

            delete network;

            return;
        }

        nlohmann::json records = data.at("hits");
        if (!records.is_array()) {
            this->_currentError = "-9";
            onComplete(this, nullptr);

            if (network != nullptr) delete network;

            return;
        }

        int levels = records.size();

        if (levels == 0) {
            this->_currentError = "-10";
            onComplete(this, nullptr);

            if (network != nullptr) delete network;

            return;
        }

        for (int i = 0; i < levels; i++) {
            // log::info("{}", i);

            nlohmann::json leveljson = records[i];
            GJGameLevel *level = GJGameLevel::create();

            PARSE_STRING(level->m_levelName, leveljson["cache_level_name"]);
            // PARSE_STRING(level->m_levelDesc, leveljson["level_description"]);
            PARSE_STRING(level->m_uploadDate, leveljson["cache_submitted"]);
            PARSE_STRING(level->m_creatorName, leveljson["cache_username"]);
            // PARSE_INT(level->m_audioTrack, leveljson["official_song"]);
            level->m_gameVersion = 22;
            PARSE_INT(level->m_ratings, leveljson["cache_rating"]);
            PARSE_INT(level->m_ratingsSum, leveljson["cache_rating_sum"]);
            PARSE_BOOL(level->m_demon, leveljson["cache_demon"]);
            PARSE_BOOL(level->m_autoLevel, leveljson["cache_auto"]);
            PARSE_INT(level->m_demonDifficulty, leveljson["cache_demon_type"]);
            PARSE_INT(level->m_downloads, leveljson["cache_downloads"]);
            // PARSE_INT(level->m_difficulty, leveljson["cache_main_difficulty"]);
            PARSE_INT(level->m_likes, leveljson["cache_likes"]);
            PARSE_INT(level->m_levelLength, leveljson["cache_length"]);
            PARSE_INT(level->m_userID, data["cache_user_id"]);
            PARSE_INT(level->m_rateStars, leveljson["cache_stars"]);
            PARSE_INT(level->m_levelID, leveljson["online_id"]);
            PARSE_INT(level->m_demonVotes, leveljson["id"]);
            PARSE_INT(level->m_featured, leveljson["cache_featured"]);
            PARSE_INT(level->m_isEpic, leveljson["cache_epic"]);

            bool has_leveldata = false;
            PARSE_BOOL(has_leveldata, leveljson["cache_level_string_available"]);
            if (has_leveldata) {
                log::info("(GDHistoryProvider) level \"{}\" by \"{}\" has leveldata", level->m_levelName, level->m_creatorName);
                level->m_dislikes = 1;
                level->m_likes++;
            } else {
                log::info("(GDHistoryProvider) level \"{}\" by \"{}\" does not has leveldata", level->m_levelName, level->m_creatorName);
                level->m_dislikes = 0;
            }

            this->makeLevelCopyable(level);

            level->retain();
                    
            this->_serverResponseParsed.push_back(level);
        }
                
        onComplete(this, this->_serverResponseParsed.at(0));

        if (network != nullptr) delete network;
    }
}

void GDHistoryProvider::parseResult(std::string &result, std::function<void(LevelProvider *, std::string, struct LevelProvider::BasicLevelInformation)> onComplete, GeodeNetwork *network) {
    if (_params.count(LPFeatures::TmpAskedLevel)) {
        struct LevelProvider::BasicLevelInformation info;

        if (result.find("Server Error") != std::string::npos) {
            this->_currentError = "-7";
            onComplete(this, "-7", info);

            if (network != nullptr) delete network;

            return;
        }

        if (result.find("This record does not contain any level data") != std::string::npos) {
            this->_currentError = "-5";
            onComplete(this, "-5", info);

            if (network != nullptr) delete network;

            return;
        }
        if (result.find("You do not have the rights to download this record") != std::string::npos) {
            this->_currentError = "-6";
            onComplete(this, "-6", info);
            
            if (network != nullptr) delete network;
            
            return;
        }

        std::ofstream gmdfile;

        std::string path = Mod::get()->getSaveDir().generic_string();
        std::string _path = fmt::format("{}/temp.gmd", path);

        gmdfile.open (_path);
        gmdfile << result;
        gmdfile.close();

        auto file = gmd::ImportGmdFile::from(_path);

        // std::filesystem::remove("temp.gmd");
        
        file.inferType();

        auto _result = file.intoLevel();
        
        if (_result.isErr() || !_result.isOk()) {
            log::info("(GDHistoryProvider) error: {}", _result.unwrapErr());
            log::info("(GDHistoryProvider) level string: {}", result);
    
            onComplete(this, "-2", info);

            if (network != nullptr) delete network;

            return;
        }

        auto level = _result.unwrap();
        level->retain();

        auto str = level->m_levelString;
        int mid = level->m_audioTrack;
        int sid = level->m_songID;

        info.musicID = mid;
        info.songID = sid;
        info._22songs = level->m_songIDs;
        info._22sfxs = level->m_sfxIDs;

        level->release();

        onComplete(this, str, info);

        if (network != nullptr) delete network;
    }
}