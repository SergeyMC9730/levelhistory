#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <nlohmann/json.hpp>
#include <hjfod.gmd-api/include/GMD.hpp>

using namespace geode::prelude;

#include "GDHistoryProvider.hpp"

std::string GDHistoryProvider::getName() {
    return "GDHistory";
}
void GDHistoryProvider::downloadLevel(std::function<void(LevelProvider *, GJGameLevel *)> onComplete) {
    cleanupLevels(false);

    if (_params.count(LPFeatures::QueryID)) {
        web::AsyncWebRequest()
            .fetch(fmt::format("{}/api/v1/level/{}", _baseUrl, std::get<int>(_params[LPFeatures::QueryID])))
            .text()
            .then([this, onComplete](std::string const& catgirl) {
                nlohmann::json data = nlohmann::json::parse(catgirl);

                if (!data.contains("records")) {
                    onComplete(this, nullptr);

                    return;
                }

                nlohmann::json records = data.at("records");
                if (!records.is_array()) {
                    onComplete(this, nullptr);

                    return;
                }

                int levels = records.size();

                if (levels == 0) {
                    onComplete(this, nullptr);

                    return;
                }

                for (int i = 0; i < levels; i++) {
                    nlohmann::json leveljson = records[i];
                    GJGameLevel *level = GJGameLevel::create();

#define PARSE_STRING(lvalue, rvalue) if (!rvalue.is_null()) lvalue = rvalue.get<std::string>().c_str()
#define PARSE_INT(lvalue, rvalue) if (!rvalue.is_null()) lvalue = rvalue.get<int>()
#define PARSE_BOOL(lvalue, rvalue) if (!rvalue.is_null()) lvalue = (int)(rvalue.get<bool>())

                    PARSE_STRING(level->m_levelName, data["cache_level_name"]);
                    PARSE_STRING(level->m_levelDesc, leveljson["level_description"]);
                    PARSE_STRING(level->m_uploadDate, leveljson["real_date"]);
                    PARSE_STRING(level->m_creatorName, data["cache_username"]);
                    PARSE_STRING(level->m_songIDs, data["song_ids"]);
                    PARSE_STRING(level->m_sfxIDs, data["sfx_ids"]);
                    PARSE_INT(level->m_audioTrack, leveljson["official_song"]);
                    PARSE_INT(level->m_gameVersion, leveljson["game_version"]);
                    PARSE_INT(level->m_ratings, leveljson["raiting"]);
                    PARSE_INT(level->m_ratingsSum, leveljson["raiting_sum"]);
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
                    if (!leveljson["feature_score"].is_null() && leveljson["feature_score"].get<int>() > 0) {
                        level->m_featured = 1;
                        PARSE_INT(level->m_rateFeature, leveljson["feature_score"]);
                    }
                    if (!leveljson["song"].is_null()) {
                        PARSE_INT(level->m_songID, leveljson["song"]["online_id"]);
                    }
                    PARSE_INT(level->m_isEpic, leveljson["epic"]);

                    level->retain();
                    
                    this->_serverResponseParsed.push_back(level);
                }

                if (_params.count(LPFeatures::LimitLevelArray)) {
                    int max = std::get<int>(_params[LPFeatures::LimitLevelArray]);

                    if (max > 0) {
                        std::vector<GJGameLevel *> new_vec;

                        for (int i = 0; i < max && i < _serverResponseParsed.size(); i++) {
                            new_vec.push_back(this->_serverResponseParsed.at(i));
                        }

                        this->_serverResponseParsed = new_vec;
                    }
                }
                
                onComplete(this, this->_serverResponseParsed.at(0));
            })
            .expect([this, onComplete](std::string const& error) {
                onComplete(this, nullptr);
            });
    }

    if (_params.count(LPFeatures::QueryLevelName)) {
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

        web::AsyncWebRequest()
            .fetch(fmt::format("{}/api/v1/search/level/advanced/?query={}&limit={}&offset={}", _baseUrl, url_encode(a), b, b * c))
            .text()
            .then([this, onComplete](std::string const& catgirl) {
                nlohmann::json data = nlohmann::json::parse(catgirl);

                if (!data.contains("hits")) {
                    onComplete(this, nullptr);

                    return;
                }

                nlohmann::json records = data.at("hits");
                if (!records.is_array()) {
                    onComplete(this, nullptr);

                    return;
                }

                int levels = records.size();

                if (levels == 0) {
                    onComplete(this, nullptr);

                    return;
                }

                for (int i = 0; i < levels; i++) {
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
                    if (!leveljson["cache_featured"].is_null() && leveljson["cache_featured"].get<int>() > 0) {
                        level->m_featured = 1;
                        PARSE_INT(level->m_rateFeature, leveljson["cache_featured"]);
                    }
                    PARSE_INT(level->m_isEpic, leveljson["cache_epic"]);

                    level->retain();
                    
                    this->_serverResponseParsed.push_back(level);
                }
                
                onComplete(this, this->_serverResponseParsed.at(0));
            })
            .expect([this, onComplete](std::string const& error) {
                onComplete(this, nullptr);
            });
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

                onComplete(this, "-4", info);

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

                onComplete(this, "-5", info);

                return;
            }

            if (propLevel->m_demonVotes <= 0) {
                // log::info("votes: {}", propLevel->m_demonVotes);

                this->cleanupLevels(true);
                this->_serverResponseParsed = old_vec;

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

    web::AsyncWebRequest()
    .fetch(fmt::format("{}/level/{}/{}/download", _baseUrl, id, recid))
    .text()
    .then([this, onComplete](std::string const& catgirl) {
        struct LevelProvider::BasicLevelInformation info;

        if (catgirl.find("This record does not contain any level data.") != std::string::npos) {
            onComplete(this, "-5", info);

            return;
        }

        std::ofstream gmdfile;

        std::string path = Mod::get()->getSaveDir().generic_string();
        std::string _path = fmt::format("{}/temp.gmd", path);

        gmdfile.open (_path);
        gmdfile << catgirl;
        gmdfile.close();

        auto file = gmd::ImportGmdFile::from(_path);

        // std::filesystem::remove("temp.gmd");
        
        file.inferType();

        auto result = file.intoLevel();
        
        if (result.isErr() || !result.isOk()) {
            log::info("(GDHistoryProvider) error: {}", result.unwrapErr());

            onComplete(this, "-2", info);

            return;
        }

        auto level = result.unwrap();
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
    })
    .expect([this, onComplete](std::string const& error) {
        struct LevelProvider::BasicLevelInformation info;

        onComplete(this, "-1", info);
    });

    _params.clear();
}

std::string GDHistoryProvider::getErrorCodeDescription(std::string err) {
    std::string res = "unknown error";

    std::unordered_map<std::string, std::string> errors = {
        {"-1", "http error."},
        {"-2", "gmd api error. (dumped into console)"},
        {"-3", "invalid record id."},
        {"-4", "level not found."},
        {"-5", "level data cannot be downloaded for this level. Note that this issue will be fixed if level would have downloadable link for it in the future."}
    };

    if (errors.count(err)) {
        res = errors[err];
    }

    return res;
}