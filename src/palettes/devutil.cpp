#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

class $modify(PaletteMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) {
			return false;
		}

		auto myButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("blackCogwheel_03_001.png"),
			this,
			menu_selector(PaletteMenuLayer::onMyButton)
		);

		auto menu = this->getChildByID("right-side-menu");
		menu->addChild(myButton);

		myButton->setID("my-button"_spr);

		menu->updateLayout();
		return true;
	}
	void onMyButton(CCObject*) {
		geode::openSettingsPopup(Mod::get(), true);
	}
};