//
// Created by vvbbnn00 on 2023/08/28.
//

#ifndef COURSESYSTEM2023_COMMON_H
#define COURSESYSTEM2023_COMMON_H

#define SECRET_KEY "COURSESYSTEM2023"
#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "0d990abae043551d804e834ee1b7e04ee9fcd5230b812a04da0aef6e499eb0c4"

// 用户名正则表达式
#define USER_PATTERN "^[a-zA-Z0-9]{3,15}$"
// 密码正则表达式
#define PASSWD_PATTERN "^(?=.*\\d)(?=.*[a-zA-Z]).{8,20}$"
// 服务器设置存放位置
#define SERVER_CONFIG_PATH "server.conf"
// 用户登录Session存放位置
#define USER_SESSION "login.session"
// 课程ID正则表达式（长度为5-30，由字母、数字和-组成的字符串，建议格式<学年>-<学期>-<课程代号>-<班级>）
#define COURSE_ID_PATTERN "^[a-zA-z0-9-]{5,30}$"
// 课程名称正则表达式（长度为5-50字符，由字母、数字、中文和()-组成的字符串）
#define COURSE_TITLE_PATTERN "^[a-zA-Z0-9()（）\u4e00-\u9fa5-]{5,50}$"
// 课程描述正则表达式（长度为 5-250 字符）
#define COURSE_DESCRIPTION_PATTERN "^[\\D0-9]{5,250}$"
// 课程类型正则表达式
#define COURSE_TYPE_PATTERN "^[0-3]{1}$"
// 学期正则表达式（长度为 4-10 字符，由字母、数字和-组成的字符串，建议格式<学年>-<学期>）
#define SEMESTER_PATTERN "^[a-zA-Z0-9-]{4,10}$"
// 数字标准正则表达式（大于0）
#define NUMBER_PATTERN "^[+]{0,1}(\\d+)$"
// 学分正则表达式（大于0的小数，最多小数点后2位）
#define POINTS_PATTERN "^[+]{0,1}(\\d+)$|^[+]{0,1}(\\d+\\.\\d{1,2})$"
// 用户角色正则表达式
#define USER_ROLE_PATTERN "^[0-2]{1}$"
// 用户姓名正则表达式
#define USER_NAME_PATTERN "^[ 0-9a-zA-Z\u4e00-\u9fa5·]{2,20}$"
// 学校正则表达式（长度为3-50字符，由字母、数字、中文和空格组成的字符串）
#define SCHOOL_PATTERN "^[a-zA-Z0-9()（）\u4e00-\u9fa5-]{3,50}$"


#endif //COURSESYSTEM2023_COMMON_H
