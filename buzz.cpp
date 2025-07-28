#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace sf;

// Initializing Dimensions.
const int resolutionX = 960;
const int resolutionY = 640;
const int boxPixelsX = 32;
const int boxPixelsY = 32;
const int gameRows = resolutionX / boxPixelsX; // Total rows on grid
const int gameColumns = resolutionY / boxPixelsY; // Total columns on grid

// Initializing GameGrid.
int gameGrid[gameRows][gameColumns] = {};

// Function Declarations
// Function to handle spray usage
bool checkCollisionWithFlowers(float player_x, float player_y, float flowerX[], float flowerY[], int flowerCount);
bool checkBeeCollisionWithHoneycomb(float beeX, float beeY, float honeycombX[], float honeycombY[], int& honeycombCount);
bool checkHoneycombCollision(float bullet_x, float bullet_y, float honeycombX, float honeycombY);
bool isBeeStuck(int beeIndex, const int MAX_HONEYCOMBS, float yellowHoneycombX[], float yellowHoneycombY[], int& yellowHoneycombCount,float redHoneycombX[], float redHoneycombY[], int& redHoneycombCount, float beeX[], float beeY[]);
bool checkBeehiveCollision(float beehiveX, float beehiveY,float bullet_y, float bullet_x);
bool checkHoneycombCollisionWithBird(float birdX, float birdY, float yellowHoneycombX[], float yellowHoneycombY[], int& yellowHoneycombCount, float redHoneycombX[], float redHoneycombY[], int& redHoneycombCount,int& score);
bool checkBulletCollisionWithBird(float bullet_x, float bullet_y, float& birdX, float& birdY, int& birdHealth, bool& birdSick, Clock& sickClock);
bool checkCollision(float bullet_x, float bullet_y, float beeX, float beeY);
void UseBullet(float& bullet_x, float& bullet_y, bool& bullet_exists, int& currentSpray, int& sprayCans, float player_x, float player_y);
void drawHUD(RenderWindow& window, int currentSpray, int sprayCans, Sprite& sprayCanSprite, Sprite& lifeSprite);
void spawnHummingbird(float& birdX, float& birdY, int& birdHealth, bool& birdSick, int resolutionX, int resolutionY);
void moveHummingbird(float& birdX, float& birdY, int& birdDirection, Clock& birdClock, int resolutionX, int resolutionY);
void respawnHummingbird(bool& birdSick, Clock& sickClock, float& birdX, float& birdY, int& birdHealth, int resolutionX, int resolutionY);
void drawHummingbird(RenderWindow& window, float birdX, float birdY, Sprite& birdSprite);
void drawPlayer(RenderWindow& window, float& player_x, float& player_y, Sprite& playerSprite);
void moveBullet(float& bullet_y, bool& bullet_exists, Clock& bulletClock);
void drawBullet(RenderWindow& window, float& bullet_x, float& bullet_y, Sprite& bulletSprite);
void moveBees(int beeCount, float beeX[], float beeY[], float beeSpeed[], bool beeMoving[], Clock& beeClock, float beeDirection[], float player_y, int beeTypes[], float yellowHoneycombX[], float yellowHoneycombY[], int& yellowHoneycombCount, float redHoneycombX[], float redHoneycombY[], int& redHoneycombCount, bool stopAllBees,float flowerX[], float flowerY[], int& flowerCount,bool& firstBeeCreatedFlowers,const int MAX_BEEHIVES,float beehiveX[],float beehiveY[],int beehiveCount);
void spawnBees(int beeCount, float beeX[], float beeY[], float beeSpeed, bool beeMoving[], float beeDirection[],int beeTypes[],int level);
void drawBees(RenderWindow& window, int beeCount, float beeX[], float beeY[], Sprite& beeSprite,Texture& killerBeeTexture,Texture& beeTexture,int beeTypes[],float beeDirection[]);
void removeBee(int index, int& beeCount, float beeX[], float beeY[], bool beeMoving[], float beeDirection[], float yellowHoneycombX[], float yellowHoneycombY[], int& yellowHoneycombCount, float redHoneycombX[], float redHoneycombY[], int& redHoneycombCount, int beeTypes[]);
void drawHoneycomb(RenderWindow& window, float beeX, float beeY, Texture& honeycombTexture);
void removeHoneycomb(int index, float yellowHoneycombX[], float yellowHoneycombY[], int& yellowHoneycombCount,
float redHoneycombX[], float redHoneycombY[], int& redHoneycombCount);
void showMenu(RenderWindow& window);
void startGame(RenderWindow& window, int level);
void generateHoneycombPositions(int level, int& yellowHoneycombCount, float yellowHoneycombX[], float yellowHoneycombY[], 
int& redHoneycombCount, float redHoneycombX[], float redHoneycombY[],float player_y);
void drawFlowers(sf::RenderWindow& window,float flowerX[], float flowerY[],int flowerCount,Sprite& flowerSprite);
void createFlowers(float beeX, float beeY, float player_y, float flowerX[], float flowerY[], int& flowerCount,const int MAX_FLOWERS,float beeDirection[]);
void movePlayer(float& player_x,float& player_y, sf::Clock& PlayerClock,float flowerX[],float flowerY[],int flowerCount);

int main() {
    srand(time(0));
    RenderWindow window(VideoMode(resolutionX, resolutionY), "Buzz Bombers", Style::Close | Style::Titlebar);
    showMenu(window);
    
}

void UseBullet(float& bullet_x, float& bullet_y, bool& bullet_exists, int& currentSpray, int& sprayCans, float player_x, float player_y) {
    if (Keyboard::isKeyPressed(Keyboard::Space) && !bullet_exists && currentSpray > 0) {
     bullet_exists = true;
     bullet_x = player_x;
     bullet_y = player_y;
     currentSpray--;
        //checking cans
        if (currentSpray == 0 && sprayCans > 0) {
         currentSpray = 56;  // Replacing in case can got empty
         sprayCans--;       // take one can off 
        }
    }
}

