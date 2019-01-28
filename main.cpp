#include<ncurses.h>
#include <unistd.h>
#include<stdlib.h>
#include <queue>
#include <string>
#include <memory>
#include <map>
#include <cstring>


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

class LogWindow {
public:
    LogWindow(const std::string& t) {
        m_title = t;
    }
    virtual ~LogWindow() {
        destroy_win();
        printw("~LogWindow\n");
        refresh();
    }
    std::deque<std::string> m_lines;
    std::string m_title = "TITLE";
    WINDOW* m_window;
    int m_h, m_w, m_y, m_x;
    void createWindow(int h, int w, int y, int x) {
        WINDOW* win = newwin(h, w, y, x);

        scrollok(win, 1);
        box(win, 2 , 2);          /* 0, 0 gives default characters
                                   * for the vertical and horizontal
                                   * lines                        */
        // wCenterTitle(win, m_title.c_str());
        m_h = h;
        m_w = w;
        m_y = y;
        m_x = x;
        wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
        m_window = win;
    }
    void refresh() {
        wrefresh(m_window);
    }
    void destroy_win()
    {
        delwin(m_window);
    }
    void print(const std::string& st) {
        // m_lines.push_back(st);
        // if(m_lines.size() >= m_h - 1) {
        //     m_lines.pop_front();
        // }
        // for(int i=0; i<m_lines.size(); i++) {
        //     mvwprintw(m_window, i + 1, 1, m_lines[i].c_str());
        // }

        auto t = st + "\n";
        waddch(m_window, ' ');
        box(m_window, 0 , 0);
        wprintw(m_window, t.c_str());
        wborder(m_window, '|', '|', '-', '-', '+', '+', '+', '+');
        // wCenterTitle(m_window, m_title.c_str());
        wrefresh(m_window);
    }
};

class LogNode;
std::map<std::string, std::shared_ptr<LogWindow>> windows;
using NodePointer = std::shared_ptr<LogNode>;
using LogWindowHash = std::map<std::string, std::shared_ptr<LogWindow>>;
struct LogNode : public std::enable_shared_from_this<LogNode> {
    std::shared_ptr<LogNode> m_left = nullptr;
    std::shared_ptr<LogNode> m_right = nullptr;
    std::shared_ptr<LogWindow> m_window = nullptr;
    std::weak_ptr<LogNode> m_parent;
    std::string m_name;
    // std::weak_ptr<LogNode> m_self;

    enum class SPLIT_TYPE {VERT, HORZ, PARENT} m_splitType;
    int m_ratio = 1;

    LogNode(const LogNode& ln) = delete;
    LogNode& operator=(const LogNode& ln) = delete;
    LogNode() {
        m_splitType = SPLIT_TYPE::HORZ;
        // m_name = "default name";
        // printf("constr\n");
    }
    virtual ~LogNode() {
        // printf("~\n");
    }

    // std::pair<NodePointer, NodePointer> splitHoriz(const std::string& name1, const std::string& name2) {

