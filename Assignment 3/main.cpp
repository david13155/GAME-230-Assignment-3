#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include <string>
#include <iostream>
#include <fstream>

using namespace sf;
using namespace std;

//Game Loop Pattern
void initialize(RenderWindow& window);
void handleInput(RenderWindow& window, Event& e);
void render(RenderWindow& window);

//functions
Vector2<int> MouseToTileMap(RenderWindow& window);
void SaveLevelToFile();
void LoadLevelFromFile();
void LoadTilesFromFile();
void LoadTiles();
//void defineLevelScale();

//const variables
const int TILES_PIECE_COUNT_DEFAULT = 21;
const int DEFAULT_TILES_COUNT_X = 10;
const int DEFAULT_TILES_COUNT_Y = 10;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 720;
const string SCREEN_SHOT_NAME = "Assignment 2 Level Screenshot.png";
const string LEVEL_FILE_NAME = "level.txt";


//global variables
Texture tileTexture[21];
Sprite tileSprite[21];
int tilesPieceCount = 21;
int tilesCountX = 10;
int tilesCountY = 10;
int tilesWidth = 70;
int tilesHeight = 70;
int tileSelected = 0;

int level[10][10];
Sprite tileMap[10][10];

int main() {
	//Process the startup input before defining a window
	LoadTiles();

	RenderWindow mainWindow(VideoMode(750, 750), "Assignment 2");

	//initialize variables & arrays
	initialize(mainWindow);

	while (mainWindow.isOpen())
	{
		// Check for events from the window & handle the input
		Event event;
		while (mainWindow.pollEvent(event))
		{
			handleInput(mainWindow, event);
		}

		//update(mainWindow);

		render(mainWindow);
	}
	return 0;
}

//Initialization
void initialize(RenderWindow& window) 
{

	tileSelected = 0;

	cout << "Window size: " << window.getSize().x << " " << window.getSize().y << endl;

	for (int i = 0; i < tilesCountY; i++) {
		for (int j = 0; j < tilesCountX; j++) {
			level[i][j] = -1;
			tileMap[i][j].setPosition(j * tilesWidth + (window.getSize().x % tilesWidth) / 2, i * tilesHeight + (window.getSize().y % tilesHeight) / 2);
		}
	}

	return;
}

void handleInput(RenderWindow& window, Event& e) {
	if (e.type == Event::Closed)
		window.close();
	if (e.type == Event::KeyPressed) {
		//Up & Down - change the selected tile
		if (e.key.code == Keyboard::Up) {
			tileSelected = (tileSelected + 1) % 21;
		}
		if (e.key.code == Keyboard::Down) {
			tileSelected = (tileSelected - 1 + 21) % 21;
		}
		//Space - screenshot
		if (e.key.code == Keyboard::Space) {
			Texture screenShot;
			screenShot.create(window.getSize().x, window.getSize().y);
			screenShot.update(window);
			if (screenShot.copyToImage().saveToFile(SCREEN_SHOT_NAME))
			{
				cout << "Screenshot saved to " << SCREEN_SHOT_NAME << endl;
			}
		}
		//S - save level
		if (e.key.code == Keyboard::S) {
			SaveLevelToFile();
		}
		//L - load level
		if (e.key.code == Keyboard::L) {
			LoadLevelFromFile();
		}

	}
	//Mouse click - add & erase tile
	if (Mouse::isButtonPressed(Mouse::Left)) {
		Vector2<int> mousePos = MouseToTileMap(window);
		level[mousePos.y][mousePos.x] = tileSelected;
	}
	if (Mouse::isButtonPressed(Mouse::Right)) {
		Vector2<int> mousePos = MouseToTileMap(window);
		level[mousePos.y][mousePos.x] = -1;
	}
	return;
}

//void update(RenderWindow& window) {
//
//
//	return;
//}

void render(RenderWindow& window) {
	window.clear();
	//draw the level based on the int array recorded
	for (int i = 0; i < tilesCountY; i++) {
		for (int j = 0; j < tilesCountX; j++) {
			if (level[i][j] >= 0 && level[i][j] < tilesPieceCount) {
				tileMap[i][j].setTexture(tileTexture[level[i][j]]);
				tileMap[i][j].setScale(1, 1);
				window.draw(tileMap[i][j]);
			}
		}
	}
	//draw the selected tile at the mouse pointed place
	Vector2<int> mousePos = MouseToTileMap(window);
	tileMap[mousePos.y][mousePos.x].setTexture(tileTexture[tileSelected]);
	window.draw(tileMap[mousePos.y][mousePos.x]);
	window.display();
	return;
}

//function to locate the mouse position on the stored int level array
Vector2<int> MouseToTileMap(RenderWindow& window) {
	int x, y;
	x = (int)(Mouse::getPosition(window).x - (window.getSize().x % tilesWidth) / 2) / tilesWidth;
	x = max(0, x);
	x = min(x, tilesCountX - 1);
	y = (int)(Mouse::getPosition(window).y - (window.getSize().y % tilesHeight) / 2) / tilesHeight;
	y = max(0, y);
	y = min(y, tilesCountY - 1);
	return Vector2<int>(x, y);

}

//Save the current level to a file
void SaveLevelToFile() {
	ofstream levelFile;
	string fileName = "level.txt";

	levelFile.open(fileName);
	if (!levelFile.is_open()) {
		cout << "Failed to save the level to file " << fileName << endl;
		return;
	}

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			levelFile << level[i][j] << " ";
		}
		levelFile << endl;
	}
	levelFile.close();
	cout << "Level saved to file " << fileName << endl;
	return;
}

//Load an existing level file
void LoadLevelFromFile() {
	ifstream levelFile;
	string fileName = "level.txt";

	levelFile.open(fileName);
	if (!levelFile.is_open()) {
		cout << "Failed to load from " << fileName << endl;
		return;
	}

	int fileLevel[10][10];

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			levelFile >> fileLevel[i][j];
		}
	}

	levelFile.close();
	cout << "Level loaded from " << fileName << endl;
	return;
}

void LoadTiles() {

	for (int i = 0; i < tilesPieceCount; i++) {
		if (!tileTexture[i].loadFromFile("Tilesets/Platformer-" + to_string(i) + ".png")) {
			cout << "Failed to load tiles" << endl;
			return;
		}
	}
	
}