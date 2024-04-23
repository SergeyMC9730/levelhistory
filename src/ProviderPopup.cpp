#include "ProviderPopup.hpp"

#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/general.hpp>
#include "ProviderPopupWait.hpp"

using namespace geode::prelude;

#include "LevelProvider.hpp"

void ProviderPopup::onToggler1PressMaybe(CCObject *sender) {
    LevelProvider *provider = static_cast<LevelProvider *>(((CCNode *)sender)->getUserData());

    auto popup = ProviderPopup::get();

    log::info("clicked provider: {}", provider->getName());

    CCNode *providerBox = getChildByIDRecursive("provider-box");
    popup->setupProviderBox((CCLayer *)providerBox, provider);
}

void ProviderPopup::onExitButton(CCObject *sender) {
    auto popup = ProviderPopup::get();

    popup->removeMeAndCleanup();
}

bool ProviderPopup::init(std::vector<std::shared_ptr<LevelProvider>> providers) {
    if (!FLAlertLayer::init(0)) return false;

    _providers = providers;

	CCLayer *objectSelector = CCLayer::create();
	CCLayer *scale9layer = CCLayer::create();

	CCScale9Sprite *spr1 = CCScale9Sprite::create("GJ_square01.png");
	auto winsize = CCDirector::sharedDirector()->getWinSize();
	//spr1->setAnchorPoint({0, 1});
	spr1->setContentSize({380, 250});

	scale9layer->addChild(spr1);
	objectSelector->addChild(scale9layer, 0);

	scale9layer->setPosition({winsize.width / 2, winsize.height / 2});

	auto bmf = CCLabelBMFont::create("Search From Other Sources", "bigFont.fnt");
	bmf->setScale(0.6f);
	bmf->setPositionX(winsize.width / 2);
	bmf->setPositionY(winsize.height / 2 + spr1->getContentSize().height / 2 - 30);
			
	objectSelector->addChild(bmf, 1);

	auto exitBtn = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
	auto btn3 = CCMenuItemSpriteExtra::create(
		exitBtn, this, menu_selector(ProviderPopup::onExitButton)
	);

    CCMenu *menu = CCMenu::create();

    CCLayer *infoLayer = CCLayer::create();
    auto vlayout = ColumnLayout::create();

    infoLayer->setLayout(vlayout);

    auto layout = RowLayout::create();
    layout->setGrowCrossAxis(true);

    menu->setLayout(layout);

    for (auto provider : providers) {
        auto spr4 = ButtonSprite::create(provider->getName().c_str());
        spr4->setScale(0.6f);
        
        auto btn4 = CCMenuItemSpriteExtra::create(
            spr4, this, menu_selector(ProviderPopup::onToggler1PressMaybe)
        );
        btn4->setUserData(provider.get());

        menu->addChild(btn4);
        menu->updateLayout();
    }

    // menu->ignoreAnchorPointForPosition(true);

    CCLayer *providerBox = CCLayer::create();
    providerBox->setID("provider-box");

    CCScale9Sprite *s9s = CCScale9Sprite::create("GJ_square02.png");
    s9s->setContentSize({300, 150});
    s9s->setAnchorPoint({0, 0});
    s9s->setID("s9s");

    providerBox->setContentSize(s9s->getContentSize());
    providerBox->addChild(s9s);

    setupProviderBox(providerBox, nullptr);

    infoLayer->addChild(providerBox);
    infoLayer->addChild(menu);

    infoLayer->updateLayout();
    infoLayer->setID("info-layer");
    infoLayer->setPositionX(winsize.width / 2);
	infoLayer->setPositionY(winsize.height / 2 - 15);

    objectSelector->addChild(infoLayer, 2);

	auto base = CCSprite::create("square.png");
	base->setPosition({ 0, 0 });
	base->setScale(500.f);
	base->setColor({0, 0, 0});
	base->setOpacity(0);
	base->runAction(CCFadeTo::create(0.3f, 125));

	this->addChild(base, -1);

    objectSelector->setID("object-selector");

	// selectorlayers.push_back(static_cast<CCLayer *>(this));

	scheduleUpdate();

    menu->setID("providers");

    auto pos = objectSelector->getPosition();
    // menu->setPositionY(menu->getPositionY() + (objectSelector->getContentSize().height / 1.5f) - 30.f);

    CCMenu *men2 = CCMenu::create();
    
    men2->setPosition({
		winsize.width / 2 - spr1->getContentSize().width / 2,
		winsize.height / 2 + spr1->getContentSize().height / 2
	});
    men2->addChild(btn3);

    objectSelector->addChild(men2, 2);

    m_mainLayer->addChild(objectSelector);

    show();

    setID("provider-popup");

    // _levelPage = new LevelPageInfo(); 

	return true;
}

void ProviderPopup::update(float delta) {
	// m_pSelectedObject->m_targetColorID = atoi(m_pInputPGID->getString());
	// if(m_pSelectedObject->m_targetColorID > MAX_PLAYER_GROUP_SIZE + 1 || m_pSelectedObject->m_targetColorID < 0) {
	// 	m_pSelectedObject->m_targetColorID = MAX_PLAYER_GROUP_SIZE;
	// }
	// m_pSelectedObject->m_tintTrigger = m_pFollowPlayer->isToggled();	
    
     // in->setID("level-array-input"); in->setID("level-page-input");

    // auto laiNd = getChildByIDRecursive("level-array-input");
    // auto lpiNd = getChildByIDRecursive("level-page-input");

    // if (laiNd) {
    //     auto lai = dynamic_cast<TextInput *>(laiNd);
    //     _levelArraySize = lai->getString();
    //     log::info("las: {}", _levelArraySize);
    // }
    // if (lpiNd) {
    //     auto lpi = dynamic_cast<TextInput *>(lpiNd);
    //     _levelPageStr = lpi->getString();
    //     log::info("lps: {}", _levelPageStr);
    // }
}

