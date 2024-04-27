#include "raylib.h"
#include "Player.h"
#include "UsefulStuff.h"
#include "MapGenerator.h"
#include "Button.h"
#include "Switch.h"
#include "InputTextWindow.h"

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
// Basicly boost includes windows.h wich has it's own graphical stuff and so names conflict with raylib
// So we cast this and it smh fixes it
// We love casting spells
#if defined(_WIN32)           // raylib uses these names as function parameters
#undef near
#undef far
#endif

using namespace boost::asio;
using boost::system::error_code;
io_service service;
ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 38001); // "192.168.1.8"

// some global vectors
std::deque<Bullet> UltimateBulletVector;
Sound soundBoard[100];

typedef enum GameScreen { StartMenu = 0, Game, Settings, Exit, TestRoom };

/* TODO:
    Fix bullet death    (get rid of memory leaks)
    Implement collision for tank (hard :( )
    Sound of buttons
*/

using namespace player;

// TODO: Kill this shit with fire
size_t read_complete(char* buff, const error_code& err, size_t bytes)
{
    if (err) return 0;
    bool found = std::find(buff, buff + bytes, '\n') < buff + bytes;
    // we read one-by-one until we get to enter, no buffering
    return found ? 0 : 1;
}

void handle_connections(std::string& message_container, bool& newMessage)
{
    ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 38001));
    char buff[1024];
    std::cout << "In work";
    while (true)
    {
        ip::tcp::socket sock(service);
        acceptor.accept(sock);
        int bytes = read(sock, buffer(buff), boost::bind(read_complete, buff, _1, _2));
        message_container = std::string(buff, bytes);
        newMessage = true;
        sock.close();
    }
}

void send_message(std::string msg)
{
    msg += "\n";
    ip::tcp::socket sock(service);
    sock.connect(ep);
    sock.write_some(buffer(msg));
    sock.close();
}

int main ()
{
	InitWindow(screenWidth - screenWidth / XCellCount, screenHeight - screenHeight/YCellCount, "Tonks de game");
    MapGenerator Map(XCellCount, YCellCount);

	SetTargetFPS(FPS);
    InitAudioDevice();

    Music music = LoadMusicStream("resources/background.mp3");
    music.looping = true;

    PlayMusicStream(music);
    SetMusicVolume(music, 0.1);

    GameScreen CurrentScreen = StartMenu;
    loadAllSounds();

    // Player initialization

    Vector2 RealCenter{ (screenWidth - screenWidth / XCellCount) / 2, (screenHeight - screenHeight / YCellCount) / 2 };

    Player p1(StdPlayerSize, RealCenter + Vector2{(int)0.3*cellWidth, (int)0.3*cellHeight}, StdPlayerVelocity);


    // Buttons
    Button PlayButton{ RealCenter - Vector2{0, 50}, Vector2{150, 80}, "Play" , 30, GRAY };
    Button SettingsButton{ RealCenter + Vector2{0, 50}, Vector2{150, 80}, "Settings", 30, GRAY };
    Button ExitButton{ RealCenter + Vector2{0, 150},  Vector2{100, 60}, "Exit", 30, GRAY };
    Button BackButton{ RealCenter + Vector2{0, 150},  Vector2{100, 60}, "Back", 30, GRAY };
    Switch CameraModeButton{ RealCenter + Vector2{-100, 0},  Vector2{100, 60}, "Fog of war mode", 40};
    InputTextWindow IP{ RealCenter + Vector2{-100, -100},  Vector2{300, 60}, "IP:", 40 };

    // Some flags
    bool ExitFlag = false;
    bool isConnectionThreaded = false;
    int CameraMode = 0;

    // Message buffer for sync
    std::string syncMessages;
    bool newMessage = false;

    // Main game cycle
    while (!(ExitFlag || (WindowShouldClose() && !IsKeyDown(KEY_ESCAPE))))
    {
        UpdateMusicStream(music);
        switch (CurrentScreen) {

            // Start window
        case StartMenu:
            if (PlayButton.IsPressed()) {
                CurrentScreen = Game;
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
            IP.DrawInputTextWindow();
            EndDrawing();
            break;

            // Actual game window
        case Game:

            // Syncronaizing area
            if (CameraMode) { // If Server
                // if we are the server, then will ask to thread connection
                if (!isConnectionThreaded) {
                    isConnectionThreaded = true;
                    std::thread server(handle_connections, std::ref(syncMessages), std::ref(newMessage));
                    server.detach();
                }
                // If new message arrived, then work with it
                if (newMessage) {
                    newMessage = false;
                    p1.Shoot(true);
                }
            }
            else { // If Client
                if (IsKeyPressed(KEY_M)) {
                    send_message("Now in game and with M");
                }
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

    // Should also unload sounds

    CloseAudioDevice();

    CloseWindow();

}