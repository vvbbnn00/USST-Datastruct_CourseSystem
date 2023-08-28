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
    system("chcp 65001");
    system("chcp 936");
    Wubi_Init();
    DB_Init();
    char status = setjmp(GLOBAL_goto_login); // ȫ�ֱ�ǵ㣬�����˳���¼���¼����ʱ��ת
    DB_registerUser("����Ա", ADMIN_USERNAME, ADMIN_PASSWORD, 2, "11451419191");
    DB_registerUser("½���", "2135060620", "123456", 0, "11451419191");
    DB_registerUser("��ʦ", "teacher", "123456", 1, "11451419191");

    printf("%lld\n", Hash_String(Hash_PreprocessString(Wubi_chn2wubi("�й���ʷ"))));
    printf("%lld\n", Hash_String(Wubi_chn2wubi("�й���ʷ")));
//    Serv_User_login(status);
    NodeList *result = NULL;
    AVL_inOrderTraverse(course_name_Index, &result);
    for (NodeList *p = result; p != NULL; p = p->next) {
        printf("%lld\n", p->indexNode->index.hash);
    }
    system("pause");

    GlobalUser = DB_getUserByEmpId(ADMIN_USERNAME);

    // ��ӡ���˵�
    UI_printMainMenu(0);

    return 0;
}
