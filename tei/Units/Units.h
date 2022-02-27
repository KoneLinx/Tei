#pragma once

#include "glm.h"

namespace tei::internal::unit
{

	namespace detail
	{
		template <uint_least32_t>
		struct Unique_t
		{
			template <typename T>
			struct type : T
			{
				using Base = T;

				using T::T;
				using T::operator=;

				constexpr type() = default;

				template <std::same_as<T> Val>
				constexpr type(Val&& val) noexcept
					: T{ std::forward<Val>(val) }
				{}
			};
		};
	}

	using Unit = float;

	constexpr static auto PRECISION = glm::precision::packed_highp;

	using Vec1              = glm::tvec1<Unit, PRECISION>;
	using Vec2              = glm::tvec2<Unit, PRECISION>;
	using Vec3              = glm::tvec3<Unit, PRECISION>;

	using Mat2              = glm::tmat2x2<Unit, PRECISION>;
	using Mat3              = glm::tmat3x3<Unit, PRECISION>;

	using Position          = detail::Unique_t<__LINE__>::type<Vec2>;
	using Rotation          = detail::Unique_t<__LINE__>::type<Vec1>;
	using Scale             = detail::Unique_t<__LINE__>::type<Vec2>;

	using RotationMatrix    = detail::Unique_t<__LINE__>::type<Mat2>;
	using ScaleMatrix       = detail::Unique_t<__LINE__>::type<Mat2>;
	using TranslationMatrix = detail::Unique_t<__LINE__>::type<Mat3>;
	using TransformMatrix   = detail::Unique_t<__LINE__>::type<Mat3>;

	using Dimentions        = detail::Unique_t<__LINE__>::type<Vec2>;
	using Rectangle         = detail::Unique_t<__LINE__>::type<Mat2>;

	using Colour            = detail::Unique_t<__LINE__>::type<Vec3>;

	static_assert(!std::convertible_to<Position, Scale>, "Shouldn't be possible"); // May fail with intellisense, build succeeds
	static_assert(!std::convertible_to<RotationMatrix, ScaleMatrix>, "Shouldn't be possible");

	struct Transform
	{
		Position position{};
		Scale    scale   {};
		Rotation rotation{};

		Transform(
			Position const& = {},
			Scale    const& = { 1, 1 },
			Rotation const& = {}
		) noexcept;

		Transform(Scale              const&) noexcept;
		Transform(Rotation           const&) noexcept;
									 
		Transform(RotationMatrix     const&) noexcept;
		Transform(ScaleMatrix        const&) noexcept;
		Transform(TranslationMatrix  const&) noexcept;
		Transform(TransformMatrix    const&) noexcept;

		operator Scale            () const   noexcept;
		operator Rotation         () const   noexcept;
		operator Position         () const   noexcept;

		operator RotationMatrix   () const   noexcept;
		operator ScaleMatrix      () const   noexcept;
		operator TranslationMatrix() const   noexcept;
		operator TransformMatrix  () const   noexcept;
	};

}

namespace tei::external
{
	namespace unit = tei::internal::unit;
}

namespace tei::internal::unit
{

	inline Transform::Transform(Position const& t, Scale const& s, Rotation const& r) noexcept
		: position{ t }
		, scale{ s }
		, rotation{ r }
	{}

	inline Transform::Transform(Scale const& s) noexcept
		: Transform{ {}, s, {} }
	{}

	inline Transform::Transform(Rotation const& r) noexcept
		: Transform{ {}, {}, r }
	{}

	inline Transform::Transform(RotationMatrix const& m) noexcept
		: rotation{ atan2(m[0].y, m[0].x) }
	{}

	inline Transform::Transform(ScaleMatrix const& m) noexcept
		: scale{ glm::length(m[0]) }
	{}

	inline Transform::Transform(TranslationMatrix const& m) noexcept
		: position{ m[2] }
	{}

	inline Transform::Transform(TransformMatrix const& m) noexcept
		: position{ m[2] }
		, scale{ glm::length(m[0]), glm::length(m[1]) }
		, rotation{ atan2(m[0].y / scale.x, m[0].x / scale.x) }
	{}

	inline Transform::operator Scale() const noexcept
	{
		return scale;
	}

	inline Transform::operator Rotation() const noexcept
	{
		return rotation;
	}

	inline Transform::operator Position() const noexcept
	{
		return position;
	}

	inline Transform::operator RotationMatrix() const noexcept
	{
		auto const
			x{ cos(rotation) },
			y{ sin(rotation) };
		return RotationMatrix{
			{  x, y }, 
			{ -y, x }
		};
	}

	inline Transform::operator ScaleMatrix() const noexcept
	{
		return ScaleMatrix{
			{ scale.x, 0 },
			{ 0, scale.x }
		};
	}

	inline Transform::operator TranslationMatrix() const noexcept
	{
		return TranslationMatrix{
			{ },
			{ },
			{ position, 0 }
		};
	}

	inline Transform::operator TransformMatrix() const noexcept
	{
		return TransformMatrix{ ScaleMatrix(*this) * RotationMatrix(*this) } * TranslationMatrix(*this);
	}

}