#pragma once
#include <modm/ui/color/concepts.hpp>

namespace modm::graphic
{

template<class B>
concept GraphicBuffer = color::Color<typename B::ColorType>;
// TODO check B for existence of ResolutionVector

template<class D>
concept GraphicDisplay = color::Color<typename D::ColorType>;
// TODO check D for existence of ResolutionVector

/**
 * @brief 	Function or functor comcept, returning a Color for each Point you pass
 *
 * @author 	Thomas Sommer
 */
template<typename F>
concept ColorPattern = requires(F p)
{
	{ p.operator()(shape::Point()) } -> color::Color;  // -> std::convertible_to<color::Rgb888>;
};

}  // namespace modm::graphic