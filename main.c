#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "models.h"
#include "utils/avl.h"
#include "utils/wubi.h"
#include "libs/hmacsha256.h"
#include "common.h"
#include "services/database.h"

int main() {
    system("chcp 936");  // �л�ҳ���ļ�ΪGBK���룬�Լ������ﻷ��ϵͳ
    Wubi_Init();
    DB_Init();
//    printf("Hello, World!\n");
//    char *wubi = Wubi_chn2wubi("Ǯѧɭ��Ʒ����");
//    printf("%s\n", wubi);
//    printf("%lld\n", Hash_String(wubi));
//    printf("%lld\n", Hash_String("a"));
//    printf("%lld\n", Hash_String("ab"));
//    printf("%lld\n", Hash_String("b"));
//
//    printf("%s\n", Hash_PreprocessString("ab cd"));
//
//    printf(calcHexHMACSHA256("admin", SECRET_KEY));

    registerUser("��", ADMIN_USERNAME, ADMIN_PASSWORD, 2, "11451419191");
    printf("%s", getUserById((int64) getUserByName("����Ա")->indexNode->index.data)->name);

    return 0;
}