ProviderPopup* ProviderPopup::create(std::vector<std::shared_ptr<LevelProvider>> providers) { 
	ProviderPopup* pRet = new ProviderPopup(); 
	if (pRet && pRet->init(providers)) { 
		pRet->autorelease();
		return pRet;
	} else {
		delete pRet;
		pRet = 0;
		return 0; 
	} 
}

void ProviderPopup::registerWithTouchDispatcher() {
    CCTouchDispatcher *dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();

    dispatcher->addTargetedDelegate(this, -502, true);
}

void ProviderPopup::setupProviderBox(CCLayer *providerBox, LevelProvider *provider) {
    _selectedProvider = provider;

    auto s9s = (CCScale9Sprite *)providerBox->getChildByID("s9s");
    auto csz = s9s->getContentSize();
    
    auto popup = ProviderPopup::get();
    if (!popup) popup = this;

    providerBox->removeAllChildrenWithCleanup(true);
    popup->_levelPage._cells.clear();
    
    s9s = CCScale9Sprite::create("GJ_square02.png");
    s9s->setContentSize(csz);
    s9s->setAnchorPoint({0, 0});
    s9s->setID("s9s");

    providerBox->addChild(s9s);

    if (!provider) {
        CCLabelBMFont *l = CCLabelBMFont::create("Nothing is currently selected.", "chatFont.fnt");
        l->setPosition(csz.width / 2, csz.height / 2);

        providerBox->addChild(l);

        return;
    }

    ProviderPopupInfo *info = new ProviderPopupInfo();
    info->provider = provider;

    providerBox->setUserData(info);

    auto features = provider->getFeatures();

    if (features[LevelProvider::LPFeatures::QueryID] && features[LevelProvider::LPFeatures::QueryLevelName]) {
        info->_pagesMax = 2;
    }

    bool has_settings = features[LevelProvider::LPFeatures::LimitLevelArray] || features[LevelProvider::LPFeatures::SetLevelArrayPage];

    if (has_settings) {
        info->_pagesMax++;
    }

    if (info->_pagesMax >= 2) {
        CCMenu *menu = CCMenu::create();
        menu->setID("page-control");

        auto prev_page_spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        prev_page_spr->setColor({64, 64, 64});
        prev_page_spr->setScale(0.8f);
        prev_page_spr->setID("prev-page-spr");

        auto prev_page = CCMenuItemSpriteExtra::create(
			prev_page_spr,
			providerBox,
			menu_selector(ProviderPopup::onPrevPage)
		);
        prev_page->setID("prev-page");
        prev_page->setPosition(prev_page->getContentSize().width - 7, csz.height / 2);
        prev_page->setEnabled(false);

        auto next_page_spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        next_page_spr->setFlipX(true);
        next_page_spr->setScale(0.8f);
        next_page_spr->setID("next-page-spr");

        auto next_page = CCMenuItemSpriteExtra::create(
			next_page_spr,
			providerBox,
			menu_selector(ProviderPopup::onNextPage)
		);
        next_page->setID("next-page");
        next_page->setPosition(csz.width - next_page->getContentSize().width + 7, csz.height / 2);

        menu->addChild(prev_page);
        menu->addChild(next_page);

        providerBox->addChild(menu, 100);

        menu->setPosition({0, 0});

        CCLabelBMFont *l = CCLabelBMFont::create("Page 0", "chatFont.fnt");
        l->setAnchorPoint({0, 0});
        l->setScale(0.5f);
        l->setPosition(9, 9);
        l->setID("page-string");
        l->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);

        providerBox->addChild(l);
    }

    CCLayer *page = CCLayer::create();

    page->setID("page");
    page->setContentSize(csz);
    page->setPosition({0, 0});

    providerBox->addChild(page);

    setupProviderBoxOnPage(providerBox);

    providerBox->setScale(0.f);
	providerBox->runAction(CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f), 0.6f));
}

void ProviderPopup::setupProviderBoxOnPage(CCLayer *providerBox) {
    ProviderPopupInfo *info = (ProviderPopupInfo *)providerBox->getUserData();

    if (!info) return;
    if ((info->_page > (info->_pagesMax - 1))) return;

    if (info->_pagesMax > 1) {     
        auto prev_page = dynamic_cast<CCMenuItemSpriteExtra *>(providerBox->getChildByIDRecursive("prev-page"));
        auto next_page = dynamic_cast<CCMenuItemSpriteExtra *>(providerBox->getChildByIDRecursive("next-page"));

        auto prev_page_spr = dynamic_cast<CCSprite *>(providerBox->getChildByIDRecursive("prev-page-spr"));
        auto next_page_spr = dynamic_cast<CCSprite *>(providerBox->getChildByIDRecursive("next-page-spr"));

        if (info->_page == 0) {
            prev_page->setEnabled(false);
            prev_page_spr->setColor({64, 64, 64});

            next_page_spr->setColor({255, 255, 255});
            next_page->setEnabled(true);
        }
        if (info->_page >= 1) {
            prev_page->setEnabled(true);
            next_page->setEnabled(true);

            next_page_spr->setColor({255, 255, 255});
            prev_page_spr->setColor({255, 255, 255});
        }
        if (info->_page == (info->_pagesMax - 1)) {
            next_page_spr->setColor({64, 64, 64});
            next_page->setEnabled(false);

            prev_page->setEnabled(true);
            prev_page_spr->setColor({255, 255, 255});
        }
        
        CCLabelBMFont *l = dynamic_cast<CCLabelBMFont *>(providerBox->getChildByID("page-string"));
        if (l) {
            l->setString(fmt::format("Page {}", info->_page + 1).c_str());
        }
    }

    CCLayer *page = dynamic_cast<CCLayer *>(providerBox->getChildByID("page"));

    if (!page) return;

    auto popup = ProviderPopup::get();

    page->removeAllChildrenWithCleanup(true);
    popup->_levelPage._cells.clear();

    switch(info->_page) {
        case 0: {
            auto feats = info->provider->getFeatures();

            if (feats[LevelProvider::LPFeatures::QueryID]) {
                popup->setupLevelIDPage(providerBox);

                return;
            }
            if (feats[LevelProvider::LPFeatures::QueryLevelName]) {
                popup->setupGenericSearchPage(providerBox);

                return;
            }
            if (feats[LevelProvider::LPFeatures::LimitLevelArray] || feats[LevelProvider::LPFeatures::SetLevelArrayPage]) {
                popup->setupSettingsPage(providerBox);

                return;
            }

            break;
        }
        case 1: {
            auto feats = info->provider->getFeatures();

            if (feats[LevelProvider::LPFeatures::QueryLevelName]) {
                popup->setupGenericSearchPage(providerBox);

                return;
            }

            if (feats[LevelProvider::LPFeatures::LimitLevelArray] || feats[LevelProvider::LPFeatures::SetLevelArrayPage]) {
                popup->setupSettingsPage(providerBox);

                return;
            }

            break;
        }
        case 2: {
            auto feats = info->provider->getFeatures();

            if (feats[LevelProvider::LPFeatures::LimitLevelArray] || feats[LevelProvider::LPFeatures::SetLevelArrayPage]) {
                popup->setupSettingsPage(providerBox);

                return;
            }

            break;
        }
    }
}

