/********************************************************************************
 * Project Name:    Adventure Designer Studio
 * Filename:        path_not_exist_exception.h
 * Description:     
 *
 * Crafted with passion and love in Canary Islands.
 *
 * Copyright (C) 2024 El Estado Web Research & Development. All rights reserved.
 *
 * This document and its contents, including but not limited to code, schemas,
 * images and diagrams are protected by copyright law in Spain and other countries.
 * Unauthorized use, reproduction, distribution, modification, public display, or
 * public performance of any portion of this material is strictly prohibited.
 *
 * Contact Information
 *
 * For permission to use any portion of this material, please contact at
 * El Estado Web Research & Development, S.L. <info@elestadoweb.com>
 ********************************************************************************/
#ifndef PATH_NOT_EXIST_EXCEPTION_H
#define PATH_NOT_EXIST_EXCEPTION_H
#include <utility>
#include "base_exception.h"

namespace ADS::Exceptions {
    class PathNotExistException final : public BaseException {
    public:
        explicit PathNotExistException(const std::string &msg, std::string file = __FILE__, const int line = __LINE__):
            BaseException(msg, std::move(file), line) {}
    };
} // ADS::Exceptions

#endif //PATH_NOT_EXIST_EXCEPTION_H
