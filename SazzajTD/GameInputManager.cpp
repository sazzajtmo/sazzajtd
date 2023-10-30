#include "GameInputManager.h"
#include "GameObject.h"
#include "AIUnit.h"
#include "GameBoard.h"
#include "MemHelper.h"

cGameInputManager* cGameInputManager::s_instance( nullptr );

cGameInputManager::cGameInputManager()
{
}

cGameInputManager::~cGameInputManager()
{
}

cGameInputManager* cGameInputManager::GetInstance()
{
	if( !s_instance )
		s_instance = snew cGameInputManager;

	return s_instance;
}

void cGameInputManager::DestroyInstance()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

void cGameInputManager::FeedMouseEvent(int mouseButton, int mouseX, int mouseY)
{
	tMouseEventData mouseEvent;
	mouseEvent.x		= mouseX;
	mouseEvent.y		= mouseY;
	mouseEvent.button	= mouseButton;

	for (const auto& callback : m_mouseCallbacks)
	{
		callback( mouseEvent );
	}
}

int cGameInputManager::RegisterForMouseEvent(std::function<void(const tMouseEventData& mouseEvent)> callback)
{
	static int mouseCallbackID = -1;

	mouseCallbackID++;

	m_mouseCallbacks.emplace_back( callback, mouseCallbackID );

	return mouseCallbackID;
}

void cGameInputManager::UnRegisterForMouseEvent(int handle)
{
	auto cbIt = std::find_if(m_mouseCallbacks.begin(), m_mouseCallbacks.end(), [handle]( const tMouseCallbackStorage& el )
	{
		return el.handle == handle;
	});

	if( cbIt != m_mouseCallbacks.end() )
		m_mouseCallbacks.erase( cbIt );
}

bool operator==(cGameInputManager::eMouseButton mouseTypeEnum, int num)
{
	return static_cast<int>(mouseTypeEnum) == num;
}

bool operator==(int num, cGameInputManager::eMouseButton mouseTypeEnum)
{
	return mouseTypeEnum == num;
}