void ProviderPopup::onPrevPage(CCObject *sender) {
    CCNode *orig = static_cast<CCNode *>(sender);
    CCLayer *box = dynamic_cast<CCLayer *>(orig->getParent()->getParent());
    auto popup = ProviderPopup::get();

    if (!box) return;

    ProviderPopupInfo *info = static_cast<ProviderPopupInfo *>(box->getUserData());

    if (!info) return;

    info->_page--;
    if (info->_page < 0) info->_page = 0;

    // for (auto _cell : _levelPage._cells) {
    //     _cell->removeMeAndCleanup();
    // }
    popup-> _levelPage._cells.clear();
    info->provider->cleanupLevels(false);

    setupProviderBoxOnPage(box);
}
void ProviderPopup::onNextPage(CCObject *sender) {
    CCNode *orig = static_cast<CCNode *>(sender);
    CCLayer *box = dynamic_cast<CCLayer *>(orig->getParent()->getParent());
    auto popup = ProviderPopup::get();

    if (!box) return;

    ProviderPopupInfo *info = static_cast<ProviderPopupInfo *>(box->getUserData());
    if (!info) return;

    info->_page++;
    if (info->_page >= (info->_pagesMax - 1)) info->_page = info->_pagesMax - 1;

    // for (auto _cell : _levelPage._cells) {
    //     _cell->removeMeAndCleanup();
    // }
    popup->_levelPage._cells.clear();
    info->provider->cleanupLevels(false);

    setupProviderBoxOnPage(box);
}

class SearchInstance {
public:
    TextInput *_input;
    CCLayer *_page;
    ProviderPopupInfo *_info;;
};

#include "LoadingCircleLayer.hpp"

void ProviderPopup::setupLevelIDPage(CCLayer *providerBox) {
    CCLayer *page = dynamic_cast<CCLayer *>(providerBox->getChildByID("page"));
    ProviderPopupInfo *info = (ProviderPopupInfo *)providerBox->getUserData();
    
    TextInput *in = TextInput::create(100, "Enter level ID...", "chatFont.fnt");
    in->setPosition(0, 0);
    in->setAnchorPoint({0.5f, 0.5f});

    int sz = 0;
    auto prev_page = static_cast<CCMenuItemSpriteExtra *>(providerBox->getChildByIDRecursive("prev-page"));
    auto next_page = static_cast<CCMenuItemSpriteExtra *>(providerBox->getChildByIDRecursive("next-page"));

    if (prev_page) {
        sz += prev_page->getContentSize().width + 10;
    }
    if (next_page) {
        sz += next_page->getContentSize().width + 10;
    }

    CCLayer *levelInputLayer = CCLayer::create();
    auto layout = RowLayout::create();

    layout->setGrowCrossAxis(true);

    levelInputLayer->setID("level-input-layer");
    levelInputLayer->setLayout(layout);
    levelInputLayer->setContentSize(page->getContentSize());

    SearchInstance *si = new SearchInstance();
    si->_input = in;
    si->_page = page;
    si->_info = info;

    auto spr4 = ButtonSprite::create("Search");
    spr4->setScale(0.7f);
        
    auto btn4 = CCMenuItemSpriteExtra::create(
        spr4, this, menu_selector(ProviderPopup::onLevelIDSearch)
    );
    btn4->setAnchorPoint({0, 0});
    btn4->setUserData(si);

    CCMenu *buttonMenu = CCMenu::create();
    buttonMenu->setContentSize(btn4->getContentSize());

    buttonMenu->addChild(btn4);

    levelInputLayer->addChild(buttonMenu);
    levelInputLayer->addChild(in);

    levelInputLayer->updateLayout();
    levelInputLayer->setPosition(page->getContentSize().width / 2, page->getContentSize().height - levelInputLayer->getContentSize().height + 5);
    
    page->addChild(levelInputLayer);

    TextInput *in2 = TextInput::create(page->getContentSize().width - sz, "", "chatFont.fnt");
    in2->setEnabled(false);
    in2->setPosition(page->getContentSize().width / 2, page->getContentSize().height / 2);
    in2->setAnchorPoint({0.5f, 0.5f});
    in2->setScaleY(2.f);

    page->addChild(in2);

    LoadingCircleLayer *circle = LoadingCircleLayer::create();
    auto csz = page->getContentSize();

    circle->setContentSize(csz);
    circle->setPosition(csz.width / 2, csz.height / 2);
    circle->setID("circle");
    circle->m_pCircle->setScale(0.7f);
    circle->m_pCircle->setOpacity(0);

    page->addChild(circle);
    
    auto popup = ProviderPopup::get();
    popup->_levelPage._cells.clear();
}

