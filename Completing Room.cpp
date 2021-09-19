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

int FindIndex(Puzzle* sidePiece, int loc) {
    for (int i = 0; i < MAX_INDEX - 1; i++) {
        if (sidePiece[i].locate == loc) return i;
    }
    return MAX_INDEX - 1;
}

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

    button_play->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        playScene->enter();
        object->hide();
        button_exit->hide();
        clear->hide();
        return true;
    });

    button_exit->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        endGame();
        return true;
    });

    for (int i = 1; i < 5; i++) {
        xPos[i] = xPos[i - 1] + INTERVAL;
        yPos[i] = yPos[i - 1] - INTERVAL;
    }

    button_retry->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        playScene->enter();
        return true;
        });

    playScene->setOnEnterCallback([&](auto scene)->bool {
        time = 0;
        timer->start();
        for (int i = 0; i < 5; i++) {
            for (int k = 0; k < 5; k++) {
                int index = i * 5 + k;
                puzzle[index].imageName = "Images/Image1-" + to_string(index + 1) + ".png";
                puzzle[index].piece = Object::create(puzzle[index].imageName, playScene, xPos[k], yPos[i]);
                puzzle[index].locate = index;
            }
            curLoc = puzzle[MAX_INDEX - 1].locate;
        }

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
        if (IsCompleted(&puzzle[0])) {
            afterScene->enter();
        }
        return true;
    });

    timer->setOnTimerCallback([&](TimerPtr)->bool {
        time += 1;
        timer->set(1.f);
        timer->start();
        return true;
    });

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