Echipa: TronGineers
Capitan: Alexandru-Catalin Ciobanu 323CA
Limbaj: C++
Bot etapa 1


Rezumat:

  Pentru aceasta etapa echipa a incercat initial implementarea
  unui algoritm de backtracking bazat pe o euristica ce intoarce
  un punctaj relativ la numarul de mutari ce se pot efectua dintr-o
  pozitie.

  Aceasta strategie a reusit sa bata majoritatea botilor pusi in acest
  moment pe site, insa nu reuseste sa bata pe explorer si pe randomBot
  in toate meciurile.

  Astfel am incercat implementarea unui algoritm de tip minimax cu o
  euristica ce calculeaza cate pozitii pe harta sunt disponibile pentru
  o mutare. Aceasta strategie insa dura prea mult si am fost nevoiti
  sa reluam backtracking-ul tot cu prima euristica.

  Cea de-a doua euristica reprezinta de fapt un BFS pe harta apeland
  functii pentru verificarea disponibilitatii unei pozitii prin
  intermediul unui obiect de tip board.

Surse de inspiratie:
  http://www.a1k0n.net/2010/03/04/google-ai-postmortem.html - pentru strategii
  https://github.com/a1k0n/tronbot/ - pentru implementarea acestor strategii

Numarul submisiei:
  https://www.hackerrank.com/contests/bucharest-tron/submissions/game/621630


Etapa 2

  Pentru aceasta etapa echipa a ajuns la concluzia de a renunta la algorimul de
  backtracking si incercarea implementarii unui algoritm de tip miniMax cu
  alpha-beta prunning

  Pentru euristica am ales Voronoi deoarece aceasta minimizeaza pozitiile in
  care poate ajunge adversarul.

  Astfel am creat doua instante de gasire a celei mai bune mutari:
    - in cazul in care cei doi jucatori se afla in componente diferite,
      si anume nu se intalnesc se incearca ocuparea cat mai multor pozitii.
      Aceasta se face in algoritmul soloMinimax cu euristica soloVoronoi
    - in cazul in care jucatorii se afla in aceeasi componenta se calculeaza
      mutarea cea mai buna cu minMiniMax si maxMiniMax cu alpha-beta avand
      asociata euristica connectedVoronoi


Surse de inspiratie:
  http://www.a1k0n.net/2010/03/04/google-ai-postmortem.html - pentru strategii
  https://github.com/a1k0n/tronbot/ - pentru implementarea acestor strategii


Numarul submisiei:
  https://www.hackerrank.com/contests/bucharest-tron/submissions/game/633295
