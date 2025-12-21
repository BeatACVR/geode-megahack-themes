#include <Geode/Geode.hpp>
#include "../palettes/paletteloader.hpp"

using namespace geode::prelude;

class PaletteSaverSettingV3 : public SettingV3 {
public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
        auto res = std::make_shared<PaletteSaverSettingV3>();
        auto root = checkJson(json, "PaletteSaverSettingV3");

        res->init(key, modID, root);
        res->parseNameAndDescription(root);
        res->parseEnableIf(root);
        
        root.checkUnknownKeys();
        return root.ok(std::static_pointer_cast<SettingV3>(res));
    }

    bool load(matjson::Value const& json) override {
        return true;
    }
    bool save(matjson::Value& json) const override {
        return true;
    }

    bool isDefaultValue() const override {
        return true;
    }
    void reset() override {}

    SettingNodeV3* createNode(float width) override;
};

class PaletteSaverSettingNodeV3 : public SettingNodeV3 {
protected:
    ButtonSprite* m_saveButtonSprite;
    ButtonSprite* m_loadButtonSprite;
    CCMenuItemSpriteExtra* m_saveButton;
    CCMenuItemSpriteExtra* m_LoadButton;
    std::string m_textboxInput = "Unnamed";

    bool init(std::shared_ptr<PaletteSaverSettingV3> setting, float width) {
        if (!SettingNodeV3::init(setting, width))
            return false;
        
        m_saveButtonSprite = ButtonSprite::create("Save", "goldFont.fnt", "GJ_button_01.png", 1.f);
        m_saveButton = CCMenuItemSpriteExtra::create(
            m_saveButtonSprite, this, menu_selector(PaletteSaverSettingNodeV3::onPaletteSave)
        );
        m_loadButtonSprite = ButtonSprite::create("Load", "goldFont.fnt", "GJ_button_01.png", 1.f);
        m_LoadButton = CCMenuItemSpriteExtra::create(
            m_loadButtonSprite, this, menu_selector(PaletteSaverSettingNodeV3::onPaletteLoad)
        );
        this->getButtonMenu()->addChild(m_saveButton);
        this->getButtonMenu()->addChild(m_LoadButton);
        this->getButtonMenu()->setContentWidth(120);
        this->getButtonMenu()->updateLayout();
        this->getButtonMenu()->setLayout(RowLayout::create());

        this->updateState(nullptr);
        
        return true;
    }

    void onPaletteSave(CCObject*) {
        FLAlertLayer* savePopup = geode::createQuickPopup(
            "Palette Saver",
            "Enter a name for your palette:\n\n\n",
            "Cancel", "Save",
            [this, savePopup](auto, bool btn2) {
                if (btn2) {
                    savePalette(m_textboxInput);
                    FLAlertLayer::create("Palette Saver", "Saved palette successfully!", "OK")->show();
                }
            }
        );
        
        auto inputField = TextInput::create(CCDirector::sharedDirector()->getWinSize().width / 3, "Palette Name", "bigFont.fnt");
        inputField->setMaxCharCount(24);
        inputField->setID("palette-name-input"_spr);
        inputField->setPosition(0.f, savePopup->getContentSize().height / 8);
        inputField->setCallback([=](std::string input) {
            m_textboxInput = inputField->getString();
        });
        savePopup->m_buttonMenu->addChild(inputField);

    }

    void onPaletteLoad(CCObject*) {
        CCLayer* loader = PaletteLoaderNode::create();
        CCDirector::sharedDirector()->getRunningScene()->addChild(loader, 10000);
    }

    void savePalette(std::string paletteName = "Failed To Name") {
        if (paletteName.empty()) paletteName = "Failed To Name";
        std::filesystem::path configDir = Mod::get()->getConfigDir();
        std::filesystem::create_directories(configDir / "savedPalettes");
        std::filesystem::path savePath = configDir / "savedPalettes" / (paletteName + ".mhpalette");

        auto mainColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("main-accent");
        auto backgroundColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("background-accent");
        auto tabtextColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("tabtext-color");
        auto lightMode = Mod::get()->getSettingValue<bool>("light-mode");

        auto jsonData = matjson::makeObject({ {"main-accent", mainColor},
                                             {"background-accent", backgroundColor},
                                             {"tabtext-color", tabtextColor},
                                             {"light-mode", lightMode} });
        
        if (!utils::file::writeString(savePath, jsonData.dump())) {
            log::error("Failed to save palette data.");
        }
    }
    
    // Both of these can just be no-ops, since they make no sense for our 
    // setting as it's just a button
    void onCommit() override {}
    void onResetToDefault() override {}

public:
    static PaletteSaverSettingNodeV3* create(std::shared_ptr<PaletteSaverSettingV3> setting, float width) {
        auto ret = new PaletteSaverSettingNodeV3();
        if (ret->init(setting, width)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool hasUncommittedChanges() const override {
        return false;
    }
    bool hasNonDefaultValue() const override {
        return false;
    }

    std::shared_ptr<PaletteSaverSettingV3> getSetting() const {
        return std::static_pointer_cast<PaletteSaverSettingV3>(SettingNodeV3::getSetting());
    }
};

SettingNodeV3* PaletteSaverSettingV3::createNode(float width) {
    return PaletteSaverSettingNodeV3::create(
        std::static_pointer_cast<PaletteSaverSettingV3>(shared_from_this()),
        width
    );
}

$execute {
    (void)Mod::get()->registerCustomSettingType("palette-saver", &PaletteSaverSettingV3::parse);
}