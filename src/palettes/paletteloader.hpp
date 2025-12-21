#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class PaletteLoaderNode : public CCLayer {
protected:
    bool init() {
        if (!CCLayer::init())
            return false;

        CCScale9Sprite* bg = CCScale9Sprite::create("GJ_square01.png", CCRectMake(0, 0, 120, 80));
        this->addChild(bg);
        // Implementation for loading palettes goes here

        return true;
    }
};