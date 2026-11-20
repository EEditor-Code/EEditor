// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 北京格瑞特皮软件科技有限公司

#ifndef DEFINE_COMMON
#define DEFINE_COMMON 1


//about log defines
#ifdef USE_SPDLOG 
    #define SPDLOG_COMPILED_LIB
    #define SPDLOG_SHARED_LIB
    
    #include <spdlog/spdlog.h>
    #include <spdlog/cfg/env.h>
    
    #define LOG_PRE_DEBUG spdlog::debug("{},{},{}", __FILE__, __FUNCTION__,__LINE__ );
    #define LOG_PRE_INFO spdlog::info("{},{},{}", __FILE__, __FUNCTION__,__LINE__ );
    #define LOG_PRE_ERROR spdlog::error("{},{},{}", __FILE__, __FUNCTION__,__LINE__ );
    
    #define LOG_DEBUG(...) LOG_PRE_DEBUG spdlog::debug(__VA_ARGS__);
    #define LOG_INFO(...) LOG_PRE_INFO spdlog::info(__VA_ARGS__);
    #define LOG_ERROR(...) LOG_PRE_ERROR spdlog::error(__VA_ARGS__);
#else
    #include <log/log.h>

    // #define LOG_USE_COLOR
    #define ELOG_DEBUG(...) log_debug(__VA_ARGS__);
    #define ELOG_INFO(...) log_info(__VA_ARGS__);
    #define ELOG_ERROR(...) log_error(__VA_ARGS__);
    #define SET_LOG_LEVEL_DEBUG log_set_level(LOG_DEBUG);
    #define SET_LOG_LEVEL_INFO log_set_level(LOG_INFO);
    #define SET_LOG_LEVEL_ERROR log_set_level(LOG_ERROR);
#endif //

/**about matrix4x4, string and other basic struct defines
 * The EMatrix4x4 need be rewrite later to aprove assignment of data
 * */
#ifdef USE_QT_BASIC
    #include <QtCore/QString>
    #include <QtCore/QStringList>
    #include <QtCore/QList>
    #include <QtCore/QMap>
    #include <QtCore/QVariant>
    
    #define EString QString
    #define EStringList QStringList
    #define EList QList
    #define EMap QMap
    #define EVariant QVariant
#else
    // #include <string>
    #include <list>
    #include <vector>
    #include <map>
    #include <EVariant.h>
    #include <EString.h>

    // #define EString std::string
    #define EList std::vector
    #define EMap std::map
    // #define EVariant QVariant
#endif //

//about smart pointer defines
// #include <memory>
#include <SmartPtr/ESmartPtr.h>
// #define SmartPtr(...) std::shared_ptr<__VA_ARGS__>
// #define NewSmartPtr(...) std::make_shared<__VA_ARGS__>

//about delegate defines
// #include <delegate/delegate.h>
// #define EDelegate(...) dlgt::delegate<__VA_ARGS__>
// #define MakeDelegate(...) dlgt::make_delegate(__VA_ARGS__)

// #include <delegate/delegate.h>
// #define EDelegate cpp::delegate
// #define EBind cpp::bind

//math related defines
#define PI_VALUE 3.141592653589793
#define PI_HALF_VALUE 1.570796326794896
#define DEGREE_TO_RADIUS(a) a/180.0*PI_VALUE 
#define RADIUS_TO_DEGREE(a) a/PI_VALUE*180.0

#endif
