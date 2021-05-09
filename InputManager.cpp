#include "MiniginPCH.h"
#include "InputManager.h"
#include <SDL.h>
#include "Renderer.h"


int mini::InputManager::MouseX = 0;
int mini::InputManager::MouseY = 0;

void mini::InputManager::Clean()
{
	//The pointer returned is a pointer to an internal SDL array. It will be valid for the whole lifetime of the application and should not be freed by the caller. 
	//if(KeyState)
	//	delete KeyState;

	for (auto& Button : Buttons)
	{
		auto command = Button.second;

		delete command;
	}

}

void mini::InputManager::Init()
{
	Update();
}

void mini::InputManager::Update()
{
	ControllerCount = 0;
	KeyState = SDL_GetKeyboardState(NULL);

	//to prevent SDL window from freezing, event isn't used anywhere
	SDL_PollEvent(&SDLEvent);

	for (size_t i = 0; i < XUSER_MAX_COUNT; i++)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		XIState[i] = state;

		DWORD dwResult;
		dwResult = XInputGetState((DWORD)i, &XIState[i]);
		if (dwResult == ERROR_SUCCESS)
		{
			ControllerCount++;
		}
	}
}

void mini::InputManager::RemoveButton(InputTypes DeviceType, Uint32 button, int playerindex)
{
	delete Buttons[std::tuple<InputTypes, Uint32, int>(DeviceType, button, playerindex)];
	Buttons.erase(std::tuple<InputTypes, Uint32, int>(DeviceType, button, playerindex));
}

bool mini::InputManager::IsPressed(InputTypes DeviceType, Uint32 button, int PlayerIndex) const
{
	return IsPressed(DeviceType, button, PlayerIndex, nullptr);
}

size_t mini::InputManager::GetControllerCount() const
{
	return ControllerCount;
}

