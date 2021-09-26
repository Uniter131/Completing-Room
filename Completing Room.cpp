// Completing Room.cpp / 제작일: 21.9.18(토) / 제작자: 20183722 김현우

#include <bangtal>
#include <string>
#include <random>
#include <time.h>

#define INTERVAL 144
#define MAX_INDEX 25

using namespace bangtal;
using namespace std;

class Puzzle {
private:
    int index;
    string imgName;
    ScenePtr scene;
    ObjectPtr piece;

public:
    int getIndex() const { return index; }
    string getImgName() const { return imgName; }
    ObjectPtr getPiece() const { return piece; }

    void setIndex(const int i) { index = i; }
    void setImageName(const string& iN) { imgName = iN; };
    void InitPiece(ScenePtr s, int x, int y, int i);
};

class Board {
private:
    int xPos, yPos;

public:
    int getX() const { return xPos; };
    int getY() const { return yPos; };

    void setPos(const int x, const int y) { xPos = x; yPos = y; };

    void InitBoard(int x, int y, int i) { xPos = x; yPos = y; };
};

ScenePtr openingScene;
ScenePtr playScene;
ScenePtr afterScene;

ObjectPtr openTitle;
ObjectPtr button_play;
ObjectPtr button_exit;
ObjectPtr button_retry;
ObjectPtr sampleImage;
ObjectPtr afterTitle;
ObjectPtr clear;

TimerPtr timer;
TimerPtr timerShow;

Puzzle piece[MAX_INDEX];
Board board[MAX_INDEX];
int blank = MAX_INDEX - 1;

// 호출될시 Puzzle 클래스의 멤버들을 초기화하고 퍼즐조각을 생성합니다
void Puzzle::InitPiece(ScenePtr s, int x, int y, int i) {
    scene = s; index = i;
    imgName = "Images/Image1-" + to_string(i + 1) + ".png";
    piece = Object::create(imgName, s, x, y);
}

// 씬, 오브젝트, 타이머를 생성합니다
void InitGame() {
    setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
    setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
    setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

    openingScene = Scene::create("", "Images/StartBackground.png");
    playScene = Scene::create("", "Images/PlayBackground.png");
    afterScene = Scene::create("", "Images/StartBackground.png");

    openTitle = Object::create("Images/Title.png", openingScene, 40, 450);
    button_play = Object::create("Images/Play.png", openingScene, 280, 320);
    button_exit = Object::create("Images/Exit.png", openingScene, 280, 250);
    button_retry = Object::create("Images/Retry.png", playScene, 780, 80);
    sampleImage = Object::create("Images/Image1.png", playScene, 775, 450);
    afterTitle = Object::create("Images/Title.png", afterScene, 40, 500);
    clear = Object::create("Images/Clear.png", afterScene, 200, 400);

    timer = Timer::create(1.f);
    timerShow = Timer::create(0.f);
}

// Board 클래스의 멤버들을 초기화하고 playScene에 퍼즐조각을 생성합니다
void CreatePieces() {
    for (int i = 0; i < MAX_INDEX; i++) {
        int x = (i % 5) * INTERVAL;
        int y = 576 - (i / 5) * INTERVAL;
        board[i].InitBoard(x, y, i);
        piece[i].InitPiece(playScene, x, y, i);
    }
}

// 몇번째 퍼즐조각인지 찾고 그 값을 반환합니다
int FindIndex(int index) {
    for (int i = 0; i < MAX_INDEX; i++) {
        if (piece[i].getIndex() == index) return i;  
    }
    return -1; //오류
}

// 퍼즐조각을 서로 바꿉니다
void ChangePieces(int numA, int numB) {
    int a = MAX_INDEX - 1;
    int b = FindIndex(numB);
    int tempIndex = piece[a].getIndex();

    piece[a].getPiece()->locate(playScene, board[numB].getX(), board[numB].getY());  // 바꾸는 조각의 위치로 locate
    piece[b].getPiece()->locate(playScene, board[numA].getX(), board[numA].getY());  // 바꾸는 조각의 위치로 locate

    piece[a].setIndex(piece[b].getIndex());
    piece[b].setIndex(tempIndex);
}   

// 빈칸이 지정된 경로로 이동하며, 만난 퍼즐조각들을 바꿉니다
void MoveToRoute(int st, int nd) {
    int xInterval = (st % 5 - nd % 5 > 0 ? st % 5 - nd % 5 : nd % 5 - st % 5);
    int yInterval = (st / 5 - nd / 5 > 0 ? st / 5 - nd / 5 : nd / 5 - st / 5);
    int start = st;
    int end = nd;

    for (int i = 0; i < xInterval; i++) {
        int direction = (start % 5 - nd % 5 > 0 ? -1 : 1);  // 빈칸이 X축으로 움직일 방향 설정
        end = start + direction;
        ChangePieces(start, end);
        start = end;
    }

    for (int i = 0; i < yInterval; i++) {
        int direction = (st / 5 - nd / 5 > 0 ? -1 : 1);  // 빈칸이 Y축으로 움직일 방향 설정
        end = start + (direction * 5);
        ChangePieces(start, end);
        start = end;
    }
}