void movePlayer(float& player_x,float& player_y, sf::Clock& PlayerClock,float flowerX[],float flowerY[],int flowerCount) {
    if (PlayerClock.getElapsedTime().asMilliseconds() < 60)
       return; // check the movement is not done unless 60 ms pass
    PlayerClock.restart();
    float newPlayerX = player_x; // Storing position
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        if (newPlayerX > 0) {
         newPlayerX -= boxPixelsX; //moving left
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        if (newPlayerX < resolutionX - boxPixelsX) {
         newPlayerX += boxPixelsX; // Moving right
        }
    }
   if (!checkCollisionWithFlowers(newPlayerX, player_y, flowerX, flowerY, flowerCount)) {
    player_x = newPlayerX; // Update player position 
   }
}
void drawPlayer(RenderWindow& window, float& player_x, float& player_y, Sprite& playerSprite) {
    playerSprite.setPosition(player_x, player_y);
    window.draw(playerSprite);
}
void moveBullet(float& bullet_y, bool& bullet_exists, Clock& bulletClock) {
    if (bulletClock.getElapsedTime().asMilliseconds() < 20){ 
     return;
    }

    bulletClock.restart();
    bullet_y -= 10;
    if (bullet_y < -32){ 
      bullet_exists = false;
    }
}
void drawBullet(RenderWindow& window, float& bullet_x, float& bullet_y, Sprite& bulletSprite) {
    bulletSprite.setPosition(bullet_x, bullet_y);
    window.draw(bulletSprite);
}
void drawBees(RenderWindow& window, int beeCount, float beeX[], float beeY[], Sprite& beeSprite, Texture& killerBeeTexture, Texture& beeTexture, int beeTypes[], float beeDirection[]) {
    for (int i = 0; i < beeCount; i++) {
        if (beeTypes[i] == 0) { // normal bee
            beeSprite.setTexture(beeTexture); // setting the picture for normal
        } else if (beeTypes[i] == 1) { // fast bee
            beeSprite.setTexture(killerBeeTexture); // setting the picture for fast
        }
        // Setting position
        beeSprite.setPosition(beeX[i], beeY[i]);
        // Checking the direction of the bee 
        if (beeDirection[i] == 1) { // Move right
         beeSprite.setScale(-1.0f, 1.0f); // flipped
        } else { // Moving left
         beeSprite.setScale(1.0f, 1.0f); // normal
         // Adjusting the position to account for the flipped scale
         beeSprite.setOrigin(beeSprite.getTexture()->getSize().x, 0); // Set origin to the right edge
        }
        window.draw(beeSprite);
    }
}
void moveBees(int beeCount, float beeX[], float beeY[], float beeSpeed[], bool beeMoving[], Clock& beeClock, float beeDirection[],float player_y, int beeTypes[], float yellowHoneycombX[], float yellowHoneycombY[], int& yellowHoneycombCount,float redHoneycombX[], float redHoneycombY[], int& redHoneycombCount, bool stopAllBees, float flowerX[],float flowerY[], int& flowerCount, bool& firstBeeCreatedFlowers, const int MAX_BEEHIVES, 
float beehiveX[], float beehiveY[], int beehiveCount) {
    if (beeClock.getElapsedTime().asMilliseconds() < 10)  // Slow down bee movement
        return;

    beeClock.restart();

    // If the stopAllBees flag is true, stop all worker bees
    if (stopAllBees) {
        for (int i = 0; i < beeCount; i++) {
            if (beeTypes[i] == 0) { // Only check for worker bees
                beeSpeed[i] = 0; // Stop the bee
            }
        }
    } else {
        // Randomly stop one worker bee if no bee is currently stopped
        static int stoppedBeeIndex = -1; // Track the currently stopped bee
        static Clock stopClock; // Clock for the stopped bee

        if (stoppedBeeIndex == -1) { // If no bee is currently stopped
            for (int i = 0; i < beeCount; i++) {
                if (beeTypes[i] == 0 && beeSpeed[i] > 0) { // Only check for moving worker bees
                    if (rand() % 100 < 20) { // 20% chance to stop one worker bee
                        stoppedBeeIndex = i; // Stop this bee
                        beeSpeed[stoppedBeeIndex] = 0; // Stop the bee
                        beeMoving[stoppedBeeIndex] = true; // Set moving to true to track the stop time
                        stopClock.restart(); // Start the stop clock
                        break; // Stop after finding one bee to stop
                    }
                }
            }
        } else {
            // Check if 2 seconds have passed for the stopped bee
            if (stopClock.getElapsedTime().asSeconds() >= 2) {
                beeSpeed[stoppedBeeIndex] = 2.0f; // Resume normal speed after 2 seconds
                beeMoving[stoppedBeeIndex] = false; // Reset moving status
                stoppedBeeIndex = -1; // Reset stopped bee index
            }
        }
    }

    for (int i = 0; i < beeCount; i++) {
        // Move the bee horizontally if it's not stopped
        if (beeSpeed[i] > 0) {
            if (beeDirection[i] == 1) {
                beeX[i] += beeSpeed[i];  // Move right
            } else {
                beeX[i] -= beeSpeed[i];  // Move left
            }

            // Collision with borders
            if (beeX[i] >= resolutionX) {
                beeX[i] = 0;  // Wrap to the left side
                beeY[i] += boxPixelsY;  // Drop down by one grid
            } else if (beeX[i] < 0) {
                beeX[i] = resolutionX - boxPixelsX;  // Wrap to the right side
                beeY[i] += boxPixelsY;  // Drop down by one grid
            }

            // Check for collision with honeycombs
            bool collided = false; // Flag to check if a collision occurred
            if (beeTypes[i] == 0) { // Only check for worker bees
                if (checkBeeCollisionWithHoneycomb(beeX[i], beeY[i], yellowHoneycombX, yellowHoneycombY, yellowHoneycombCount) ||
                    checkBeeCollisionWithHoneycomb(beeX[i], beeY[i], redHoneycombX, redHoneycombY, redHoneycombCount)) {
                    collided = true; // Collision detected
                }
            }

            // Handle collision response
            if (collided) {
                // Reverse the direction of the worker bee
                beeDirection[i] *= -1; // Reverse direction

                // Move the bee away from the honeycomb by 15 pixels
                if (beeDirection[i] == 1) {
                    beeX[i] += 15; // Move right by 15 pixels
                } else {
                    beeX[i] -= 15; // Move left by 15 pixels
                }
            }

            // Check if the bee is stuck between honeycombs
            if (isBeeStuck(i, 100, yellowHoneycombX, yellowHoneycombY, yellowHoneycombCount, redHoneycombX, redHoneycombY, redHoneycombCount, beeX, beeY)) {
                // Handle the stuck condition (e.g., reset position or change direction)
                beeDirection[i] *= -1; // Reverse direction if stuck
            }

            createFlowers(beeX[i], beeY[i], player_y, flowerX, flowerY, flowerCount, 100, beeDirection);
            // Reset Y position if it crosses player's y position
            if (beeY[i] >= player_y) {
                // Check if the bee is in a proper position to create flowers
                beeY[i] = 0;
                beeX[i] = rand() % resolutionX;  // Place the bee randomly on the screen
                beeDirection[i] = (rand() % 2 == 0) ? 1 : -1;  // Random direction (left or right)
            }
        }

        if (beeTypes[i] == 0 && (beeX[i] <= 0 || beeX[i] >= resolutionX - boxPixelsX)) {
            // Check if there is space for a new beehive
            if (beehiveCount < MAX_BEEHIVES) {
                beehiveX[beehiveCount] = beeX[i]; // Store x-coordinate
                beehiveY[beehiveCount] = beeY[i]; // Store y-coordinate
                beehiveCount++; // Increment the count
            }
        }
    }
}
void spawnBees(int beeCount, float beeX[], float beeY[], float beeSpeed[], bool beeMoving[], float beeDirection[], int beeTypes[], int level) {
    int killerBeeCount = 0; // Counter for killer bees
    int totalKillerBees = 0; 
    // Determine the number of killer bees based on the level
    if (level == 1) {
        totalKillerBees = 0; 
    } else if (level == 2) {
        totalKillerBees = 5; 
    } else if (level == 3) {
        totalKillerBees = 10; 
    }
    for (int i = 0; i < beeCount; i++) {
        bool overlapping;
        do {
            overlapping = false;
            beeX[i] = rand() % resolutionX; // Random x position
            beeY[i] = rand() % (resolutionY / 2); // Random y position in the upper half

            // Check for overlap with already spawned bees
            for (int j = 0; j < i; j++) {
                if (abs(beeX[i] - beeX[j]) < 32 && abs(beeY[i] - beeY[j]) < 32) { // Assuming bee size is 32x32
                    overlapping = true; // Set flag if overlapping
                    break; // Exit the loop
                }
            }
        } while (overlapping); // Repeat until a non-overlapping position is found
        if(rand() % 2 == 0){
         beeDirection[i]=1;
        }
       else{
         beeDirection[i]=-1;
        } 
        beeMoving[i] = true;  // All bees are moving at start
        // Assign bee type based on level and ensure the correct number of killer bees
        if (killerBeeCount < totalKillerBees) {
         beeTypes[i] = 1;
         beeSpeed[i]=3.0f; 
         killerBeeCount++; 
        } else {
         beeSpeed[i]=2.0f; 
         beeTypes[i] = 0; // Assign worker bee type
        }
    }
}
void removeBee(int index, int& beeCount, float beeX[], float beeY[], bool beeMoving[], float beeDirection[], 
float yellowHoneycombX[], float yellowHoneycombY[], int& yellowHoneycombCount,float redHoneycombX[], float redHoneycombY[], int& redHoneycombCount, int beeTypes[]) {
    // Check the type of bee before removing
    if (beeTypes[index] == 0) { // normal bee
        if (yellowHoneycombCount < 100) { // max honeycomb count is 100
         yellowHoneycombX[yellowHoneycombCount] = beeX[index];
         yellowHoneycombY[yellowHoneycombCount] = beeY[index];
         yellowHoneycombCount++;
        }
    } else if (beeTypes[index] == 1) { // Killer bee
        // Create a red honeycomb at the killer bee position
        if (redHoneycombCount < 100) {
         redHoneycombX[redHoneycombCount] = beeX[index];
         redHoneycombY[redHoneycombCount] = beeY[index];
         redHoneycombCount++;
        }
    }
    // Shift remaining bees
    for (int i = index; i < beeCount - 1; i++) {
     beeX[i] = beeX[i + 1];
     beeY[i] = beeY[i + 1];
     beeMoving[i] = beeMoving[i + 1];
     beeDirection[i] = beeDirection[i + 1];
     beeTypes[i] = beeTypes[i + 1]; // Shift bee types as well
    }
    beeCount--;
}
void drawHoneycomb(RenderWindow& window, float honeycombX, float honeycombY, Texture& honeycombTexture) {
    Sprite honeycombSprite;
    honeycombSprite.setTexture(honeycombTexture);
    honeycombSprite.setPosition(honeycombX, honeycombY);
    window.draw(honeycombSprite);
}
void removeHoneycomb(int index, float yellowHoneycombX[], float yellowHoneycombY[], int& yellowHoneycombCount,float redHoneycombX[], float redHoneycombY[], int& redHoneycombCount) {
    if (index < yellowHoneycombCount) { // Yellow honeycomb
        for (int i = index; i < yellowHoneycombCount - 1; i++) {
         yellowHoneycombX[i] = yellowHoneycombX[i + 1];
         yellowHoneycombY[i] = yellowHoneycombY[i + 1];
        }
        yellowHoneycombCount--;  // Decrease yellow honeycomb count
    } else { // Red honeycomb
        index -= yellowHoneycombCount; // Adjust index for red honeycomb
        for (int i = index; i < redHoneycombCount - 1; i++) {
         redHoneycombX[i] = redHoneycombX[i + 1];
         redHoneycombY[i] = redHoneycombY[i + 1];
        }
        redHoneycombCount--;  // Decrease red honeycomb count
    }
}
void spawnHummingbird(float& birdX, float& birdY, int& birdHealth, bool& birdSick, int resolutionX, int resolutionY) {
    birdX = rand() % resolutionX;
    birdY = rand() % resolutionY / 2;
    birdHealth = 3; 
    birdSick = false;
}

