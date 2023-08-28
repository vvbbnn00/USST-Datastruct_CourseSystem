//
// Created by vvbbnn00 on 2023/08/28.
//

#ifndef COURSESYSTEM2023_COMMON_H
#define COURSESYSTEM2023_COMMON_H

#define SECRET_KEY "COURSESYSTEM2023"
#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "123456"
#define VERSION "v1.0(AVL DB Core v0.1)"

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
#define COURSE_TITLE_PATTERN "^[a-zA-Z0-9\(\)（）\u4e00-\u9fa5\-]{5,50}$"
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


#define WORDS_Login_success "---- 用户登录 - 登陆成功 ----\n用户名：      %s\n姓  名：      %s\n角  色：      %s\n过期时间：    %s\n\n登陆成功，%s，欢迎您（按任意键进入主页）。\n\n"
#define WORDS_Login_role_student "学生"
#define WORDS_Login_role_teacher "教师"
#define WORDS_Login_role_admin "管理员"
#define WORDS_Login_role_unknown "未知角色"

#define WORDS_Login_parse_role_error "[解析错误] 返回参数role解析错误，登录终止。\n"
#define WORDS_Login_save_session_error "[文件错误] 写入登录状态异常，自动登录功能可能不可用。\n"

#include <setjmp.h>
jmp_buf GLOBAL_goto_login;

#endif //COURSESYSTEM2023_COMMON_H
