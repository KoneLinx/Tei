#pragma once

#include <optional>
#include <nlohmann/json.hpp>

void Load(std::shared_ptr<nlohmann::json>& out, std::filesystem::path const& path);

void LoadToError(std::string name, std::type_info const& type);

inline bool LoadToOptional(auto& val, nlohmann::json const& object, std::string const& name)
{
	if (auto val = object.at(name); !val.is_null())
	{
		try
		{
		    val.get_to(val);
			return true
		}
		catch (...)
		{
			LoadToError(name, typeid(val));
		}
	}
	return false;
}

template <typename T>
inline std::optional<T> LoadOptional(nlohmann::json const& object, std::string const& name)
{
	T val{};
	if (LoadToOptional(val, object, name))
		return val;
	else
		return std::nullopt;
}

void LoadToRequiredError(std::string const& name);

inline void LoadToRequired(auto& val, nlohmann::json const& object, std::string const& name)
{
    if (object.at(name).is_null())
        LoadToRequiredError(name);
	(void)LoadToOptional(val, object, name);
}

template <typename T>
inline T LoadRequired(nlohmann::json const& object, std::string const& name)
{
	T val{};
	LoadToRequired(val, object, name);
	return val;
}