void moveHummingbird(float& birdX, float& birdY, int& birdDirection, Clock& birdClock, int resolutionX, int resolutionY) {
    if (birdClock.getElapsedTime().asMilliseconds() < 500)  // Pause for half a second
        return;

    birdClock.restart();

    //  change direction every few moves
    if (rand() % 5 == 0) {
        birdDirection = rand() % 4 + 1;  // Random direction: 1 (right), 2 (left), 3 (up), 4 (down)
    }
    // Move bird based on direction
    if (birdDirection == 1 && birdX < resolutionX - 32) {
     birdX +=boxPixelsX;  // Move right
    } else if (birdDirection == 2 && birdX > 0) {
     birdX -= boxPixelsX;  // Move left
    } else if (birdDirection == 3 && birdY > 0) {
     birdY -= boxPixelsY;  // Move up
    } else if (birdDirection == 4 && birdY < resolutionY - 32) {
     birdY += boxPixelsY;  // Move down
    }
}
void respawnHummingbird(bool& birdSick, Clock& sickClock, float& birdX, float& birdY, int& birdHealth, int resolutionX, int resolutionY) {
    if (birdSick && sickClock.getElapsedTime().asSeconds() > 5) {  // Respawn after 5 seconds
        birdSick = false;
        spawnHummingbird(birdX, birdY, birdHealth, birdSick, resolutionX, resolutionY);
    }
}

