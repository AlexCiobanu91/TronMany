// TronGineers
// Alexandru-Catalin Ciobanu
// Andrei Pavel
// Mihai Gradinescu
// Costin Pirosca


#include <iostream>
#include <vector>
#include <math.h>
#include <queue>
#include <ctime>
#include <cmath>
#include <cassert>
using namespace std;

#define Player1  'r'
#define Player2  'g'
#define ZID      '#'
#define Player1V 'R'
#define Player2V 'G'
#define NRMOVES   4
#define NRPLAYERS 2
#define MAXDEPTH  300
#define TIMEOUT   2.5
#define MAXSCORE  99999
#define REMIZA    0
#define WALL      99999
typedef enum {
  UP,
  LEFT,
  DOWN,
  RIGHT,
  NOMOVE,
} mutari;

typedef enum {
 BACKTRACK,
 MINIMAX
} strategie;

typedef struct pozitie {
  int posX;
  int posY;
} position;

typedef struct depthpozitie {
  int posX;
  int posY;
  int depth;
} depthPosition;

class board
{
  public:
    int sizeX_;
    int sizeY_;
    board();
    board(vector<string> teren, position myPosition,
                                position enemyPosition,
                                int sizeX, int sizeY);
    board    copyBoard();
    void     printBoard();

    bool     isEmpty(int direction, position playerPosition);
    bool     isMovePossible(int move, position playerPosition);
    bool     outOfBounds(int move, position playerPosition);
    void     resetPlayerPosition(int direction, int playerNo);

    void     doMove (int direction, int playerNo, char value);
    void     undoMove (int direction, int playerNo, char value);

    int      getEnemy(int playerNo);
    char     getPosition(int posX, int posY);
    char     getPosition(int move, position playerPosition);
    char     getPosition(position playerPosition);
    int      getPossibleMoves(position playerPosition);
    int      getDepthAfterMove(int move, position playerPosition);
    int      getDepth(position playerPosition);
    int      getDistance();
    position getPlayerPosition(int playerNo);
    position getPositionAfterMove(int move, position playerPosition);

    void     setPosition(int posX, int posY, int value);
    void     setPosition(position playerPosition, int value);
    void     setPlayerPosition(position playerPosition,int playerNo);
    void     setDepth(position playerPosition, int value);
    void     setDepth(int posX, int posY, int value);

  private:
    char** teren_;
    int** terenDepth_;
    position playerPosition_[3];
    vector<pair<int,int> > mutari_;
};

// initializare teren cu toti paramtrii nuli
board::board()
{
  sizeX_ = 0;
  sizeY_ = 0;
  mutari_.push_back(pair<int, int>(-1,0));
  mutari_.push_back(pair<int, int>(1,0));
  mutari_.push_back(pair<int, int>(0,1));
  mutari_.push_back(pair<int, int>(0,-1));
}

board::board(vector<string> teren, position redPosition,
                                   position greenPosition,
                                   int sizeX, int sizeY)
{
  sizeX_ = sizeX;
  sizeY_ = sizeY;
  teren_ = new char*[sizeX_];
  terenDepth_ = new int*[sizeX_];
  // initializare teren
  for (int i = 0; i < sizeX; i++) {
    teren_[i] = new char[sizeY];
    for (int j = 0; j < sizeY; j++) {
      teren_[i][j] = teren[i][j];
    }
  }

  for (int i = 0; i < sizeX_; i++) {
    terenDepth_[i] = new int[sizeY_];
    for (int j = 0; j < sizeY_; j++) {
      if (teren_[i][j] == '-') {
        terenDepth_[i][j] = 0;
      } else {
        terenDepth_[i][j] = WALL;
      }
    }
  }
  // pozitia mea primita de la server
  playerPosition_[1] = redPosition;
  // pozitia inamiclui primita de la server
  playerPosition_[2] = greenPosition;


  mutari_.push_back(pair<int, int>(-1,0));
  mutari_.push_back(pair<int, int>(1,0));
  mutari_.push_back(pair<int, int>(0,1));
  mutari_.push_back(pair<int, int>(0,-1));
}

