#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ThemeSaverSettingV3 : public SettingV3 {
public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
        auto res = std::make_shared<ThemeSaverSettingV3>();
        auto root = checkJson(json, "ThemeSaverSettingV3");

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

class ThemeSaverSettingNodeV3 : public SettingNodeV3 {
protected:
    ButtonSprite* m_saveButtonSprite;
    ButtonSprite* m_loadButtonSprite;
    CCMenuItemSpriteExtra* m_saveButton;
    CCMenuItemSpriteExtra* m_LoadButton;

    bool init(std::shared_ptr<ThemeSaverSettingV3> setting, float width) {
        if (!SettingNodeV3::init(setting, width))
            return false;
        
        m_saveButtonSprite = ButtonSprite::create("Save", "goldFont.fnt", "GJ_button_01.png", 1.f);
        m_saveButton = CCMenuItemSpriteExtra::create(
            m_saveButtonSprite, this, menu_selector(ThemeSaverSettingNodeV3::onThemeSave)
        );
        m_loadButtonSprite = ButtonSprite::create("Load", "goldFont.fnt", "GJ_button_01.png", 1.f);
        m_LoadButton = CCMenuItemSpriteExtra::create(
            m_loadButtonSprite, this, menu_selector(ThemeSaverSettingNodeV3::onThemeLoad)
        );
        this->getButtonMenu()->addChild(m_saveButton);
        this->getButtonMenu()->addChild(m_LoadButton);
        this->getButtonMenu()->setContentWidth(120);
        this->getButtonMenu()->updateLayout();
        this->getButtonMenu()->setLayout(RowLayout::create());

        this->updateState(nullptr);
        
        return true;
    }

    void onThemeSave(CCObject*) {
        auto savePopup = geode::createQuickPopup(
            "Theme Saver",
            "Enter a name for your theme:\n\n\n",
            "Cancel", "Save",
            [this](auto, bool btn2) {
                if (btn2) {
                    FLAlertLayer::create(
                        "Theme Saver",
                        "Still need to implement saving",
                        "OK"
                    )->show();
                }
            }
        );
        auto inputFieldBg = CCLayerColor::create(ccc4(0, 0, 0, 100), 210.f, 35.f);
        auto inputField = TextInput::create(1.f, "Theme Name", "bigFont.fnt");
        inputField->setMaxCharCount(24);

        inputFieldBg->setID("theme-name-input-bg"_spr);
        inputField->setID("theme-name-input"_spr);
        inputField->setPosition(0.f, savePopup->getContentSize().height / 8);
        inputFieldBg->setPosition(0.f, 0);
        inputFieldBg->setAnchorPoint({0.5f, 0.f});
        inputFieldBg->ignoreAnchorPointForPosition(false);
        inputField->addChild(inputFieldBg, -1);
        savePopup->m_buttonMenu->addChild(inputField);

    }

    void onThemeLoad(CCObject*) {
        FLAlertLayer::create(
            "Theme Loader",
            "Still need to implement loading",
            "OK"
        )->show();
    }

    // Both of these can just be no-ops, since they make no sense for our 
    // setting as it's just a button
    void onCommit() override {}
    void onResetToDefault() override {}

public:
    static ThemeSaverSettingNodeV3* create(std::shared_ptr<ThemeSaverSettingV3> setting, float width) {
        auto ret = new ThemeSaverSettingNodeV3();
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

    std::shared_ptr<ThemeSaverSettingV3> getSetting() const {
        return std::static_pointer_cast<ThemeSaverSettingV3>(SettingNodeV3::getSetting());
    }
};

SettingNodeV3* ThemeSaverSettingV3::createNode(float width) {
    return ThemeSaverSettingNodeV3::create(
        std::static_pointer_cast<ThemeSaverSettingV3>(shared_from_this()),
        width
    );
}

$execute {
    (void)Mod::get()->registerCustomSettingType("theme-saver", &ThemeSaverSettingV3::parse);
}