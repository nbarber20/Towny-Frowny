#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <ctime>
#include <math.h>   
#include <cmath>
#include "Camera.h"
#include "UIHandler.h"
#include <iostream>
enum logContext {
	ERROR,
	SYSTEM,
	GLOBAL,
	WORLD,
};
struct Log {
	Log(std::string time, std::string text, sf::Vector2i position, logContext context) {
		this->time = time;
		this->text = text;
		this->position = position;
		this->context = context;
	}
	std::string time;
	std::string text;
	sf::Vector2i position;
	logContext context;
};

class LogHandler
{
public:
	static LogHandler& Instance()
	{
		static LogHandler INSTANCE;
		return INSTANCE;
	}
	LogHandler() {
		
	};

	void WriteLog(std::string text, sf::Vector2i position, logContext context) {
		Logs.push_back(new Log(getTime(), text, position, context));
		if (Logs.size() > MaxLogs) {
			delete Logs[0];
			Logs.erase(Logs.begin());
			Logs.shrink_to_fit();
		}
		std::cout << text << "\n";
	}
	void WriteLog(std::string text, logContext context) {
		Logs.push_back(new Log(getTime(), text, sf::Vector2i(0,0), context));
		if (Logs.size() > MaxLogs) {
			delete Logs[0];
			Logs.erase(Logs.begin());
			Logs.shrink_to_fit();
		}
		std::cout << text << "\n";
	}
	void WriteLog(std::string text) {
		Logs.push_back(new Log(getTime(), text, sf::Vector2i(0, 0), logContext::SYSTEM));
		if (Logs.size() > MaxLogs) {
			delete Logs[0];
			Logs.erase(Logs.begin());
			Logs.shrink_to_fit();
		}
		std::cout << text << "\n";
	}

	std::vector<Log*> GetOnScreenLogs() {
		std::vector<Log*> onScreenLogs;
		const int num = Logs.size();
		Log** ptr = (num > 0) ? Logs.data() : nullptr;
		for (int i = 0; i < num; i++)
		{
			logContext c = (*ptr[i]).context;
			if (c != logContext::SYSTEM &&c != logContext::ERROR&&c != logContext::GLOBAL)
			{
				sf::Vector2f vec;
				vec.x = (*ptr[i]).position.x;
				vec.y = (*ptr[i]).position.y;
				if (Camera::Instance().IsOnScreen(vec)) {
					onScreenLogs.push_back(ptr[i]);
				}
			}
			else {
				onScreenLogs.push_back(ptr[i]);
			}
		}
		return onScreenLogs;
	}

	void UpdateLogs() {
		logsToShow.clear();
		if (FilterOffScreen) {
			logsToShow = GetOnScreenLogs();
		}
		else {
			logsToShow = Logs;
		}

		std::vector<std::string> logstext;
		std::vector<sf::Color> logColors;
		const int num = logsToShow.size();
		if (num == 0) {
			logColors.push_back(sf::Color(248, 255, 138));
			logstext.push_back("No Logs..");
		}
		Log** ptr = (num > 0) ? logsToShow.data() : nullptr;
		for (int i = 0; i < num; i++)
		{
			if (doColorLogs == false) logColors.push_back(sf::Color(248, 255, 138));
			else logColors.push_back(GetLogColor((*ptr[i]).context));

			std::string l = "(" + (*ptr[i]).time + ")" + (*ptr[i]).text + "\n";
			if (l.length() > 40)
			{
				l.erase(40);
				l += "..";
			}
			logstext.push_back(l);
		}
		UiHandler::Instance().UpdateLogs(logstext,logColors);

	}
	void OnLogUIClick(int i) {
		if(i>=0&&i< logsToShow.size()){
			if (logsToShow[i]->position != sf::Vector2i(0, 0)) {
				sf::Vector2f v(logsToShow[i]->position.x, logsToShow[i]->position.y);
				Camera::Instance().MoveToPos(v);
				Camera::Instance().setCameraZoom(2.5);
			}
		}
	}
private:

	sf::Color GetLogColor(logContext context) {
		switch (context)
		{
		case ERROR:
			return sf::Color(248,0, 0);
		case SYSTEM:
			return sf::Color(0, 255, 138);
		case GLOBAL:
			return sf::Color(0, 255, 0);
		case WORLD:
			return sf::Color(248, 255, 138);
		default:
			return sf::Color(248, 255, 138);
		}
	}
	std::string getTime() {
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
		std::string timestr(buffer);
		return timestr;
	}

	std::vector<Log*> logsToShow;
	std::vector<Log*> Logs;
	int MaxLogs = 40;
	bool FilterOffScreen = true;
	bool doColorLogs = true;
	
};

