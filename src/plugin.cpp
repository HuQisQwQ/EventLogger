#include <llapi/EventAPI.h>
#include <llapi/LoggerAPI.h>
// #include <llapi/FormUI.h>
#include <llapi/DynamicCommandAPI.h>

// #include <llapi/mc/Block.hpp>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/ItemStack.hpp>
#include <llapi/mc/Level.hpp>
// #include <llapi/mc/BlockInstance.hpp>

#include "version.h"
#include "pch.h"

string filePath = "plugins/EventLogger/setting.json";

Logger Tc("EventLogger");

void GetCurrentFormattedTimeAndWriteToFile(std::string filename, std::string message) {
    // 获取当前时间的系统时钟
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    // 将时间转换为本地时间结构
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    // 使用本地时间结构创建 tm 结构体
    std::tm localTime = *std::localtime(&currentTime);

    // 格式化日期和时间
    std::ostringstream formattedDateTime;
    formattedDateTime << std::setfill('0') << std::setw(4) << (localTime.tm_year + 1900) << '.'
                      << std::setw(2) << std::setfill('0') << (localTime.tm_mon + 1) << '.'
                      << std::setw(2) << std::setfill('0') << localTime.tm_mday << "->"
                      << std::setw(2) << std::setfill('0') << localTime.tm_hour << ':'
                      << std::setw(2) << std::setfill('0') << localTime.tm_min << ':'
                      << std::setw(2) << std::setfill('0') << localTime.tm_sec;

    // 打开文件以追加写入模式
    std::ofstream fileStream(filename, std::ios::app);
    if (fileStream.is_open()) {
        // 将格式化的时间信息写入文件并添加换行符
        fileStream << formattedDateTime.str() << "[" << message << "]" << std::endl;

        // 关闭文件流
        fileStream.close();
    } else {
        Tc.error("Can\'t find file!");
    }
}

void SendSuccessText(CommandOrigin const& origin, std::string text) {
	if (origin.getPlayer() == nullptr) { //console
		Tc.info(text);
	} else { // game master
		origin.getPlayer()->sendTextPacket(text);
	}
}

void MainLogger(std::string message) {
	vector<Player*> players = Level::getAllPlayers();
	for (Player* it : players) {
		if (it->getPlayerPermissionLevel() == PlayerPermissionLevel::Operator) it->sendTextPacket(message);
	}
	Tc.info(message);
	string path = "plugins/EventLogger/log.txt";
	GetCurrentFormattedTimeAndWriteToFile(path, message);

}

void PluginInit() {
	
	Event::ServerStartedEvent::subscribe([](const Event::ServerStartedEvent& ev) {

		MainLogger("Server Started!");

		return true;
	});

	Event::ServerStoppedEvent::subscribe([](const Event::ServerStoppedEvent& ev) {
		CommandSetup();

		MainLogger("Server Stopped!");

		return true;
	});

	Event::PlayerJoinEvent::subscribe([](const Event::PlayerJoinEvent& ev) {

		MainLogger(fmt::format("Player {} joined", ev.mPlayer->getRealName()));

		return true;
	});

	Event::PlayerLeftEvent::subscribe([](const Event::PlayerLeftEvent& ev) {

		MainLogger(fmt::format("Player {} left server", ev.mPlayer->getRealName()));

		return true;
	});

	Event::PlayerEatEvent::subscribe([](const Event::PlayerEatEvent& ev) {
		MainLogger(fmt::format("Player {} is eatting {}", ev.mPlayer->getRealName(), ev.mFoodItem->getTypeName()));
		return true;
	});

	/*
	Event::PlayerDestroyBlockEvent::subscribe([](const Event::PlayerDestroyBlockEvent& ev) {

		const auto* block = ev.mBlockInstance.getBlock();
		

		MainLogger(fmt::format("Player {} in {} destroyed {}, destroyed block is in {}",
		ev.mPlayer->getRealName(),
		ev.mPlayer->getBlockPos().toString(),
		block->getTypeName(),
		ev.mBlockInstance.getPosition().toString()
		));

		return true;
	});
	*/
	Event::MobSpawnedEvent::subscribe([](const Event::MobSpawnedEvent& ev) {

		string dim;

		switch (ev.mDimensionId) {
			case 0:
				dim = "Overworld";
				break;
			case 1:
				dim = "Nether";
				break;
			case 2:
				dim = "End";
				break;
			default:
				dim = "Unknown";
				break;
		}

		MainLogger(fmt::format("Entity {} spawned in {} {}", ev.mMob->getTypeName(), dim, ev.mPos.toString()));

		return true;
	});

}
