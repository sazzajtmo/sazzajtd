#ifndef GAME_LOG_H
#define GAME_LOG_H

#include <string>

class cGameLog final
{
private:
	cGameLog();
	virtual ~cGameLog();

public:
	static	cGameLog*		GetInstance();
	static	void			DestroyInstance();

			void			Print( const std::string& format, ... );

private:
	static	cGameLog*		s_instance;
};

#define GAME_LOG cGameLog::GetInstance()->Print

#endif