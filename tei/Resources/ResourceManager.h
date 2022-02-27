#pragma once

#include <memory>
#include <filesystem>
#include <any>

#include <tei/utility.h>

namespace tei::internal::resource
{

	class ResourceManager final
	{
		struct LoaderBase;

	public:

		template <typename T>
		class Loader;

		ResourceManager(std::filesystem::path defaultPath = std::filesystem::current_path());
		~ResourceManager();

		std::filesystem::path const& DefaultPath() const
		{ return m_DefaultPath; }

		template <typename T>
		T& Load(std::filesystem::path path, std::optional<T> init = std::nullopt) const;

		template<typename T>
		Loader<T>& AddLoader(Loader<T>* init = {});

		std::filesystem::path GetExisting(std::filesystem::path const& path) const;

	private:

		ResourceManager(ResourceManager const&) = delete;
		ResourceManager(ResourceManager&&) = delete;
		ResourceManager& operator = (ResourceManager const&) = delete;
		ResourceManager& operator = (ResourceManager&&) = delete;

		std::filesystem::path m_DefaultPath;

		struct LoaderBase
		{
			virtual ~LoaderBase() = default;
		};

		std::vector<std::pair<std::type_info const*, std::unique_ptr<LoaderBase>>> m_Loaders;

		mutable std::vector<std::any> m_Resources;

	public:

		template <typename T>
		class Loader : LoaderBase
		{
		public:

			~Loader() = default;
			explicit Loader() = default;

		protected:

			using ResourceManager = ResourceManager;

		private:

			virtual T* OnLoad(std::filesystem::path const& path, ResourceManager const& resources, std::optional<T> init) const = 0;
			virtual void OnFree(ResourceManager const& resources, T* object) const = 0;

			virtual inline std::type_info const& ID() const noexcept final
			{
				return typeid(T);
			}

			friend ResourceManager;

			Loader(Loader const&) = delete;
			Loader(Loader&&) = delete;
			Loader& operator = (Loader const&) = delete;
			Loader& operator = (Loader&&) = delete;

		};

		using Service = utility::Service<ResourceManager const>;

	};

	extern ResourceManager::Service Resources;

}

namespace tei::external
{

	static constexpr auto const& Resources{ tei::internal::resource::Resources };

}

#pragma region _impl

namespace tei::internal::resource
{

	template<typename T>
	inline T& ResourceManager::Load(std::filesystem::path path, std::optional<T> init) const
	{
		METRICS_TIMEBLOCK;

		if (auto const it = std::ranges::find(m_Loaders, &typeid(T), [] (auto const& p) { return std::get<0>(p); }); it != end(m_Loaders))
		{
			auto const loader = static_cast<Loader<T>*>(it->second.get());
			auto resource = std::shared_ptr<T>{
				loader->OnLoad(path, *this, init),
				[loader, this] (T* object) 
				{ 
					loader->OnFree(*this, object); 
				}
			};
			m_Resources.push_back(resource);
			return *resource;
		}
		else throw utility::TeiRuntimeError{ std::string{ "No loader for type " } + typeid(T).name() };
	}

	template<typename T>
	inline ResourceManager::Loader<T>& ResourceManager::AddLoader(Loader<T>* pLoader)
	{
		METRICS_TIMEBLOCK;

		auto& pair = m_Loaders.emplace_back();
		pair.first = &pLoader->ID();
		pair.second.reset(pLoader);
		return *pLoader;
	}

}

#pragma endregion