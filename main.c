#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "models.h"
#include "utils/avl.h"
#include "utils/wubi.h"
#include "libs/hmacsha256.h"
#include "common.h"
#include "database/database.h"
#include "database/users.h"
#include "services/users.h"
#include <setjmp.h>

int main() {
    system("chcp 936");  // 切换页面文件为GBK编码，以兼容日语环境系统
    Wubi_Init();
    DB_Init();
//    printf("Hello, World!\n");
//    char *wubi = Wubi_chn2wubi("钱学森作品导读");
//    printf("%s\n", wubi);
//    printf("%lld\n", Hash_String(wubi));
//    printf("%lld\n", Hash_String("a"));
//    printf("%lld\n", Hash_String("ab"));
//    printf("%lld\n", Hash_String("b"));
//
//    printf("%s\n", Hash_PreprocessString("ab cd"));
//
//    printf(calcHexHMACSHA256("admin", SECRET_KEY));

    char status = setjmp(GLOBAL_goto_login); // 全局标记点，用于退出登录或登录过期时跳转
    DB_registerUser("管理员", ADMIN_USERNAME, ADMIN_PASSWORD, 2, "11451419191");
    DB_registerUser("陆天成", "2135060620", "123456", 0, "11451419191");
    DB_registerUser("老师", "teacher", "123456", 1, "11451419191");
    printf("%s", DB_getUserById((int64) DB_getUsersByName("管")->indexNode->index.data)->name);

    system("pause");

    Serv_User_login(status);
    // 打印主菜单
    UI_printMainMenu(0);

    return 0;
}
