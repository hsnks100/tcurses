# tnurses
 
# Installation

```bash
git clone https://github.com/hsnks100/tcurses.git
cd tcurses
mkdir build && cd build 
cmake ..
make && ./tcurses
```

simple example

```cpp
#include <log_node.h>

int main()
{
    initscr(); /* Start curses mode */
    noecho();
    std::shared_ptr<LogNode> root = std::shared_ptr<LogNode>(new LogNode());
    root->m_name = "r1";
    auto r2 = root->splitVert("r2", 2, 10);
    r2 = r2->m_right;
    auto r3 = r2->splitHoriz("r3", 4, 5);
    r3 = r3->m_right;
    r3->splitVert("r4", 1, 1);
    /*
      ----------------|            ----------------|      ----------------|      ----------------|
      |               |            |       |       |      |       |       |      |       |       |
      |               |            |       |       |      |       | r2:4  |      |       | r2:4  |
      |       r1      |  ========> |   r1:2| r2:10 | ===> |   r1  |-------| ==>  |   r1  |-------|
      |               |            |       |       |      |       | r3:5  |      |       | r3|r4 |
      |               |            |       |       |      |       |       |      |       |   |   |
      ----------------             ----------------|      ----------------|      ----------------|
    */
    root->createWindows(0, LINES, COLS, 0, 0);
    int i=0;
    while(1) {
        i++;
        root->findNode("r1")->print(std::string("i'am r1: ") + std::to_string(i));
        root->findNode("r2")->print(std::string("i'am r2: ") + std::to_string(i));
        root->findNode("r3")->print(std::string("i'am r3: ") + std::to_string(i));
        root->findNode("r4")->print(std::string("i'am r4: ") + std::to_string(i));
        usleep(50000);
    }
    endwin();
    return 0;
}

```

# result

![title-bar](https://user-images.githubusercontent.com/3623889/52909561-a4700500-32cd-11e9-9309-fee3cdf0d3e6.gif)
