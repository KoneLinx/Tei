#include "Controller.h"

#pragma warning(disable: 5105) // macro error in windows.h
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Xinput.h>

using namespace tei::internal::input;
using enum DeviceId;

ControllerInput::Button::Type const ControllerInput::Button::A              { CONTROLER, XINPUT_GAMEPAD_A };
ControllerInput::Button::Type const ControllerInput::Button::B              { CONTROLER, XINPUT_GAMEPAD_B };
ControllerInput::Button::Type const ControllerInput::Button::X              { CONTROLER, XINPUT_GAMEPAD_X };
ControllerInput::Button::Type const ControllerInput::Button::Y              { CONTROLER, XINPUT_GAMEPAD_Y };
ControllerInput::Button::Type const ControllerInput::Button::DPAD_UP        { CONTROLER, XINPUT_GAMEPAD_DPAD_UP };
ControllerInput::Button::Type const ControllerInput::Button::DPAD_DOWN      { CONTROLER, XINPUT_GAMEPAD_DPAD_DOWN };
ControllerInput::Button::Type const ControllerInput::Button::DPAD_LEFT      { CONTROLER, XINPUT_GAMEPAD_DPAD_LEFT };
ControllerInput::Button::Type const ControllerInput::Button::DPAD_RIGHT     { CONTROLER, XINPUT_GAMEPAD_DPAD_RIGHT };
ControllerInput::Button::Type const ControllerInput::Button::START          { CONTROLER, XINPUT_GAMEPAD_START };
ControllerInput::Button::Type const ControllerInput::Button::BACK           { CONTROLER, XINPUT_GAMEPAD_BACK };
ControllerInput::Button::Type const ControllerInput::Button::THUMB_LEFT     { CONTROLER, XINPUT_GAMEPAD_LEFT_THUMB };
ControllerInput::Button::Type const ControllerInput::Button::THUMB_RIGHT    { CONTROLER, XINPUT_GAMEPAD_RIGHT_THUMB };
ControllerInput::Button::Type const ControllerInput::Button::SHOULDER_LEFT  { CONTROLER, XINPUT_GAMEPAD_LEFT_SHOULDER };
ControllerInput::Button::Type const ControllerInput::Button::SHOULDER_RIGHT { CONTROLER, XINPUT_GAMEPAD_RIGHT_SHOULDER };

ControllerInput::Trigger::Type const ControllerInput::Trigger::LEFT  { CONTROLER, Index::LEFT };
ControllerInput::Trigger::Type const ControllerInput::Trigger::RIGHT { CONTROLER, Index::RIGHT };

ControllerInput::Stick::Type const ControllerInput::Stick::LEFT  { CONTROLER, Index::LEFT };
ControllerInput::Stick::Type const ControllerInput::Stick::RIGHT { CONTROLER, Index::RIGHT };