void ProviderPopup::onLevelIDSearch(CCObject *sender) {
    auto popup = ProviderPopup::get();

    if (popup->_locked) return;

    SearchInstance *si = (SearchInstance *)(((CCNode *)sender)->getUserData());

    CCNode *existingCirclePtr = si->_page->getChildByID("circle");
    if (existingCirclePtr == nullptr) {
        return;
    }

    LoadingCircleLayer *existingCircle = dynamic_cast<LoadingCircleLayer *>(existingCirclePtr);

    if (existingCircle->m_pCircle->getOpacity() > 0) {
        return;
    }

    if (si->_input->getString().empty()) {
        FLAlertLayer::create("Error", "<cr>Level ID input should not be empty!</c>", "OK")->show();
        return;
    
    }

    bool d = si->_input->getString().find_first_not_of("0123456789") == std::string::npos;

    if (!d) {
        FLAlertLayer::create("Error", "<cr>Level ID input should contain numbers only!</c>", "OK")->show();
        return;
    }

    int levels_limit = 00;

    std::string levelArraySize = ProviderPopup::get()->_levelArraySize;

    d = levelArraySize .find_first_not_of("0123456789") == std::string::npos;

    log::info("levelArraySize ={}",levelArraySize );

    if (!d && !levelArraySize .empty()) {
        FLAlertLayer::create("Error", "<cr>Level array size should contain numbers only!</c>", "OK")->show();
        return;
    }

    if (!levelArraySize.empty()) {
        levels_limit = std::stoi(levelArraySize );
    }

    int id = std::stoi(si->_input->getString());

    si->_info->provider->setParameter(LevelProvider::QueryID, id);
    si->_info->provider->setParameter(LevelProvider::LimitLevelArray, levels_limit);

    for (auto _cell : popup->_levelPage._cells) {
        _cell->removeMeAndCleanup();
    }
    popup->_levelPage._cells.clear();
    si->_info->provider->cleanupLevels(false);

    existingCircle->m_pCircle->setOpacity(0);
    existingCircle->m_pCircle->runAction(cocos2d::CCFadeTo::create(0.35f, 255));
    existingCircle->m_pCircle->setScale(0.7f);
    existingCircle->m_pCircle->setRotation(0);

    CCNode *levelControls  = si->_page->getChildByID("level-page-control");
    if (levelControls != nullptr) {
        levelControls->removeMeAndCleanup();
    }

    // delete this->_levelPage;
    // this->_levelPage = new LevelPageInfo();

    popup->_levelPage._cells.clear();
    popup->_levelPage._cId = id;

    si->_info->provider->downloadLevel([this, si, existingCircle, popup](LevelProvider *prov, GJGameLevel *level) {
        popup->lambdaOnDownloadLevel(si, existingCircle, popup, prov, level);
    });
}

LevelCell *ProviderPopup::createLevelCell(GJGameLevel *level, CCLayer *page) {
    auto csz = page->getContentSize();
    log::info("1");

    LevelCell *cell = LevelCell::create(0.f, 0.f);
    log::info("2");
    cell->loadFromLevel(level);
    log::info("3");
    cell->setPosition(0, csz.height / 2);
    log::info("4");

    CCLayer *base = dynamic_cast<CCLayer *>(cell->getChildByID("main-layer"));
    base->setAnchorPoint({0, 0});
    base->setPositionX(csz.width / 3);
    base->setScale(0.65f);

    std::vector<CCNode *> to_lower;

    to_lower.push_back(dynamic_cast<CCNode *>(cell->getChildByIDRecursive("length-label")));
    to_lower.push_back(dynamic_cast<CCNode *>(cell->getChildByIDRecursive("downloads-label")));
    to_lower.push_back(dynamic_cast<CCNode *>(cell->getChildByIDRecursive("likes-label")));
    to_lower.push_back(dynamic_cast<CCNode *>(cell->getChildByIDRecursive("length-icon")));
    to_lower.push_back(dynamic_cast<CCNode *>(cell->getChildByIDRecursive("downloads-icon")));
    to_lower.push_back(dynamic_cast<CCNode *>(cell->getChildByIDRecursive("likes-icon")));

    for (auto node : to_lower) {
        node->setPositionY(node->getPositionY() - 4);
    }

    CCLabelBMFont *song_name = dynamic_cast<CCLabelBMFont *>(base->getChildByID("song-name"));
    CCMenuItemSpriteExtra *creator_name = dynamic_cast<CCMenuItemSpriteExtra *>(base->getChildByIDRecursive("creator-name"));
    song_name->setPositionY(song_name->getPositionY() - 15);
    creator_name->setPositionY(creator_name->getPositionY() - 15);

    page->addChild(cell);

    CCLabelBMFont *dateLabel = CCLabelBMFont::create(level->m_uploadDate.c_str(), "chatFont.fnt");
    dateLabel->setScale(0.5f);
    dateLabel->setPosition(csz.width / 2, -37);

    cell->addChild(dateLabel);

    return cell;
}

