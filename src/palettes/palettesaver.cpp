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
    CircleButtonSprite* m_folderButtonSprite;
    CCMenuItemSpriteExtra* m_saveButton;
    CCMenuItemSpriteExtra* m_LoadButton;
    CCMenuItemSpriteExtra* m_folderButton;
    std::string m_saveTextboxInput = "Unnamed";
    std::string m_loadTextboxInput = "";

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
        m_folderButtonSprite = CircleButtonSprite::createWithSpriteFrameName("folderIcon_001.png", 1.1f, CircleBaseColor::Green);
        m_folderButtonSprite->setScale(.7f);
        m_folderButton = CCMenuItemSpriteExtra::create(
            m_folderButtonSprite, this, menu_selector(PaletteSaverSettingNodeV3::onFolderButton)
        );

        this->getButtonMenu()->addChild(m_folderButton);
        this->getButtonMenu()->addChild(m_saveButton);
        this->getButtonMenu()->addChild(m_LoadButton);
        this->getButtonMenu()->setContentWidth(160);
        this->getButtonMenu()->updateLayout();
        this->getButtonMenu()->setLayout(RowLayout::create());

        this->updateState(nullptr);
        
        return true;
    }

    void onFolderButton(CCObject*) {
        file::openFolder(Mod::get()->getConfigDir() / "savedPalettes");
    }

    void onPaletteSave(CCObject*) {
        FLAlertLayer* savePopup = geode::createQuickPopup(
            "Palette Saver",
            "Enter a name for your palette:\n\n\n",
            "Cancel", "Save",
            [this, savePopup](auto, bool btn2) {
                if (btn2) {
                    savePalette(m_saveTextboxInput);
                    FLAlertLayer::create("Palette Saver", "Saved palette successfully!", "OK")->show();
                }
            }
        );
        
        auto inputField = TextInput::create(CCDirector::sharedDirector()->getWinSize().width / 3, "Palette Name", "bigFont.fnt");
        inputField->setMaxCharCount(24);
        inputField->setID("palette-name-input"_spr);
        inputField->setPosition(0.f, savePopup->getContentSize().height / 8);
        inputField->setCallback([=](std::string input) {
            m_saveTextboxInput = inputField->getString();
        });
        savePopup->m_buttonMenu->addChild(inputField);

    }

    void onPaletteLoad(CCObject*) {
        FLAlertLayer* loadPopup = geode::createQuickPopup(
            "Palette Loader",
            "Enter the name of the palette you want to load:\n\n\n",
            "Cancel", "Load",
            [this, loadPopup](auto, bool btn2) {
                if (btn2) {
                    FLAlertLayer::create("Palette Loader", fmt::format("{}", loadPalette(m_loadTextboxInput)), "OK")->show();
                }
            }
        );
        
        auto inputField = TextInput::create(CCDirector::sharedDirector()->getWinSize().width / 3, "Palette Name", "bigFont.fnt");
        inputField->setMaxCharCount(24);
        inputField->setID("palette-name-input"_spr);
        inputField->setPosition(0.f, loadPopup->getContentSize().height / 8);
        inputField->setCallback([=](std::string input) {
            m_loadTextboxInput = inputField->getString();
        });
        loadPopup->m_buttonMenu->addChild(inputField);
    }

    void savePalette(std::string paletteName = "Failed To Name") {
        if (paletteName.empty()) paletteName = "Failed To Name";
        std::filesystem::path configDir = Mod::get()->getConfigDir();
        std::filesystem::create_directories(configDir / "savedPalettes");
        std::filesystem::path savePath = configDir / "savedPalettes" / (paletteName + ".mhpalette");

        auto mainColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("main-accent");
        auto backgroundColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("background-accent");
        auto tabtextColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("tabtext-accent");
        auto lightMode = Mod::get()->getSettingValue<bool>("light-mode");

        auto jsonData = matjson::makeObject({ {"main-accent", mainColor},
                                             {"background-accent", backgroundColor},
                                             {"tabtext-color", tabtextColor},
                                             {"light-mode", lightMode} });
        
        if (!utils::file::writeString(savePath, jsonData.dump())) {
            log::error("Failed to save palette data.");
        }
    }

    std::string loadPalette(std::string paletteName = "") {
        if (paletteName.empty()) return "Please enter a palette name.";
        std::filesystem::path configDir = Mod::get()->getConfigDir();
        std::filesystem::create_directories(configDir / "savedPalettes");
        std::filesystem::path palettesPath = configDir / "savedPalettes";

        if (!std::filesystem::exists(palettesPath / (paletteName + ".mhpalette"))) {
            return "Palette does not exist.";
        }

        auto jsonData = utils::file::readJson(palettesPath / (paletteName + ".mhpalette")).unwrapOr(-2);

        // main-accent
        {
            auto& c = jsonData["main-accent"];
            cocos2d::ccColor3B color{
                static_cast<GLubyte>(c["r"].asInt().unwrap()),
                static_cast<GLubyte>(c["g"].asInt().unwrap()),
                static_cast<GLubyte>(c["b"].asInt().unwrap())
            };
            Mod::get()->setSettingValue<cocos2d::ccColor3B>("main-accent", color);
        }

        // background-accent
        {
            auto& c = jsonData["background-accent"];
            cocos2d::ccColor3B color{
                static_cast<GLubyte>(c["r"].asInt().unwrap()),
                static_cast<GLubyte>(c["g"].asInt().unwrap()),
                static_cast<GLubyte>(c["b"].asInt().unwrap())
            };
            Mod::get()->setSettingValue<cocos2d::ccColor3B>("background-accent", color);
        }

        // tabtext-color
        {
            auto& c = jsonData["tabtext-color"];
            cocos2d::ccColor3B color{
                static_cast<GLubyte>(c["r"].asInt().unwrap()),
                static_cast<GLubyte>(c["g"].asInt().unwrap()),
                static_cast<GLubyte>(c["b"].asInt().unwrap())
            };
            Mod::get()->setSettingValue<cocos2d::ccColor3B>("tabtext-color", color);
        }

        // light-mode
        {
            bool lightMode = jsonData["light-mode"].asBool().unwrap();
            Mod::get()->setSettingValue<bool>("light-mode", lightMode);
        }

        return "Palette loaded!";
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