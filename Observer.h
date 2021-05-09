#pragma once
#include "GameObject.h"


namespace mini 
{
	class Observer
	{
	public:
		virtual ~Observer() = default;
		virtual void onNotify(mini::GameObject* entity, Uint8 event) = 0;
	};

}