void drawHummingbird(RenderWindow& window, float birdX, float birdY, Sprite& birdSprite) {
    birdSprite.setPosition(birdX, birdY);
    window.draw(birdSprite);
}
void drawHUD(RenderWindow& window, int currentSpray, int sprayCans,Sprite& sprayCanSprite) {
    // Draw spray level bar
    RectangleShape sprayBar(Vector2f(currentSpray * 5, 20));  // Scale bar based on sprays left
    sprayBar.setFillColor(Color::Yellow);
    sprayBar.setPosition(10, resolutionY - 40);  // Position bar in the bottom-left corner
    window.draw(sprayBar);
    // Draw remaining spray cans
    for (int i = 0; i < sprayCans; i++) {
     sprayCanSprite.setPosition(10 + i * 50, resolutionY - 80);  // Offset cans horizontally
     window.draw(sprayCanSprite);
    }
}
void showMenu(RenderWindow& window) {
    // Load font for menu
    Font font;
     if (!font.loadFromFile("./Textures/p.ttf")) { // Update this path
        cout << "Error loading font!" << endl;
        return;
    }
    // Create  items for selection
    Text menu[4];
    string menuItems[4] = { "Level 1", "Level 2", "Level 3", "Exit" };
    for (int i = 0; i < 4; i++) {
     menu[i].setFont(font);
     menu[i].setFillColor(Color::White);
     menu[i].setString(menuItems[i]);
     menu[i].setCharacterSize(50);
     menu[i].setPosition(400, 200 + i * 60); // Adjust position for each menu item
    }
    int selectedItemIndex = 0;
    menu[selectedItemIndex].setFillColor(Color::Red); // Highlight the first item
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Up) {
                 menu[selectedItemIndex].setFillColor(Color::White); // Reset current item color
                 selectedItemIndex = (selectedItemIndex - 1 + 4) % 4; // Move up in the menu
                 menu[selectedItemIndex].setFillColor(Color::Red); // Highlight new item
                }
                if (event.key.code == Keyboard::Down) {
                 menu[selectedItemIndex].setFillColor(Color::White); // Reset current item color
                 selectedItemIndex = (selectedItemIndex + 1) % 4; // Move down in the menu
                 menu[selectedItemIndex].setFillColor(Color::Red); // Highlight new item
                }
                if (event.key.code == Keyboard::Enter) {
                    if (selectedItemIndex == 3) {
                     window.close(); // Exit the game
                    } else {
                     startGame(window, selectedItemIndex + 1); // Start the selected level
                    }
                }
            }
        }
        window.clear();
        for (int i = 0; i < 4; i++) {
         window.draw(menu[i]);
        }
        window.display();
    }

}
void startGame(RenderWindow& window, int level) {
     int score =0;
    // Local constants for maximum counts
    const int maxBeeCount = 20;
    const int maxHoneycombCount = 100;
     //flowers section
     const int MAX_FLOWERS = 100; // Maximum number of flowers
     float flowerX[MAX_FLOWERS];   // Array to hold flower X positions
     float flowerY[MAX_FLOWERS];   // Array to hold flower Y positions
     int flowerCount = 0;  
     bool firstBeeCreatedFlowers = false;       
     Texture flowerTexture;    // Texture for the flower
     Sprite flowerSprite;
     if (!flowerTexture.loadFromFile("./Textures/flower.png")) { 
        cout<<"Flowers not found"<<endl;
    }
    flowerSprite.setTexture(flowerTexture);
    // Initializing Background Music.
    Music bgMusic;
     
     Texture backgroundTexture;
     Sprite backgroundSprite;
     //loading background and music
     if (level == 1) {
       if (!backgroundTexture.loadFromFile("./Textures/level1_background.png")) {
        cout << "Error loading level 1 background!" << endl;
    }
    bgMusic.openFromFile("./Music/Music3.ogg");
    }  else if (level == 2) {
    if (!backgroundTexture.loadFromFile("./Textures/level_2_background.png")) {
        cout << "Error loading level 2 background!" << endl;
    }
    bgMusic.openFromFile("./Music/Music2.ogg");
    } else if (level == 3) {
    if (!backgroundTexture.loadFromFile("./Textures/level_3_background.png")) {
        cout << "Error loading level 3 background!" << endl;
    }
    bgMusic.openFromFile("./Music/Music1.ogg");
    }
    bgMusic.setVolume(50);
    bgMusic.setLoop(true);
    bgMusic.play();
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(static_cast<float>(resolutionX) / backgroundTexture.getSize().x, static_cast<float>(resolutionY) / backgroundTexture.getSize().y);
    // Players section
    float player_x = (gameRows / 2) * boxPixelsX;
    float player_y = (gameColumns - 4) * boxPixelsY;
    Texture playerTexture;
    Sprite playerSprite;
    playerTexture.loadFromFile("./Textures/spray.png");
    playerSprite.setTexture(playerTexture);
     // spray can section
     Texture sprayCanTexture;
     sprayCanTexture.loadFromFile("./Textures/spray.png");
     Sprite sprayCanSprite(sprayCanTexture);
    // bullet section
    int currentSpray = 56;         
    int sprayCans = 2;                          
    float bullet_x = player_x;
    float bullet_y = player_y;
    bool bullet_exists = true;
    Clock PlayerClock;
    Clock bulletClock;
    Texture bulletTexture;
    Sprite bulletSprite;
    bulletTexture.loadFromFile("./Textures/bullet.png");
    bulletSprite.setTexture(bulletTexture);
    bulletSprite.setScale(3, 3);
    bulletSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
    //bees section
    float beeSpeed[maxBeeCount];
    int honeycombCount;
    int beeCount = 20;
    int beeTypes[maxBeeCount];
    float beeX[maxBeeCount], beeY[maxBeeCount], beeDirection[maxBeeCount];
    bool beeMoving[maxBeeCount]; // Track if bee is moving
    Clock beeClock;
    // Modify the beeCount and types based on the level
    if (level == 1) {
     honeycombCount=3;
     beeCount = 20; // Regular bees
     spawnBees(beeCount, beeX, beeY, beeSpeed, beeMoving, beeDirection,beeTypes,level);
    }  else if (level == 2) {
       beeCount = 20; // Regular bees
       honeycombCount=9;
    spawnBees(beeCount, beeX, beeY,beeSpeed, beeMoving, beeDirection,beeTypes,level);
    // Add logic to spawn fast bees
    } else if (level == 3) {
    honeycombCount=15;
      beeCount = 20; // Regular bees
      // Fast bees
    // You will need to implement a way to spawn fast bees
    spawnBees(beeCount, beeX, beeY, beeSpeed, beeMoving, beeDirection,beeTypes,level);
    // Add logic to spawn fast bees
    }
    Texture beeTexture;
    Sprite beeSprite;
    beeTexture.loadFromFile("./Textures/Regular_bee.png");
 // Resize bees for better visibility
    Texture killerBeeTexture;
 if (!killerBeeTexture.loadFromFile("./Textures/Fast_bee.png")) {
    cout << "Error loading killer bee texture!" << endl;
}
    // Initializing Honeycomb Texture
    Texture honeycombTexture;
    if (!honeycombTexture.loadFromFile("./Textures/honeycomb.png")) {
        cout << "Error loading honeycomb texture!" << endl;
    }
    Texture redHoneycombTexture;
    if (!redHoneycombTexture.loadFromFile("./Textures/honeycomb_red.png")) {
    cout << "Error loading red honeycomb texture!" << endl;
    }

    // The ground on which player moves
    RectangleShape groundRectangle(Vector2f(960, 64));
    groundRectangle.setPosition(0, (gameColumns - 2) * boxPixelsY);
    groundRectangle.setFillColor(Color::Green);
      // Declare arrays for honeycomb position tracking
      float yellowHoneycombX[maxHoneycombCount];
      float yellowHoneycombY[maxHoneycombCount];
      int yellowHoneycombCount = 0;  // Tracks the number of yellow honeycombs draw
      float redHoneycombX[maxHoneycombCount];
      float redHoneycombY[maxHoneycombCount];
      int redHoneycombCount = 0;  // Tracks the number of red honeycombs drawn;  // Tracks the number of honeycombs drawn
    // humming bird
    float birdX, birdY;
    int birdHealth;
    bool birdSick = false;
    int birdDirection = 1;
    Clock birdClock, sickClock;
    spawnHummingbird(birdX, birdY, birdHealth, birdSick, resolutionX, resolutionY);
    Texture birdTexture;
    Sprite birdSprite;
    if (!birdTexture.loadFromFile("./Textures/bird.png")) {
    cout << "Error loading hummingbird texture!" << endl;
    }
    birdSprite.setTexture(birdTexture);
    bool hummingbirdHasEaten = false;
    // pre generate comb
      generateHoneycombPositions( level,yellowHoneycombCount,yellowHoneycombX,yellowHoneycombY, 
redHoneycombCount,redHoneycombX,redHoneycombY,player_y);
   //beehive
    Texture beehiveTexture;

     if (!beehiveTexture.loadFromFile("./Textures/hive.png")) {
       cout << "Error loading beehive texture!" << endl;
     }
     const int MAX_BEEHIVES = 100; // Maximum number of beehives
     float beehiveX[MAX_BEEHIVES]; // Array for x-coordinates of beehives
     float beehiveY[MAX_BEEHIVES]; // Array for y-coordinates of beehives
     int beehiveCount = 0;
     Sprite beehiveSprite;
     while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
            }
        }
         if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Escape) {
                    return; // Exit to menu
                }
            }
        // Call functions for moving and drawing elements
         window.draw(groundRectangle);
         window.draw(backgroundSprite);
        UseBullet(bullet_x, bullet_y, bullet_exists, currentSpray,sprayCans, player_x, player_y);
        if (bullet_exists) {
            moveBullet(bullet_y, bullet_exists, bulletClock);
            drawBullet(window, bullet_x, bullet_y, bulletSprite);
        } else {
            bullet_x = player_x;
            bullet_y = player_y;
        }

        movePlayer(player_x, player_y,  PlayerClock, flowerX, flowerY,flowerCount);
        drawFlowers(window,flowerX,flowerY,flowerCount,flowerSprite);
        moveBees( beeCount,beeX, beeY, beeSpeed, beeMoving, beeClock,beeDirection,player_y, beeTypes, yellowHoneycombX,yellowHoneycombY, yellowHoneycombCount,  redHoneycombX,redHoneycombY,redHoneycombCount, false,flowerX, flowerY,flowerCount, firstBeeCreatedFlowers,100, beehiveX,beehiveY, beehiveCount);
        drawPlayer(window, player_x, player_y, playerSprite);
        //humming bird
         // Game Loop
        if (!birdSick) {
         moveHummingbird(birdX, birdY, birdDirection, birdClock, resolutionX, resolutionY);
         // Check for honeycomb collision
         if (!hummingbirdHasEaten) { // Only check for honeycomb if it hasn't eaten one yet
           if (checkHoneycombCollisionWithBird(birdX, birdY, yellowHoneycombX, yellowHoneycombY, yellowHoneycombCount, redHoneycombX, redHoneycombY, redHoneycombCount, score)) {
             hummingbirdHasEaten = true; // Set the flag to true after eating a honeycomb
            }
          }
          // Check for bullet collision with the bird
          if (checkBulletCollisionWithBird(bullet_x, bullet_y, birdX, birdY, birdHealth, birdSick, sickClock)) {
           bullet_exists = false;  // Bullet disappears after hitting the bird
          }
          drawHummingbird(window, birdX, birdY, birdSprite);
          } else {
           respawnHummingbird(birdSick, sickClock, birdX, birdY, birdHealth, resolutionX, resolutionY);
          }