// creeaza o copie a tebelei primita ca argument
board board::copyBoard()
{
  board tempBoard;
  tempBoard.sizeX_ = sizeX_;
  tempBoard.sizeY_ = sizeY_;

  tempBoard.playerPosition_[0] = playerPosition_[0];
  tempBoard.playerPosition_[1] = playerPosition_[1];
  tempBoard.playerPosition_[2] = playerPosition_[2];

  tempBoard.teren_ = new char*[sizeX_];
  for (int i = 0; i < sizeX_; i++) {
    tempBoard.teren_[i] = new char[sizeY_];
    for (int j = 0; j < sizeY_; j++) {
      tempBoard.teren_[i][j] = teren_[i][j];
    }
  }

  tempBoard.terenDepth_ = new int*[sizeX_];
  for (int i = 0; i < sizeX_; i++) {
    tempBoard.terenDepth_[i] = new int[sizeY_];
    for (int j = 0; j < sizeY_; j++) {
      tempBoard.terenDepth_[i][j] = terenDepth_[i][j];
    }
  }
  return tempBoard;
}

void board::printBoard()
{
    cerr << endl;
    for (int i = 0 ; i < sizeX_; i++) {
      cerr << teren_[i] << endl;
    }
    cerr << endl;
}

// verifica daca se poate face o miscare intr-o directie
bool board::isEmpty(int move, position playerPosition)
{
  int posX = playerPosition.posX;
  int posY = playerPosition.posY;
  switch (move) {
    case (UP)    : return teren_[posX - 1][posY] == '-';
    break;
    case (DOWN)  : return teren_[posX + 1][posY] == '-';
    break;
    case (LEFT)  : return teren_[posX][posY - 1] == '-';
    break;
    case (RIGHT) : return teren_[posX][posY + 1] == '-';
    break;
  }
  return true;
}

bool board::isMovePossible(int move, position playerPosition)
{
  int posX = playerPosition.posX;
  int posY = playerPosition.posY;

  switch (move) {
    case (UP)    : return (posX - 1 > 0) &&
                          (terenDepth_[posX - 1][posY] != WALL);
    break;
    case (DOWN)  : return (posX + 1 < sizeX_) &&
                          (terenDepth_[posX + 1][posY] != WALL);
    break;
    case (LEFT)  : return (posY - 1 > 0) &&
                          (terenDepth_[posX][posY - 1] != WALL);
    break;
    case (RIGHT) : return (posY + 1 < sizeY_) &&
                          (terenDepth_[posX][posY + 1] != WALL);
    break;
  }
  return true;

}

bool  board::outOfBounds(int move, position playerPosition)
{
  int posX = playerPosition.posX;
  int posY = playerPosition.posY;

  switch (move) {
    case (UP)    : return posX - 1 > 0;
    break;
    case (DOWN)  : return posX + 1 < sizeX_ ;
    break;
    case (LEFT)  : return posY - 1 > 0;
    break;
    case (RIGHT) : return posY + 1 < sizeY_;
    break;
  }
  return true;
}

// face update la pozitia motocicletei mele
void board::resetPlayerPosition(int move, int playerNo)
{
  switch (move) {
    case (UP)    : playerPosition_[playerNo].posX --;
    break;
    case (DOWN)  : playerPosition_[playerNo].posX ++;
    break;
    case (LEFT)  : playerPosition_[playerNo].posY --;
    break;
    case (RIGHT) : playerPosition_[playerNo].posX ++;
    break;
  }
}

void board::doMove (int move, int playerNo, char value)
{
  int posX = playerPosition_[playerNo].posX;
  int posY = playerPosition_[playerNo].posY;
  switch (move) {
    case (UP)    : teren_[posX - 1][posY] = value;
                   playerPosition_[playerNo].posX --;
    break;
    case (DOWN)  : teren_[posX + 1][posY] = value;
                   playerPosition_[playerNo].posX ++;
    break;
    case (LEFT)  : teren_[posX][posY - 1] = value;
                   playerPosition_[playerNo].posY --;
    break;
    case (RIGHT) : teren_[posX][posY + 1] = value;
                   playerPosition_[playerNo].posY ++;
    break;
  }
}

void board::undoMove (int move, int playerNo, char value)
{
  int posX = playerPosition_[playerNo].posX;
  int posY = playerPosition_[playerNo].posY;
  switch (move) {
    case (UP)    : teren_[posX + 1][posY] = value;
                   playerPosition_[playerNo].posX ++;
    break;
    case (DOWN)  : teren_[posX - 1][posY] = value;
                   playerPosition_[playerNo].posX --;
    break;
    case (LEFT)  : teren_[posX][posY + 1] = value;
                   playerPosition_[playerNo].posY ++;
    break;
    case (RIGHT) : teren_[posX][posY - 1] = value;
                   playerPosition_[playerNo].posY --;
    break;
  }
}
// adversarul jucatorului 1 este 0 si invers
int board::getEnemy(int playerNo)
{
  if (playerNo == 1) {
    return 2;
  }
  if (playerNo == 2) {
    return 1;
  }
  return 0;
}

