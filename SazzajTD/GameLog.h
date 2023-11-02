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

			void			Print( const std::wstring& format, ... );

private:
	static	cGameLog*		s_instance;
};



#endif