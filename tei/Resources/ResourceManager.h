#pragma once

#include <memory>
#include <filesystem>
#include <any>
#include <unordered_map>

#include <tei/internal/utility.h>

//namespace tei::internal::resource
//{
//
//	class ResourceManager final
//	{
//		struct LoaderBase;
//
//	public:
//
//		template <typename T>
//		class Loader;
//
//		ResourceManager(std::filesystem::path defaultPath = std::filesystem::current_path());
//		~ResourceManager();
//
//		std::filesystem::path const& DefaultPath() const
//		{ return m_DefaultPath; }
//
//		template <typename T>
//		T& Load(std::filesystem::path path, std::optional<T> init = std::nullopt) const;
//
//		template<typename T>
//		Loader<T>& AddLoader(Loader<T>* init = {});
//
//		std::filesystem::path GetExisting(std::filesystem::path const& path) const;
//
//	private:
//
//		ResourceManager(ResourceManager const&) = delete;
//		ResourceManager(ResourceManager&&) = delete;
//		ResourceManager& operator = (ResourceManager const&) = delete;
//		ResourceManager& operator = (ResourceManager&&) = delete;
//
//		std::filesystem::path m_DefaultPath;
//
//		struct LoaderBase
//		{
//			virtual ~LoaderBase() = default;
//		};
//
//		std::vector<std::pair<std::type_info const*, std::unique_ptr<LoaderBase>>> m_Loaders;
//
//		mutable std::vector<std::any> m_Resources;
//
//	public:
//
//		template <typename T>
//		class Loader : LoaderBase
//		{
//		public:
//
//			~Loader() = default;
//			explicit Loader() = default;
//
//		protected:
//
//			using ResourceManager = ResourceManager;
//
//		private:
//
//			virtual T* OnLoad(std::filesystem::path const& path, ResourceManager const& resources, std::optional<T> init) const = 0;
//			virtual void OnFree(ResourceManager const& resources, T* object) const = 0;
//
//			virtual inline std::type_info const& ID() const noexcept final
//			{
//				return typeid(T);
//			}
//
//			friend ResourceManager;
//
//			Loader(Loader const&) = delete;
//			Loader(Loader&&) = delete;
//			Loader& operator = (Loader const&) = delete;
//			Loader& operator = (Loader&&) = delete;
//
//		};
//
//		using Service = utility::Service<ResourceManager const>;
//
//	};
//
//	extern ResourceManager::Service Resources;
//
//}
//
//namespace tei::external
//{
//
//	static constexpr auto const& Resources{ tei::internal::resource::Resources };
//
//}
//
//#pragma region _impl
//
//namespace tei::internal::resource
//{
//
//	template<typename T>
//	inline T& ResourceManager::Load(std::filesystem::path path, std::optional<T> init) const
//	{
//		METRICS_TIMEBLOCK;
//
//		if (auto const it = std::ranges::find(m_Loaders, &typeid(T), [] (auto const& p) { return std::get<0>(p); }); it != end(m_Loaders))
//		{
//			auto const loader = static_cast<Loader<T>*>(it->second.get());
//			auto resource = std::shared_ptr<T>{
//				loader->OnLoad(path, *this, init),
//				[loader, this] (T* object) 
//				{ 
//					loader->OnFree(*this, object); 
//				}
//			};
//			m_Resources.push_back(resource);
//			return *resource;
//		}
//		else throw utility::TeiRuntimeError{ "No loader for resource", typeid(T).name() };
//	}
//
//	template<typename T>
//	inline ResourceManager::Loader<T>& ResourceManager::AddLoader(Loader<T>* pLoader)
//	{
//		METRICS_TIMEBLOCK;
//
//		auto& pair = m_Loaders.emplace_back();
//		pair.first = &pLoader->ID();
//		pair.second.reset(pLoader);
//		return *pLoader;
//	}
//
//}
//
//#pragma endregion

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

	class ResourceManager
	{
	public:

		// Load resource with duplication check
		template <typename Data, typename ... Args> //requires LoadableResource<Data, Args...>
		[[nodiscard]] Resource<Data> LoadShared(std::filesystem::path const& filepath, Args&& ...);
		
		// Load resource regardless of duplication
		template <typename Data, typename ... Args> //requires LoadableResource<Data, Args...>
		[[nodiscard]] Resource<Data> LoadUnique(std::filesystem::path const& filepath = {}, Args&& ...);

	private:

		std::unordered_map<size_t, std::weak_ptr<void>> m_Resources;

	public:

		using Service = utility::Service<ResourceManager>;

	};

	template <typename Data>
	class ToLoad
	{
		constexpr ToLoad() noexcept = default;
		friend ResourceManager;
	};

	extern ResourceManager::Service Resources;

	template <typename Data>
	class Resource
	{

		friend ResourceManager;

	public:

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

	template<typename Data, typename ...Args> //requires LoadableResource<Data, Args...>
	inline Resource<Data> ResourceManager::LoadShared(std::filesystem::path const& filepath, Args && ... args)
	{
		auto absolute{ std::filesystem::absolute(filepath) };
		auto& weak = m_Resources[std::hash<std::filesystem::path::string_type>{}(absolute.native())];
		if (!weak.expired())
		{
			return { std::static_pointer_cast<Data>(weak.lock()) };
		}
		else
		{
			Resource<Data> resource{ LoadUnique<Data>(absolute, std::forward<Args>(args)...) };
			weak = resource.m_Data;
			return resource;
		}
	}

	template<typename Data, typename ...Args> //requires LoadableResource<Data, Args...>
	inline Resource<Data> ResourceManager::LoadUnique(std::filesystem::path const& filepath, Args && ... args)
	{
		auto absolute{ std::filesystem::absolute(filepath) };
		ToLoad<Data> dummy{};
		return { ::Load(dummy, absolute, std::forward<Args>(args)...) };
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