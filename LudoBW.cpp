//-----------------------------------------------------------------------------
//
//                                 LUDO GAME (Black & White)
//
//                            Fait le : 10/01/2020
//                            Auteur : 
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DIRECTIVES
//-----------------------------------------------------------------------------

/*#ifdef _WIN32
#define NOMINMAX 
#include <windows.h>
#endif*/

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <limits>
#include <random>

using namespace std;

#define RESET "\u001B[0m"
#define RED "\u001B[31m"
#define GREEN "\u001B[32m"
#define YELLOW "\u001B[33m"
#define BLUE "\u001B[34m"
#define CYAN "\u001B[36m"

#define SIZE 11
#define CELL_NUMBER 40
#define MAX_PLAYER 4
#define MAX_DE 6

//-----------------------------------------------------------------------------
// Objects
//-----------------------------------------------------------------------------

class Cell {
public:
	int x;
	int y;
	int value;
};

class Player {
public:
	int x;
	int y;
	int team;
	int index;
	int id;
};

//-----------------------------------------------------------------------------
// PROTOTYPES
//-----------------------------------------------------------------------------

void initBoard(char[SIZE][SIZE]);
void initGame(Cell[CELL_NUMBER], Cell[MAX_PLAYER][4], Player[MAX_PLAYER][4]);

void menu(int&, int&);
int play(char[SIZE][SIZE], int);

void verifyInput();
string idToColor(int);
int getIndexByTurn(int);
bool isFinalWay(int, int);
void moveOnFinalWay(Player[MAX_PLAYER][4], int, int);

void displayTitle();
void displayCurrent(char[SIZE][SIZE], Player[MAX_PLAYER][4]);
void display(char[SIZE][SIZE]);

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main()
{
	//srand(static_cast<unsigned int>(std::time(nullptr)));

	char board[SIZE][SIZE];
	initBoard(board);

	while (true) {
		int choice = -1;
		int players = -1;
		menu(choice, players);

		if (choice == 2) {
#ifdef _WIN32
			cout << endl << "Arret du jeu..." << endl;
#else
			cout << endl << "Arrêt du jeu..." << endl;
#endif
			return 0;
		}

		int winner;
		winner = play(board, players);
#ifdef _WIN32
		cout << endl << "Le joueur " << winner + 1 << " (" << idToColor(winner) << ") a gagner !" << endl << endl;
#else
		cout << endl << "Le joueur " << winner + 1 << " à gagner !" << endl << endl;
#endif
	}

	return 0;
}

//-----------------------------------------------------------------------------
// INIT 
//-----------------------------------------------------------------------------

void initBoard(char b[SIZE][SIZE]) {
	char newBoard[SIZE][SIZE] = {
		{'r','r',' ','O','O','O','O','g',' ','g','g'},
		{'r','r',' ','O',' ','g',' ','O',' ','g','g'},
		{' ',' ',' ','O',' ','g',' ','O',' ',' ',' '},
		{'r','O','O','O',' ','g',' ','O','O','O','O'},
		{'O',' ',' ',' ',' ','g',' ',' ',' ',' ','O'},
		{'O','r','r','r','r','0','b','b','b','b','O'},
		{'O',' ',' ',' ',' ','y',' ',' ',' ',' ','O'},
		{'O','O','O','O',' ','y',' ','O','O','O','b'},
		{' ',' ',' ','O',' ','y',' ','O',' ',' ',' '},
		{'y','y',' ','O',' ','y',' ','O',' ','b','b'},
		{'y','y',' ','y','O','O','O','O',' ','b','b'},
	}; // Ludo board is 11x11

	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			b[i][j] = newBoard[i][j];
		}
	}
}

