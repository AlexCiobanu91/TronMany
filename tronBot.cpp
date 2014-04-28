// TronGineers
// Alexandru-Catalin Ciobanu
// Andrei Pavel
// Mihai Gradinescu
// Costin Pirosca


#include <iostream>
#include <vector>
#include <math.h>
#include <queue>
using namespace std;

#define Player1 'r'
#define Player2 'g'
#define NRMOVES  4
#define MAXDEPTH 10
#define MAXSCORE 99999
#define REMIZA   30000
typedef enum {
  UP,
  DOWN,
  RIGHT,
  LEFT,
  PLAYDEAD
} mutari;

typedef enum {
 BACKTRACK,
 MINIMAX
} strategie;

typedef struct pozitie {
  int posX;
  int posY;
} position;


class board
{
  private:
    vector<string> teren_;
    position myPosition_;
    position enemyPosition_;
    vector<pair<int,int> > mutari_;

    int distantaIntreNoi_;
    int sizeX_;
    int sizeY_;

  public:
    // initializare teren cu toti paramtrii nuli
    board()
    {
      distantaIntreNoi_ = 0;
      sizeX_ = 0;
      sizeY_ = 0;
      mutari_.push_back(pair<int, int>(-1,0));
      mutari_.push_back(pair<int, int>(1,0));
      mutari_.push_back(pair<int, int>(0,1));
      mutari_.push_back(pair<int, int>(0,-1));
    }

    board(vector<string> teren,position myPosition,
                               position enemyPosition,
                               int sizeX, int sizeY) {
      sizeX_ = sizeX;
      sizeY_ = sizeY;
      // initializare teren 
      for (int i = 0; i < sizeX; i++) {
        teren_.push_back(teren[i]);
      }
      // pozitia mea primita de la server
      myPosition_.posX= myPosition.posX;
      myPosition_.posY= myPosition.posY;

      // pozitia inamiclui primita de la server
      enemyPosition_.posX= enemyPosition.posX;
      enemyPosition_.posY= enemyPosition.posY;
      updateDistance();

      mutari_.push_back(pair<int, int>(-1,0));
      mutari_.push_back(pair<int, int>(1,0));
      mutari_.push_back(pair<int, int>(0,1));
      mutari_.push_back(pair<int, int>(0,-1));
    }

    // creeaza o copie a tebelei primita ca argument
    board copyBoard() {
      board tempBoard;
      tempBoard.teren_            = teren_;
      tempBoard.myPosition_       = myPosition_;
      tempBoard.enemyPosition_    = enemyPosition_;
      tempBoard.distantaIntreNoi_ = distantaIntreNoi_;
      tempBoard.updateDistance();
      return tempBoard;
    }