// Reset the hummingbird status
      bool isNearHoneycomb = false;
// Check if the hummingbird is near any honeycomb
      for (int i = 0; i < yellowHoneycombCount; i++) {
         if (abs(birdX - yellowHoneycombX[i]) < 32 && abs(birdY - yellowHoneycombY[i]) < 32) {
        isNearHoneycomb = true;
        break; // Exit the loop
          }
       }
      if (!isNearHoneycomb) {
        for (int i = 0; i < redHoneycombCount; i++) {
           if (abs(birdX - redHoneycombX[i]) < 32 && abs(birdY - redHoneycombY[i]) < 32) {
            isNearHoneycomb = true;
            break; // Exit the loop 
           }
        }
      }
     // Reset the flag if the hummingbird is not near any honeycomb
     if (!isNearHoneycomb) {
       hummingbirdHasEaten = false; 
     }
     if (checkBulletCollisionWithBird(bullet_x, bullet_y, birdX, birdY, birdHealth, birdSick, sickClock)) {
        bullet_exists = false;  // Bullet disappears after hitting the bird
     }
     else {
      respawnHummingbird(birdSick, sickClock, birdX, birdY, birdHealth, resolutionX, resolutionY);
     }
      drawHummingbird(window, birdX, birdY, birdSprite);
        // Collision detection
       for (int i = 0; i < beeCount; i++) {
         if (checkCollision(bullet_x, bullet_y, beeX[i], beeY[i]) && bullet_exists) {
         removeBee(i,beeCount,beeX,beeY,beeMoving,beeDirection,yellowHoneycombX, yellowHoneycombY,yellowHoneycombCount,      redHoneycombX,redHoneycombY,redHoneycombCount,beeTypes);
        bullet_exists = false;
        break;           // Remove bullet after it hits
        }
          
    }
    bool bulletHitHoneycomb = false; // Flag to track if a honeycomb was hit
    int honeycombIndex = -1; // Store the index of the honeycomb hit
    //beehive
    for (int i = 0; i < beehiveCount; ++i) {
      if (checkBeehiveCollision( beehiveX[i], beehiveY[i],bullet_y, bullet_x)) {
        // Remove the beehive
        for (int j = i; j < beehiveCount - 1; ++j) {
         beehiveX[j] = beehiveX[j + 1];
         beehiveY[j] = beehiveY[j + 1];
        }
        beehiveCount--; 
        bullet_exists = false; // Remove bullet
        break; // Exit the loop after removing the beehive
      }
    }
    // Check yellow honeycombs
    for (int i = 0; i < yellowHoneycombCount; i++) {
     if (checkHoneycombCollision(bullet_x, bullet_y, yellowHoneycombX[i], yellowHoneycombY[i])) {
        honeycombIndex = i; // Store the index of the honeycomb hit
        bulletHitHoneycomb = true; // Set the flag
        break; // Exit the loop after the first hit
     }
    }
    // Check red honeycombs if no yellow honeycomb was hit
    if (!bulletHitHoneycomb) {
      for (int i = 0; i < redHoneycombCount; i++) {
         if (checkHoneycombCollision(bullet_x, bullet_y, redHoneycombX[i], redHoneycombY[i])) {
          honeycombIndex = i + yellowHoneycombCount; // Adjust index for red honeycomb
          bulletHitHoneycomb = true; // Set the flag
          break; // Exit the loop after the first hit
         }
      }
    }
    if (bulletHitHoneycomb) {
     removeHoneycomb(honeycombIndex, yellowHoneycombX, yellowHoneycombY, yellowHoneycombCount, redHoneycombX, redHoneycombY, redHoneycombCount);
     bullet_exists = false; // Remove bullet after it hits the honeycomb
    }
      // Draw honeycomb at all stored positions
    for (int i = 0; i < yellowHoneycombCount; i++) {
      drawHoneycomb(window, yellowHoneycombX[i], yellowHoneycombY[i], honeycombTexture);  // Draw yellow honeycomb
     }
    for (int i = 0; i < redHoneycombCount; i++) {
    drawHoneycomb(window, redHoneycombX[i], redHoneycombY[i], redHoneycombTexture);  // Draw red honeycomb
    }
        drawBees(window, beeCount, beeX, beeY, beeSprite,killerBeeTexture,beeTexture,beeTypes,beeDirection); 
        for (int i = 0; i < beehiveCount; ++i) {
         beehiveSprite.setPosition(beehiveX[i], beehiveY[i]);
          window.draw(beehiveSprite);
        } 
        drawHUD(window, currentSpray, sprayCans, sprayCanSprite);
        window.display();
        window.clear();
    }
    
  }
