#include "raylib.h"
#include "Player.h"
#include "UsefulStuff.h"
#include "MapGenerator.h"
#include "Button.h"
#include "Switch.h"
#include "InputTextWindow.h"

#include <boost/json.hpp>
#include <boost/lexical_cast.hpp>

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

typedef enum GameScreen { StartMenu = 0, Network, SingleMode, HostMode, ClientMode, Settings, Exit, TestRoom, DeathScreen };

/* TODO:
    Implement collision for tank  --- IN PROGRESS
*/

using namespace player;

// Network shit

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

void connectAsClient(ip::tcp::endpoint& ep, ip::tcp::socket& sock, bool& sucess) {
    for (int _ = 0; _ < 5; _++) {
        try {
            sock.connect(ep);
            sucess = true;
            break;
        }
        catch (boost::system::system_error& err) {
            std::cout << "Couldn't connect to server " << err.what() << std::endl;
        }
    }
}

void readMessage(ip::tcp::socket& sock, std::string& message_container, bool& isMessageNew)
{
    char buff[1024];
    if (sock.available()) {
        int bytes = read(sock, buffer(buff), boost::bind(read_complete, buff, _1, _2));
        isMessageNew = true;
        message_container = std::string(buff, bytes);
    }
}

void writeMessage(std::string msg, ip::tcp::socket& sock)
{
    msg += "\n";
    sock.write_some(buffer(msg));
}

double parseMessage(std::string& msg) {
    double res = boost::lexical_cast<double>(msg.substr(0, msg.find(";")));
    msg = msg.substr(msg.find(";") + 1, msg.size()); // overflow size so it cuts till the end
    return res;
}

