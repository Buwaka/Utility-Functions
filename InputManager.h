#pragma once
#include <XInput.h>
#include "Singleton.h"
#include "InputCommand.h"
#include "Minigin.h"
#include <unordered_map>

namespace mini
{

	//https://www.nullptr.me/2018/01/15/hashing-stdpair-and-stdtuple/
	template<typename T>
	inline void hash_combine(std::size_t& seed, const T& val)
	{
		std::hash<T> hasher;
		seed ^= hasher(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}


	struct hash_Triple {
		template <class T1, class T2, class T3>
		size_t operator()(const std::tuple<T1, T2, T3>& p) const
		{
			size_t seed = 666;
			hash_combine(seed, std::get<0>(p));
			hash_combine(seed, std::get<1>(p));
			hash_combine(seed, std::get<2>(p));
			return seed;
		}
	};

	enum ControllerButton
	{
		Button_A		= XINPUT_GAMEPAD_A,
		Button_B		= XINPUT_GAMEPAD_B,
		Button_X		= XINPUT_GAMEPAD_X,
		Button_Y		= XINPUT_GAMEPAD_Y,
		Dpad_Down		= XINPUT_GAMEPAD_DPAD_DOWN,
		Dpad_Up			= XINPUT_GAMEPAD_DPAD_UP,
		Dpad_Right		= XINPUT_GAMEPAD_DPAD_RIGHT,
		Dpad_left		= XINPUT_GAMEPAD_DPAD_LEFT,
		Shoulder_Left	= XINPUT_GAMEPAD_LEFT_SHOULDER,
		Shoulder_Right	= XINPUT_GAMEPAD_RIGHT_SHOULDER,
		Start			= XINPUT_GAMEPAD_START,
		Back			= XINPUT_GAMEPAD_BACK,
		Trigger_Left, //special case
		Trigger_Right, //special case
		Left_ThumbStick,//special case
		Left_ThumbStick_Horizontal, //special case
		Left_ThumbStick_Vertical, //special case
		Right_ThumbStick,//special case
		Right_ThumbStick_Horizontal, //special case
		Right_ThumbStick_Vertical, //special case
	};

	enum InputTypes
	{
		XInput,
		Keyboard,
		Mouse
	};


	class InputManager : public Singleton<InputManager>
	{
	public:
		template<class T>
		void AssignButton(InputTypes DeviceType, Uint32 button, int playerindex, T* command);
		void RemoveButton(InputTypes DeviceType, Uint32 button, int playerindex);
		bool IsPressed(InputTypes DeviceType, Uint32 button, int PlayerIndex = 0) const;

		size_t GetControllerCount() const;

	protected:
		friend Minigin;
		void Clean();
		void Init();
		void Update();
		void ProcessInput();

	private:
		bool IsPressed(InputTypes DeviceType, Uint32 button, int PlayerIndex = 0, InputCommand* command = nullptr) const;

		std::unordered_map<std::tuple<InputTypes, Uint32, int>, InputCommand*, hash_Triple> Buttons;

		static int MouseX, MouseY;
		size_t ControllerCount = 0;

		XINPUT_STATE XIState[XUSER_MAX_COUNT];
		const Uint8* KeyState = nullptr;
		SDL_Event SDLEvent;
	};



	template<class T>
	void InputManager::AssignButton(InputTypes DeviceType, Uint32 button, int playerindex, T* command)
	{
		Buttons[std::tuple<InputTypes,Uint32,int>(DeviceType, button, playerindex)] = command;
	}
}