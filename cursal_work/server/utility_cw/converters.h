//
// Created by Des Caldnd on 6/2/2024.
//

#ifndef MP_OS_CONVERTERS_H
#define MP_OS_CONVERTERS_H

#include "../in_memory/controller/memory_controller.h"
#include "../in_disk/controller/disk_controller.h"

template<serializable tkey, serializable tvalue, compator<tkey> compare, size_t t>
friend disk_controller<tkey, tvalue, compare, t> memory_to_disk(const memory_controller<tkey, tvalue, compare, t>& cont, const std::string& path);

#endif //MP_OS_CONVERTERS_H
