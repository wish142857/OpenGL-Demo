#include <iostream>
#include "world.h"

World World::world;		// ���쵥��ʵ������

/********************
 * [����] ������
 ********************/
int main() {
    return World::getInstance()->run();
}
