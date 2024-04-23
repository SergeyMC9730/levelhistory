#pragma once

#include <Geode/Geode.hpp>

class LoadingCircleLayer;

class ProviderPopupWait : public cocos2d::CCLayer {
private:
    LoadingCircleLayer *_circle;
public:
    CREATE_FUNC(ProviderPopupWait);

    bool init();
    void cleanup();
};