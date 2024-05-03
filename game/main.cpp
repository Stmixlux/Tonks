#include "raylib.h"
#include "Player.h"
#include "UsefulStuff.h"
#include "MapGenerator.h"
#include "Button.h"
#include "Switch.h"
#include "InputTextWindow.h"

#include <boost/json.hpp>

#if defined(_WIN32)           
#define NOGDI             // All GDI defines and routines
#define NOUSER            // All USER defines and routines
#endif

// for networking
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
// Basically boost includes windows.h which has it's own graphical stuff and so names conflict with raylib
// So we cast this and it smh fixes it
// We love casting spells
#if defined(_WIN32)           // raylib uses these names as function parameters
#undef near
#undef far
#endif

using namespace boost::asio;
using boost::system::error_code;
io_service service;

// some global vectors
std::deque<Bullet> UltimateBulletVector;
Sound soundBoard[100];

typedef enum GameScreen { StartMenu = 0, Network, Game, Settings, Exit, TestRoom };

/* TODO:
    Implement collision for tank (hard :( )
*/

using namespace player;

// ToDo: a JSON messages transfer, perhaps?
size_t read_complete(char* buff, const error_code& err, size_t bytes)
{
    if (err) return 0;
    bool found = std::find(buff, buff + bytes, '\n') < buff + bytes;
    // we read one-by-one until we get to enter, no buffering
    return found ? 0 : 1;
}

void waitForConnectionAsServer(ip::tcp::socket& sock) {
    ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 38001));
    char buff[1024];
    std::cout << "Waiting for client\n";

    acceptor.accept(sock);
    
    std::cout << "Connection secured\n";
}

void connectAsClient(ip::tcp::endpoint& ep, ip::tcp::socket& sock) {
    try {
        sock.connect(ep);
    }
    catch (boost::system::system_error& err) {
        std::cout << "Something went wrong: " << err.what() << std::endl;
    }
}

void readMessage(ip::tcp::socket& sock, std::string& message_container, bool& isMessageNew)
{
    char buff[1024];
    if (sock.available()) {
        int bytes = read(sock, buffer(buff), boost::bind(read_complete, buff, _1, _2));
        isMessageNew = true;
        message_container = std::string(buff, bytes);

        std::cout << "Recieved message:" << message_container << std::endl;
    }
}

void writeMessage(std::string msg, ip::tcp::socket& sock)
{
    std::cout << "Sent Message:" << msg << std::endl;
    msg += "\n";
    sock.write_some(buffer(msg));
}


