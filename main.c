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
    system("chcp 936");  // �л�ҳ���ļ�ΪGBK���룬�Լ������ﻷ��ϵͳ
    Wubi_Init();
    DB_Init();
    char status = setjmp(GLOBAL_goto_login); // ȫ�ֱ�ǵ㣬�����˳���¼���¼����ʱ��ת
    DB_registerUser("����Ա", ADMIN_USERNAME, ADMIN_PASSWORD, 2, "11451419191");
    DB_registerUser("½���", "2135060620", "123456", 0, "11451419191");
    DB_registerUser("��ʦ", "teacher", "123456", 1, "11451419191");

//    Serv_User_login(status);

    GlobalUser = DB_getUserByEmpId("admin");

    // ��ӡ���˵�
    UI_printMainMenu(0);

    return 0;
}
