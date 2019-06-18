#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace sf;
using std::cout;
using std::endl;

std::string toString(int Integer)
{
	return static_cast<std::ostringstream*>(&(std::ostringstream() << Integer))->str();
}

struct Point
{int x, y;} a[4], b[4];

class WindowManager
{
public:
	RenderWindow window;
	WindowManager(RenderWindow& window);
};

WindowManager::WindowManager(RenderWindow& window) {
	window.create(VideoMode(320, 480), "The Game!");
}

class Tetris
{
public:

	Tetris();
	
	void displayScore();
	bool check();
	void resetBoard();
	void calculateScore();
	void rotateBlock();
	void tick();
	void checkLines();
	void draw(Sprite s, Sprite background, Sprite frame);
	void windowControls();
	void loadTextures(Texture& t1, Texture& t2, Texture& t3);
	void startGame();


private:
	int M = 20;
	int N = 10;
	int score{ 0 }, combo{ 1 };
	bool pastScore = false, newScore = false;
	int field[20][10] = { 0 };
	int dx = 0; bool rotate = 0; int colorNum = 1;
	double timer = 0, delay = 0.3;
	int figures[7][4] =
	{
		1,3,5,7, // I
		2,4,5,7, // Z
		3,5,4,6, // S
		3,5,4,7, // T
		2,3,5,7, // L
		3,5,7,6, // J
		2,3,4,5, // O
	};

	Font myFont;
	Text dScore, dHighScore, dScoreName;
	std::string scoreName;
	int highscore{ 0 };
	RenderWindow window;
};

Tetris::Tetris()
{	
	window.create(VideoMode(320, 480), "The Game!");
	myFont.loadFromFile("LibreBaskerville-Regular.ttf");

	std::fstream fileIn("highscores.dat");
	fileIn >> scoreName >> highscore;
}

void Tetris::displayScore() {
	dScore.setFont(myFont);
	dScore.setString("Score: " + toString(score));
	dScore.setCharacterSize(24);
	dScore.setFillColor(Color::White);
	dScore.setOutlineColor(Color::Black);
	dScore.setOutlineThickness(1);
	dScore.setPosition(25, 405);
	//scoreBoard.setStyle(Text::Bold | Text::Underlined);

	if (score > highscore) {
		highscore = score; 
		newScore = true; 
		scoreName = "";
	}
	dHighScore.setFont(myFont);
	dHighScore.setString("Highscore: " + scoreName + " - " + toString(highscore));
	dHighScore.setCharacterSize(20);
	dHighScore.setFillColor(Color::White);
	dHighScore.setOutlineColor(Color::Black);
	dHighScore.setOutlineThickness(1);
	dHighScore.setPosition(25, 437);
}

bool Tetris::check()
{
   for (int i=0;i<4;i++)
	  if (a[i].x<0 || a[i].x>=N || a[i].y>=M) return 0;
      else if (field[a[i].y][a[i].x]) return 0;

   return 1;
};

void Tetris::resetBoard() {
	//Display Game Over screen and ask for a name for a new high score
	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < N; j++) field[i][j] = 0;
	}
	pastScore = false;

	if (newScore) {
		//ask for name for new high score
		cout << "New Highscore! Enter Name: ";
		std::cin >> scoreName;

		//Writes to a file to save new highscore
		std::ofstream fileOut("highscores.dat");
		fileOut << scoreName << " " << highscore;

		//resets newScore to false for enxt round
		newScore = false;
	}

	//display game over screen with high scores
	cout << "Game over! Your score was " << score << endl;
	score = 0;
}

void Tetris::calculateScore() {
	
	combo++;
	if (combo > 1) cout << "Combo: " << combo << endl;
	score += 100 * combo;
	//cout << "Score: " << score << endl << endl;
	pastScore = true;
}

void Tetris::rotateBlock() {
	Point p = a[1]; //center of rotation
	for (int i = 0; i<4; i++)
	{
		int x = a[i].y - p.y;
		int y = a[i].x - p.x;
		a[i].x = p.x - x;
		a[i].y = p.y + y;
	}
	if (!check()) for (int i = 0; i<4; i++) a[i] = b[i];
}