int main()
{
    InitWindow(screenWidth - screenWidth / XCellCount, screenHeight - screenHeight / YCellCount, "Tonks de game");
    MapGenerator Map(XCellCount, YCellCount);

    SetTargetFPS(FPS);
    InitAudioDevice();

    Music music = LoadMusicStream("resources/background.mp3");
    music.looping = true;

    GameScreen CurrentScreen = StartMenu;
    loadAllSounds();

    // Player initialization

    Vector2 RealCenter{ (screenWidth - screenWidth / XCellCount) / 2, (screenHeight - screenHeight / YCellCount) / 2 };

    Player p1(StdPlayerSize, RealCenter + Vector2{ (int)(XCellCount * 0.3 + 0.3) * cellWidth, (int)(-YCellCount * 0.3 + 0.3) * cellHeight }, StdPlayerVelocity);


    // Buttons
    // Start menu
    Button PlayButton{ RealCenter - Vector2{0, 50}, Vector2{150, 80}, "Play" , 30, GRAY };
    Button SettingsButton{ RealCenter + Vector2{0, 50}, Vector2{150, 80}, "Settings", 30, GRAY };
    Button ExitButton{ RealCenter + Vector2{0, 150},  Vector2{100, 60}, "Exit", 30, GRAY };

    // Settings menu
    Button BackButton{ RealCenter + Vector2{0, 150},  Vector2{100, 60}, "Back", 30, GRAY };
    Switch CameraModeButton{ RealCenter + Vector2{-100, 0},  Vector2{100, 60}, "Fog of war mode", 40 };

    // Network menu
    Button HostButton{ RealCenter - Vector2{0, 100},  Vector2{100, 60}, "Host", 30, GRAY };
    Button ConnectButton{ RealCenter - Vector2{0, 25},  Vector2{200, 60}, "Connect", 30, GRAY };
    InputTextWindow IP{ RealCenter + Vector2{0, 50},  Vector2{350, 60}, "IP and port: ", 30 };

    // Some flags
    bool ExitFlag = false;
    int CameraMode = 0;
    bool Inputs[4];

    // Sruff for networks
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 38001); // "192.168.1.8"
    ip::tcp::socket sock(service);
    std::string message; // Here would lie the last message
    std::string toSend;
    bool isMessageNew = false;
    bool isHost = true;

    std::string ip;
    int port;
    bool created_ep = false;
    bool got_ip = false;


    // Main game cycle
    while (!(ExitFlag || (WindowShouldClose() && !IsKeyDown(KEY_ESCAPE))))
    {
        isHost = CameraMode; // Easier to debug
        UpdateMusicStream(music);
        switch (CurrentScreen) {

            // Start window
        case StartMenu:
            if (PlayButton.IsPressed()) {
                CurrentScreen = Network;
            }

            else if (ExitButton.IsPressed()) {
                CurrentScreen = Exit;
            }

            else if (SettingsButton.IsPressed()) {
                CurrentScreen = Settings;
            }

            else if (IsKeyDown(KEY_T)) {
                CurrentScreen = TestRoom;
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Tonks the Game", RealCenter.x - MeasureText("Tonks the Game", 50) / 2, RealCenter.y - 50 / 2 - 200, 50, BLACK);
            PlayButton.DrawButton();
            ExitButton.DrawButton();
            SettingsButton.DrawButton();

            // Calibrating pink circle in the center
            //DrawCircle((screenWidth - screenWidth / XCellCount) / 2, (screenHeight - screenHeight / YCellCount) / 2, 4, PINK);

            EndDrawing();
            break;

        case Network:

            if (BackButton.IsPressed()) {
                CurrentScreen = StartMenu;
            }
            else if (HostButton.IsPressed()) {

                if (isHost) {
                    waitForConnectionAsServer(sock);
                }
                else { // If client try getting connection with host
                    connectAsClient(ep, sock);
                }

                // Start playing music
                PlayMusicStream(music);
                SetMusicVolume(music, 0.0);
                CurrentScreen = Game;
            }
            else if (ConnectButton.IsPressed()) {
                isHost = false;

                ip = IP.GetIp();
                port = IP.GetPort();
                got_ip = true;
            }
            if (!created_ep && got_ip) {
                ip::tcp::endpoint ep(ip::address::from_string(ip), port);
                created_ep = true;
            }
            IP.UpdateState();
            IP.UpdateText();

            //std::cout << "IP: " << IP.GetIp() << " Port: " << IP.GetPort() << std::endl;


            BeginDrawing();
            ClearBackground(RAYWHITE);
            HostButton.DrawButton();
            ConnectButton.DrawButton();
            BackButton.DrawButton();
            IP.DrawInputTextWindow();
            EndDrawing();
            break;

        case Settings:

            if (BackButton.IsPressed()) {
                CurrentScreen = StartMenu;
            }
            CameraModeButton.UpdateSwitch();
            CameraMode = CameraModeButton.GetState();

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Placeholder page for settings", RealCenter.x - MeasureText("Placeholder page for settings", 30) / 2, RealCenter.y - 30 / 2 - 200, 30, BLACK);
            CameraModeButton.DrawSwitch();
            BackButton.DrawButton();

            EndDrawing();
            break;

            // Actual game window
        case Game:

            // Syncronaizing area
            if (isHost) { // If Server
                for (bool& inp : Inputs)  inp = false; // Nullify inputs
                // If new message arrived, then work with it
                readMessage(sock, message, isMessageNew);

                if (isMessageNew) {
                    isMessageNew = false;
                    if (message.find("f") != message.npos) {
                        p1.Shoot(true);
                    }
                    if (message.find("w") != message.npos) {
                        std::cout << "I count that as a win";
                        Inputs[0] = true;
                    }
                    if (message.find("s") != message.npos) {
                        Inputs[1] = true;
                    }
                    if (message.find("d") != message.npos) {
                        Inputs[2] = true;
                    }
                    if (message.find("a") != message.npos) {
                        Inputs[3] = true;
                    }
                    p1.MovePlayer(Inputs);
                }
            }

            else { // If Client
                toSend = "";
                if (IsKeyDown(KEY_UP)) {
                    toSend += "w";
                }
                if (IsKeyDown(KEY_DOWN)) {
                    toSend += "s";
                }
                if (IsKeyDown(KEY_RIGHT)) {
                    toSend += "d";
                }
                if (IsKeyDown(KEY_LEFT)) {
                    toSend += "a";
                }
                if (IsKeyDown(KEY_SPACE)) {
                    toSend += "f";
                }
                writeMessage(toSend, sock);
            }

            // Player moving
            p1.MovePlayer();

            // Collision with walls for player
            for (Rectangle wall : Map.getNeighbourhoodRect(p1.PlayerPosition)) {
                p1.CollideWall(wall);
            }


            // Collsion with walls for bullets
            for (int i = 0; i < UltimateBulletVector.size(); i++) {
                for (Rectangle rect : Map.getNeighbourhoodRect(UltimateBulletVector[i].Position)) {
                    UltimateBulletVector[i].Collide(rect);
                }
            }

            // Moving for bullets
            for (int i = 0; i < UltimateBulletVector.size(); i++) {
                UltimateBulletVector[i].MoveBullet();
            }

            // Shooting
            p1.Shoot();


            // Here begins drawing
            BeginDrawing();
            ClearBackground(RAYWHITE);

            // Map drawing Standard mode
            if (CameraMode == 0) {
                Map.Draw();
            }
            else if (CameraMode == 1) {
                for (Rectangle r : Map.getNeighbourhoodRect(p1.PlayerPosition)) {
                    DrawRectangleRec(r, BLACK);
                }
            }

            p1.DrawPlayer();

            // Calibrating pink circle in the center
            DrawCircle((screenWidth - screenWidth / XCellCount) / 2, (screenHeight - screenHeight / YCellCount) / 2, 4, PINK);

            // Map regenerator
            if (IsKeyDown(KEY_G)) {
                Map.regenerateMap();
            }

            // Bullet drawer
            for (int i = 0; i < UltimateBulletVector.size(); i++) {
                UltimateBulletVector[i].DrawBullet();
            }
            EndDrawing();
            break;

            // Case for exit
        case Exit:
            ExitFlag = true;
            break;


        case TestRoom:
            CurrentScreen = Settings;
            break;
            // For now using CameraMode to diffirintiate server/client
            /*
            if (CameraMode) {
                handle_connections(syncMessages);
            }
            else {
                std::cout << "May start";
                sync_echo("Staying Alive");
            }*/
        }
    }

    UnloadMusicStream(music);   // Unload music stream buffers from RAM

    unloadAllSounds();

    CloseAudioDevice();

    CloseWindow();

}