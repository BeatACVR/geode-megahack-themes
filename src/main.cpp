#include <Geode/Geode.hpp>

using namespace geode::prelude;

#define GET_SAVED_INT(key) Mod::get()->getSavedValue<int>(key)
#define GET_SAVED_BOOL(key) Mod::get()->getSavedValue<bool>(key)
#define SET_SAVED_INT(key, value) Mod::get()->setSavedValue<int>(key, value)

class themeUtils {
public:
    // mega hack uses ints for colors but geode settings use cccolor3b for colors.
	static unsigned int colorToInt(cocos2d::ccColor3B color) {
        return (color.r << 16) | (color.g << 8) | color.b;
    }
	// child proofing because toddlers will set colors the same and complain about visibility
    static void checkForMatchingColors(cocos2d::ccColor3B mainColor, cocos2d::ccColor3B otherColor) {
        if (mainColor == otherColor) {
            log::debug("Warning: Main accent color matches background accent color!");
            FLAlertLayer::create("Warning", "Your colors are conflicting! This may cause visibility issues.", "OK")->show();
        }
    }

	static void prepMainAccent(cocos2d::ccColor3B newMainColor) {
		int mainColorInt = themeUtils::colorToInt(newMainColor);
        cocos2d::ccColor3B currentBackground = Mod::get()->getSettingValue<cocos2d::ccColor3B>("background-accent");
        Mod::get()->setSavedValue<int>("MAIN_ACCENT_INT", mainColorInt);
	}
	static void prepBackgroundAccent(cocos2d::ccColor3B newBackgroundColor) {
		int backgroundColorInt = themeUtils::colorToInt(newBackgroundColor);
        cocos2d::ccColor3B currentMain = Mod::get()->getSettingValue<cocos2d::ccColor3B>("main-accent");
        Mod::get()->setSavedValue<int>("BACKGROUND_ACCENT_INT", backgroundColorInt);
	}
	static void prepTabTextAccent(cocos2d::ccColor3B newTabTextColor) {
		int tabTextColorInt = themeUtils::colorToInt(newTabTextColor);
		cocos2d::ccColor3B currentMain = Mod::get()->getSettingValue<cocos2d::ccColor3B>("main-accent");
		Mod::get()->setSavedValue<int>("TAB_TEXT_ACCENT_INT", tabTextColorInt);
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
			jsonData.get("V_INT").unwrap()["HOME/TAB_TEXT"] = Mod::get()->getSavedValue<int>("TAB_TEXT_ACCENT_INT");
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
    // write new colors after game restart and make sure colors arent pure black.
	themeUtils::prepMainAccent(Mod::get()->getSettingValue<cocos2d::ccColor3B>("main-accent"));
	themeUtils::prepBackgroundAccent(Mod::get()->getSettingValue<cocos2d::ccColor3B>("background-accent"));
	themeUtils::prepTabTextAccent(Mod::get()->getSettingValue<cocos2d::ccColor3B>("tabtext-accent"));

	for (auto key : { "MAIN_ACCENT_INT", "BACKGROUND_ACCENT_INT", "TAB_TEXT_ACCENT_INT" }) {
		if (!GET_SAVED_INT(key)) Mod::get()->setSavedValue<int>(key, 65793);
	}
	
	if (GET_SAVED_INT("MAIN_ACCENT_INT") || GET_SAVED_INT("BACKGROUND_ACCENT_INT") || GET_SAVED_INT("TAB_TEXT_ACCENT_INT") || GET_SAVED_BOOL("light-mode")) {
		themeUtils::writeStartupValues();
	}

    listenForSettingChanges("main-accent", [](cocos2d::ccColor3B newMainColor) {
        themeUtils::prepMainAccent(newMainColor);
		themeUtils::checkForMatchingColors(newMainColor, Mod::get()->getSettingValue<cocos2d::ccColor3B>("background-accent"));
    });
    listenForSettingChanges("background-accent", [](cocos2d::ccColor3B newBackgroundColor) {
        themeUtils::prepBackgroundAccent(newBackgroundColor);
		themeUtils::checkForMatchingColors(Mod::get()->getSettingValue<cocos2d::ccColor3B>("main-accent"), newBackgroundColor);
    });
	listenForSettingChanges("tabtext-accent", [](cocos2d::ccColor3B newTabTextColor) {
		themeUtils::prepTabTextAccent(newTabTextColor);
		themeUtils::checkForMatchingColors(Mod::get()->getSettingValue<cocos2d::ccColor3B>("main-accent"), newTabTextColor);
	});
};