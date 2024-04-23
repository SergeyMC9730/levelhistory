#include "ProviderPopupWait.hpp"
#include "LoadingCircleLayer.hpp"

bool ProviderPopupWait::init() {
    if (!CCLayer::init()) return false;

    CCLayer *l = cocos2d::CCLayer::create();
    l->setID("main-layer");

    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    auto base = cocos2d::CCSprite::create("square.png");

    base->setPosition({ 0, 0 });
    base->setScale(500.f);
    base->setColor({0, 0, 0});
    base->setOpacity(0);
    base->runAction(cocos2d::CCFadeTo::create(0.5f, 125));

    auto loadingCircle = LoadingCircleLayer::create();
    loadingCircle->m_pCircle->setOpacity(0);
    loadingCircle->m_pCircle->runAction(cocos2d::CCFadeTo::create(0.5f, 255));
    loadingCircle->setPosition(winSize.width / 2, winSize.height / 2 );
    addChild(base, -1);

    l->addChild(loadingCircle, 1);

    _circle = loadingCircle;

    addChild(l, 1024);

    // setTouchPriority(-1024);
    // registerWithTouchDispatcher();

    // cocos2d::CCTouchDispatcher *dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    // dispatcher->registerForcePrio(this, -1024);
    // dispatcher->addPrioTargetedDelegate(this, -1024, true);

    return true;
};

void ProviderPopupWait::cleanup() {
    // cocos2d::CCTouchDispatcher *dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    // setTouchPriority(1);
    // dispatcher->registerForcePrio(this, 1);
    // dispatcher->unregisterForcePrio(this);
    // dispatcher->addPrioTargetedDelegate(this, 1, true);
}