bool mini::InputManager::IsPressed(InputTypes DeviceType, Uint32 button, int PlayerIndex, InputCommand* command) const
{
	switch (DeviceType)
	{
	case XInput:
		switch (button)
		{
		case mini::ControllerButton::Button_A:
		case mini::ControllerButton::Button_B:
		case mini::ControllerButton::Button_X:
		case mini::ControllerButton::Button_Y:
		case mini::ControllerButton::Dpad_Down:
		case mini::ControllerButton::Dpad_Up:
		case mini::ControllerButton::Dpad_Right:
		case mini::ControllerButton::Dpad_left:
		case mini::ControllerButton::Shoulder_Left:
		case mini::ControllerButton::Shoulder_Right:
		case mini::ControllerButton::Start:
		case mini::ControllerButton::Back:

			if ((XIState[PlayerIndex].Gamepad.wButtons & (WORD)button) == (WORD)button)
			{
				return true;
			}
			break;
		case mini::ControllerButton::Trigger_Left:

			if (XIState[PlayerIndex].Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				if (command)
				{
					command->SetFloatValue(XIState[PlayerIndex].Gamepad.bLeftTrigger / (float)UCHAR_MAX);
				}

				return true;
			}
			break;
		case mini::ControllerButton::Trigger_Right:

			if (XIState[PlayerIndex].Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				if (command)
				{
					command->SetFloatValue(XIState[PlayerIndex].Gamepad.bRightTrigger / (float)UCHAR_MAX);
				}
				return true;
			}
			break;
		case mini::ControllerButton::Left_ThumbStick_Horizontal:

			if (std::abs(XIState[PlayerIndex].Gamepad.sThumbLX) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			{
				if (command)
				{
					command->SetFloatValue(XIState[PlayerIndex].Gamepad.sThumbLX / (float)SHRT_MAX);
				}
				return true;
			}
			break;
		case mini::ControllerButton::Left_ThumbStick_Vertical:

			if (std::abs(XIState[PlayerIndex].Gamepad.sThumbLY) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			{
				if (command)
				{
					command->SetFloatValue(-XIState[PlayerIndex].Gamepad.sThumbLY / (float)SHRT_MAX);
				}
				return true;
			}
			break;
		case mini::ControllerButton::Right_ThumbStick_Horizontal:

			if (std::abs(XIState[PlayerIndex].Gamepad.sThumbRX) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			{
				if (command)
				{
					command->SetFloatValue(XIState[PlayerIndex].Gamepad.sThumbRX / (float)SHRT_MAX);
				}
				return true;
			}
			break;
		case mini::ControllerButton::Right_ThumbStick_Vertical:

			if (std::abs(XIState[PlayerIndex].Gamepad.sThumbRY) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			{
				if (command)
				{
					command->SetFloatValue(-XIState[PlayerIndex].Gamepad.sThumbRY / (float)SHRT_MAX);
				}
				return true;
			}
			break;

		case mini::ControllerButton::Right_ThumbStick:

			if ((std::abs(XIState[PlayerIndex].Gamepad.sThumbRX) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ||
				(std::abs(XIState[PlayerIndex].Gamepad.sThumbRY) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
			{
				if (command)
				{
					command->Set2DFloatValue(XIState[PlayerIndex].Gamepad.sThumbRX / (float)SHRT_MAX,
						-XIState[PlayerIndex].Gamepad.sThumbRY / (float)SHRT_MAX);
				}
				return true;
			}
			break;
		case mini::ControllerButton::Left_ThumbStick:

			if ((std::abs(XIState[PlayerIndex].Gamepad.sThumbLX) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ||
				(std::abs(XIState[PlayerIndex].Gamepad.sThumbLY) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
			{
				if (command)
				{
					command->Set2DFloatValue(XIState[PlayerIndex].Gamepad.sThumbLX / (float)SHRT_MAX,
						-XIState[PlayerIndex].Gamepad.sThumbLY / (float)SHRT_MAX);
				}
				return true;
			}
			break;


		//keyboard and mouse basically
		default:
			break;
		}
		break;
	case Keyboard:
		if (KeyState[SDL_GetScancodeFromKey(button)] > 0)
		{
			return true;
		}
		break;
	case Mouse:

	//case mini::ControllerButton::MouseMovement_Absolute:
	//{
	//	int oldX, oldY;
	//	oldX = MouseX;
	//	oldY = MouseY;

	//	SDL_GetMouseState(&MouseX, &MouseY);

	//	int diffX = MouseX - oldX;
	//	int diffY = MouseY - oldY;
	//	if (diffX != 0 || diffY != 0)
	//	{
	//		if (command)
	//		{
	//			float screenwidth = Renderer::GetInstance().GetWindowWidth() / 2.f;
	//			float screenHeight = Renderer::GetInstance().GetWindowHeight() / 2.f;
	//			command->Set2DFloatValue((float)MouseX / screenwidth, (float)MouseY / screenHeight);
	//		}
	//	}
	//}
	//break;

	//case mini::ControllerButton::MouseMovement_Relative:
	//{
	//	int oldX, oldY;
	//	oldX = MouseX;
	//	oldY = MouseY;

	//	SDL_GetMouseState(&MouseX, &MouseY);

	//	int diffX = MouseX - oldX;
	//	int diffY = MouseY - oldY;
	//	if (diffX != 0 || diffY != 0)
	//	{
	//		if (command)
	//		{
	//			float screenwidth = Renderer::GetInstance().GetWindowWidth() / 2.f;
	//			float screenHeight = Renderer::GetInstance().GetWindowHeight() / 2.f;
	//			command->Set2DFloatValue((float)diffX / screenwidth, (float)diffY / screenHeight);
	//		}
	//	}
	//}
	//break;


		break;
	}


	return false;
}



void mini::InputManager::ProcessInput()
{
	Update();

	for (auto& Button : Buttons)
	{
		InputTypes InputType = std::get<0>(Button.first);
		Uint32 ButtonIndex	 = std::get<1>(Button.first);
		int PlayerIndex		 = std::get<2>(Button.first);
		InputCommand* command	 = Button.second;

		if (IsPressed(InputType, ButtonIndex, PlayerIndex, command))
			command->Execute();
	}
}