char board::getPosition(int posX, int posY)
{
  return teren_[posX][posY];
}

// intoarce caracterul gasit la pozitia rezultata dupa efectuarea mutarii
char board::getPosition(int move, position playerPosition)
{
  int posX = playerPosition.posX;
  int posY = playerPosition.posY;
  switch (move) {
    case (UP)    : return teren_[posX - 1][posY];
    break;
    case (DOWN)  : return teren_[posX + 1][posY];
    break;
    case (LEFT)  : return teren_[posX][posY - 1];
    break;
    case (RIGHT) : return teren_[posX][posY + 1];
    break;
  }
  return 'O';
}

// la fiecare mutare
// verifica cate mutari poate face inamicul
int board::getPossibleMoves(position playerPosition)
{
  int mutariPosibile = 0;
  for (int i = 0 ; i < 4 ; i++) {
    if (isEmpty(i, playerPosition)) {
      mutariPosibile++;
    }
  }
  return mutariPosibile;
}

char board::getPosition(position playerPosition)
{
  int posX = playerPosition.posX;
  int posY = playerPosition.posY;
  return teren_[posX][posY];
}

// intoarce pozitia unui jucator pe ambele coordonate
position board::getPlayerPosition(int playerNo)
{
  return playerPosition_[playerNo];
}

position board::getPositionAfterMove(int move, position playerPosition)
{
  switch (move) {
    case (UP)    : playerPosition.posX --;
    break;
    case (DOWN)  : playerPosition.posX ++;
    break;
    case (LEFT)  : playerPosition.posY --;
    break;
    case (RIGHT) : playerPosition.posY ++;
    break;
  }
  return playerPosition;
}

int board::getDepthAfterMove(int move, position playerPosition)
{
  int posX = playerPosition.posX;
  int posY = playerPosition.posY;
  switch (move) {
    case (UP)    : return terenDepth_[posX - 1][posY];
    break;
    case (DOWN)  : return terenDepth_[posX + 1][posY];
    break;
    case (LEFT)  : return terenDepth_[posX][posY - 1];
    break;
    case (RIGHT) : return terenDepth_[posX][posY + 1];
    break;
  }
  return 0;
}

int board::getDepth(position playerPosition)
{
  int posX = playerPosition.posX;
  int posY = playerPosition.posY;
  return terenDepth_[posX][posY];
}

int board::getDistance()
{
  int playerPosX = playerPosition_[1].posX;
  int playerPosY = playerPosition_[1].posY;
  int enemyPosX = playerPosition_[2].posX;
  int enemyPosY = playerPosition_[2].posY;
  return abs(playerPosX - enemyPosX) + abs(playerPosY - enemyPosY);
}
// seteaza o pozitie pe harta ca fiind ocupata cu
// un caracter specific fiecarui jucator
void board::setPosition(int posX, int posY, int value)
{
  teren_[posX][posY] = value;
}

// ca si functia de mai sus daor ca va seta alt set de caractere
// si va fi folosita doar pentru generarea unei solutii
// in cadrul euristicii Voronoi
void board::setPosition(position playerPosition, int value)
{
  int posX = playerPosition.posX;
  int posY = playerPosition.posY;
  teren_[posX][posY] = value;
}

void board::setPlayerPosition(position playerPosition,int playerNo)
{
  playerPosition_[playerNo] = playerPosition;
}

void board::setDepth(position playerPosition, int value)
{
  int posX = playerPosition.posX;
  int posY = playerPosition.posY;
  terenDepth_[posX][posY] = value;
}

void board::setDepth(int posX, int posY, int value)
{
  terenDepth_[posX][posY] = value;
}

class miniMax
{
  public:
    miniMax();
    miniMax(int maxDepth);
    int  getSoloPunctaj(board someBoard, int playerNo);
    int  getConnectedPunctaj(board someBoard, int playerNo);
    int  connectedVoronoi(board boardVoronoi, int playerNo);
    int  soloVoronoi(board someBoard, int playerNo);
    bool getPlayersConnected(board someBoard, int playerNo);
    int  maxMiniMax(board someBoard, int playerNo, int alpha, int beta,
                                     int currentDepth, time_t startTime);
    int  minMiniMax(board someBoard, int playerNo, int alpha, int beta,
                                     int currentDepth, time_t startTime);
    int  soloMiniMax(board someBoard, int playerNo, int currentDepth,
                                                    time_t startTime);
    int  nextMove(board someBoard, int playerNo, time_t startTime);
    int  tieMove(board someBoardm, int playerNo);