    // verifica daca se poate face o miscare intr-o directie
    bool isEmpty(int direction, position playerPosition) {
      int posX = playerPosition.posX;
      int posY = playerPosition.posY;
      switch (direction) {
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

    // face update la pozitia motocicletei mele
    void resetPlayerPosition(int direction, int playerNo) {
      switch (playerNo) {
        case (1) :
          switch (direction) {
            case (UP)    : myPosition_.posX --;
            break;
            case (DOWN)  : myPosition_.posX ++;
            break;
            case (LEFT)  : myPosition_.posY --;
            break;
            case (RIGHT) : myPosition_.posY ++;
            break;
          }
        break;
        case (2) :
          switch (direction) {
            case (UP)    : enemyPosition_.posX --;
            break;
            case (DOWN)  : enemyPosition_.posX ++;
            break;
            case (LEFT)  : enemyPosition_.posY --;
            break;
            case (RIGHT) : enemyPosition_.posY ++;
            break;
          }
        break;
      }
    }

    int getEnemy(int playerNo) {
      return 1 - playerNo;
    }

    // coordonata pe X
    int getPlayerPositionX(int playerNo) {
      return playerNo == 1 ? myPosition_.posX : enemyPosition_.posX;
    }

    // coordonata pe Y
    int getPlayerPositionY(int playerNo) {
      return playerNo == 1 ? myPosition_.posY : enemyPosition_.posY;
    }

    // intoarce pozitia unui jucator pe ambele coordonate
    position getPlayerPosition(int playerNo) {
      return playerNo == 1 ? myPosition_ : enemyPosition_;
    }

    // calculeaza distanta intre cele doua motociclete
    void updateDistance() {
      int x1 = myPosition_.posX;
      int y1 = myPosition_.posY;
      int x2 = enemyPosition_.posX;
      int y2 = enemyPosition_.posY;
      distantaIntreNoi_ = sqrt((x1 - x2) * (x1 - x2) +
                               (y1 - y2) * (y1 - y2));
    }

    int getDistance() {
      return distantaIntreNoi_;
    }

    // la fiecare mutare 
    // verifica cate mutari poate face
    // ale inamicului
    int getPossibleMoves(position playerPosition) {
      int mutariPosibile = 0;
      for (int i; i < NRMOVES ; i++) {
        if (isEmpty(i, playerPosition)) {
          mutariPosibile++;
        }
      }
      return mutariPosibile;
    }

    // wrapper peste functia de mai sus care intoarce 
    // numarul de mutari posibile pentru coordonate pe X si pe Y
    int getPossibleMoves(int posX, int posY) {
      position positionTemp;
      positionTemp.posX = posX;
      positionTemp.posY = posY;
      return getPossibleMoves(positionTemp);
    }

    // aplica o mutare pentru playerul primit ca parametru
    // ocupanda tabla cu o graza de M inutile
    // caracterul 'M' din cuvantul "zid"
    void doMove (int direction,int playerNo) {
      int posY = 0;
      int posX = 0;
      switch (playerNo) {
        case (1) :
          posX = myPosition_.posX;
          posY = myPosition_.posY;
          switch (direction) {
            case (UP)    : teren_[posX - 1][posY] = 'M';
                           resetPlayerPosition(UP, playerNo);
            break;
            case (DOWN)  : teren_[posX + 1][posY] = 'M';
                           resetPlayerPosition(DOWN, playerNo);
            break;
            case (LEFT)  : teren_[posX][posY - 1] = 'M';
                           resetPlayerPosition(LEFT, playerNo);
            break;
            case (RIGHT) : teren_[posX][posY + 1] = 'M';
                           resetPlayerPosition(RIGHT, playerNo);
            break;
          }
        break;
        case (2) :
          posX = myPosition_.posX;
          posY = myPosition_.posY;
          switch (direction) {
            case (UP)    : teren_[posX - 1][posY] = 'M';
                           resetPlayerPosition(UP, playerNo);
            break;
            case (DOWN)  : teren_[posX + 1][posY] = 'M';
                           resetPlayerPosition(DOWN, playerNo);
            break;
            case (LEFT)  : teren_[posX][posY - 1] = 'M';
                           resetPlayerPosition(LEFT, playerNo);
            break;
            case (RIGHT) : teren_[posX][posY + 1] = 'M';
                           resetPlayerPosition(RIGHT, playerNo);
            break;
          }
        break;
      }
    }
    // calculeaza numarul de casute la care se poate ajunge din
    // pozita curenta (DFS)
    int getReachable(position playerPosition)
    {
      bool visited[sizeX_][sizeY_];
      cerr <<"ajunge aici2" <<endl;
      int noReach = 0;
      int posX = playerPosition.posX;
      int posY = playerPosition.posY;
      visited[posX][posY] = true;
      queue<position> coadaMutari;
      coadaMutari.push(playerPosition);

      while (!coadaMutari.empty()) {
        position crtPosition = coadaMutari.front();
        coadaMutari.pop();
        noReach ++;
        for (int crtMove = 0; crtMove < NRMOVES; crtMove++) {
          posX = crtPosition.posX;
          posY = crtPosition.posY;
          if (isEmpty(crtMove, crtPosition)) {
            posX += mutari_[crtMove].second;
            posY += mutari_[crtMove].first;

            if (!visited[posX][posY]) {
              position tempPosition;
              tempPosition.posX = posX;
              tempPosition.posY = posY;
              coadaMutari.push(tempPosition);
              visited[posX][posY] = true;
            }
          }
        }
      }
      return noReach;
    }
};

class backtrack {

  private:

    vector<int> mutari_;
    int         maxDepth_;

  public:
    backtrack(int maxDepth) 
    {
      maxDepth_ = maxDepth;
    }

    int getMovePoints(board previousBoard, int currentDepth,
                      vector<int> mutari) {
      position posXYEnemy = previousBoard.getPlayerPosition(2);
      position posXYMyPlayer = previousBoard.getPlayerPosition(1);
      if (currentDepth >= maxDepth_ ||
          previousBoard.getPossibleMoves(posXYEnemy) == 0 ||
          previousBoard.getPossibleMoves(posXYMyPlayer) == 0
          ) {
        //return getPoints(previousBoard, mutari, 1 /*Player1*/);
          return previousBoard.getReachable(posXYMyPlayer);
      } else {
        int maxProfit = 0;
        for (int i = 0; i < NRMOVES; i++) {
          int eachProfit = 0;
          board tempBoard = previousBoard.copyBoard();
          position myPosition = tempBoard.getPlayerPosition(1);
          if (tempBoard.isEmpty(i, myPosition)) {
            tempBoard.doMove(i,1);
            mutari.push_back(i);
            eachProfit = getMovePoints(tempBoard, currentDepth + 1, mutari);
            if (eachProfit > maxProfit) {
              maxProfit = eachProfit;
            }
            mutari.pop_back();
          }
        }
        return maxProfit;
      }
    }

    // acorda puncte in functie de cate mutari 
    // posibile sunt din acea pozitie
    int awardPoints(int howMany) {
      switch (howMany) {
        case (0):  return 50;
        break;
        case (1): return 100;
        break;
        case (2): return 200;
        break;
        case (3): return 300;
        break;
        case(4): return 400;
        break;
      }
      return 0;
    }

    // merge in sens invers al parcurgerii miscarilor si calculeaza
    // un punctaj pentru fiecare mutare
    int getPoints(board someBoard, vector<int> mutari, int playerNo) {
      int punctajFinal = 0;
      int posX = someBoard.getPlayerPositionX(playerNo);
      int posY = someBoard.getPlayerPositionY(playerNo);
      position posXY =  someBoard.getPlayerPosition(playerNo);
      int mutariPosibile = someBoard.getPossibleMoves(posXY);
      int punctajTemp = awardPoints(mutariPosibile);
      punctajFinal += punctajTemp;
      for (int i = mutari.size() - 1; i >= 0; i--) {
        switch (mutari[i]) {
          case (UP)    : posX ++;
                         mutariPosibile = someBoard.getPossibleMoves
                                                    (posX,posY);
                         punctajTemp = awardPoints(mutariPosibile);
                         punctajFinal += punctajTemp;
          break;
          case (DOWN)  : posX --;
                         mutariPosibile = someBoard.getPossibleMoves
                                                    (posX,posY);
                         punctajTemp = awardPoints(mutariPosibile);
                         punctajFinal += punctajTemp;
          break; 
          case (LEFT)  : posY ++;
                         mutariPosibile = someBoard.getPossibleMoves
                                                    (posX,posY);
                         punctajTemp = awardPoints(mutariPosibile);
                         punctajFinal += punctajTemp;
          break;
          case (RIGHT) : posY --;
                         mutariPosibile = someBoard.getPossibleMoves
                                                    (posX,posY);
                         punctajTemp = awardPoints(mutariPosibile);
                         punctajFinal += punctajTemp;
          break;
        }
      }
      return punctajFinal;
    }

    int nextMove(board currentBoard) {
      int punctajBun  = 0;
      int punctajTemp = 0;
      int mutareBuna  = 0;
      for (int i = 0; i < NRMOVES; i++) {
        board tempBoard = currentBoard.copyBoard();
        position currentPosition = tempBoard.getPlayerPosition(1);
        if (tempBoard.isEmpty(i,currentPosition)) {
          tempBoard.doMove(i, 1);
          punctajTemp = getMovePoints(tempBoard, 0, mutari_);
          if (punctajTemp > punctajBun) {
            punctajBun = punctajTemp;
            mutareBuna = i;
          }
        }
      }
      return mutareBuna;
    }
};

class miniMax
{
  private:
    int maxDepth_;

  public:
    miniMax() {
      maxDepth_ = 0;
    }

    miniMax(int maxDepth) {
      maxDepth_ = maxDepth;
    }

    int getPunctaj(board someBoard, int playerNo)
    {
      // int enemyNo = someBoard.getEnemy(playerNo);
      position playerPosition = someBoard.getPlayerPosition(playerNo);
      int playerPunctaj = someBoard.getReachable(playerPosition);
      // int enemyPunctaj  = someBoard.getReachable(enemyPunctaj);
      return playerPunctaj;
    }

    int searchMiniMax(board someBoard, int playerNo, int alpha,
                      int beta, int currentDepth)
    {
      int outcome = 0;
      int enemyNo = someBoard.getEnemy(playerNo);
      position enemyPosition  = someBoard.getPlayerPosition(enemyNo);
      position playerPosition = someBoard.getPlayerPosition(playerNo);
      // nu mai sunt mutari posibile sau e remiza
      if (someBoard.getPossibleMoves(playerPosition) == 0) {
        if (someBoard.getPossibleMoves(enemyPosition) == 0) {
          // e remiza
          return REMIZA;
        }
        // jucatorul nu mai are mutari posibile
        return -MAXSCORE;
      }

      if (currentDepth >= maxDepth_) {
        cerr <<"trying evaluate" <<endl;
        outcome = getPunctaj(someBoard, playerNo);
        cerr <<"finished evaluate" <<endl;
        return outcome;
      }

      for (int crtMove = 0; crtMove < NRMOVES; crtMove++) {
        if (someBoard.isEmpty(crtMove,playerPosition)) {
          board tempBoard = someBoard.copyBoard();
          tempBoard.doMove(crtMove,playerNo);
             cerr <<"ajunge aici23" <<endl;
          outcome = searchMiniMax(tempBoard, enemyNo,-beta,
                                  -alpha,currentDepth + 1);
             cerr <<"ajunge aici2345" <<endl;
          // am gasit o mutare mai buna
          if (outcome > alpha) {
            alpha = outcome;
          }
          // beta cut-off
          if (alpha >= beta) {
            break;
          }
        }
      }
      return alpha;
    }

    int nextMove(board someBoard, int playerNo)
    {
      int goodMove     = 0;
      int outcome      = 0;
      int outcomeSoFar = 0;
      int currentDepth = 0;
      int enemyNo  = someBoard.getEnemy(playerNo);
      position playerPosition = someBoard.getPlayerPosition(playerNo);
      for (int crtMove = 0; crtMove < NRMOVES; crtMove++) {
        if (someBoard.isEmpty(crtMove, playerPosition)) {
          board tempBoard = someBoard.copyBoard();
          tempBoard.doMove(crtMove, playerNo);
          cerr << "before outcome" <<endl;
          outcome = searchMiniMax(tempBoard, enemyNo, -MAXSCORE,
                                  MAXSCORE, currentDepth);
          cerr << "after outcome: " <<outcome <<endl;
          // inamicul nu mai poate muta deci am castigat
          // intoarcem mutarea curenta
          if (outcome == MAXSCORE) {
            return crtMove;
          }
          // nu mai putem efectua mutari pentru ca ne-am blocat
          // incercam mutarea urmatoare
          if (outcome == -MAXSCORE) {
            continue;
          }
          // verificam daca solutia partiala e cea mai buna de
          // pana acum si o memoram daca e cazul
          if (outcome > outcomeSoFar) {
            outcomeSoFar = outcome;
            goodMove = crtMove;
          }
        }
      }
      return goodMove;
    }
};


void makeMove(int mutareUrmatoare) {
  switch (mutareUrmatoare) {
    case (UP) : cout << "UP" <<endl;
    break;
    case (DOWN) : cout << "DOWN" <<endl;
    break;
    case (LEFT) : cout << "LEFT" <<endl;
    break;
    case (RIGHT) : cout << "RIGHT" <<endl;
    break;
  }
}

int main()
{
  char player;
  int mutareUrmatoare = 4;
  int sizeX = 0;
  int sizeY = 0;
  int playerNo = 0;
  position myPosition;
  position enemyPosition;
  vector<string> teren;
  board* initialBoard;
  cin >> player;
  switch (player) {
    case (Player1) :
      cin >> myPosition.posX >> myPosition.posY;
      cin >> enemyPosition.posX >> enemyPosition.posY;
      playerNo = 0;
    break;
    case (Player2) :
      cin >> enemyPosition.posX >> enemyPosition.posY;
      cin >> myPosition.posX >> myPosition.posY;
      playerNo = 1;
    break;
  }

  cin >> sizeX;
  cin >> sizeY;

  for (int i = 0; i < sizeX; i++) {
    string s; cin >> s;
    teren.push_back(s);
  }

  int strategie = BACKTRACK;
  initialBoard = new board(teren, myPosition, enemyPosition,
                            sizeX, sizeY);
  if (strategie == 0 ) {
    backtrack* bkt;
    bkt = new backtrack(MAXDEPTH);
    mutareUrmatoare = bkt->nextMove(*initialBoard);
  } else {
    miniMax* minMax;
    minMax = new miniMax(MAXDEPTH);
    mutareUrmatoare = minMax->nextMove(*initialBoard, playerNo);
  }
  makeMove(mutareUrmatoare);
  return 0;
}
