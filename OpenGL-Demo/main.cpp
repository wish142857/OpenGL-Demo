#include <iostream>
#include "world.h"

World World::world;		// 构造单体实例对象

/********************
 * [函数] 主函数
 ********************/
int main() {
    return World::getInstance()->run();
}