  private:
    int maxDepth_;

    void printErr(string msg) {
      cerr << msg << endl;
    }
};
miniMax::miniMax()
{
  maxDepth_ = 0;
}

miniMax::miniMax(int maxDepth)
{
  maxDepth_ = maxDepth;
}

// intoarce un punctaj dupa euristica Voronoi
// pentru cazul in care cei doi jucatori nu se intalnesc
int miniMax::getSoloPunctaj(board someBoard, int playerNo)
{
  board tempBoard = someBoard.copyBoard();
  return soloVoronoi(tempBoard, playerNo);
}

// intoarce punctajul corespunzator pentru cazul
// in care cei doi jucatori se intalnesc
int miniMax::getConnectedPunctaj(board someBoard, int playerNo)
{
  board tempBoard = someBoard.copyBoard();
  // int enemyNo = tempBoard.getEnemy(playerNo);
  position myPosition    = tempBoard.getPlayerPosition(playerNo);
  // position enemyPosition = tempBoard.getPlayerPosition(enemyNo);
  // int posX      = myPosition.posX;
  // int posY      = myPosition.posY;
  // int enemyPosX = enemyPosition.posX;
  // int enemyPosY = enemyPosition.posY;

  // cand distanta < 3 
  if (tempBoard.getDistance() <= 3) {
    // calculam numarul de pozitii disponibile
    int posMoves = someBoard.getPossibleMoves(myPosition);
    // daca distanta == 1 (remiza posibila)
    if (tempBoard.getDistance() == 1){
      // am mai multe posibilitati
      if (posMoves > 1){
        // calculez pos cea mai buna
        int punctajVoronoi = 2 * connectedVoronoi(tempBoard, playerNo);
        return punctajVoronoi;  // getSoloPunctaj(tempBoard, playerNo);
      } else {
        // facem remiza 
        return MAXSCORE;
      }
    } else if (posMoves <= 0) {
      // blocat 
      return -MAXSCORE;
    } else {
      // joc normal
      int punctajVoronoi = 2 * connectedVoronoi(tempBoard, playerNo);
      return punctajVoronoi;
    }
  } else {
    int punctajVoronoi = 2 * connectedVoronoi(tempBoard, playerNo);
    return punctajVoronoi;
  }
}

