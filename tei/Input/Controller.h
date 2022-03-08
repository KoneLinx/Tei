#pragma once

#include "InputType.h"

namespace tei::internal::input
{

	struct ControllerInput
	{
	private:
		struct _Detail
		{
			enum Index
			{
				LEFT,
				RIGHT
			};
		};
	public:

		struct Button
		{
			using Type = InputBinary;
			static Type const A;
			static Type const B;
			static Type const X;
			static Type const Y;
			static Type const DPAD_UP;
			static Type const DPAD_DOWN;
			static Type const DPAD_LEFT;
			static Type const DPAD_RIGHT;
			static Type const START;
			static Type const BACK;
			static Type const THUMB_LEFT;
			static Type const THUMB_RIGHT;
			static Type const SHOULDER_LEFT;
			static Type const SHOULDER_RIGHT;
		};
		

		struct Trigger
		{
			using Index = _Detail::Index;
			using Type = InputAnalog;
			static Type const LEFT;
			static Type const RIGHT;
		};

		struct Stick
		{
			using Index = _Detail::Index;
			using Type = InputAnalog2;
			static Type const LEFT;
			static Type const RIGHT;
		};

	};

}

namespace tei::external::input
{
	using tei::internal::input::ControllerInput;
}