void initGame(Cell cells[CELL_NUMBER], Cell houses[MAX_PLAYER][4], Player players[MAX_PLAYER][4]) {
	//{y,x}
	int newCells[CELL_NUMBER][2] = {
		{3,0},{3,1},{3,2},{3,3},
		{2,3},{1,3},{0,3},
		{0,4},{0,5},{0,6},{0,7},
		{1,7},{2,7},{3,7},
		{3,8},{3,9},{3,10},
		{4,10},{5,10},{6,10},{7,10},
		{7,9},{7,8},{7,7},
		{8,7},{9,7},{10,7},
		{10,6},{10,5},{10,4},{10,3},
		{9,3},{8,3},{7,3},
		{7,2},{7,1},{7,0},
		{6,0},{5,0},{4,0}
	};

	int newHouses[MAX_PLAYER][4][2] = {
		{{0,0},{0,1},{1,1},{1,0}},
		{{0,9},{0,10},{1,10},{1,9}},
		{{9,9},{9,10},{10,10},{10,9}},
		{{9,0},{9,1},{10,1},{10,0}},
	};

	for (int i = 0; i < CELL_NUMBER; i++) {
		cells[i].y = newCells[i][0];
		cells[i].x = newCells[i][1];
		cells[i].value = 0;
	}

	for (int i = 0; i < MAX_PLAYER; i++) {
		for (int j = 0; j < 4; j++) {
			houses[i][j].y = newHouses[i][j][0];
			houses[i][j].x = newHouses[i][j][1];
			houses[i][j].value = (j + 1) + (i) * 10;
		}
	}

	for (int i = 0; i < MAX_PLAYER; i++) {
		for (int j = 0; j < 4; j++) {
			players[i][j].x = houses[i][j].x;
			players[i][j].y = houses[i][j].y;
			players[i][j].index = -1;
			players[i][j].team = i;
			players[i][j].id = j + 1;
		}
	}
}

//-----------------------------------------------------------------------------
// MENU + PLAY
//-----------------------------------------------------------------------------

void menu(int& choice, int& players) {
	displayTitle();

	while (choice > 2 || choice < 1) {
		cout << endl << "1 - Jouer" << endl;
		cout << "2 - Quitter" << endl << endl << ">";
		cin >> choice;
		verifyInput();
	}

	if (choice == 1) {
		while (players < 2 || players > 4) {
			cout << endl << "Combien de joueur (entre 2 et 4) ?" << endl << endl << ">";
			cin >> players;
			verifyInput();
		}
	}
}

int play(char board[SIZE][SIZE], int nbPlayer) {
	Cell cells[CELL_NUMBER];
	Cell houses[MAX_PLAYER][4];
	Player players[MAX_PLAYER][4];
	initGame(cells, houses, players);

	/*for (Cell cell : cells){
		cout << cell.x << " " << cell.y << " " << cell.value << endl;
	}*/

	/*for (int i=0; i<MAX_PLAYER; i++) {
		for (Cell house : houses[i]) {
			cout << house.x << " " << house.y << " " << house.value << endl;
		}
		cout << endl;
	}*/

	/*for (int i=0; i<MAX_PLAYER; i++) {
		for (Player player : players[i]) {
			cout << player.x << " " << player.y << " " << player.index << " " << player.team << " " << player.id << endl;
		}
		cout << endl;
	}*/
	
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(1, 6);

	bool finished = false;
	int turn = 0; // Player turn : 0, 1, 2 or 3
	while (!finished) {
		//string text1 =  "C'est au joueur " + idToColor(turn) + " de jouer." + RESET;

		displayCurrent(board, players);

		cout << endl << "C'est au joueur " << (turn+1) << " (" << idToColor(turn) << ") de jouer." << endl;
		//int result = (rand() % MAX_DE) + 1;
		int result = dis(gen);

		int choice = 0;
		while (choice < 1 || choice > 4) {
#ifdef _WIN32
			cout << "Lancement du de ... Resultat : " << result << endl;
			cout << "Quelle piece voulez-vous deplacer avec un lancer de " << result << endl << endl;
#else
			cout << "Lancement du dé ... Résultat : " << result << endl;
			cout << "Quelle pièce voulez-vous déplacer avec un lancer de " << result << endl << endl;
#endif
			cout << ">";
			cin >> choice;
			verifyInput();
		}
		//cout << players[turn][choice-1].index;
		if (players[turn][choice - 1].index == -1 && result == 6) {
			players[turn][choice - 1].index = getIndexByTurn(turn);
			players[turn][choice - 1].x = cells[getIndexByTurn(turn)].x;
			players[turn][choice - 1].y = cells[getIndexByTurn(turn)].y;
		}
		else if (players[turn][choice - 1].index != -1) {
			int step = result;
			while (step > 0) {
				if (isFinalWay(turn, players[turn][choice - 1].index)) {
					players[turn][choice - 1].index = 100;
				}
				if (players[turn][choice - 1].index < CELL_NUMBER) {

					players[turn][choice - 1].index = (players[turn][choice - 1].index + 1) % CELL_NUMBER;
					players[turn][choice - 1].x = cells[players[turn][choice - 1].index].x;
					players[turn][choice - 1].y = cells[players[turn][choice - 1].index].y;
				}
				else {
					players[turn][choice - 1].index = players[turn][choice - 1].index + 1;
					moveOnFinalWay(players, turn, choice);

					if (players[turn][choice - 1].x == 5 && players[turn][choice - 1].y == 5) {
						displayCurrent(board, players);
						return turn;
					}
				}
				step--;
			}
			for (int i = 0; i < MAX_PLAYER; i++) {
				if (i != turn) {
					for (int j = 0; j < 4; j++) {
						if (players[i][j].x == players[turn][choice - 1].x && players[i][j].y == players[turn][choice - 1].y) {
							players[i][j].index = -1;
							players[i][j].x = houses[i][j].x;
							players[i][j].y = houses[i][j].y;
						}
					}
				}
			}
		}

		turn = (turn + 1) % nbPlayer;
	}
	return 7;
}

