//
// Created by Des Caldnd on 6/2/2024.
//

#ifndef MP_OS_CONTROLLER_INT_H
#define MP_OS_CONTROLLER_INT_H

#include <string>
#include <nlohmann/json.hpp>
#include <optional>
#include <unordered_map>
#include <chrono>

class CW_GUID
{
    CW_GUID();

public:

    static CW_GUID get_next();
    //TODO: CW_GUID

    nlohmann::json to_json() const;
    CW_GUID(nlohmann::json j);

};

template<>
struct std::hash<CW_GUID>
{
    size_t operator()(const CW_GUID&) const noexcept
    {
        // TODO: hash
        return 1;
    }
};




template<typename compare, typename tkey>
concept compator = requires(const compare c, const tkey& lhs, const tkey& rhs)
                   {
                       {c(lhs, rhs)} -> std::same_as<bool>;
                   } && std::copyable<compare> && std::default_initializable<compare>;

template<typename f_iter, typename tkey, typename tval>
concept input_iterator_for_pair = std::input_iterator<f_iter> && std::same_as<typename std::iterator_traits<f_iter>::value_type, std::pair<tkey, tval>>;

template<typename T>
concept serializable = requires (const T t, std::fstream s, const nlohmann::json j)
                       {
                           {t.serialize(s)};
                           {T::deserialize(s)} -> std::same_as<T>;
                           {t.serializa_size()} -> std::same_as<size_t>;
                           {t.to_json()} -> std::same_as<nlohmann::json>;
                           {T::from_json(j)} -> std::same_as<T>;
                       } && std::copyable<T>;


template <serializable tkey, serializable tvalue>
class controller_int
{
public:

    using time_point_t = std::chrono::time_point<std::chrono::utc_clock>;

    virtual CW_GUID add_pool(std::string pool_name) =0;
    virtual CW_GUID remove_pool(std::string pool_name) =0;

    virtual CW_GUID add_scheme(std::string pool_name, std::string scheme_name) =0;
    virtual CW_GUID remove_scheme(std::string pool_name, std::string scheme_name) =0;

    virtual CW_GUID add_collection(std::string pool_name, std::string scheme_name, std::string collection_name) =0;
    virtual CW_GUID remove_collection(std::string pool_name, std::string scheme_name, std::string collection_name) =0;

    virtual CW_GUID insert(std::string pool_name, std::string scheme_name, std::string collection_name, tkey key, tvalue value) =0; // insert if not exist
    virtual CW_GUID read_value(std::string pool_name, std::string scheme_name, std::string collection_name, tkey key, bool need_persist, time_point_t time = std::chrono::utc_clock::now()) =0;
    virtual CW_GUID read_range(std::string pool_name, std::string scheme_name, std::string collection_name, tkey lower, tkey upper, bool need_persist, time_point_t time = std::chrono::utc_clock::now()) =0;
    virtual CW_GUID update(std::string pool_name, std::string scheme_name, std::string collection_name, tkey key) =0; // updates if exist
    virtual CW_GUID remove(std::string pool_name, std::string scheme_name, std::string collection_name, tkey key) =0;

    virtual std::optional<nlohmann::json> get(CW_GUID id) =0;
};

#endif //MP_OS_CONTROLLER_INT_H
