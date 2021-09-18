// Completing Room.cpp / 제작일: 21.9.18(토) / 제작자: 20183722 김현우

#include <bangtal>
#define INTERVAL 50

using namespace bangtal;
using namespace std;

int main()
{
    setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
    setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
    setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

    auto openingScene = Scene::create("메인", "Images/StartBackground.png");
    auto playScene = Scene::create("퍼즐", "Images/PlayBackground.png");
    auto title = Object::create("Images/Title.png", openingScene, 150, 750);
    auto buttton_play = Object::create("Images/Play.png", openingScene, 500, 550);
    auto buttton_exit = Object::create("Images/Exit.png", openingScene, 500, 450);

    ObjectPtr puzzlePieces[25];
    string imageNames[24];

    int xPos[5] = { 0, };
    int yPos[5] = { 250, };

    buttton_play->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        playScene->enter();
        object->hide();
        buttton_exit->hide();
        return true;
        });

    buttton_exit->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        endGame();
        return true;
        });

    for (int i = 1; i < 5; i++) {
        xPos[i] = xPos[i - 1] + INTERVAL;
        yPos[i] = yPos[i - 1] - INTERVAL;
    }

    playScene->setOnEnterCallback([&](auto scene)->bool {
        for (int i = 0; i < 5; i++) {
            for (int k = 0; k < 5; k++) {
                int index = i * 5 + k;
                imageNames[index] = "Images/Image1-" + (i + 1);
                imageNames[index] += ".png";
                puzzlePieces[index] = Object::create(imageNames[index], playScene, xPos[k], yPos[i]);
            }
        }
        return true;
        });

    startGame(openingScene);

    return 0;
}