// euristica Voronoi aplicata pentru un singur jucator
int miniMax::connectedVoronoi(board someBoard, int playerNo)
{
  int enemyNo = someBoard.getEnemy(playerNo);
  position playerPosition = someBoard.getPlayerPosition(playerNo);
  position enemyPosition = someBoard.getPlayerPosition(enemyNo);
  int countMe        = 0;
  int countEn        = 0;
  int countXred      = 0;
  int countXblack    = 0;
  int countYred      = 0;
  int countYblack    = 0;
  int currentDepth   = 0;
  int workingDepth   = 0;
  queue<position> positionQueue;
  queue<int>      depthQueue;
  someBoard.setDepth(playerPosition, currentDepth);
  someBoard.setDepth(enemyPosition, -1);
  positionQueue.push(playerPosition);
  depthQueue.push(currentDepth);
  // pentru fiecare pozitie din coada
  while (!positionQueue.empty()) {
    position crtPosition = positionQueue.front();
    currentDepth = depthQueue.front() + 1;
    depthQueue.pop();
    positionQueue.pop();
    // verificam pozitiile in care se poate ajunge
    for (int crtMove = 0; crtMove < NRMOVES; crtMove++) {
      someBoard.setPlayerPosition(crtPosition, playerNo);
      workingDepth = currentDepth;
      if (someBoard.isEmpty(crtMove, crtPosition)) {
        countMe++;
        someBoard.doMove(crtMove, playerNo, 'R');
        position newPosition = someBoard.getPlayerPosition(playerNo);
        someBoard.setDepth(newPosition, workingDepth);
        int newPosX = newPosition.posX;
        int newPosY = newPosition.posY;
        // le introducem in coada si pe acestea
        positionQueue.push(newPosition);
        depthQueue.push(workingDepth);
        // numaram cate pozitii  din fiecare culoare gasim
        if ((newPosX % 2 == 1 && newPosY % 2 == 1) ||
            (newPosX % 2 == 0 && newPosY % 2 == 0)) {
          countXred++;
        } else {
          countXblack++;
        }
      }
    }
  }
  // cerr << "R turn" << endl;
  // someBoard.printBoard();
  cerr << "Voronoi countX1: " << countMe << endl;
  assert(countXred + countXblack == countMe);

  currentDepth = 0;

  positionQueue.push(enemyPosition);
  depthQueue.push(currentDepth);
  someBoard.setDepth(enemyPosition, currentDepth);
  // pentru fiecare pozitie din coada
  while (!positionQueue.empty()) {
    position crtPosition = positionQueue.front();
    currentDepth = depthQueue.front() - 1;
    depthQueue.pop();
    positionQueue.pop();
    // verificam pozitiile in care se poate ajunge
    for (int crtMove = 0; crtMove < NRMOVES; crtMove++) {
      someBoard.setPlayerPosition(crtPosition, enemyNo);
      workingDepth = currentDepth;
      if (someBoard.isEmpty(crtMove, crtPosition)) {
        someBoard.doMove(crtMove, enemyNo, 'G');
        position newPosition = someBoard.getPlayerPosition(enemyNo);
        someBoard.setDepth(newPosition, workingDepth);
        int newPosX = newPosition.posX;
        int newPosY = newPosition.posY;
        // le introducem in coada si pe acestea
        positionQueue.push(newPosition);
        depthQueue.push(workingDepth);
        // numaram cate pozitii  din fiecare culoare gasim
        countEn++;
        if ((newPosX % 2 == 1 && newPosY % 2 == 1) ||
            (newPosX % 2 == 0 && newPosY % 2 == 0)) {
          countYred++;
        } else {
          countYblack++;
        }
      } else {
        if (someBoard.isMovePossible(crtMove, crtPosition) &&
                 someBoard.getDepthAfterMove(crtMove, crtPosition)
                 > -(currentDepth - 1)) {
          someBoard.doMove(crtMove, enemyNo, 'G');
          position newPosition = someBoard.getPlayerPosition(enemyNo);
          someBoard.setDepth(newPosition, currentDepth);
          countMe--;
          countEn++;
          int newPosX = newPosition.posX;
          int newPosY = newPosition.posY;
          // le introducem in coada si pe acestea
          positionQueue.push(newPosition);
          depthQueue.push(currentDepth);
          // numaram cate pozitii  din fiecare culoare gasim
          if ((newPosX % 2 == 1 && newPosY % 2 == 1) ||
              (newPosX % 2 == 0 && newPosY % 2 == 0)) {
            countXred--;
            countYred++;
          } else {
            countYblack++;
            countXblack--;
          }
        } else if (someBoard.isMovePossible(crtMove, crtPosition) &&
                   someBoard.getDepthAfterMove(crtMove, crtPosition)
                   == -(currentDepth - 1)) {
          countMe--;
          someBoard.doMove(crtMove, enemyNo, 'G');
          position newPosition = someBoard.getPlayerPosition(enemyNo);
          someBoard.setDepth(newPosition, currentDepth);
          int newPosX = newPosition.posX;
          int newPosY = newPosition.posY;
          // le introducem in coada si pe acestea
          positionQueue.push(newPosition);
          depthQueue.push(currentDepth);
          // numaram cate pozitii  din fiecare culoare gasim
          if ((newPosX % 2 == 1 && newPosY % 2 == 1) ||
             (newPosX % 2 == 0 && newPosY % 2 == 0)) {
            countXred--;
          } else {
            countXblack--;
          }
        }
      }
    }
  }
  // someBoard.printBoard();
  // generam numarul total de pozitii accesibile
  // jucatorului nostru
  assert(countXred + countXblack == countMe);
  assert(countYred + countYblack == countEn);
 // cerr << "Voronoi countY1: " << countEn << endl;

  // cerr << "G turn" << endl;
  // someBoard.printBoard();
  // int posX = playerPosition.posX;
  // int posY = playerPosition.posY;

  //int countX = 0, countY = 0;
  //if ((posX % 2 == 1 && posY % 2 == 1) ||
  //    (posX % 2 == 0 && posY % 2 == 0)) {
  //  // red
  //  countX = 1 + (2 * min(countXblack - 1, countXred));
  //} else {
   // // black
  //  countX = 1 + (2 * min(countXblack, countXred - 1));
  // }

  // generam numarul total de pozitii accesibile
  // adversarului
  // posX = enemyPosition.posX;
  // posY = enemyPosition.posY;

  //if ((posX % 2 == 1 && posY % 2 == 1) ||
  //    (posX % 2 == 0 && posY % 2 == 0)) {
  //  // red
  //  countY = 1 + (2 * min(countYblack - 1, countYred));
  //} else {
    // black
  //  countY = 1 + (2 * min(countYblack, countYred - 1));
  //}

  //countX = min(countX, (countXred + countXblack));
  //countY = min(countY, (countYred + countYblack));
  //countX = max(countX, 0);
  //countY = max(countY, 0);
  //cerr << "Voronoi countX2: " << countX << endl;
  //cerr << "Voronoi countY2: " << countY << endl;
  return countMe - countEn;
}