void ProviderPopup::onLevelPage(CCObject *sender) {
    auto button = dynamic_cast<CCMenuItemSpriteExtra *>(sender);

    if (!button) return;

    CCScene *currentScene = CCScene::get();
    auto popup_ = currentScene->getChildByID("provider-popup");
    auto popup = dynamic_cast<ProviderPopup *>(popup_);

    popup->_levelPage._currentLevelsIndex += button->getTag();

    log::info("tag={}", button->getTag());

    if (popup->_levelPage._currentLevelsIndex > (popup->_levelPage._currentLevels.size() - 1)) {
        popup->_levelPage._currentLevelsIndex = popup->_levelPage._currentLevels.size() - 1;
    }
    if (popup->_levelPage._currentLevelsIndex < 0) {
        popup->_levelPage._currentLevelsIndex = 0;
    }

    // CCNode *menu = _levelPage->page->getChildByID("level-page-control");
    // if (!menu) {
    //     FLAlertLayer::create("Error", "1!", "OK")->show();
    //     return;
    // }

    auto prev_page = dynamic_cast<CCMenuItemSpriteExtra *>(getChildByIDRecursive("level-prev-page"));
    auto next_page = dynamic_cast<CCMenuItemSpriteExtra *>(getChildByIDRecursive("level-next-page"));

    auto prev_page_spr = dynamic_cast<CCSprite *>(getChildByIDRecursive("level-prev-page-spr"));
    auto next_page_spr = dynamic_cast<CCSprite *>(getChildByIDRecursive("level-next-page-spr"));

    if (popup->_levelPage._currentLevelsIndex == 0) {
        prev_page->setEnabled(false);
        prev_page_spr->setColor({64, 64, 64});

        next_page_spr->setColor({255, 255, 255});
        next_page->setEnabled(true);
    }
    if (popup->_levelPage._currentLevelsIndex >= 1) {
        prev_page->setEnabled(true);
        next_page->setEnabled(true);

        next_page_spr->setColor({255, 255, 255});
        prev_page_spr->setColor({255, 255, 255});
    }
    if (popup->_levelPage._currentLevelsIndex == (popup->_levelPage._currentLevels.size() - 1)) {
        next_page_spr->setColor({64, 64, 64});
        next_page->setEnabled(false);

        prev_page->setEnabled(true);
        prev_page_spr->setColor({255, 255, 255});
    }

    auto level = popup->_levelPage._currentLevels.at(popup->_levelPage._currentLevelsIndex);

    for (auto cell : popup->_levelPage._cells) {
        cell->setVisible(false);
    }
    
    // int id = level->m_levelID.value();
    // if (id == 0) {
    //     id = popup->_levelPage._cId;
    // }

    // std::string cellname = fmt::format("LevelCell_{}_{}", id, popup->_levelPage._currentLevelsIndex);
    // CCNode *_cell = popup->_levelPage.page->getChildByID(cellname);

    popup->_levelPage._cells[popup->_levelPage._currentLevelsIndex]->setVisible(true);
}

void ProviderPopup::lambdaOnDownloadLevel(SearchInstance *si, LoadingCircleLayer *existingCircle, ProviderPopup *popup, LevelProvider *prov, GJGameLevel *level) {
    if (!popup->isPageHasFeature(si->_info, LevelProvider::QueryID)) {
            delete si;

            return;
        }

        CCScene *currentScene = CCScene::get();
        auto popup_ = currentScene->getChildByID("provider-popup");

        if (!popup_) {
            delete si;

            return;
        }

        CCNode *existingCirclePtr = si->_page->getChildByID("circle");
        if (existingCirclePtr == nullptr) {
            delete si;

            return;
        }

        existingCircle->m_pCircle->setOpacity(0);
    
        if (level == nullptr) {
            FLAlertLayer::create("Error", "<cr>No such level!</c>", "OK")->show();
            return;
        }

        auto more = prov->askMultipleLevels();

        popup->_levelPage._cells.clear();

        if (more.size() == 1) {
            popup->_levelPage._cells.push_back(this->createLevelCell(level, si->_page));
            popup-> _levelPage._cells[0]->setVisible(true);

            return;
        }

        int i = 0;
        for (auto lvl : more) {
            auto cell = popup->createLevelCell(lvl, si->_page);
            cell->setID(fmt::format("LevelCell_{}_{}", lvl->m_levelID.value(), i));
            cell->setVisible(false);

            popup->_levelPage._cells.push_back(cell);

            i++;
        }

        popup->_levelPage._cells[0]->setVisible(true);

        popup->_levelPage._currentLevels = more;
        popup->_levelPage._currentLevelsIndex = 0;
        popup->_levelPage.page = si->_page;

        auto csz = si->_page->getContentSize();

        CCMenu *menu = CCMenu::create();
        menu->setID("level-page-control");

        int delta = 40;

        auto prev_page_spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        prev_page_spr->setColor({64, 64, 64});
        prev_page_spr->setScale(0.5f);
        prev_page_spr->setID("level-prev-page-spr");

        auto prev_page = CCMenuItemSpriteExtra::create(
			prev_page_spr,
			this,
			menu_selector(ProviderPopup::onLevelPage)
		);
        prev_page->setID("level-prev-page");
        prev_page->setPosition(delta, 0);
        prev_page->setEnabled(false);
        prev_page->setTag(-1);

        auto next_page_spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        next_page_spr->setFlipX(true);
        next_page_spr->setScale(0.5f);
        next_page_spr->setID("level-next-page-spr");

        auto next_page = CCMenuItemSpriteExtra::create(
			next_page_spr,
			this,
			menu_selector(ProviderPopup::onLevelPage)
		);
        next_page->setID("level-next-page");
        next_page->setPosition(csz.width - delta, 0);
        next_page->setTag(1);

        menu->addChild(prev_page);
        menu->addChild(next_page);
        menu->setPosition(0, next_page->getContentSize().height / 2 + 10);

        si->_page->addChild(menu);

    applyBottomButtons(si->_page);
}