    NodePointer split(const std::string& name2) {
        // printf("%s!!!!!\n", m_parent.lock()->m_name.c_str());

        if(auto parentPtr = m_parent.lock()) {
            NodePointer np = std::shared_ptr<LogNode>(new LogNode());
            // printf("%s!!!\n", m_parent.lock()->m_name.c_str());
            np->m_name = "np111";
            np->m_ratio = m_ratio;
            if(parentPtr->m_left.get() == this) {
                np->m_left = parentPtr->m_left;
                np->m_left->m_parent = np;
                np->m_right = std::shared_ptr<LogNode>(new LogNode());
                np->m_right->m_name = name2;
                np->m_right->m_parent = np;
                parentPtr->m_left = np;
            }
            else {
                np->m_left = parentPtr->m_right;
                np->m_left->m_parent = np; // ?????
                np->m_right = std::shared_ptr<LogNode>(new LogNode());
                np->m_right->m_name = name2;
                np->m_right->m_parent = np;
                parentPtr->m_right = np;
            }
            np->m_parent = parentPtr;
            // np->m_rows = m_rows;
            // np->m_cols = m_cols;
            this->m_parent = np;
            return np;
        }
        else {
            // printw("no root!!\n");
            m_left = std::shared_ptr<LogNode>(new LogNode());
            m_left->m_parent = shared_from_this();
            m_left->m_name = m_name;

            m_right = std::shared_ptr<LogNode>(new LogNode());
            m_right->m_parent = shared_from_this();
            m_right->m_name = "r2";
            m_name = "np111";

            return shared_from_this();
        }
    }
    NodePointer splitHoriz(const std::string& name2, int r1 = 1, int r2 = 1) {
        if(m_left == nullptr && m_right == nullptr) {
            this->m_splitType = SPLIT_TYPE::HORZ;
            auto p = split(name2);
            p->m_left->m_ratio = r1;
            p->m_right->m_ratio = r2;
            // p->m_left->m_rows = p->m_rows / 2;
            // p->m_left->m_cols = p->m_cols;
            // printw("rows %d\n", m_rows / 2);
            // p->m_right->m_rows = p->m_rows / 2;
            // p->m_right->m_cols = p->m_cols;
            // printw("rows %d\n", m_rows / 2);
            return p;
        }
        else {
            printf("nonono\n");
        }
        return nullptr;
    }
    NodePointer splitVert(const std::string& name2, int r1 = 1, int r2 = 1) {
        if(m_left == nullptr && m_right == nullptr) {
            this->m_splitType = SPLIT_TYPE::VERT;
            auto p = split(name2);
            p->m_left->m_ratio = r1;
            p->m_right->m_ratio = r2;
            // p->m_left->m_rows = p->m_rows;
            // p->m_left->m_cols = p->m_cols / 2;
            // p->m_right->m_rows = p->m_rows;
            // p->m_right->m_cols = p->m_cols / 2;
            return p;
        }
        else {
            printf("nonono\n");
        }
        return nullptr;
    }
    LogWindowHash createWindows(int st, int h, int w, int y, int x) {
        LogWindowHash lwh;
        __drawWindows(shared_from_this(), st, h, w, y, x, lwh);
        return lwh;
    }
    void __drawWindows(std::shared_ptr<LogNode> ln, int st, int h, int w, int y, int x, LogWindowHash& lwh) {
        if(ln->m_left == nullptr && ln->m_right == nullptr) {
            ln->m_window = std::shared_ptr<LogWindow>(new LogWindow(ln->m_name));
            ln->m_window->createWindow(h, w, y, x);
            lwh[ln->m_name] = ln->m_window;
        }

        // printw("%d %d\n", ln->, rightRatio);
        // float leftRatio = ln->m_left->m_ratio / (float)(ln->m_left->m_ratio + ln->m_right->m_ratio);
        // float rightRatio = ln->m_right->m_ratio / (float)(ln->m_left->m_ratio + ln->m_right->m_ratio);
        // printf("%f %f\n", leftRatio, rightRatio);
        // refresh();
        // float leftRatio = ln->m_left->m_ratio / (float)(ln->m_left->m_ratio + ln->m_right->m_ratio);
        float leftRatio = 1;
        float rightRatio = 2;
        // int rows, cols;
        if(ln->m_left) {
            leftRatio = ln->m_left->m_ratio / (float)(ln->m_left->m_ratio + ln->m_right->m_ratio);
            rightRatio = ln->m_right->m_ratio / (float)(ln->m_left->m_ratio + ln->m_right->m_ratio);
        }
        if(ln->m_splitType == LogNode::SPLIT_TYPE::HORZ) {
            if(ln->m_left) {
                __drawWindows(ln->m_left, st + 1, h * leftRatio, w, y, x, lwh);
            }
            if(ln->m_right) {
                __drawWindows(ln->m_right, st + 1, h * rightRatio, w, y + h * leftRatio, x, lwh);
            }
        }

        if(ln->m_splitType == LogNode::SPLIT_TYPE::VERT) {
            // printw("%f %f\n", leftRatio, rightRatio);
            // refresh();
            if(ln->m_left) {
                __drawWindows(ln->m_left, st + 1, h, w * leftRatio, y, x, lwh);
            }
            if(ln->m_right) {
                __drawWindows(ln->m_right, st + 1, h, w * rightRatio, y, x + w * leftRatio, lwh);
            }
        }
    }
    // std::pair<NodePointer, NodePointer> splitVert(const std::string& name1, const std::string& name2);
};



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
    auto lwh = root->createWindows(0, LINES, COLS, 0, 0);
    int i=0;
    while(1) {
        i++;
        lwh["r1"]->print(std::string("i'am r1: ") + std::to_string(i));
        lwh["r2"]->print(std::string("i'am r2: ") + std::to_string(i));
        lwh["r3"]->print(std::string("i'am r3: ") + std::to_string(i));
        // getch();
        usleep(50000);
    }
    // getch();
    endwin();
    return 0;
}
