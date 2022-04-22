#pragma once

#include "InputType.h"
#include <string_view>

namespace tei::internal::input
{

	struct KeyboardInput
	{

        using Type = InputBinary;
        
        struct Main
        {
			using Type = Type;
            static Type const A;
            static Type const B;
            static Type const C;
            static Type const D;
            static Type const E;
            static Type const F;
            static Type const G;
            static Type const H;
            static Type const I;
            static Type const J;
            static Type const K;
            static Type const L;
            static Type const M;
            static Type const N;
            static Type const O;
            static Type const P;
            static Type const Q;
            static Type const R;
            static Type const S;
            static Type const T;
            static Type const U;
            static Type const V;
            static Type const W;
            static Type const X;
            static Type const Y;
            static Type const Z;
            static Type const NUM_1;
            static Type const NUM_2;
            static Type const NUM_3;
            static Type const NUM_4;
            static Type const NUM_5;
            static Type const NUM_6;
            static Type const NUM_7;
            static Type const NUM_8;
            static Type const NUM_9;
            static Type const NUM_0;
            static Type const RETURN;
            static Type const ESCAPE;
            static Type const BACKSPACE;
            static Type const TAB;
            static Type const SPACE;
            static Type const SEMICOLON;
            static Type const APOSTROPHE;
            static Type const GRAVE;
            static Type const COMMA;
            static Type const PERIOD;
            static Type const SLASH;
            static Type const CAPSLOCK;
        };

        struct F
        {
			using Type = Type;
            static Type const F1;
            static Type const F2;
            static Type const F3;
            static Type const F4;
            static Type const F5;
            static Type const F6;
            static Type const F7;
            static Type const F8;
            static Type const F9;
            static Type const F10;
            static Type const F11;
            static Type const F12;
            static Type const F13;
            static Type const F14;
            static Type const F15;
            static Type const F16;
            static Type const F17;
            static Type const F18;
            static Type const F19;
            static Type const F20;
            static Type const F21;
            static Type const F22;
            static Type const F23;
            static Type const F24;
        };

        struct Home
        {
			using Type = Type;
            static Type const PRINTSCREEN;
            static Type const SCROLLLOCK;
            static Type const PAUSE;
            static Type const INSERT;
            static Type const HOME;
            static Type const PAGEUP;
            static Type const DELETE;
            static Type const END;
            static Type const PAGEDOWN;
        };

        struct Arrow
        {
			using Type = Type;
            static Type const RIGHT;
            static Type const LEFT;
            static Type const DOWN;
            static Type const UP;
        };

        struct Control
        {
			using Type = Type;
            static Type const EXECUTE;
            static Type const HELP;
            static Type const MENU;
            static Type const SELECT;
            static Type const STOP;
            static Type const REDO;
            static Type const UNDO;
            static Type const CUT;
            static Type const COPY;
            static Type const PASTE;
            static Type const FIND;
            static Type const MUTE;
            static Type const MEDIASELECT;
            static Type const VOLUMEUP;
            static Type const VOLUMEDOWN;

            struct Audio
            {
			    using Type = Type;
                static Type const AUDIONEXT;
                static Type const AUDIOPREV;
                static Type const AUDIOSTOP;
                static Type const AUDIOPLAY;
                static Type const AUDIOMUTE;
                static Type const AUDIOREWIND;
                static Type const AUDIOFASTFORWARD;
            };
        };

        struct Mod
        {
			using Type = Type;
            static Type const META; //LMETA
            static Type const MODE;
            static Type const LCTRL;
            static Type const LSHIFT;
            static Type const LALT;
            static Type const LMETA;
            static Type const RCTRL;
            static Type const RSHIFT;
            static Type const RALT;
            static Type const RMETA;
        };

        struct Lang
        {
			using Type = Type;
            static Type const INTERNATIONAL1;
            static Type const INTERNATIONAL2;
            static Type const INTERNATIONAL3;
            static Type const INTERNATIONAL4;
            static Type const INTERNATIONAL5;
            static Type const INTERNATIONAL6;
            static Type const INTERNATIONAL7;
            static Type const INTERNATIONAL8;
            static Type const INTERNATIONAL9;
            static Type const LANG1;
            static Type const LANG2;
            static Type const LANG3;
            static Type const LANG4;
            static Type const LANG5;
            static Type const LANG6;
            static Type const LANG7;
            static Type const LANG8;
            static Type const LANG9;
        };

		struct Keypad
		{
			using Type = Type;
            static Type const NUMLOCK;
            static Type const DIVIDE;
            static Type const MULTIPLY;
            static Type const MINUS;
            static Type const PLUS;
            static Type const ENTER;
            static Type const NUM_1;
            static Type const NUM_2;
            static Type const NUM_3;
            static Type const NUM_4;
            static Type const NUM_5;
            static Type const NUM_6;
            static Type const NUM_7;
            static Type const NUM_8;
            static Type const NUM_9;
            static Type const NUM_0;
            static Type const PERIOD;
            static Type const COMMA;
            static Type const EQUALS;
		};

        SomeCommonInputTypeRef InputByName(std::string_view name);

	};

}

namespace tei::external::input
{
	using tei::internal::input::KeyboardInput;
}
