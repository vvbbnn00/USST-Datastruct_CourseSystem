cmake_minimum_required(VERSION 3.26)
project(CourseSystem2023 VERSION 1.0.9)

set(CMAKE_C_STANDARD 23)

# 引入第三方库，该处使用的库如下（若因系统不兼容代码自带库而编译失败，需要下载源代码自行编译）：
# pcre2 https://pcre2project.github.io/pcre2/
include_directories("./cmake-libraries/include")
link_directories("./cmake-libraries/libs")


# 加入utils/wubi.txt到编译目录
configure_file(
        "${PROJECT_SOURCE_DIR}/utils/wubi.txt"
        "${PROJECT_BINARY_DIR}/utils/wubi.txt"
        COPYONLY
)

set(VERSIONINFO_RC "${CMAKE_CURRENT_SOURCE_DIR}/VersionInfo.rc")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/version.rc.in" "${VERSIONINFO_RC}")


add_executable(CourseSystem2023
        ${VERSIONINFO_RC}
        libs/link_list_object.c
        libs/link_list_object.h
        main.c
        models.h
        libs/hmacsha256.c
        libs/hmacsha256.h
        utils/avl.h
        utils/wubi.h
        utils/hash.h
        database/database.h
        common.h
        database/users.h
        services/users.h
        utils/string_ext.h
        services/ui.h
        database/courses.h
        database/selections.h
        utils/math_ext.h
        services/course.h)


# 为指定项目添加 include 路径
target_include_directories(CourseSystem2023 PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(CourseSystem2023 pcre2-8)