#pragma once

// Credits to
// https://www.kdab.com/jsonify-with-nlohmann-json/

#include <nlohmann/json.hpp>
#include <optional>

// partial specialization (full specialization works too)
namespace nlohmann
{
    template<class T> void optional_to_json(nlohmann::json& j, const char* name, const std::optional<T>& value)
    {
        if (value)
            j[name] = *value;
    }

    template<class T> void optional_from_json(const nlohmann::json& j, const char* name, std::optional<T>& value)
    {
        const auto it = j.find(name);
        if (it != j.end() && !it->is_null())
            value = it->get<T>();
        else
            value = std::nullopt;
    }

    template<typename> constexpr bool is_optional = false;
    template<typename T> constexpr bool is_optional<std::optional<T>> = true;

    template<typename T> void extended_to_json(const char* key, nlohmann::json& j, const T& value)
    {
        if constexpr (is_optional<T>)
            nlohmann::optional_to_json(j, key, value);
        else
            j[key] = value;
    }

    template<typename T> void extended_from_json(const char* key, const nlohmann::json& j, T& value)
    {
        if constexpr (is_optional<T>)
            nlohmann::optional_from_json(j, key, value);
        else
            j.at(key).get_to(value);
    }
} // namespace nlohmann

#define EXTEND_JSON_TO(v1) extended_to_json(#v1, nlohmann_json_j, nlohmann_json_t.v1);
#define EXTEND_JSON_FROM(v1) extended_from_json(#v1, nlohmann_json_j, nlohmann_json_t.v1);

#define NLOHMANN_DEFINE_TYPE_EXTENSION(Type, ...)                                                                                                              \
    inline void to_json(nlohmann::json& nlohmann_json_j, const Type& nlohmann_json_t)                                                                          \
    {                                                                                                                                                          \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(EXTEND_JSON_TO, __VA_ARGS__))                                                                                 \
    }                                                                                                                                                          \
    inline void from_json(const nlohmann::json& nlohmann_json_j, Type& nlohmann_json_t)                                                                        \
    {                                                                                                                                                          \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(EXTEND_JSON_FROM, __VA_ARGS__))                                                                               \
    }
