#include <Geode/Geode.hpp>
#include <iostream>

using namespace geode::prelude;

inline std::pair<int, int> get_attempt_count_for_folder(int folder, CCDictionary* dict) {
    int total = 0;
    int downloaded_total = 0;
    CCDictElement* value;
    CCDICT_FOREACH(dict, value) {
        auto lvl = reinterpret_cast<GJGameLevel*>(value->getObject());
        if (lvl && (!folder || lvl->m_levelFolder == folder)) {
            total += lvl->m_attempts;
            if (!lvl->m_levelNotDownloaded) downloaded_total += lvl->m_attempts;
        }
    }
    return {total, downloaded_total};
}

inline std::pair<int, int> get_attempt_count_for_folder(int folder, CCArray* arr) {
    int total = 0;
    CCObject* value;
    CCARRAY_FOREACH(arr, value) {
        auto lvl = reinterpret_cast<GJGameLevel*>(value);
        if (lvl && (!folder || lvl->m_levelFolder == folder)) {
            total += lvl->m_attempts;
        }
    }
    return {total, total};
}

inline const std::string format_commas(int value) {
    // https://stackoverflow.com/a/24192835/9124836
    auto str = std::to_string(value);
    int n = str.length() - 3;
    while (n > 0) {
        str.insert(n, ",");
        n -= 3;
    }
    return str;
}

#include <Geode/modify/LevelBrowserLayer.hpp>
class $modify(MyLevelBrowserLayer, LevelBrowserLayer) {
	bool init(GJSearchObject* so) {
		if (!LevelBrowserLayer::init(so)) {
			return false;
		}

		if (so->m_searchType == SearchType::MyLevels || so->m_searchType == SearchType::SavedLevels) {
            auto menu = this->getChildByID("my-levels-menu");

            auto sprite = CCSprite::create("GJ_button_01.png");
            auto skull = CCSprite::createWithSpriteFrameName("miniSkull_001.png");
            skull->setPosition({20, 20});
            sprite->addChild(skull);
            auto btn = CCMenuItemSpriteExtra::create(
                sprite,
                this,
                menu_selector(MyLevelBrowserLayer::callback)
            );
            
            if (menu) {
                menu->addChild(btn);
                menu->updateLayout();
            } else {
                auto menu = CCMenu::create();
                
                menu->addChild(btn);
                menu->setPosition({30, 120});
                this->addChild(menu);
            }
		}

		return true;
	}

	void callback(CCObject*) {
        auto folder = this->m_searchObject->m_folder;
        int total, downloaded_total;
        std::pair<int, int> result;
        if (this->m_searchObject->m_searchType == SearchType::MyLevels) {
            result = get_attempt_count_for_folder(folder, LocalLevelManager::sharedState()->m_localLevels);
        } else {
            result = get_attempt_count_for_folder(folder, GameLevelManager::sharedState()->m_onlineLevels);
        }
        total = result.first;
        downloaded_total = result.second;
        auto str = format_commas(downloaded_total) + " attempts";
        if (total != downloaded_total)
            str += "\n" + format_commas(total) + " attempts <cr>from unloaded levels</c>";
        FLAlertLayer::create("Attempt Count", str.c_str(), "OK")->show();
	}
};