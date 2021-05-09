#pragma once
#include "Observer.h"
#include <vector>

namespace mini
{
	class Subject
	{
	public:
		Subject();


		void AddObserver(Observer *observer);
		void RemoveObserver(Observer *observer);
		void Notify(GameObject* entity, Uint8 event);
	private:
		std::vector<Observer*> Observers;
	};
}