// End of network shit

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

    //Player p1(StdPlayerSize, RealCenter + Vector2{ (int)(XCellCount * 0.3 + 0.3) * cellWidth, (int)(-YCellCount * 0.3 + 0.3) * cellHeight }, StdPlayerVelocity);
    Player p1(StdPlayerSize, RealCenter + Vector2{ (int)(XCellCount * 0.3 + 0.3) * cellWidth, (int)(-YCellCount * 0.3 + 0.3) * cellHeight }, StdPlayerVelocity, 1);
    Player p2(StdPlayerSize, RealCenter + Vector2{ (int)(-XCellCount * 0.3 - 0.7) * cellWidth, (int)(YCellCount * 0.3 + 0.3) * cellHeight }, StdPlayerVelocity, 2);

    // Buttons
    // Start menu
    Button PlayButton{ RealCenter - Vector2{0, 50}, Vector2{150, 80}, "Play" , 30, GRAY };
    Button SettingsButton{ RealCenter + Vector2{0, 50}, Vector2{150, 80}, "Settings", 30, GRAY };
    Button ExitButton{ RealCenter + Vector2{0, 150},  Vector2{100, 60}, "Exit", 30, GRAY };

    // Settings menu
    Button BackButton{ RealCenter + Vector2{0, 150},  Vector2{100, 60}, "Back", 30, GRAY };
    Switch CameraModeButton{ RealCenter + Vector2{-100, 0},  Vector2{100, 60}, "Fog of war mode", 40 };

    // Network menu
    Button Single{ RealCenter - Vector2{0, 175},  Vector2{100, 60}, "Single", 30, GRAY };
    Button HostButton{ RealCenter - Vector2{0, 100},  Vector2{100, 60}, "Host", 30, GRAY };
    Button ConnectButton{ RealCenter - Vector2{0, 25},  Vector2{200, 60}, "Connect", 30, GRAY };
    InputTextWindow IP{ RealCenter + Vector2{0, 50},  Vector2{350, 60}, "IP and port: ", 30 };

    // Death Screen Button
    Switch RematchButton{ RealCenter + Vector2{-100, 180},  Vector2{100, 60}, "Rematch", 40 };
    Button MenuButton{ RealCenter + Vector2{200, 180},  Vector2{200, 60}, "Menu", 30, GRAY };

    // Some flags
    bool ExitFlag = false;
    int CameraMode = 0;
    bool Inputs[4];
    int whoWon = 0;

    // Stuff for networks
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 38001); // "192.168.1.8"
    ip::tcp::socket sock(service);
    std::string message; // Here would lie the last message
    std::string toSend;
    bool isMessageNew = false;
    bool isHost = false;

    std::string ip;
    int port;
    bool gotMap = false;
    bool isConnected = false;

    // For client drawing
    double x1 = 0, y1 = 0, angle1 = 0, x2 = 0, y2 = 0, angle2 = 0;
    int numberOfBullets = 0, oldNumberOfBullets = 0; // deeply important
    int sense = 0;
    std::vector<Vector2> bullets;

    // For DeathScreen restarts
    bool otherReady = false, weReady = false;

    // Main game cycle
    while (!(ExitFlag || (WindowShouldClose() && !IsKeyDown(KEY_ESCAPE))))
    {
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
            // DrawCircle((screenWidth - screenWidth / XCellCount) / 2, (screenHeight - screenHeight / YCellCount) / 2, 4, PINK);

            EndDrawing();
            break;

        case Network:

            if (BackButton.IsPressed()) {
                CurrentScreen = StartMenu;
            }
            else if (Single.IsPressed()) {
                CurrentScreen = SingleMode;
                PlayMusicStream(music);
                SetMusicVolume(music, 0.0);
            }
            else if (HostButton.IsPressed()) {

                isHost = true;
                waitForConnectionAsServer(sock);
                isConnected = true;
                Map.regenerateMap();
                writeMessage("0;" + Map.toString(), sock);

                // Start playing music
                PlayMusicStream(music);
                SetMusicVolume(music, 0.0);
                CurrentScreen = HostMode;

            }
            else if (ConnectButton.IsPressed()) {

                isHost = false;
                ip = IP.GetIp();
                port = IP.GetPort();
                ip::tcp::endpoint ep(ip::address::from_string(ip), port);

                connectAsClient(ep, sock, isConnected);

                if (isConnected) {
                    PlayMusicStream(music);
                    SetMusicVolume(music, 0.0);
                    CurrentScreen = ClientMode;
                }
                else {
                    std::cout << "Timed out";
                }
            }
            IP.UpdateState();
            IP.UpdateText();

            //std::cout << "IP: " << IP.GetIp() << " Port: " << IP.GetPort() << std::endl;


            BeginDrawing();
            ClearBackground(RAYWHITE);
            Single.DrawButton();
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
        case HostMode:
            readMessage(sock, message, isMessageNew);

            // Syncronaizing area
            for (bool& inp : Inputs)  inp = false; // Nullify inputs

            // Handle input messages
            if (isMessageNew) {
                isMessageNew = false;
                if (message.find("f") != message.npos) {
                    p2.Shoot(true);
                }
                if (message.find("w") != message.npos) {
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
                p2.MovePlayer(Inputs, Map.getNeighbourhoodRect(p2.PlayerPosition));
            }


            p1.MovePlayer(Map.getNeighbourhoodRect(p1.PlayerPosition));
            p1.Shoot();
            p2.Shoot(false);

            // is Player Dead
            if (!p1.GetIsAlive() || !p2.GetIsAlive()) {
                whoWon = p1.GetIsAlive() ? 1 : 2;

                p1 = Player(StdPlayerSize, RealCenter + Vector2{ (int)(XCellCount * 0.3 + 0.3) * cellWidth, (int)(-YCellCount * 0.3 + 0.3) * cellHeight }, StdPlayerVelocity, 1);
                p2 = Player(StdPlayerSize, RealCenter + Vector2{ (int)(-XCellCount * 0.3 - 0.7) * cellWidth, (int)(YCellCount * 0.3 + 0.3) * cellHeight }, StdPlayerVelocity, 2);
                UltimateBulletVector.clear();

                CurrentScreen = DeathScreen;
                PlaySound(soundBoard[SoundPLayerDeath]);

                toSend = "2;";
                toSend += boost::lexical_cast<std::string>(whoWon) + ";";
                writeMessage(toSend, sock);
                break;
            }

            // Moving for bullets
            for (int i = 0; i < UltimateBulletVector.size(); i++) {
                UltimateBulletVector[i].MoveBullet();
            }

            // Collision player with bullets
            for (int i = 0; i < UltimateBulletVector.size(); i++) {
                p1.CollideBullet(UltimateBulletVector[i]);
                p2.CollideBullet(UltimateBulletVector[i]);
            }


            // Collsion with walls for bullets
            for (int i = 0; i < UltimateBulletVector.size(); i++) {
                for (Rectangle rect : Map.getNeighbourhoodRect(UltimateBulletVector[i].Position)) {
                    UltimateBulletVector[i].Collide(rect);
                }
            }

            // Here begins drawing
            BeginDrawing();
            ClearBackground(RAYWHITE);


            Map.Draw();

            p1.DrawPlayer();
            p2.DrawPlayer();


            // Bullet drawer
            for (int i = 0; i < UltimateBulletVector.size(); i++) {
                UltimateBulletVector[i].DrawBullet();
            }
            EndDrawing();


            // Send the whole state_of_the_game to client
            toSend = "1;";
            toSend += p1.toString();
            toSend += p2.toString();
            toSend += boost::lexical_cast<std::string>(UltimateBulletVector.size()) + ";";
            for (Bullet& b : UltimateBulletVector) toSend += b.toString();
            writeMessage(toSend, sock);
            break;

        case ClientMode:

            readMessage(sock, message, isMessageNew);
            if (isMessageNew) {
                isMessageNew = false;
                int sense = (int)parseMessage(message);
                switch (sense) {
                case 0: // they send us new map
                    Map.setMapFromString(message);
                    break;
                case 1: // all positions

                    // let's recollect all data:
                    x1 = parseMessage(message);
                    y1 = parseMessage(message);
                    angle1 = parseMessage(message);
                    x2 = parseMessage(message);
                    y2 = parseMessage(message);
                    angle2 = parseMessage(message);

                    oldNumberOfBullets = numberOfBullets;
                    numberOfBullets = parseMessage(message);
                    if (oldNumberOfBullets > numberOfBullets) {
                        PlaySound(soundBoard[SoundBulletBurst]);
                    }
                    else if (oldNumberOfBullets < numberOfBullets) {
                        PlaySound(soundBoard[SoundPlayerShoot]);
                    }

                    bullets.clear();
                    for (int i = 0; i < numberOfBullets; i++) {
                        bullets.push_back(Vector2{ (float)parseMessage(message) , (float)parseMessage(message) });
                    }
                    break;

                case 2: // Someone died
                    PlaySound(soundBoard[SoundPLayerDeath]);
                    whoWon = parseMessage(message);
                    CurrentScreen = DeathScreen;
                    break;
                }
            }
            BeginDrawing();
            ClearBackground(RAYWHITE);
            Map.Draw();

            DrawPlayerClient(x1, y1, angle1, 1);
            DrawPlayerClient(x2, y2, angle2, 2);

            for (Vector2& shot : bullets) {
                DrawCircleV(shot, StdBulletRadius, BLACK);
            }
            EndDrawing();

            // send inputs
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
            if (IsKeyPressed(KEY_SPACE)) {
                toSend += "f";
            }
            writeMessage(toSend, sock);
            break;

        case SingleMode:

            if (IsKeyPressed(KEY_ESCAPE)) {
                p1 = Player(StdPlayerSize, RealCenter + Vector2{ (int)(XCellCount * 0.3 + 0.3) * cellWidth, (int)(-YCellCount * 0.3 + 0.3) * cellHeight }, StdPlayerVelocity, 1);
                UltimateBulletVector.clear();
                CurrentScreen = StartMenu;
                break;
            }

            if (!p1.GetIsAlive()) {
                p1 = Player(StdPlayerSize, RealCenter + Vector2{ (int)(XCellCount * 0.3 + 0.3) * cellWidth, (int)(-YCellCount * 0.3 + 0.3) * cellHeight }, StdPlayerVelocity, 1);
                UltimateBulletVector.clear();
                PlaySound(soundBoard[SoundPLayerDeath]);
            }

            // Player moving
            p1.MovePlayer(Map.getNeighbourhoodRect(p1.PlayerPosition));

            // Shooting
            p1.Shoot();

            // Moving for bullets
            for (int i = 0; i < UltimateBulletVector.size(); i++) {
                UltimateBulletVector[i].MoveBullet();
            }

            // Collision with walls for player (old)
            /*
            for (Rectangle wall : Map.getNeighbourhoodRect(p1.PlayerPosition)) {
                if(p1.CheckCollisionWall(wall)) std::cout << "COLLISION" << std::endl;
                p1.CollideWall(wall);
                std::cout << "check" << std::endl;
            }
            */

            // Collision player with bullets
            for (int i = 0; i < UltimateBulletVector.size(); i++) {
                p1.CollideBullet(UltimateBulletVector[i]);
            }

            // Collsion with walls for bullets
            for (int i = 0; i < UltimateBulletVector.size(); i++) {
                for (Rectangle rect : Map.getNeighbourhoodRect(UltimateBulletVector[i].Position)) {
                    UltimateBulletVector[i].Collide(rect);
                }
            }



            // Player respawn (for debug purposes)
            if (IsKeyPressed(KEY_R)) {
                p1 = Player(StdPlayerSize, RealCenter + Vector2{ (int)(XCellCount * 0.3 + 0.3) * cellWidth, (int)(-YCellCount * 0.3 + 0.3) * cellHeight }, StdPlayerVelocity, 1);
            }


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

                Map.Draw();
            }

            p1.DrawPlayer();
            //DrawPlayerClient(p1.PlayerPosition.x, p1.PlayerPosition.y, p1.PlayerAngle, 1);

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

        case DeathScreen:
            readMessage(sock, message, isMessageNew);

            // Handle incoming message
            if (isMessageNew) {
                isMessageNew = false;
                if (message.find("r") != message.npos) {
                    otherReady = true;
                }
                if (message.find("b") != message.npos) {
                    CurrentScreen = StartMenu;
                    sock.close();
                }
            }

            // Handle interface
            if (MenuButton.IsPressed()) {
                // end connection
                toSend = "3;b";
                writeMessage(toSend, sock);
                CurrentScreen = StartMenu;
                sock.close();

                // reverting all flags
                isConnected = false;
                isHost = false;
                whoWon = 0;
                gotMap = false;
                numberOfBullets = 0;
                oldNumberOfBullets = 0;
                weReady = false;
                otherReady = false;
                if(RematchButton.GetState()) RematchButton.ChangeState();

                break;
            }
            if (RematchButton.IsPressed()) {
                toSend = "3;r";
                writeMessage(toSend, sock);
            }

            if (!weReady) {
                RematchButton.UpdateSwitch();
                weReady = RematchButton.GetState();
            }

            if (weReady && otherReady) {
                if (isHost) {
                    Map.regenerateMap();
                    writeMessage("0;" + Map.toString(), sock);
                }
                CurrentScreen = isHost ? HostMode : ClientMode;

                // revert some flags
                weReady = false;
                otherReady = false;
                whoWon = 0;
                gotMap = false;
                numberOfBullets = 0;
                oldNumberOfBullets = 0;
                RematchButton.ChangeState();

                break;
            }

            // Draw
            BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("GAME OVER", RealCenter.x - MeasureText("GAME OVER", 70) / 2, RealCenter.y - 70 / 2 - 100, 70, BLACK);
            if (whoWon == 1) {
                DrawText("RED", RealCenter.x - MeasureText("RED", 70) / 2, RealCenter.y - 70 / 2, 70, RED);
            }
            else {
                DrawText("BLUE", RealCenter.x - MeasureText("BLUE", 70) / 2, RealCenter.y - 70 / 2, 70, BLUE);
            }
            DrawText("WON", RealCenter.x - MeasureText("WON", 70) / 2, RealCenter.y - 70 / 2 + 100, 70, BLACK);

            RematchButton.DrawSwitch();
            MenuButton.DrawButton();
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
