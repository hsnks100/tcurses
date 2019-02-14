#include<ncurses.h>
#include <unistd.h>
#include<stdlib.h>
#include <queue>
#include <string>
#include <memory>
#include <map>
#include <cstring>

#include <log_node.h>


void wCenterTitle(WINDOW *pwin, const char * title)
{
  
    int x, maxy, maxx, stringsize;
    getmaxyx(pwin, maxy, maxx);
    stringsize = 4 + strlen(title);
    x = (maxx - stringsize)/2;
    mvwaddch(pwin, 0, x, ACS_RTEE);
    waddch(pwin, ' ');
    waddstr(pwin, title);
    waddch(pwin, ' ');
    waddch(pwin, ACS_LTEE);
}




// 1 |2|3
//   |----
//   | 4

// auto a = root->splitHoriz("1", "");
// auto b = a.second->splitVert("", "4");
// b.first->splitHoriz("2", "3");



void check(std::shared_ptr<LogNode> ln, int st, int h, int w, int y, int x) {
    for(int i=0; i<st; i++) {
        printw(".");
    }
    printw("%s: %d\n", ln->m_name.c_str(), ln->m_ratio);
    refresh();
    if(ln->m_left) {
        if(ln->m_left->m_parent.lock().get() != ln.get()) {
            for(int i=0; i<st; i++) {
                printw(".");
            }
            refresh();
            printw("wrong structure %s\n", ln->m_left->m_name.c_str());
        }
        if(ln->m_splitType == LogNode::SPLIT_TYPE::HORZ) {
            check(ln->m_left, st + 1, h / 2, w, y, x);
        }
        else {
            check(ln->m_left, st + 1, h, w / 2, y, x);
        }
    }
    if(ln->m_right) {
        if(ln->m_right->m_parent.lock().get() != ln.get()) {
            for(int i=0; i<st; i++) {
                printw(".");
            }
            refresh();
            printw("wrong structure %s, %s\n", ln->m_right->m_parent.lock()->m_name.c_str(), ln->m_name.c_str());
        }
        if(ln->m_splitType == LogNode::SPLIT_TYPE::HORZ) {
            check(ln->m_right, st + 1, h / 2, w, y + h / 2, x);
        }
        else {
            check(ln->m_right, st + 1, h, w / 2, y, x + w / 2);
        }
    }
}


int main()
{
    initscr(); /* Start curses mode */
    noecho();
    // attron(A_BOLD);

    std::shared_ptr<LogNode> root = std::shared_ptr<LogNode>(new LogNode());
    root->m_name = "r1";
    auto p = root->splitVert("r2", 2, 10);
    p->m_right->splitHoriz("r3", 4, 5);
    /*
      ----------------|            ----------------|      ----------------|
      |               |            |       |       |      |       |       |
      |               |            |       |       |      |       | r2:4  |
      |       r1      |  ========> |   r1:1| r2:10 | ===> |   r1  |-------|
      |               |            |       |       |      |       | r3:5  |
      |               |            |       |       |      |       |       |
      ----------------             ----------------|      ----------------|
    */
    // check(root, 0, LINES / 2, COLS, 0, 0);
    root->createWindows(0, LINES, COLS, 0, 0);


    int i=0;
    while(1) {
        i++;
        auto t = root->findNode("r1");
        if(t) {
            t->print(std::string("i'am r2: ") + std::to_string(i));
        }
        t = root->findNode("r2");
        if(t) {
            t->print(std::string("i'am r2: ") + std::to_string(i));
        }
        t = root->findNode("r3");
        if(t) {
            t->print(std::string("i'am r3: ") + std::to_string(i));
        }
        usleep(50000);
    }
    // getch();
    endwin();
    return 0;
}