void ProviderPopup::setupSettingsPage(CCLayer *providerBox) {
    CCLayer *page = dynamic_cast<CCLayer *>(providerBox->getChildByID("page"));
    ProviderPopupInfo *info = (ProviderPopupInfo *)providerBox->getUserData();

    auto *layout = RowLayout::create();
    CCLayer *settings = CCLayer::create();
    auto csz = page->getContentSize();

    settings->setLayout(layout);
    settings->setContentSize(csz);
    settings->setPosition(0, 0);

    page->addChild(settings);

    auto feats = info->provider->getFeatures();
    
    CCSprite *checkOn = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
	CCSprite *checkOff = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");

    // feats[LevelProvider::LPFeatures::LimitLevelArray] || feats[LevelProvider::LPFeatures::SetLevelArrayPage]
    if (feats[LevelProvider::LPFeatures::LimitLevelArray]) {
        // m_pFollowPlayer = CCMenuItemToggler::create(checkOff, checkOn, this, menu_selector(ProviderPopup::onToggler1PressMaybe));
		// CCLabelBMFont *men1_info = CCLabelBMFont::create("Follow Player", "bigFont.fnt");
		// men1_info->setAnchorPoint({0.5f, 0.5f});
		// men1_info->setScale(0.5f);
		// men1_info->setPositionX(100.f);
        // std::string _final = "Set level array size...";
        // if (!this->_levelArraySize.empty()) {
        //     _final = this->_levelArraySize;
        // }

        TextInput *in = TextInput::create(100, "Set level array size...", "chatFont.fnt");
        in->setID("level-array-input");

        if (!ProviderPopup::get()->_levelArraySize.empty()) {
            in->setString(ProviderPopup::get()->_levelArraySize, false);
        }

        in->setCallback([this](const std::string &value) {
            ProviderPopup::get()->_levelArraySize = value;
        });

        settings->addChild(in);
        settings->updateLayout();
    }

    if (feats[LevelProvider::LPFeatures::SetLevelArrayPage]) {
        TextInput *in = TextInput::create(100, "Set level page...", "chatFont.fnt");
        in->setID("level-page-input");

        if (!ProviderPopup::get()->_levelPageStr.empty()) {
            in->setString(ProviderPopup::get()->_levelPageStr, false);
        }
        in->setCallback([this](const std::string &value) {
            ProviderPopup::get()->_levelPageStr = value;
        });

        settings->addChild(in);
        settings->updateLayout();
    }

    settings->setPosition(csz.width / 2, (csz.height - settings->getContentSize().height) / 2 + 15);
}

ProviderPopup *ProviderPopup::get() {
    CCScene *currentScene = CCScene::get();
    auto popup_ = currentScene->getChildByID("provider-popup");
    if (!popup_) return nullptr;
    auto popup = dynamic_cast<ProviderPopup *>(popup_);
    return popup;
}

void ProviderPopup::setupGenericSearchPage(CCLayer *providerBox) {
    CCLayer *page = dynamic_cast<CCLayer *>(providerBox->getChildByID("page"));
    ProviderPopupInfo *info = (ProviderPopupInfo *)providerBox->getUserData();
    
    TextInput *in = TextInput::create(150, "Enter level ID/level name/etc...", "chatFont.fnt");
    in->setPosition(0, 0);
    in->setAnchorPoint({0.5f, 0.5f});

    int sz = 0;
    auto prev_page = static_cast<CCMenuItemSpriteExtra *>(providerBox->getChildByIDRecursive("prev-page"));
    auto next_page = static_cast<CCMenuItemSpriteExtra *>(providerBox->getChildByIDRecursive("next-page"));

    if (prev_page) {
        sz += prev_page->getContentSize().width + 10;
    }
    if (next_page) {
        sz += next_page->getContentSize().width + 10;
    }

    CCLayer *levelInputLayer = CCLayer::create();
    auto layout = RowLayout::create();

    layout->setGrowCrossAxis(true);

    levelInputLayer->setID("level-input-layer");
    levelInputLayer->setLayout(layout);
    levelInputLayer->setContentSize(page->getContentSize());

    SearchInstance *si = new SearchInstance();
    si->_input = in;
    si->_page = page;
    si->_info = info;

    auto spr4 = ButtonSprite::create("Search");
    spr4->setScale(0.7f);
        
    auto btn4 = CCMenuItemSpriteExtra::create(
        spr4, this, menu_selector(ProviderPopup::onGenericSearch)
    );
    btn4->setAnchorPoint({0, 0});
    btn4->setUserData(si);

    CCMenu *buttonMenu = CCMenu::create();
    buttonMenu->setContentSize(btn4->getContentSize());

    buttonMenu->addChild(btn4);

    levelInputLayer->addChild(buttonMenu);
    levelInputLayer->addChild(in);

    levelInputLayer->updateLayout();
    levelInputLayer->setPosition(page->getContentSize().width / 2, page->getContentSize().height - levelInputLayer->getContentSize().height + 5);
    
    page->addChild(levelInputLayer);

    TextInput *in2 = TextInput::create(page->getContentSize().width - sz, "", "chatFont.fnt");
    in2->setEnabled(false);
    in2->setPosition(page->getContentSize().width / 2, page->getContentSize().height / 2);
    in2->setAnchorPoint({0.5f, 0.5f});
    in2->setScaleY(2.f);

    page->addChild(in2);

    LoadingCircleLayer *circle = LoadingCircleLayer::create();
    auto csz = page->getContentSize();

    circle->setContentSize(csz);
    circle->setPosition(csz.width / 2, csz.height / 2);
    circle->setID("circle");
    circle->m_pCircle->setScale(0.7f);
    circle->m_pCircle->setOpacity(0);

    page->addChild(circle);
    
    auto popup = ProviderPopup::get();
    popup->_levelPage._cells.clear();
}