// euristica Voronoi aplicata pentru un singur jucator
int miniMax::soloVoronoi(board someBoard, int playerNo)
{
  position playerPosition = someBoard.getPlayerPosition(playerNo);
  int count       = 0;
  int countXred   = 0;
  int countXblack = 0;
  // int posX = playerPosition.posX;
  // int posY = playerPosition.posX;

  queue<position> positionQueue;
  positionQueue.push(playerPosition);
  // pentru fiecare pozitie din coada
  while (!positionQueue.empty()) {
    position crtPosition = positionQueue.front();
    positionQueue.pop();
    // verificam pozitiile in care se poate ajunge
    for (int crtMove = 0; crtMove < NRMOVES; crtMove++) {
      someBoard.setPlayerPosition(crtPosition, playerNo);
      if (someBoard.isEmpty(crtMove, crtPosition)) {
        count++;
        someBoard.doMove(crtMove, playerNo, '#');
        position newPosition = someBoard.getPlayerPosition(playerNo);
        int newPosX = newPosition.posX;
        int newPosY = newPosition.posY;
        // le introducem in coada si pe acestea
        positionQueue.push(newPosition);
        // numaram cate pozitii  din fiecare culoare gasim
        if ((newPosX % 2 == 1 && newPosY % 2 == 1) ||
            (newPosX % 2 == 0 && newPosY % 2 == 0)) {
          countXred++;
        } else {
          countXblack++;
        }
      }
    }
  }

  assert(countXred + countXblack == count);

  // generam numarul total de pozitii accesibile
  // jucatorului nostru
  //int countX = 0;
  // if ((posX % 2 == 1 && posY % 2 == 1) ||
  //    (posX % 2 == 0 && posY % 2 == 0)) {
    // red
  //  countX = 1 + (2 * min(countXblack - 1, countXred));
  //} else {
    // black
  //  countX = 1 + (2 * min(countXblack, countXred - 1));
  //}
  // countX = min(countX, (countXred + countXblack));
  // countX = max(countX, 0);
  return count;
}

bool miniMax::getPlayersConnected(board someBoard, int playerNo)
{
  int enemyNo = someBoard.getEnemy(playerNo);
  position playerPosition = someBoard.getPlayerPosition(playerNo);
  position enemyPosition = someBoard.getPlayerPosition(enemyNo);
  int enemyPosX = enemyPosition.posX;
  int enemyPosY = enemyPosition.posY;
  cerr << "finding if players are connected" << endl;
  // someBoard.printBoard();
  queue<position> queue;
  queue.push(playerPosition);
  while (queue.size() > 0) {
    position crtPosition = queue.front();
    queue.pop();
    for (int crtMove = 0; crtMove < NRMOVES; crtMove++) {
      someBoard.setPlayerPosition(crtPosition, playerNo);
      if (someBoard.isEmpty(crtMove, crtPosition)) {
        someBoard.doMove(crtMove, playerNo, ZID);
        // cerr << "new board" << endl;
        // someBoard.printBoard();
        position newPosition = someBoard.getPlayerPosition(playerNo);
        queue.push(newPosition);
      } else {
        board copyBoard = someBoard.copyBoard();
        copyBoard.doMove(crtMove, playerNo, ZID);
        position newPosition = copyBoard.getPlayerPosition(playerNo);
        int newPosX = newPosition.posX;
        int newPosY = newPosition.posY;
        if (newPosX == enemyPosX && newPosY == enemyPosY) {
          return true;
        }
      }
    }
  }
  return false;
}

