// Completing Room.cpp / 제작일: 21.9.18(토) / 제작자: 20183722 김현우

#include <bangtal>
#include <string>
#include <random>

#define INTERVAL 144
#define MAX_INDEX 25

using namespace bangtal;
using namespace std;

typedef struct Puzzzle {
    int locate;
    string imageName;
    ObjectPtr piece;
} Puzzle;

// 퍼즐조각의 위치와 이미지를 서로 바꿉니다
void ChangePieces(Puzzle* pieceA, Puzzle* pieceB) {
    int tempLoc;
    string temp;

    temp = pieceA->imageName;
    pieceA->imageName = pieceB->imageName;
    pieceB->imageName = temp;

    pieceA->piece->setImage(pieceA->imageName);
    pieceB->piece->setImage(pieceB->imageName);

    tempLoc = pieceA->locate;
    pieceA->locate = pieceB->locate;
    pieceB->locate = tempLoc;
}

// 퍼즐조각의 원래 위치를 찾아 반환합니다
int FindIndex(Puzzle* sidePiece, int loc) {
    for (int i = 0; i < MAX_INDEX - 1; i++) {
        if (sidePiece[i].locate == loc) return i;
    }
    return MAX_INDEX - 1;
}

// 퍼즐이 완성되었는지 검사합니다
bool IsCompleted(Puzzle* piece) {
    for (int i = 0; i < MAX_INDEX; i++) {
        if (piece[i].locate != i) return false;
    }
    return true;
}

int main()
{
    setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
    setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
    setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

    auto openingScene = Scene::create("", "Images/StartBackground.png");
    auto playScene = Scene::create("", "Images/PlayBackground.png");
    auto afterScene = Scene::create("", "Images/StartBackground.png");

    auto openTitle = Object::create("Images/Title.png", openingScene, 40, 450);
    auto button_play = Object::create("Images/Play.png", openingScene, 280, 320);
    auto button_exit = Object::create("Images/Exit.png", openingScene, 280, 250);

    auto button_retry = Object::create("Images/Retry.png", playScene, 780, 80);
    auto sampleImage = Object::create("Images/Image1.png", playScene, 775, 450);

    auto afterTitle = Object::create("Images/Title.png", afterScene, 40, 500);
    auto clear = Object::create("Images/Clear.png", afterScene, 200, 400);

    auto timer = Timer::create(1.f);

    int xPos[5] = { 0, };
    int yPos[5] = { 576, };
    int curLoc;
    int time = 0;

    Puzzle puzzle[MAX_INDEX];

    // play버튼이 눌리면 playScene을 열고 관련 UI를 활성화합니다
    button_play->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        playScene->enter();
        object->hide();
        button_exit->hide();
        clear->hide();
        return true;
    });

    // exit버튼이 눌리면 게임을 종료합니다
    button_exit->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        endGame();
        return true;
    });

    // 게임 시작 시 퍼즐조각들이 처음에 자리잡을 위치를 지정합니다
    for (int i = 1; i < 5; i++) {
        xPos[i] = xPos[i - 1] + INTERVAL;
        yPos[i] = yPos[i - 1] - INTERVAL;
    }

    // retry버튼이 눌리면 게임을 재시작합니다
    button_retry->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        playScene->enter();
        return true;
        });

    // playScene이 열리면 타이머를 시작하고 퍼즐을 지정된 위치로 배치합니다
    playScene->setOnEnterCallback([&](auto scene)->bool {
        time = 0;
        timer->start();
        showMessage("화살표 방향기로 퍼즐을 이동할 수 있습니다");
        for (int i = 0; i < 5; i++) {
            for (int k = 0; k < 5; k++) {
                int index = i * 5 + k;
                puzzle[index].imageName = "Images/Image1-" + to_string(index + 1) + ".png";
                puzzle[index].piece = Object::create(puzzle[index].imageName, playScene, xPos[k], yPos[i]);
                puzzle[index].locate = index;
            }
            curLoc = puzzle[MAX_INDEX - 1].locate;
        }

        // 랜덤으로 퍼즐을 재배치합니다
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(0, MAX_INDEX - 2);

        for (int i = 0; i < MAX_INDEX - 1; i++) {
            int num = dis(gen);
            int idx1 = FindIndex(&puzzle[0], i);
            int idx2 = FindIndex(&puzzle[0], num);
            ChangePieces(&puzzle[idx1], &puzzle[idx2]);
        }
        return true;
        });

    //화살표 방향키에 따라 퍼즐을 이동시킵니다
    playScene->setOnKeyboardCallback([&](auto scene, auto keyCode, auto pressed)->bool {
        if (pressed != false) {
            switch (keyCode) {
                case KeyCode::KEY_LEFT_ARROW:
                    if (curLoc % 5 != 4) {
                        ChangePieces(&puzzle[curLoc], &puzzle[curLoc + 1]);
                        curLoc += 1;
                    }
                    break;
                case KeyCode::KEY_RIGHT_ARROW:
                    if (curLoc % 5 != 0) {
                        ChangePieces(&puzzle[curLoc], &puzzle[curLoc - 1]);
                        curLoc -= 1;
                    }
                    break;
                case KeyCode::KEY_UP_ARROW:
                    if (curLoc / 5 != 4) {
                        ChangePieces(&puzzle[curLoc], &puzzle[curLoc + 5]);
                        curLoc += 5;
                    }
                    break;
                case KeyCode::KEY_DOWN_ARROW: 
                    if (curLoc / 5 != 0) {                   
                        ChangePieces(&puzzle[curLoc], &puzzle[curLoc - 5]);
                        curLoc -= 5;
                    }
                    break;
            }
        }

        // 퍼즐을 클리어했는지 검사하고 클리했다면 afterScene을 엽니다
        if (IsCompleted(&puzzle[0])) {
            afterScene->enter();
        }
        return true;
    });

    // 타이머는 1초마다 종료되고 재시작합니다. 타이머가 종료될 때마다 플레이 시간이 1초 누적됩니다
    timer->setOnTimerCallback([&](TimerPtr)->bool {
        time += 1;
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