bool ProviderPopup::isPageHasFeature(ProviderPopupInfo *info, int feature) {
    auto feats = info->provider->getFeatures();

    bool settings = feats[LevelProvider::LPFeatures::LimitLevelArray] || feats[LevelProvider::LPFeatures::SetLevelArrayPage];

    switch(info->_page) {
        case 0: {
            if (feats[LevelProvider::LPFeatures::QueryID] && feature == LevelProvider::LPFeatures::QueryID) {
                return true;
            }
            if (feats[LevelProvider::LPFeatures::QueryLevelName] && feature == LevelProvider::LPFeatures::QueryLevelName) {
                return true;
            }
            if (settings &&
                ((feature == LevelProvider::LPFeatures::LimitLevelArray) ||
                 (feature == LevelProvider::LPFeatures::SetLevelArrayPage)
                )
            ) {
                return true;
            }

            break;
        }
        case 1: {
            if (feats[LevelProvider::LPFeatures::QueryLevelName] && feature == LevelProvider::LPFeatures::QueryLevelName) {
                return true;
            }

            if (settings &&
                ((feature == LevelProvider::LPFeatures::LimitLevelArray) ||
                 (feature == LevelProvider::LPFeatures::SetLevelArrayPage)
                )
            ) {
                return true;
            }

            break;
        }
        case 2: {
            if (settings &&
                ((feature == LevelProvider::LPFeatures::LimitLevelArray) ||
                 (feature == LevelProvider::LPFeatures::SetLevelArrayPage)
                )
            ) {
                return true;
            }

            break;
        }
    }

    return false;
}

void ProviderPopup::lambdaOnDownloadLevelList(SearchInstance *si, LoadingCircleLayer *existingCircle, ProviderPopup *popup, LevelProvider *prov, GJGameLevel *level) {
    if (!popup->isPageHasFeature(si->_info, LevelProvider::QueryLevelName)) {
            delete si;

            return;
        }

        CCScene *currentScene = CCScene::get();
        auto popup_ = currentScene->getChildByID("provider-popup");

        if (!popup_) {
            delete si;

            return;
        }

        CCNode *existingCirclePtr = si->_page->getChildByID("circle");
        if (existingCirclePtr == nullptr) {
            delete si;

            return;
        }

        existingCircle->m_pCircle->setOpacity(0);
    
        if (level == nullptr) {
            FLAlertLayer::create("Error", "<cr>No levels were found!</c>", "OK")->show();
            return;
        }

        auto more = prov->askMultipleLevels();

        popup->_levelPage._cells.clear();

        if (more.size() == 1) {
            popup->_levelPage._cells.push_back(this->createLevelCell(level, si->_page));
            popup-> _levelPage._cells[0]->setVisible(true);

            return;
        }

        int i = 0;
        for (auto lvl : more) {
            auto cell = popup->createLevelCell(lvl, si->_page);
            cell->setID(fmt::format("LevelCell_{}_{}", lvl->m_levelID.value(), i));
            cell->setVisible(false);

            popup->_levelPage._cells.push_back(cell);

            i++;
        }

        popup->_levelPage._cells[0]->setVisible(true);

        popup->_levelPage._currentLevels = more;
        popup->_levelPage._currentLevelsIndex = 0;
        popup->_levelPage.page = si->_page;

        auto csz = si->_page->getContentSize();

        CCMenu *menu = CCMenu::create();
        menu->setID("level-page-control");

        int delta = 40;

        auto prev_page_spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        prev_page_spr->setColor({64, 64, 64});
        prev_page_spr->setScale(0.5f);
        prev_page_spr->setID("level-prev-page-spr");

        auto prev_page = CCMenuItemSpriteExtra::create(
			prev_page_spr,
			this,
			menu_selector(ProviderPopup::onLevelPage)
		);
        prev_page->setID("level-prev-page");
        prev_page->setPosition(delta, 0);
        prev_page->setEnabled(false);
        prev_page->setTag(-1);

        auto next_page_spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        next_page_spr->setFlipX(true);
        next_page_spr->setScale(0.5f);
        next_page_spr->setID("level-next-page-spr");

        auto next_page = CCMenuItemSpriteExtra::create(
			next_page_spr,
			this,
			menu_selector(ProviderPopup::onLevelPage)
		);
        next_page->setID("level-next-page");
        next_page->setPosition(csz.width - delta, 0);
        next_page->setTag(1);

        menu->addChild(prev_page);
        menu->addChild(next_page);
        menu->setPosition(0, next_page->getContentSize().height / 2 + 10);

        si->_page->addChild(menu);

    applyBottomButtons(si->_page);
}