// cauta cea mai buna mutare pentru jucatorul max
int miniMax::maxMiniMax(board someBoard, int playerNo, int alpha, int beta,
                                         int currentDepth, time_t startTime)
{
  int enemyNo = someBoard.getEnemy(playerNo);
  int outcome = 0;
  time_t crtTime;
  time(&crtTime);
  int maxOutcome = -MAXSCORE;
  position playerPosition = someBoard.getPlayerPosition(playerNo);

  // daca am consumat prea mult timp oprim generarea
  // arborelui
  if (difftime(crtTime,startTime) > TIMEOUT) {
    outcome = getConnectedPunctaj(someBoard, playerNo);
    return outcome;
  }

  // daca am ajuns la adancimea maxima
  if (currentDepth >= maxDepth_) {
    outcome = getConnectedPunctaj(someBoard, playerNo);
    return outcome;
  }
  // generam cate un arbore pentru fiecare mutare
  for (int crtMove = 0; crtMove < NRMOVES; crtMove++) {
    if (someBoard.isEmpty(crtMove,playerPosition)) {
      //board tempBoard = someBoard.copyBoard();
      someBoard.doMove(crtMove, playerNo, ZID);
      position newPosition = someBoard.getPlayerPosition(playerNo);
      someBoard.setDepth(newPosition, WALL);
      outcome = minMiniMax(someBoard, enemyNo, alpha, beta,
                                        currentDepth + 1, startTime);
      // am gasit o mutare mai buna
      if (outcome >= beta) {
        return outcome;
      }

      if (outcome > alpha) {
        alpha = outcome;
      }

      if (outcome > maxOutcome) {
        maxOutcome = outcome;
      }
      someBoard.undoMove(crtMove, playerNo, '-');
      someBoard.setDepth(newPosition, 0);
    }
  }
  // intoarcem punctajul pentru cea mai buna mutare
  return maxOutcome;
}


// cauta cea mai buna mutare pentru jucatorul min
int miniMax::minMiniMax(board someBoard, int playerNo, int alpha, int beta,
                                         int currentDepth, time_t startTime)
{
  int enemyNo = someBoard.getEnemy(playerNo);
  int outcome = 0;
  time_t crtTime;
  time(&crtTime);
  int minOutcome = MAXSCORE;
  position playerPosition = someBoard.getPlayerPosition(playerNo);

  // generam cate un arbore pentru fiecare mutare
  for (int crtMove = 0; crtMove < NRMOVES; crtMove++) {
    if (someBoard.isEmpty(crtMove,playerPosition)) {
      // board tempBoard = someBoard.copyBoard();
      someBoard.doMove(crtMove, playerNo, ZID);
      position newPosition = someBoard.getPlayerPosition(playerNo);
      someBoard.setDepth(newPosition, WALL);
      outcome = maxMiniMax(someBoard, enemyNo, alpha, beta,
                                        currentDepth + 1, startTime);
      if (outcome <= alpha) {
        return outcome;
      }

      // am gasit o mutare mai buna
      if (outcome < beta) {
        beta = outcome;
      }

      if (outcome < minOutcome) {
        minOutcome = outcome;
      }
      someBoard.undoMove(crtMove, playerNo, '-');
      someBoard.setDepth(newPosition, 0);
    }
  }
  // intoarcem punctajul pentru cea mai buna mutare
  return minOutcome;
}