//-----------------------------------------------------------------------------
// UTILS
//-----------------------------------------------------------------------------

void verifyInput() {
	if (!cin)
	{
		cout << "ERREUR - Entrez un nombre valide";

		cin.clear();

		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
}

string idToColor(int id) {
	switch (id) {
	case 0:
		return "rouge";
	case 1:
		return "vert";
	case 2:
		return "bleu";
	case 3:
		return "jaune";
	default:
		return "";
	}
	return "";
}

int getIndexByTurn(int turn) {
	return turn * 10;
}

bool isFinalWay(int turn, int index) {
	switch (turn) {
	case 0:
		if (index == 38) {
			return true;
		}
		else {
			return false;
		}
	case 1:
		if (index == 8) {
			return true;
		}
		else {
			return false;
		}
	case 2:
		if (index == 18) {
			return true;
		}
		else {
			return false;
		}
	case 3:
		if (index == 28) {
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}


void moveOnFinalWay(Player players[MAX_PLAYER][4], int turn, int choice) {
	switch (turn) {
	case 0:
		players[turn][choice - 1].x += 1;
		break;
	case 2:
		players[turn][choice - 1].x -= 1;
		break;
	case 1:
		players[turn][choice - 1].y += 1;
		break;
	case 3:
		players[turn][choice - 1].y -= 1;
		break;
	}
}

//-----------------------------------------------------------------------------
// DISPLAY functions
//-----------------------------------------------------------------------------

void displayTitle() {
	cout << " __________________________________________________________" << endl;
	cout << "|     __              __         ______                    |" << endl;
	cout << "|    / /   __  ______/ /___     / ____/___ _____ ___  ___  |" << endl;
	cout << "|   / /   / / / / __  / __ \\   / / __/ __ `/ __ `__ \\/ _ \\ |" << endl;
	cout << "|  / /___/ /_/ / /_/ / /_/ /  / /_/ / /_/ / / / / / /  __/ |" << endl;
	cout << "| /_____/\\__,_/\\__,_/\\____/   \\____/\\__,_/_/ /_/ /_/\\___/  |" << endl;
	cout << "|__________________________________________________________|" << endl;
}

void displayCurrent(char board[SIZE][SIZE], Player players[MAX_PLAYER][4]) {
	char currentBoard[SIZE][SIZE];

	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++) {
			currentBoard[i][j] = board[i][j];
		}
	}
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		for (int j = 0; j < 4; j++) {
			currentBoard[players[i][j].y][players[i][j].x] = '0' + players[i][j].id;
		}
	}

	cout << endl;
    for (int i = 0; i < SIZE; i++) {
		cout << " ";
		for (int j = 0; j < SIZE; j++) {
			string s = "";
			s = s + currentBoard[i][j] + "  ";
			switch (currentBoard[i][j]) {
			case '1':
			case '2':
			case '3':
			case '4':
				s = currentBoard[i][j];
				s += " ";
				//cout << s;
				for (int a = 0; a < MAX_PLAYER; a++)
				{
					for (int b = 0; b < 4; b++) {
						if (i == players[a][b].y && j == players[a][b].x) {
							switch (players[a][b].team) {
							case 0:
								s = "r"+s;
								break;
							case 1:
								s = "g"+s;
								break;
							case 2:
								s = "b"+s;
								break;
							case 3:
								s = "y"+s;
								break;
							}
						}
					}
				}
				break;
			}
			//cout << s;
			cout << s;
		}
		cout << endl;
	}
	cout << endl;
}

void display(char board[SIZE][SIZE]) {
	cout << endl;
	for (int i = 0; i < SIZE; i++) {
		cout << " ";
		for (int j = 0; j < SIZE; j++) {
			string s = "";
			s = s + board[i][j] + " ";
			//cout << s;
			cout << s;
		}
		cout << endl;
	}
	cout << endl;
}
