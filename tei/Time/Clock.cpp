#include "Clock.h"

tei::internal::time::Clock::Source::time_point const tei::internal::time::Clock::m_Epoch{ tei::internal::time::Clock::Source::now() };
