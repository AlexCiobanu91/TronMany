#include <iostream>
#include <vector>
#include <math.h>
using namespace std;

#define Player1 'r'
#define Player2 'g'
#define MAXDEPTH 15

typedef enum {
  UP,DOWN,RIGHT,LEFT,PLAYDEAD   
} mutari;

typedef struct pozitie {
  int posX;
  int posY;
} position;

class board {
   
   private:
 
    vector<string> teren_;
    
    position myPosition_;
    position enemyPosition_;
    
    int distantaIntreNoi_;
    
  public:  
    
    // initializare teren cu toti paramtrii nuli
    board() {

      distantaIntreNoi_ = 0;
    }

    board(vector<string> teren,position myPosition,
                               position enemyPosition) {
      // initializare teren 
      for (int i = 0; i <15; i++) {
        teren_.push_back(teren[i]);
      }  
      // pozitia mea primita de la server
      myPosition_.posX= myPosition.posX;
      myPosition_.posY= myPosition.posY;
        
      // pozitia inamiclui (numero UNO) primita de la server
      enemyPosition_.posX= enemyPosition.posX;
      enemyPosition_.posY= enemyPosition.posY;
      updateDistance();
    }
    
    
    // creeaza o copie a tebelei primita ca argument
    // copiind inclusiv muschii din talpa
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
    // sau da cu nasul in zid
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
    
    // face update la postia motocicletei mele
    // la inamic oare trebe ?
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
    
    // coordonata pe X
    // pacatosii de pamant
    int getPlayerPositionX(int playerNo) {
      return playerNo == 1 ? myPosition_.posX : enemyPosition_.posX;  
    }
    
    // intoarce pozitia unui jucator pe ambele coordonate
    // adica toate neamurile acelui jucator  
    position getPlayerPosition(int playerNo) {
      return playerNo == 1 ? myPosition_ : enemyPosition_;  
    }
    
    // coordonata pe Y
    // ingerii din rai si popii 
    int getPlayerPositionY(int playerNo) {
      return playerNo == 1 ? myPosition_.posY : enemyPosition_.posY;  
    }
    
    // calculeaza distanta intre cele doua motociclete
    // matematica analitica ce pnm
    void updateDistance() {
      int x1 = myPosition_.posX;
      int y1 = myPosition_.posY;
      int x2 = enemyPosition_.posX;
      int y2 = enemyPosition_.posY;
      distantaIntreNoi_ = sqrt((x1 - x2) * (x1 - x2) + 
                               (y1 - y2) * (y1 - y2)); 
    }
    
    // pe zi ce trece-alene 
    // intre-un rasarit s'apus de soare
    // ne apropiem cu gandul 
    // la fiecare ciclu
    // pana in momentul
    // cand ma vei orbi
    // cu lumina farului tau stang
    int getDistance() {
      return distantaIntreNoi_;   
    }
    
    // la fiecare mutare (when we're close togteher... 
    // http://www.trilulilu.ro/muzica-diverse/catalin-josan-when-we-re-together-1 )
    // verifica cate mutari putem face
    // ale inamicului
    int getPossibleMoves(position playerPosition) {
      int mutariPosibile = 0;
      for (int i; i < 4 ; i++) {
        if (isEmpty(i, playerPosition)) {
          mutariPosibile++;   
        }
      }
      return mutariPosibile;      
    }
    
    // wrapper peste functia de mai sus care intoarce 
    // numarul de mutari posibile pentru coordonate pe X si pe Y
    // still getting closer...
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
};
                
class backtrack { 
   
  private:  
     
    vector<int> mutari_;
    int         maxDepth_;
    
  public:
     
