#pragma once

#include "Object.h"
//#include <tei/internal/Utility/Projector_to_address.h>

//#include <functional>
//#include <ranges>

namespace tei::internal::ecs
{

	//auto Object::GetAllChildren()
	//{
	//	namespace v = std::views;
	//	return v::transform(m_Children, utility::projectors::to_address<true>{});
	//}
	//auto Object::GetAllChildren() const
	//{
	//	namespace v = std::views;
	//	return v::transform(m_Children, utility::projectors::to_address<true, true>{});
	//}

	//auto Object::GetActiveChildren()		
	//{
	//	namespace v = std::views;
	//	return v::filter(GetAllChildren(), std::identity{});
	//}
	//auto Object::GetActiveChildren() const		
	//{
	//	namespace v = std::views;
	//	return v::filter(GetAllChildren(), std::identity{});
	//}

	//auto Object::GetInactiveChildren() const		
	//{
	//	namespace v = std::views;
	//	return m_Children | v::transform(utility::projectors::to_address<true, true>{}) | v::filter(std::logical_not{});
	//}

}