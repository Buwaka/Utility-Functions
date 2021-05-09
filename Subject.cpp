#include "MiniginPCH.h"
#include "Subject.h"
#include "Observer.h"


mini::Subject::Subject()
{
}

void mini::Subject::AddObserver(Observer * observer)
{
	Observers.push_back(observer);
}

void mini::Subject::RemoveObserver(Observer * observer)
{
	auto it = std::find(Observers.begin(), Observers.end(), observer);
	if (it != Observers.end())
	{
		Observers.erase(it);
	}
}

void mini::Subject::Notify( mini::GameObject * entity, Uint8 event)
{
	for (size_t i = 0; i < Observers.size(); i++)
	{
		Observers[i]->onNotify(entity, event);
	}
}