void ProviderPopup::onGenericSearch(CCObject *sender) {
    auto popup = ProviderPopup::get();

    if (popup->_locked) return;

    SearchInstance *si = (SearchInstance *)(((CCNode *)sender)->getUserData());

    CCNode *existingCirclePtr = si->_page->getChildByID("circle");
    if (existingCirclePtr == nullptr) {
        return;
    }

    LoadingCircleLayer *existingCircle = dynamic_cast<LoadingCircleLayer *>(existingCirclePtr);

    if (existingCircle->m_pCircle->getOpacity() > 0) {
        return;
    }

    if (si->_input->getString().empty()) {
        FLAlertLayer::create("Error", "<cr>Query should not be empty!</c>", "OK")->show();
        return;
    
    }

    int levels_limit = 00;
    int lp = 0;

    std::string levelArraySize = ProviderPopup::get()->_levelArraySize;
    std::string levelPageStr = ProviderPopup::get()->_levelPageStr;

    bool d = levelArraySize .find_first_not_of("0123456789") == std::string::npos;

    log::info("levelArraySize ={}",levelArraySize );

    if (!d && !levelArraySize .empty()) {
        FLAlertLayer::create("Error", "<cr>Level array size should contain numbers only!</c>", "OK")->show();
        return;
    }

    d = levelPageStr .find_first_not_of("0123456789") == std::string::npos;

    if (!d && !levelPageStr .empty()) {
        FLAlertLayer::create("Error", "<cr>Page number should contain numbers only!</c>", "OK")->show();
        return;
    }

    if (!levelArraySize.empty()) {
        levels_limit = std::stoi(levelArraySize );
    }
    if (!levelPageStr.empty()) {
        lp = std::stoi(levelPageStr);
    }

    si->_info->provider->setParameter(LevelProvider::QueryLevelName, si->_input->getString());
    si->_info->provider->setParameter(LevelProvider::LimitLevelArray, levels_limit);
    si->_info->provider->setParameter(LevelProvider::SetLevelArrayPage, lp);

    for (auto _cell : popup->_levelPage._cells) {
        _cell->removeMeAndCleanup();
    }
    popup->_levelPage._cells.clear();
    si->_info->provider->cleanupLevels(false);

    existingCircle->m_pCircle->setOpacity(0);
    existingCircle->m_pCircle->runAction(cocos2d::CCFadeTo::create(0.35f, 255));
    existingCircle->m_pCircle->setScale(0.7f);
    existingCircle->m_pCircle->setRotation(0);

    CCNode *levelControls  = si->_page->getChildByID("level-page-control");
    if (levelControls != nullptr) {
        levelControls->removeMeAndCleanup();
    }

    // delete this->_levelPage;
    // this->_levelPage = new LevelPageInfo();

    popup->_levelPage._cells.clear();
    popup->_levelPage._cId = 0;

    si->_info->provider->downloadLevel([this, si, existingCircle, popup](LevelProvider *prov, GJGameLevel *level) {
        popup->lambdaOnDownloadLevelList(si, existingCircle, popup, prov, level);
    });
}

void ProviderPopup::onCopyID(CCObject *sender) {
    auto popup = ProviderPopup::get();

    auto level = popup->_levelPage._currentLevels[popup->_levelPage._currentLevelsIndex];

    if (level->m_levelID.value() <= 0) {
        auto notification = Notification::create("Level has an invalid ID.", NotificationIcon::Error, 0.5f);
        notification->show();

        return;
    }

    clipboard::write(fmt::format("{}", level->m_levelID.value()));

    auto notification = Notification::create("Level ID has been copied.", NotificationIcon::Success, 0.5f);
    notification->show();
}

#include <Geode/binding/PlayLayer.hpp>

void ProviderPopup::onPlayLevelDownload(CCObject *sender) {
    auto popup = ProviderPopup::get();

    if (popup->_locked) return;

    popup->_locked = true;

    CCScene *sc = CCScene::get();

    ProviderPopupWait *wait = ProviderPopupWait::create();

    popup->addChild(wait);

    auto level = popup->_levelPage._currentLevels[popup->_levelPage._currentLevelsIndex];

    popup->_selectedProvider->setParameter(LevelProvider::SpecificRecord, level->m_demonVotes);

    popup->_selectedProvider->getLevelData(level->m_levelID.value(), [popup, level, wait] (LevelProvider *prov, std::string data, struct LevelProvider::BasicLevelInformation info) {
        auto newpopup = ProviderPopup::get();

        if (!newpopup || newpopup != popup) return;

        wait->cleanup();
        wait->removeMeAndCleanup();

        newpopup->_locked = false;

        if (data.empty()) {
            FLAlertLayer::create("Error", "<cr>Level data is empty!</c>", "OK")->show();
            return;
        }

        if (data[0] == '-') {
            FLAlertLayer::create("Error", fmt::format("<cr>Error while getting level data: </c><cy>{}</c>", prov->getErrorCodeDescription(data)), "OK")->show();
            return;
        }

        level->m_levelString = data;
        level->m_songID = info.songID;
        level->m_audioTrack = info.musicID;
        level->m_songIDs = info._22songs;
        level->m_sfxIDs = info._22sfxs;

        // log::info("level string: {}", data);

        auto scene = PlayLayer::scene(level, false, false);

        auto transition = CCTransitionFade::create(0.5f, scene);

        CCDirector::sharedDirector()->replaceScene(transition);
    });
}

void ProviderPopup::applyBottomButtons(CCLayer *page) {
    CCMenu *levelActionsMenu = CCMenu::create();
    RowLayout *rLayout = RowLayout::create();
    levelActionsMenu->setLayout(rLayout);

    {
        auto spr4 = ButtonSprite::create("Copy ID");
        spr4->setScale(0.6f);
            
        auto btn4 = CCMenuItemSpriteExtra::create(
            spr4, this, menu_selector(ProviderPopup::onCopyID)
        );

        levelActionsMenu->addChild(btn4);
        levelActionsMenu->updateLayout();
    }

    {
        auto spr4 = ButtonSprite::create("Play Level");
        spr4->setScale(0.6f);
            
        auto btn4 = CCMenuItemSpriteExtra::create(
            spr4, this, menu_selector(ProviderPopup::onPlayLevelDownload)
        );

        levelActionsMenu->addChild(btn4);
        levelActionsMenu->updateLayout();
    }

    levelActionsMenu->setPosition(0, 0);

    auto csz1 = page->getContentSize();
    auto csz2 = levelActionsMenu->getContentSize();
    
    csz2.width = csz1.width;

    levelActionsMenu->setContentSize(csz2);
    levelActionsMenu->updateLayout();

    levelActionsMenu->setPosition(csz1.width / 2, csz2.height);

    page->addChild(levelActionsMenu);
}