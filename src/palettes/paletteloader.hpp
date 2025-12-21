//
// UNFINISHED AND UNUSED
/*
#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class PaletteLoaderNode : public CCLayer {
public:
    static CCLayer* create() {
        auto ret = new PaletteLoaderNode();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool init() {
        if (!CCLayer::init())
            return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        
        // Background
        CCScale9Sprite* bgSpr = CCScale9Sprite::create("GJ_square01.png");
        bgSpr->setContentSize({ 360, 240 });
        bgSpr->setID("paletteLoaderBG"_spr);
        this->addChildAtPosition(bgSpr, Anchor::Center, { winSize.width / 2 , winSize.height / 2 });
        this->setZOrder(1055);

        // Close button
        auto closeSpr = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
        auto closeBtn = CCMenuItemSpriteExtra::create(closeSpr, this, menu_selector(PaletteLoaderNode::onCloseBtn));
        closeBtn->setID("paletteLoaderCloseBtn"_spr);
        closeBtn->setPosition({ bgSpr->getPositionX() / 1.5f, bgSpr->getPositionY() * 1.25f});
        closeBtn->setZOrder(1002);
        this->addChild(closeBtn);

        return true;
    }

    void onCloseBtn(CCObject* sender) {
        log::debug("close button pressed");
        this->setKeypadEnabled(false);
        this->setTouchEnabled(false);
        this->removeFromParentAndCleanup(true);
    }

    virtual void keyBackClicked() {
        this->onCloseBtn(nullptr);
    }
};
*/