/* 랜덤으로 퍼즐을 재배치합니다. 빈칸이 랜덤한 경로를 여러번 이동하며 마지막에는 오른쪽 아래 구석으로 이동합니다. 이 과정에서 퍼즐이 랜덤하게 섞이게 됩니다
*  이 방식으로 퍼즐을 섞었더니 타이머로 애니메이션을 넣기 어려워졌습니다... 
*/
void RandomSetting() {
    int end;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, MAX_INDEX - 2);

    for (int i = 0; i < 15; i++) {
        end = dis(gen);  
        MoveToRoute(blank, end);   // 빈칸 위치에서 랜덤한 위치로 이동
        blank = end;
    }
    MoveToRoute(blank, MAX_INDEX - 1);  // 이동이 끝나면 마지막으로 오른쪽 아래 구석으로 이동
    blank = MAX_INDEX - 1;
}
// 퍼즐이 완성되었는지 검사합니다
bool IsCompleted() {
    for (int i = 0; i < MAX_INDEX; i++) {
        if (piece[i].getIndex() != i) return false;
    }
    return true;
}

int main()                                                                                                                                                                                                                                                                            {
    int time = 0;
    InitGame();

    // play버튼이 눌리면 playScene을 열고 관련 UI를 활성화합니다
    button_play->setOnMouseCallback([&](auto playButton, auto x, auto y, auto action)->bool {
        playButton->hide();
        button_exit->hide();
        clear->hide();   
        playScene->enter();
        showMessage("화살표 방향키로 퍼즐을 이동할 수 있습니다");
        return true;
    });

    // exit버튼이 눌리면 게임을 종료합니다
    button_exit->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        endGame();
        return true;
    });

    // retry버튼이 눌리면 게임을 재시작합니다
    button_retry->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        playScene->enter();
        return true;
    });

    // playScene이 열리면 타이머를 시작하고 퍼즐을 랜덤한 위치로 배치합니다
    playScene->setOnEnterCallback([&](auto scene)->bool {
        CreatePieces();
        time = 0;
        timer->start();
        showTimer(timerShow);
        RandomSetting();    // 이 부분을 주석처리하면 퍼즐을 쉽게 클리어할 수 있습니다.
        return true;
    });
    //화살표 방향키에 따라 빈칸을 이동시킵니다
    playScene->setOnKeyboardCallback([&](auto scene, auto keyCode, auto pressed)->bool {
        if (pressed != false) {
            switch (keyCode) {
                case KeyCode::KEY_LEFT_ARROW:
                    if (blank % 5 != 0) {
                        ChangePieces(blank, (blank - 1));
                        blank -= 1;
                    }
                    break;
                case KeyCode::KEY_RIGHT_ARROW:
                    if (blank % 5 != 4) {
                        ChangePieces(blank, (blank + 1));
                        blank += 1;
                    }
                    break;
                case KeyCode::KEY_UP_ARROW:
                    if (blank / 5 != 0) {
                        ChangePieces(blank, (blank - 5));
                        blank -= 5;
                    }
                    break;
                case KeyCode::KEY_DOWN_ARROW: 
                    if (blank / 5 != 4) {
                        ChangePieces(blank, (blank + 5));
                        blank += 5;
                    }
                    break;
            }
        }

        // 퍼즐을 완성했는지 검사하고 완성했다면 afterScene을 엽니다
        if (IsCompleted()) {
            afterScene->enter();
        }
        return true;
    });

    // 타이머는 1초마다 종료되고 재시작합니다. 타이머가 종료될 때마다 플레이 시간이 1초 누적됩니다. 누적된 시간은 다른 타이머를 통해 게임화면에 나타납니다
    timer->setOnTimerCallback([&](TimerPtr)->bool {
        time += 1;
        timerShow->increase(1.f);
        timer->set(1.f);
        timer->start();
        return true;
    });

    //afterScene이 열리면 타이머를 멈추고 클리어 시간을 보여줍니다 
    afterScene->setOnEnterCallback([&](auto scene)->bool {
        afterTitle->show();
        timer->stop();
        button_retry->locate(afterScene, 270, 240);
        button_exit->locate(afterScene, 270, 180);
        button_exit->show();
        clear->show();
        showMessage("클리어 시간(초): " + to_string(time));
        return true;
    });

    startGame(openingScene);

    return 0;
}