// cauta cea mai buna mutare neluand in considerare
// celalalt jucator
int miniMax::soloMiniMax(board someBoard, int playerNo, int currentDepth,
                                                        time_t startTime)
{
  int outcome = 0;
  time_t crtTime;
  time(&crtTime);
  int bestScore = 0;
  position playerPosition = someBoard.getPlayerPosition(playerNo);

  // daca am consumat prea mult timp oprim generarea arborelui
  if (difftime(crtTime, startTime) > TIMEOUT) {
    cerr << "calculating outcome" << endl;  
    outcome = getSoloPunctaj(someBoard, playerNo);
    cerr << "finished calculating outcome" << endl;
    return outcome;
  }

  // daca am ajuns la adancimea maxima
  if (currentDepth >= maxDepth_) {
    outcome = getSoloPunctaj(someBoard, playerNo);
    return outcome;
  }
    cerr << "check3 " << endl;
  // generam cate un arbore pentru fiecare mutare
  for (int crtMove = 0; crtMove < NRMOVES; crtMove++) {
    if (someBoard.isEmpty(crtMove, playerPosition)) {
      //board tempBoard = someBoard.copyBoard();
      someBoard.doMove(crtMove,playerNo,ZID);
      outcome = soloMiniMax(someBoard, playerNo, currentDepth + 1, startTime);
      // am gasit o mutare mai buna
      if (outcome > bestScore) {
        bestScore = outcome;
      }
      someBoard.undoMove(crtMove, playerNo, '-');
    }
  }
  // intoarcem punctajul pentru cea mai buna mutare
  return bestScore;
}
// alege urmatoarea mutare in functie de pozitiile
// celor doi jucatori
int miniMax::nextMove(board someBoard, int playerNo, time_t startTime)
{
  int goodMove     = 0;
  int outcome      = 0;
  int outcomeSoFar = 0;
  int currentDepth = 0;
  board tempBoard = someBoard.copyBoard();
  position playerPosition = someBoard.getPlayerPosition(playerNo);
  // daca se intalnesc aleg o mutare care maximizeaza numarul
  // de poztii disponibile pentru jucatorul meu si
  // minimizeaza numarul de pozitii disponibile pentru adversar
  //cerr << "trying to choose a move" << endl;
  if (getPlayersConnected(tempBoard, playerNo)) {
    cerr << "we are connected" << endl;
    for (int crtMove = 0; crtMove < NRMOVES; crtMove++) {
      if (someBoard.isEmpty(crtMove, playerPosition)) {
        //board tempBoard = someBoard.copyBoard();
        someBoard.doMove(crtMove, playerNo, ZID);
        position newPosition = someBoard.getPlayerPosition(playerNo);
        tempBoard.setDepth(newPosition, WALL);
        outcome = maxMiniMax(someBoard, playerNo, currentDepth, -MAXSCORE,
                                                  MAXSCORE, startTime);
        // verificam daca solutia partiala e cea mai buna de
        // pana acum si o memoram daca e cazul
        cerr << "Both outcome: " << outcome << endl;
        cerr << "Move: " << crtMove << endl;
        if (outcome > outcomeSoFar) {
          outcomeSoFar = outcome;
          goodMove = crtMove;
        }
        someBoard.undoMove(crtMove, playerNo, '-');
      }
    }
    if ( outcomeSoFar == 0 ) {
      for (int crtMove = 0; crtMove < NRMOVES; crtMove++) {
        if (someBoard.isEmpty(crtMove, playerPosition)) {
          goodMove = crtMove;
          break;
        }
      }
    }
  } else {
    cerr << "we are not connected" << endl;
    // daca cei doi jucatori nu se intalnesc aleg o mutare
    // care maximizeaza numarul de pozitii ocupate
    for (int crtMove = 0; crtMove < NRMOVES; crtMove++) {
      if (someBoard.isEmpty(crtMove, playerPosition)) {
        //board tempBoard = someBoard.copyBoard();
        someBoard.doMove(crtMove, playerNo, ZID);
        outcome = soloMiniMax(someBoard, playerNo, currentDepth, startTime);
        cerr << "Solo outcome: " << outcome << endl;
        // verificam daca solutia partiala e cea mai buna de
        // pana acum si o memoram daca e cazul
        if (outcome > outcomeSoFar) {
          outcomeSoFar = outcome;
          goodMove = crtMove;
        }
        someBoard.undoMove(crtMove, playerNo, '-');
      }
    }
    if ( outcomeSoFar == 0 ) {
      for (int crtMove = 0; crtMove < NRMOVES; crtMove++) {
        if (someBoard.isEmpty(crtMove, playerPosition)) {
          goodMove = crtMove;
          break;
        }
      }
    }
  }
  return goodMove;
}

void makeMove(int mutareUrmatoare) {
  switch (mutareUrmatoare) {
    case (UP)    : cout << "UP" <<endl;
    break;
    case (DOWN)  : cout << "DOWN" <<endl;
    break;
    case (LEFT)  : cout << "LEFT" <<endl;
    break;
    case (RIGHT) : cout << "RIGHT" <<endl;
    break;
  }
}

int main()
{
  char player;
  int sizeX = 0;
  int sizeY = 0;
  time_t startTime;
  time_t endTime;
  int playerNo = 0;
  time(&startTime);
  position redPosition;
  position greenPosition;
  vector<string> teren;
  board* initialBoard;

  cin >> player;
  cin >> redPosition.posX   >> redPosition.posY;
  cin >> greenPosition.posX >> greenPosition.posY;
  cin >> sizeX;
  cin >> sizeY;


  for (int i = 0; i < sizeX; i++) {
    string s; cin >> s;
    teren.push_back(s);
  }

  if (player == 'g') {
  cerr << "I am " << player << endl;
  cerr << "PosX " << greenPosition.posX << endl;
  cerr << "PosY " << greenPosition.posY << endl;
    playerNo = 2;
  } else {
  cerr << "I am " << player << endl;
  cerr << "PosX " << redPosition.posX << endl;
  cerr << "PosY " << redPosition.posY << endl;
    playerNo = 1;
  }
  initialBoard = new board(teren, redPosition, greenPosition, sizeX, sizeY);
  //initialBoard->printBoard();
  miniMax* minMax;
  minMax = new miniMax(MAXDEPTH);
  int mutareUrmatoare = 0;
  mutareUrmatoare = minMax->nextMove(*initialBoard, playerNo, startTime);
  makeMove(mutareUrmatoare);
  time(&endTime);
  cerr << "Time to make move:" << difftime(endTime,startTime) <<endl;
  return 0;
}