void generateHoneycombPositions(int level, int& yellowHoneycombCount, float yellowHoneycombX[], float yellowHoneycombY[], 
int& redHoneycombCount, float redHoneycombX[], float redHoneycombY[],float player_y) {
    yellowHoneycombCount = 0; // Reset count
    redHoneycombCount = 0;    // Reset count
    int numYellowHoneycombs, numRedHoneycombs;
    // Determine the number of honeycombs based on the level
    if (level == 1) { 
        numYellowHoneycombs = 3;
        numRedHoneycombs = 0;
    } else if (level == 2) {
        numYellowHoneycombs = 9; 
        numRedHoneycombs = 0; 
    } else if (level == 3) {
        numYellowHoneycombs = 15; 
        numRedHoneycombs = 0; 
    } else {
        numYellowHoneycombs = 0; // Default case, should not happen
        numRedHoneycombs = 0;    // Default case, should not happen
    }
    // Generate yellow honeycomb positions
    while (yellowHoneycombCount < numYellowHoneycombs) {
        float newX = static_cast<float>(rand() % (resolutionX - boxPixelsX));
        float newY = static_cast<float>(rand() % (static_cast<int>(player_y) - boxPixelsY + 1)); // Ensure honeycomb is above player
        // Check for overlap with existing yellow honeycombs
        bool overlap = false;
        for (int i = 0; i < yellowHoneycombCount; i++) {
            if (abs(newX - yellowHoneycombX[i]) < boxPixelsX && abs(newY - yellowHoneycombY[i]) < boxPixelsY) {
                overlap = true;
                break;
            }
        }
        // If no overlap, add the new yellow honeycomb position
        if (!overlap) {
            yellowHoneycombX[yellowHoneycombCount] = newX;
            yellowHoneycombY[yellowHoneycombCount] = newY;
            yellowHoneycombCount++;
        }

    }
    // Generate red honeycomb positions
    while (redHoneycombCount < numRedHoneycombs) {
        float newX = static_cast<float>(rand() % (resolutionX - boxPixelsX));
        float newY = static_cast<float>(rand() % (static_cast<int>(player_y) - boxPixelsY + 1)); // Ensure honeycomb is above player
        // Check for overlap with existing red honeycombs
        bool overlap = false;
        for (int i = 0; i < redHoneycombCount; i++) {
            if (abs(newX - redHoneycombX[i]) < boxPixelsX && abs(newY - redHoneycombY[i]) < boxPixelsY) {
                overlap = true;
                break;
            }
        }
        // If no overlap, add the new red honeycomb position
        if (!overlap) {
            redHoneycombX[redHoneycombCount] = newX;
            redHoneycombY[redHoneycombCount] = newY;
            redHoneycombCount++;
        }

    }

}
void drawFlowers(sf::RenderWindow& window, float flowerX[], float flowerY[], int flowerCount, Sprite& flowerSprite) {
    for (int i = 0; i < flowerCount; i++) {
     flowerSprite.setPosition(flowerX[i], flowerY[i]);
     window.draw(flowerSprite);
    }
}
void createFlowers(float beeX, float beeY, float player_y, float flowerX[], float flowerY[], int& flowerCount, const int MAX_FLOWERS, float beeDirection[]) {
    static bool firstFlower = true; // Track if the initial flowers have been created
    static float lastFlowerX = -1; // Track the last X position where a flower was created
    static float firstFlowerX = -1; // Track the position of the first flower
    // Create flowers only if the bee's Y position is less than or equal to the player's Y position
    if (flowerCount < MAX_FLOWERS && beeY >= player_y) {
        if (firstFlower) {
         // Create the first flower at the left border
         flowerX[flowerCount] = 0; 
         flowerY[flowerCount] = player_y; 
         firstFlowerX = flowerX[flowerCount]; // Store the position of the first flower
         flowerCount++;
         // Create the second flower at the right border
         flowerX[flowerCount] = 928; //resolutionX-boxpixelX = 928
         flowerY[flowerCount] = player_y; 
         lastFlowerX = flowerX[flowerCount]; 
         flowerCount++;
         firstFlower = false; // Mark that the initial flowers have been created
        } else {
            // Create flowers based on bee direction
            if (beeDirection[0] == 1) { // If moving right
             float flowerPositionXRight = beeX + boxPixelsX;
             // Ensure spacing from the last flower and the first flower
             if (flowerPositionXRight > lastFlowerX + boxPixelsX) { 
              flowerX[flowerCount] = flowerPositionXRight; // Set new flower X position
              flowerY[flowerCount] = player_y; // Set to player's Y coordinate
              flowerCount++;
              lastFlowerX = flowerPositionXRight; 
             }
            } else if (beeDirection[0] == -1) { // If moving left
                float flowerPositionXLeft = beeX - boxPixelsX;
                // Ensure spacing from the last flower and the first flower
                if (flowerPositionXLeft < firstFlowerX - boxPixelsX) { 
                 flowerX[flowerCount] = flowerPositionXLeft; // Set new flower X position
                 flowerY[flowerCount] = player_y; // Set to player's Y coordinate
                 flowerCount++;
                 lastFlowerX = flowerPositionXLeft; // Update last flower X position
                }
            }
        }
    }
}
bool checkBeehiveCollision(float beehiveX, float beehiveY,float bullet_y, float bullet_x) {
    if (bullet_x < beehiveX + boxPixelsX&& 
     bullet_x + boxPixelsX > beehiveX && 
     bullet_y < beehiveY + boxPixelsY && 
     bullet_y + boxPixelsY > beehiveY) {
     return true; // Collision detected
    }
     return false; // No collision
} 
bool isBeeStuck(int beeIndex, const int MAX_HONEYCOMBS, float yellowHoneycombX[], float yellowHoneycombY[], int& yellowHoneycombCount,
float redHoneycombX[], float redHoneycombY[], int& redHoneycombCount, float beeX[], float beeY[]) {
    static const int STUCK_THRESHOLD = 5; // Number of frames to consider a bee stuck
    static int stuckFrameCount[100] = {0}; // Array to count frames a bee has been stuck
    static float previousX[100] = {0}; // Store previous X position
    static float previousY[100] = {0}; // Store previous Y position
    // Check against all yellow honeycombs
    bool collidingWithYellow = false;
    for (int i = 0; i < yellowHoneycombCount; i++) {
        if (beeX[beeIndex] >= yellowHoneycombX[i] - 32 && beeX[beeIndex] <= yellowHoneycombX[i] + 32 &&
         beeY[beeIndex] >= yellowHoneycombY[i] - 32 && beeY[beeIndex] <= yellowHoneycombY[i] + 32) {
         collidingWithYellow = true; // The bee is colliding with a yellow honeycomb
         break;
        }
    }
    // Check against all red honeycombs
    bool collidingWithRed = false;
    for (int i = 0; i < redHoneycombCount; i++) {
        if (beeX[beeIndex] >= redHoneycombX[i] - 32 && beeX[beeIndex] <= redHoneycombX[i] + 32 &&
            beeY[beeIndex] >= redHoneycombY[i] - 32 && beeY[beeIndex] <= redHoneycombY[i] + 32) {
            collidingWithRed = true; // The bee is colliding with a red honeycomb
            break;
        }
    }
    // Check if the bee is colliding with both honeycombs
    if (collidingWithYellow || collidingWithRed) {
        // Check if the bee has not moved significantly
        if ((beeX[beeIndex] >= previousX[beeIndex] - 0.01 && beeX[beeIndex] <= previousX[beeIndex] + 0.01) &&
            (beeY[beeIndex] >= previousY[beeIndex] - 0.01 && beeY[beeIndex] <= previousY[beeIndex] + 0.01)) {
            stuckFrameCount[beeIndex]++;
        } else {
            stuckFrameCount[beeIndex] = 0; // Reset the count if the bee has moved
        }
        // Update previous position
        previousX[beeIndex] = beeX[beeIndex];
        previousY[beeIndex] = beeY[beeIndex];
        // If the bee has been stuck for a certain number of frames, return true
        return stuckFrameCount[beeIndex] >= STUCK_THRESHOLD;
    }
    stuckFrameCount[beeIndex] = 0;
    return false; 
}
bool checkCollision(float bullet_x, float bullet_y, float beeX, float beeY) {
    // Define the boundaries of the bullet
    float bulletLeft = bullet_x;
    float bulletRight = bullet_x + boxPixelsX; // Assuming bullet width is boxPixelsX
    float bulletTop = bullet_y;
    float bulletBottom = bullet_y + boxPixelsY; // Assuming bullet height is boxPixelsY
    // Define the boundaries of the bee
    float beeLeft = beeX;
    float beeRight = beeX + 32; // Assuming bee width is beeWidth
    float beeTop = beeY;
    float beeBottom = beeY + 32; // Assuming bee height is beeHeight
    // Check for collision
    return bulletRight > beeLeft && bulletLeft < beeRight &&
           bulletBottom > beeTop && bulletTop < beeBottom;
}
bool checkHoneycombCollision(float bullet_x, float bullet_y, float honeycombX, float honeycombY){
   // Check yellow honeycombs
     return (bullet_x >= honeycombX - 16 && bullet_x <= honeycombX + 16 &&
            bullet_y >= honeycombY - 16 && bullet_y <= honeycombY + 16);

}
bool checkHoneycombCollisionWithBird(float birdX, float birdY, float yellowHoneycombX[], float yellowHoneycombY[], int& yellowHoneycombCount, float redHoneycombX[], float redHoneycombY[], int& redHoneycombCount, int& score) {
    // Check yellow honeycombs
    for (int i = 0; i < yellowHoneycombCount; i++) {
        if (abs(birdX - yellowHoneycombX[i]) < 32 && abs(birdY - yellowHoneycombY[i]) < 32) {
            // Remove the yellow honeycomb
            removeHoneycomb(i, yellowHoneycombX, yellowHoneycombY, yellowHoneycombCount, redHoneycombX, redHoneycombY, redHoneycombCount);
            score += 10;  // Award points
            return true; // Return true to indicate a honeycomb was eaten
        }
    }
    // Check red honeycombs
    for (int i = 0; i < redHoneycombCount; i++) {
        if (abs(birdX - redHoneycombX[i]) < 32 && abs(birdY - redHoneycombY[i]) < 32) {
            // Remove the red honeycomb
            removeHoneycomb(i + yellowHoneycombCount, yellowHoneycombX, yellowHoneycombY, yellowHoneycombCount, redHoneycombX, redHoneycombY, redHoneycombCount);
            score += 10;  // Award points
            return true; // Return true to indicate a honeycomb was eaten
        }
    }
    return false; // No honeycomb was eaten
}
bool checkBulletCollisionWithBird(float bullet_x, float bullet_y, float& birdX, float& birdY, int& birdHealth, bool& birdSick, Clock& sickClock) {
    if (abs(bullet_x - birdX) < 32 && abs(bullet_y - birdY) < 32) {
        birdHealth--;
        if (birdHealth <= 0) {
            birdSick = true;  // Mark bird as sick
            sickClock.restart();  // Start respawn timer
            birdX = -100;  // Move bird off-screen
            birdY = -100;
        }
        return true;
    }
    return false;
}
bool checkCollisionWithFlowers(float player_x, float player_y, float flowerX[], float flowerY[], int flowerCount) {
    for (int i = 0; i < flowerCount; i++) {
        if (player_x < flowerX[i] + boxPixelsX && 
            player_x + boxPixelsX > flowerX[i] && 
            player_y < flowerY[i] + boxPixelsY && 
            player_y + boxPixelsY > flowerY[i]) {
            return true; // Collision detected
        }
    }
    return false; // No collision
}// No collision
bool checkBeeCollisionWithHoneycomb(float beeX, float beeY, float honeycombX[], float honeycombY[], int& honeycombCount) {
    for (int i = 0; i < honeycombCount; i++) {
        if (abs(beeX - honeycombX[i]) < 32 && abs(beeY - honeycombY[i]) < 32) {
            return true; // Collision detected
        }
    }
    return false; // No collision
}




