#pragma once

#include <unordered_map>
#include <filesystem>
#include <typeindex>
#include <variant>
#include <memory>

#include <tei/internal/utility.h>
#include <tei/internal/ecs.h>

namespace tei::internal::resource
{

	template <typename Data>
	class Resource;

	template <typename Data>
	class ToLoad;
	
	template <typename Data, typename ... Args>
	concept LoadableResource = requires (ToLoad<Data> d, std::filesystem::path path, Args ... args)
	{
		{ ::Load(d, path, std::forward<Args>(args)...) } -> std::same_as<Resource<Data>>;
	};
	
	template <typename Data>
	concept DefaultResource = requires (ToLoad<Data> d)
	{
		{ ::Load(d) } -> std::same_as<Resource<Data>>;
	};

	class ResourceManager
	{
	public:

		// Load resource with duplication check
		template <typename Data, typename ... Args> requires LoadableResource<Data, Args...>
		[[nodiscard]] Resource<Data> LoadShared(std::filesystem::path const& filepath, Args&& ...);

		template <typename Data> requires DefaultResource<Data>
		[[nodiscard]] Resource<Data> LoadShared();
		
		// Load resource regardless of duplication
		template <typename Data, typename ... Args> requires LoadableResource<Data, Args...>
		[[nodiscard]] Resource<Data> LoadUnique(std::filesystem::path const& filepath, Args&& ...);

		template <typename Data> requires DefaultResource<Data>
		[[nodiscard]] Resource<Data> LoadUnique();

	private:

		template <typename Data, typename Creator>
		Resource<Data> GetResource(std::filesystem::path const& absolute, Creator);

		std::unordered_map<size_t, std::weak_ptr<void>, std::identity> m_Resources;

	public:

		using Service = utility::Service<ResourceManager>;

	};

	extern ResourceManager::Service Resources;

	template <typename Data>
	class ToLoad
	{
		constexpr ToLoad() noexcept = default;
		friend ResourceManager;
	};

	template <typename Data>
	class Resource
	{

		friend ResourceManager;

	public:
		
		Resource() requires DefaultResource<Data>
			: Resource{ Resources->LoadUnique<Data>() }
		{}

		Resource(std::shared_ptr<Data> ptr)
			: m_Data{ std::move(ptr) }
		{}
		
		Data& operator * () const
		{ return m_Data.operator*(); }

		Data* operator -> () const
		{ return m_Data.operator->(); }

	private:

		std::shared_ptr<Data> m_Data{};
	};

	template<typename Data, typename ...Args> requires LoadableResource<Data, Args...>
	inline Resource<Data> ResourceManager::LoadShared(std::filesystem::path const& filepath, Args && ... args)
	{
		auto abs = std::filesystem::absolute(filepath);
		return GetResource<Data>(
			abs,
			[&] 
			{
				return LoadUnique<Data>(abs, std::forward<Args>(args)...);
			}
		);
	}

	template<typename Data> requires DefaultResource<Data>
	inline Resource<Data> internal::resource::ResourceManager::LoadShared()
	{
		return GetResource<Data>(
			{},
			[]
			{ 
				return LoadUnique<Data>(); 
			}
		);
	}

	template<typename Data, typename ...Args> requires LoadableResource<Data, Args...>
	inline Resource<Data> ResourceManager::LoadUnique(std::filesystem::path const& filepath, Args && ... args)
	{
		auto abs = std::filesystem::absolute(filepath);
		return ::Load(ToLoad<Data>{}, abs, std::forward<Args>(args)...);
	}

	template<typename Data> requires DefaultResource<Data>
	inline Resource<Data> ResourceManager::LoadUnique()
	{
		return ::Load(ToLoad<Data>{});
	}

	template<typename Data, typename Creator>
	inline Resource<Data> ResourceManager::GetResource(std::filesystem::path const& absolute, Creator creator)
	{
		size_t hash = typeid(Data).hash_code();

		// Possible key collision, TODO try to fix or mitigate
		if (!absolute.empty())
			hash ^= std::hash<std::filesystem::path::string_type>{}(absolute.native());

		auto& weak = m_Resources[hash];
		if (!weak.expired())
		{
			return Resource<Data>{ std::static_pointer_cast<Data>(weak.lock()) };
		}
		else
		{
			Resource<Data> resource{ creator() };
			weak = resource.m_Data;
			return resource; // RVO
		}
	}

}

namespace tei::external
{
	using tei::internal::resource::Resource;

	namespace resource
	{
		using tei::internal::resource::ToLoad;
	}

	constexpr static auto const& Resources { tei::internal::resource::Resources };
}
