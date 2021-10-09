#include <cstdio>
#include <string>
#include <chrono>
#include <thread>
#include <algorithm>
#include <vector>
#include <csignal>

template<typename T>
T choose(T first_option, T second_option, float probability){
    return (std::rand() < probability * (float)RAND_MAX) ? second_option : first_option;
}

sig_atomic_t signalled = 0;

void handler(int sig){
    signalled = 1;
}

bool rule_b(int n, std::vector<int> const &b){
    return std::find(std::begin(b), std::end(b), n) != std::end(b);
}

bool rule_s(int n, std::vector<int> const &s){
    return std::find(std::begin(s), std::end(s), n) != std::end(s);
}

volatile bool keep_running = true;

void userInput_thread() {
    signal(SIGINT, handler);
    while(keep_running) {
        if (signalled){
            keep_running = false;
            return;
        }
    }
}

void run(char **init, int size_x, int size_y, int max_iter, int time_delay, std::vector<int> b, std::vector<int> s){
    char init_next[size_y][size_x];
    for (;keep_running && max_iter != 0;max_iter--) {
        for (int i = 0; i < size_y; i++) {
            for (int j = 0; j < size_x; j++) {
                char sum=0;
                for (int c1 = -1; c1 <= 1; c1++){
                    for (int c2 = -1; c2 <= 1; c2++){
                        sum += init[(size_y+i+c1) % size_y][(size_x + j + c2) % size_x];
                    }
                }
                if (init[i][j] == 0){
                    if (rule_b(sum, b)) init_next[i][j]=1;
                    else init_next[i][j]=0;
                }
                else {
                    --sum;
                    if (rule_s(sum, s)) init_next[i][j]=1;
                    else init_next[i][j]=0;
                }
            }
        }

        printf(" ");
        for (int j = 0; j<size_x;j++)printf("\u23BD\u23BD");
        printf(" \n");
        for (int i = 0; i < size_y; i++) {
            printf("\u239F");
            for (int j = 0; j < size_x; j++) {
                printf(init[i][j]==1 ? "\u2588\u2588" : "  ");
            }
            printf("\u239C\n");
        }
        printf(" ");
        for (int j = 0; j<size_x;j++)printf("\u23BA\u23BA");
        printf(" \n");
        std::this_thread::sleep_for(std::chrono::milliseconds(time_delay));
        printf("\033[%dA", size_y+2);

        for (int i = 0; i < size_y; i++) {
            for (int j = 0; j < size_x; j++) {
                init[i][j] = init_next[i][j];
            }
        }
    }
    printf("\033[%dB", size_y+5);
    exit(0);
}

int main(int argc, char* argv[]) {
    int size_x = 100, size_y = 50;
    int max_iter = -1;
    float prob = 0.5;
    int time_delay = 50;
    int time_ui = time(nullptr) % RAND_MAX;
    srand(time_ui);
    char game = 0;
    std::vector<int> b;
    std::vector<int> s;
    for (int i = 1; i < argc; i++){
        std::string arg = argv[i];
        if (arg == "-w"){
            i++;
            int width = std::stoi(argv[i]);
            size_x = width;
            continue;
        }
        if (arg == "-h") {
            i++;
            int height = std::stoi(argv[i]);
            size_y = height;
            continue;
        }
        if (arg == "-p") {
            i++;
            float probability = std::stof(argv[i]);
            prob = probability;
            continue;
        }
        if (arg == "-t") {
            i++;
            time_delay = std::stoi(argv[i]);
            continue;
        }
        if (arg == "-i") {
            i++;
            max_iter = std::stoi(argv[i]);
            continue;
        }
        if (arg == "-l"){
            game = 0;
        }
        else if (arg == "-d"){
            game = 1;
        }
        else if (arg == "-b"){
            i++;
            game = 2;
            for(;i < argc && argv[i][0]!='-';i++){
                b.push_back(std::stoi(argv[i]));
            }
            i--;
        }
        else if (arg == "-s"){
            i++;
            game = 2;
            for(;i < argc && argv[i][0]!='-';i++){
                s.push_back(std::stoi(argv[i]));
            }
            i--;
        }
        else if (arg == "--help" or true){
            char help[] = "Options:\n\t-w\t\twindow width\n\t-h\t\twindow height\n\t-t\t\titeration time (ms)\n\t-i\t\tnumber of iterations\n\t-p\t\tfilling density [0..1]\n\t-l\t\tConway's Game of Life cellular automaton with B3/S23 rule (-b 3 -s 2 3)\n\t-d\t\tDay and Night cellular automaton with B3678/S34678 rule (-b 3 6 7 8 -s 3 4 6 7 8)\n\t-b [NUM]\tdead cell becomes live (is born) if it has [NUM] live neighbors\n\t-s [NUM]\ta live cell remains alive (survives) if it has [NUM] live neighbors\n";
            printf("%s", help);
            return 0;
        }
    }

    if (game == 0){
        b = std::vector<int>{3};
        s = std::vector<int>{2,3};
    } else if (game == 1){
        b = std::vector<int>{3,6,7,8};
        s = std::vector<int>{3,4,6,7,8};
    }

    char **init = new char*[size_y];
    for (int i = 0; i < size_y; i++){
        init[i] = new char[size_x];
    }

    for (int i = 0; i < size_y; i++) {
        for (int j = 0; j < size_x; j++) {
            init[i][j] = choose<char>(0,1, prob);
        }
    }

    std::thread t2(userInput_thread);
    run(init, size_x, size_y, max_iter, time_delay, b, s);
    t2.join();
    printf("\033[%dB", size_y+5);
    return 0;
}
