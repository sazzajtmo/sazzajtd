#ifndef GAME_INPUT_MANAGER_H
#define GAME_INPUT_MANAGER_H

#include <vector>
#include <functional>

/// <summary>
/// simplified input class in case I change the underlying engine
/// more of a translate from SDL class
/// </summary>
class cGameInputManager final
{
public:
	enum class eMouseButton : int
	{
		None = 0,
		Left,
		Middle,
		Right		
	};

	struct tMouseEventData
	{
		int x		= -1;
		int y		= -1;
		int button	= static_cast<int>( eMouseButton::None );
	};

	struct tMouseCallbackStorage
	{
		std::function<void(const tMouseEventData&)>	callback;
		unsigned int								handle;

		bool operator==(const tMouseCallbackStorage& b)
		{
			return handle == b.handle;
		}

		void operator()(const tMouseEventData& param) const
		{
			callback(param);
		}

		tMouseCallbackStorage( std::function<void(const tMouseEventData&)> _cb, int _handle )
		: callback( _cb )
		, handle( _handle )
		{ }
	};

private:
	cGameInputManager();
	~cGameInputManager();

public:
	static cGameInputManager*	GetInstance();
	static void					DestroyInstance();

			void				FeedMouseEvent( int mouseButton, int mouseX, int mouseY );
			int					RegisterForMouseEvent( std::function<void(const tMouseEventData& mouseEvent)> callback );	//returns callback handle
			void				UnRegisterForMouseEvent( int mouseCbHandle );	

private:
	static cGameInputManager*	s_instance;

			std::vector<tMouseCallbackStorage>
								m_mouseCallbacks;
};

bool operator==(cGameInputManager::eMouseButton mouseTypeEnum, int num);
bool operator==(int num, cGameInputManager::eMouseButton mouseTypeEnum);

#endif