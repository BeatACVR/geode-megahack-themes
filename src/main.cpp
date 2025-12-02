#include <Geode/Geode.hpp>

using namespace geode::prelude;

class themeUtils {
public:
    // mega hack uses ints for colors but geode settings use cccolor3b for colors.
	static unsigned int colorToInt(cocos2d::ccColor3B color) {
        return (color.r << 16) | (color.g << 8) | color.b;
    }
	// child proofing because toddlers will set both colors the same and complain about visibility
    static void checkForMatchingColors(cocos2d::ccColor3B mainColor, cocos2d::ccColor3B backgroundColor) {
        if (mainColor == backgroundColor) {
            log::debug("Warning: Main accent color matches background accent color!");
            FLAlertLayer::create("Warning", "Main accent color matches background accent color! This may cause visibility issues.", "OK")->show();
        }
    }

    static void writeStartupValues() {
        auto path = Loader::get()->getInstalledMod("absolllute.megahack")->getSaveDir() / "v9" / "home.json";

        // read file
        std::ifstream jsonFile(path);
        if (!jsonFile.is_open()) return;
        std::string content((std::istreambuf_iterator<char>(jsonFile)), std::istreambuf_iterator<char>());
        jsonFile.close();

        auto jsonData = matjson::parse(content).unwrapOr(-2);

        // set to new values
		if (jsonData.contains("V_INT")) {
            jsonData.get("V_INT").unwrap()["HOME/ACCENT"] = Mod::get()->getSavedValue<int>("MAIN_ACCENT_INT");
            jsonData.get("V_INT").unwrap()["HOME/BACKGROUND"] = Mod::get()->getSavedValue<int>("BACKGROUND_ACCENT_INT");
        }
        if (jsonData.contains("V_BOOL")) {
            jsonData.get("V_BOOL").unwrap()["HOME/LIGHT_MODE"] = Mod::get()->getSettingValue<bool>("light-mode");
        }

		// write file
        std::ofstream outFile(path);
        if (!outFile.is_open()) return;
        outFile << jsonData.dump();
    }
};

$execute {
    // write new colors after game restart
	themeUtils::writeStartupValues();

    listenForSettingChanges("main-accent", [](cocos2d::ccColor3B newMainColor) {
        int mainColorInt = themeUtils::colorToInt(newMainColor);
        cocos2d::ccColor3B currentBackground = Mod::get()->getSettingValue<cocos2d::ccColor3B>("background-accent");
        themeUtils::checkForMatchingColors(newMainColor, currentBackground);
        Mod::get()->setSavedValue<int>("MAIN_ACCENT_INT", mainColorInt);
    });
    listenForSettingChanges("background-accent", [](cocos2d::ccColor3B newBackgroundColor) {
        int backgroundColorInt = themeUtils::colorToInt(newBackgroundColor);
        cocos2d::ccColor3B currentMain = Mod::get()->getSettingValue<cocos2d::ccColor3B>("main-accent");
        themeUtils::checkForMatchingColors(currentMain, newBackgroundColor);
        Mod::get()->setSavedValue<int>("BACKGROUND_ACCENT_INT", backgroundColorInt);
    });
};