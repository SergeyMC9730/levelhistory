#pragma once

#include <Geode/Geode.hpp>
#include <memory>
#include <vector>

class LevelProvider;
class SearchInstance;
class LoadingCircleLayer;
class LevelCell;

class ProviderPopupInfo {
public:
    int _page = 0;
    int _pagesMax = 1;
    LevelProvider *provider = nullptr;
};

class LevelPageInfo {
public:
    std::vector<GJGameLevel *> _currentLevels = {};
    std::vector<LevelCell *> _cells = {};
    int _currentLevelsIndex = 0;
    cocos2d::CCLayer *page = nullptr;
    int _cId = 0;
};

class ProviderPopup : public FLAlertLayer {
public:
	CCTextInputNode *m_pInputPGID;
	CCMenuItemToggler *m_pFollowPlayer;

    LevelProvider *_selectedProvider;

    std::vector<std::shared_ptr<LevelProvider>> _providers;

    void setupProviderBox(CCLayer *providerBox, LevelProvider *provider);
    void setupProviderBoxOnPage(CCLayer *providerBox);

    void setupLevelIDPage(CCLayer *providerBox);
    void setupSettingsPage(CCLayer *providerBox);
    void setupGenericSearchPage(CCLayer *providerBox);

    static void setColorToButtonSprite(ButtonSprite *spr, cocos2d::_ccColor3B color);

    bool _locked = false;
public:
    static ProviderPopup *get();

    LevelPageInfo _levelPage;

    std::string _levelArraySize = "";
    std::string _levelPageStr = "";

    std::string _enterLevelID = "";
    std::string _enterQuery = "";

    static std::string _currentError;

    LevelCell *createLevelCell(GJGameLevel *level, CCLayer *page);

    void lambdaOnDownloadLevel(SearchInstance *si, LoadingCircleLayer *existingCircle, ProviderPopup *popup, LevelProvider *prov, GJGameLevel *level);
    void lambdaOnDownloadLevelList(SearchInstance *si, LoadingCircleLayer *existingCircle, ProviderPopup *popup, LevelProvider *prov, GJGameLevel *level);

    void onToggler1PressMaybe(cocos2d::CCObject *sender);
    void onExitButton(CCObject *sender);

    void update(float delta) override;

    static ProviderPopup* create(std::vector<std::shared_ptr<LevelProvider>> providers);
    bool init(std::vector<std::shared_ptr<LevelProvider>> providers);

    void registerWithTouchDispatcher() override;

    void onPrevPage(CCObject *sender);
    void onNextPage(CCObject *sender);

    void onLevelPage(CCObject *sender);

    void onLevelIDSearch(CCObject *sender);
    void onGenericSearch(CCObject *sender);

    bool isPageHasFeature(ProviderPopupInfo *info, int feature);
    
    void onCopyID(CCObject *sender);
    
    void onPlayLevelDownload(CCObject *sender);

    void applyBottomButtons(CCLayer *page);
    
    static void removeThumbnailForCell(LevelCell *cell);
    void removeLevelRatings();

    void scenePrintError(float delta);

    void onProviderInfoBtn(CCObject *sender);
    // static void setButtonSpriteColor(ButtonSprite)
    // bool ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override;
};