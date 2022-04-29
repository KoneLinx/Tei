#pragma once

#include <random>
#include <algorithm>
#include <array>

namespace tei::internal::utility
{

	namespace detail
	{

		using RandomEngine = std::default_random_engine;

		template <typename T>
		using BytesArray = std::array<std::byte, sizeof(T)>;

		template <typename T>
		concept ValidByteRep =
		requires (BytesArray<T> bytes)
		{
			std::bit_cast<T>(bytes);
		};

		template <typename ByteArray>
		constexpr inline bool CopyBytes(std::ranges::subrange<ByteArray>& to, RandomEngine::result_type bytes)
		{
			auto data{ std::bit_cast<BytesArray<RandomEngine::result_type>>(bytes) };
			std::ranges::subrange from{ data };
			while (!to.empty() && !from.empty())
			{
				to[0] = from[0];
				to.advance(1), from.advance(1);
			}
			return !to.empty();
		}

		inline static auto RandomSeed()
		{
			static std::random_device rd{};
			return rd();
		}

	}

	template <detail::ValidByteRep T>
	inline auto RandomGeneratorSeeded(detail::RandomEngine::result_type seed)
	{
		return [rng = detail::RandomEngine{ seed }] () mutable -> T
		{
			detail::BytesArray<T> data{};
			std::ranges::subrange datarng{ data };
			while (detail::CopyBytes(datarng, rng()));
			return std::bit_cast<T>(data);
		};
	}

	template <detail::ValidByteRep T>
	inline auto RandomGenerator()
	{
		return RandomGeneratorSeeded<T>(detail::RandomSeed());
	}


	template <std::floating_point T>
	inline auto RandomNormGeneratorSeeded(detail::RandomEngine::result_type seed)
	{
		return [rng = RandomGeneratorSeeded<size_t>(seed) ] () mutable -> T
		{
			static constexpr auto step = T(1) / std::numeric_limits<uint64_t>::max();
			return rng() * step;
		};
	}
	
	template <detail::ValidByteRep T>
	inline auto RandomNormGenerator()
	{
		return RandomNormGeneratorSeeded<T>(detail::RandomSeed());
	}

	template <size_t N>
	inline auto RandomByteGeneratorSeeded(detail::RandomEngine::result_type seed)
	{
		return RandomGeneratorSeeded<std::array<std::byte, N>>(seed);
	}

	template <size_t N>
	inline auto RandomByteGenerator()
	{
		return RandomByteGeneratorSeeded<N>(detail::RandomSeed());
	}

	template <detail::ValidByteRep T>
	inline T Random()
	{
		return RandomGenerator<T>()();
	}

}

namespace tei::external::random
{
	using tei::internal::utility::RandomGeneratorSeeded;
	using tei::internal::utility::RandomGenerator;
	using tei::internal::utility::RandomNormGeneratorSeeded;
	using tei::internal::utility::RandomNormGenerator;
	using tei::internal::utility::RandomByteGenerator;
	using tei::internal::utility::Random;
}
