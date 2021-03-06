#include <iostream>
#include "world.h"
using namespace std;

World World::world;		// 构造单体实例对象

/********************
 * [函数] 主函数
 ********************/
int main() {
	std::string cmd;
	std::cout << "---------- [iWorld] ----------" << std::endl;
	std::cout << "[STD] Phong Algorithm" << std::endl;
	std::cout << "[RAY] Ray Tracing Algorithm" << std::endl;
	std::cout << "[SPR] Speedy Ray Tracing Algorithm" << std::endl << std::endl;
	while (true) {
		std::cout << "Main::Please input 'STD' 'RAY' or 'SPR'." << std::endl;
		std::cout << "<World> ";
		std::cin >> cmd;
		std::cout << std::endl;
		if (cmd == "EXIT") { return 0; }
		if (cmd == "STD") { return World::getInstance()->run("STD"); }
		if (cmd == "RAY") { return World::getInstance()->run("RAY"); }
		if (cmd == "SPR") { return World::getInstance()->run("SPR"); }
	}
}
