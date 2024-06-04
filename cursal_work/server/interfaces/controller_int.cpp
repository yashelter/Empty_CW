#include "controller_int.h"

generator<CW_GUID> guid_sequence()
{
    while (true)
    {
        co_yield CW_GUID();
    }
}

CW_GUID::CW_GUID() : xg::Guid(xg::newGuid()) {}

bool CW_GUID::operator==(const CW_GUID &other) const noexcept
{
    return xg::Guid::operator==(other);
}

nlohmann::json CW_GUID::to_json() const
{
    nlohmann::json res;
    res["id"] = std::string(*this);
    return res;
}

CW_GUID::CW_GUID(nlohmann::json j) : xg::Guid(j["id"].get<std::string>()) {}
