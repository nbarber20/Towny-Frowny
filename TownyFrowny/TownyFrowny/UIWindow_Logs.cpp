#include "UIWindow_Logs.h"
#include "LogHandler.h"
void UIWindow_Logs::Draw(sf::RenderTexture* renderTexture, sf::Text* text)
{	
	renderTexture->draw(*LogWindowSprite);
	std::vector<Log*> logs = LogHandler::Instance().GetLogs();
	std::vector<std::string> logstext;
	std::vector<sf::Color> logColors;
	const int num = logs.size();
	if (num == 0) {
		logColors.push_back(sf::Color(248, 255, 138));
		logstext.push_back("No Logs..");
	}
	Log** ptr = (num > 0) ? logs.data() : nullptr;
	for (int i = 0; i < num; i++)
	{
		logColors.push_back(LogHandler::Instance().GetLogColor((*ptr[i]).context));

		std::string l = "(" + (*ptr[i]).time + ")" + (*ptr[i]).text + "\n";
		if (l.length() > 100)
		{
			l.erase(100);
			l += "..";
		}
		logstext.push_back(l);
	}
	for (int i = 0; i < logstext.size(); i++)
	{
		text->setPosition(410, 22 + (i * 16));
		text->setFillColor(logColors[i]);
		text->setString(logstext[i]);
		renderTexture->draw(*text);
	}
}

void UIWindow_Logs::OnMouseLeftClickDown(sf::Vector2f MousePos)
{
	if (MousePos.x * 800 > 410 && MousePos.y * 800 > 22)
	{
		int i = floor(((MousePos.y*800.0f) - 22) / 16.0f);
		LogHandler::Instance().OnLogUIClick(i);
	} 
}