void Tetris::tick() {
	for (int i = 0; i<4; i++) { b[i] = a[i]; a[i].y += 1; }

	if (!check())
	{
		for (int i = 0; i < 4; i++) field[b[i].y][b[i].x] = colorNum; //Creates a new piece

		colorNum = 1 + rand() % 7;
		int n = rand() % 7;
		for (int i = 0; i<4; i++)
		{
			a[i].x = figures[n][i] % 2;
			a[i].y = figures[n][i] / 2;
		}
		if (pastScore) pastScore = false;
		else if (!pastScore) combo = 0;
	}

	timer = 0;
}

void Tetris::checkLines() {
	int k = M - 1;
	for (int i = M - 1; i > 0; i--)
	{
		int count = 0;
		for (int j = 0; j < N; j++)
		{
			if (field[i][j]) count++;
			field[k][j] = field[i][j];
		}
		if (count < N) k--;
		else calculateScore(); //Calculate Score when a row is removed
	}
	//Checks to see if any peice is touching the top of the field
	for (int i = 0; i < M; i++)
		if (field[0][i]) resetBoard(); //Reset the board if one is found at the top
}

void Tetris::draw(Sprite s, Sprite background, Sprite frame) {
	window.clear(Color::White);
	window.draw(background);

	for (int i = 0; i<M; i++)
		for (int j = 0; j<N; j++)
		{
			if (field[i][j] == 0) continue;
			s.setTextureRect(IntRect(field[i][j] * 18, 0, 18, 18));
			s.setPosition(j * 18, i * 18);
			s.move(28, 31); //offset
			window.draw(s);
		}

	for (int i = 0; i<4; i++)
	{
		s.setTextureRect(IntRect(colorNum * 18, 0, 18, 18));
		s.setPosition(a[i].x * 18, a[i].y * 18);
		s.move(28, 31); //offset
		window.draw(s);
	}

	window.draw(frame);

	displayScore(); //updates the scores to draw
	window.draw(dScore); //updates what to draw for the score and highscore
	window.draw(dHighScore);
	window.display(); //What actually changes the screen
}

void Tetris::windowControls()
{
	Event e;
	while (window.pollEvent(e))
	{
		if (e.type == Event::Closed)
			window.close();

		if (e.type == Event::KeyPressed)
			if (e.key.code == Keyboard::Up) rotate = true;
			else if (e.key.code == Keyboard::Left) dx = -1;
			else if (e.key.code == Keyboard::Right) dx = 1;
			else if (e.key.code == Keyboard::R) resetBoard(); //"R" resets the board
	}
}

void Tetris::loadTextures(Texture& t1, Texture& t2, Texture& t3)
{//Loads the different textures
	t1.loadFromFile("images/tiles.png");
	t2.loadFromFile("images/background.png");
	t3.loadFromFile("images/frame.png");
}

void Tetris::startGame() 
{ //Starts the game and holds the main game loop
	Texture t1, t2, t3;
	loadTextures(t1, t2, t3);
	Sprite s(t1), background(t2), frame(t3);
	Clock clock;

	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;
		windowControls();

		if (Keyboard::isKeyPressed(Keyboard::Down)) delay = 0.05;

		//// <- Move -> ///
		for (int i = 0; i < 4; i++) { b[i] = a[i]; a[i].x += dx; }
		if (!check()) for (int i = 0; i < 4; i++) a[i] = b[i];

		//////Rotate//////
		if (rotate)
			rotateBlock();

		///////Tick//////
		if (timer > delay)
			tick();

		///////check lines//////////
		checkLines();
		dx = 0; rotate = 0; delay = 0.3;

		/////////draw//////////
		draw(s, background, frame);
	}
}

int main()
{
	srand(time(0));
	Tetris myTetris;
	myTetris.startGame();

    
	return 0;
}