    backtrack(int maxDepth) {
      maxDepth_ = maxDepth;
    }
    
    
    // aplica un backtracking amarat pe fiecare dintre cele 4 directii cardinale
    // (la rascruce de vanturi) si calculeaza in momentul in care ajunge la adancimea 
    // maxima (fundul putului) calculeaza un punctaj pentru fiecare mutare
    // din pacate drumul inapoi nu are gropi k ar mai fi taiat din punctaj 
    int getMovePoints(board previousBoard, int currentDepth, vector<int> mutari) {
      position posXYEnemy = previousBoard.getPlayerPosition(2); 
      position posXYMyPlayer = previousBoard.getPlayerPosition(1); 
      if (currentDepth >= maxDepth_ || 
          previousBoard.getPossibleMoves(posXYEnemy) == 0 ||
          previousBoard.getPossibleMoves(posXYMyPlayer) == 0 
          ) {
        return getPoints(previousBoard, mutari, 1 /*Player1*/);    
      } else {
        int maxProfit = 0; 
        for (int i = 0; i < 4; i++) {
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
    
    // acorda puncte in functie de cate mutari posibile sunt din acea pozitie
    // pe prncipiul ia de aici k nu dau de la mine            
    int awardPoints(int howMany) {
      switch (howMany) {
        // te-ai izbit in zid si ti-ai spart nasul
        // si mai vrei si puncte ? 
        case (0):  return -4569;
        break;   
        // o mutare? si zici k mai stii sa mergi pe motocicleta...
        // ia si tu pomana... huoooooo...
        case (1): return 100;   
        break;
        // oleak de skill observ...
        // stii sa ocolesti gropi dar tot te zgaraii oleak
        // hai k meriti ceva mai mult
        case (2): return 200;
        break;  
        // ar merge de-un campionat local
        // o medalie de argint poate chiar aur
        // dar tot mai ai de munca... hai ia si tu ceva
        case (3): return 300;
        break;
        // ba baiatu.... bosus bosilor
        // numero uno... il capitono (mi casa es su casa :P)
        // dau punctaj la tine k la lautari cand is beat de mort
        // fara numar... fara numar...
        case(4): return 400;
        break;  
      }
      return -12345; 
    }          
                
    // merge in sens invers al parcurgerii miscarilor si calculeaza
    // un punctaj pentru fiecare mutare
    // citez : "Drumurile noastre poate... Se vor intalni vreodata"
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
                         mutariPosibile = someBoard.getPossibleMoves(posX,posY);
                         punctajTemp = awardPoints(mutariPosibile);
                         punctajFinal += punctajTemp;
          break;
          case (DOWN)  : posX --;
                         mutariPosibile = someBoard.getPossibleMoves(posX,posY);
                         punctajTemp = awardPoints(mutariPosibile);
                         punctajFinal += punctajTemp;
          break; 
          case (LEFT)  : posY ++;
                         mutariPosibile = someBoard.getPossibleMoves(posX,posY);
                         punctajTemp = awardPoints(mutariPosibile);
                         punctajFinal += punctajTemp;
          break;
          case (RIGHT) : posY --;
                         mutariPosibile = someBoard.getPossibleMoves(posX,posY);
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
      int mutareBuna  = 4; //PlayDead

      for (int i = 0; i < 4; i++) {
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

/* Tail starts here */
int main() {

    char player;
    int mutareUrmatoare = 4;
    position myPosition;
    position enemyPosition;
    vector<string> teren;
    board* initialBoard;
    backtrack* bkt;
    cin >> player;
    switch (player) {
        case (Player1) :
            cin >> myPosition.posX >> myPosition.posY;
            cin >> enemyPosition.posX >> enemyPosition.posY;
        break;
        case (Player2) :
            cin >> enemyPosition.posX >> enemyPosition.posY;
            cin >> myPosition.posX >> myPosition.posY;
        break;
    }
           
    for (int i = 0; i < 15; i++) {
        string s; cin >> s;
        teren.push_back(s);
    }
    
    initialBoard = new board(teren, myPosition, enemyPosition);
    bkt = new backtrack(MAXDEPTH);
    mutareUrmatoare = bkt->nextMove(*initialBoard);
    makeMove(mutareUrmatoare);
    return 0;
}

