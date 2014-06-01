#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <climits>
#include <list>

using namespace std;

int* getIntArray(string s, int count, int &size) {
  int *array;
  int nextSpace = s.find(" ");
  int nextIndex = (nextSpace == -1) ? s.length() : nextSpace;

  if(nextSpace == -1) {
    array = new int[count];
    size = count;
  } else {
    array = getIntArray(s.substr(nextIndex + 1), count + 1, size);
  }

  array[count-1] = atoi(s.substr(0, nextIndex).c_str());

  return array;
}

void printArray(int *array, int size) {
  for(int i = 0; i < size; i++) {
    cout << array[i] << " ";
  }
  cout << endl;
}

int* getInitialAssignment(int cols) {
  int *array = new int[cols];
  for(int i = 0; i < cols; i++) {
    array[i] = rand() % 2;
  }

  //cout << "Initial Assignment:" << endl;
  //printArray(array, cols);
  //cout << endl;

  return array;
}

int calcCost(int **matrix, int *weights, int *sortedWeights, int* n, int cols, int rows) {
  int cost = 0;
  int *rowsAssigned = new int[rows];

  for(int i = 0; i < rows; i++) {
    rowsAssigned[i] = 0;
  }

  for(int i = 0; i < cols; i++) {
    if(n[i] == 1) {
      cost += weights[i];
      
      for(int j = 0; j < rows; j++) {
        if(matrix[j][i] == 1) {
          rowsAssigned[j] = 1;
        }
      }
    }
  }

  int unassigned = rows;
  for(int i = 0; i < rows; i++) {
    unassigned -= rowsAssigned[i];
  }

  cost += unassigned;
  for(int i = 0; i < unassigned && i < cols; i++) {
    cost += sortedWeights[cols - 1 - i];
  }

  delete[] rowsAssigned;

  return cost;
}

int* getRandNeighbour(int **matrix, int *weights, int *sortedWeights, int* s, int cols, int rows) {

  int randCol = rand() % cols;
  s[randCol] = (s[randCol] + 1) % 2;

  return s;
}

bool inTabuList(list<int*> tabuList, int *array, int cols) {
  list<int*>::iterator it;
  for(it = tabuList.begin(); it != tabuList.end(); ++it) {

    bool isEqual = true;
    for(int j = 0; j < cols; j++) {
      if((*it)[j] != array[j]) {
        isEqual = false;
        break;
      }
    }

    if(isEqual) return true;
  }

  return false;
}

int* getBestNeighbour(list<int*> tabuList, int **matrix, int *weights, int *sortedWeights, int* s, int cols, int rows) {
  int **neighbours = new int*[cols];
  int *cost = new int[cols];

  for(int i = 0; i < cols; i++) {
    neighbours[i] = new int[cols];

    for(int j = 0; j < cols; j++) {
      neighbours[i][j] = s[j];
    }

    neighbours[i][i] = (s[i] + 1) % 2;
    cost[i] = calcCost(matrix, weights, sortedWeights, neighbours[i], cols, rows);
  }

  int minCost = INT_MAX;
  int minIndex;
  for(int i = 0; i < cols; i++) {
    if(cost[i] < minCost && !inTabuList(tabuList, neighbours[i], cols)) {
      minCost = cost[i];
      minIndex = i;
    }
  }

/*
  cout << "Neighbours:" << endl;
  for(int i = 0; i < cols; i++) {
    for(int j = 0; j < cols; j++) {
      cout << neighbours[i][j] << " ";
    }
    cout << "cost: " << cost[i] << endl;
  }
  cout << endl;
*/

  for(int i = 0; i < cols; i++) {
    s[i] = neighbours[minIndex][i];
  }

  for(int i = 0; i < cols; i++) {
    delete[] neighbours[i];
  }

  delete[] neighbours;
  delete[] cost;

  return s;
}

void runAlgo(int **matrix, int *weights, int *sortedWeights, int cols, int rows) {
  list<int*> tabuList;
  int *best = NULL;

  for(int i = 0; i < 100; i++) {
    int* s = getInitialAssignment(cols);

    for(int j = 0; j < 1000; j++) {
      int* r = getBestNeighbour(tabuList, matrix, weights, sortedWeights, s, cols, rows);
      int rCost = calcCost(matrix, weights, sortedWeights, r, cols, rows);
      int sCost = calcCost(matrix, weights, sortedWeights, s, cols, rows);

      if(rCost - sCost < 0) {
        tabuList.push_front(r);
        if(tabuList.size() > 20) tabuList.pop_back();
        s = r;
      }
    }

    if(best == NULL || calcCost(matrix, weights, sortedWeights, s, cols, rows) - calcCost(matrix, weights, sortedWeights, best, cols, rows) < 0) {
      best = s;
    }
   
  }

  cout << "Best soln: ";
  printArray(best, cols);

}

int main(int argc, char* argv[]) {
  if(argc <= 1) {
    cout << "Usage: ./a1 \"filename\"" << endl;
    return -1;
  }

  srand(time(NULL));

  string line;
  string strWeights = "";
  string strMatrix = "";
  ifstream dataFile(argv[1]);
  bool gettingWeights = false;
  bool gettingMatrix = false;
  int cols = 0;
  int rows = 0;

  while(getline(dataFile, line)) {
    if(line.find("Column weights") != -1) {
      gettingWeights = true;
      gettingMatrix = false;
      continue;
    } else if(line.find("Set covering (0,1)-matrix") != -1) {
      gettingWeights = false;
      gettingMatrix = true;
      continue;
    }

    if(gettingWeights) {
      strWeights += line;
    } else if(gettingMatrix) {
      if(line != "") {
        rows++;
        strMatrix += line;
      }
    }
  }

  dataFile.close();

  int *weights = getIntArray(strWeights, 1, cols);
  int **matrix = new int*[rows];

  int *sortedWeights = new int[cols];
  for(int i = 0; i < cols; i++) sortedWeights[i] = weights[i]; 
  sort(sortedWeights, sortedWeights + cols);

  cout << "Weights:" << endl;
  printArray(weights, cols);
  cout << endl;

  cout << "Sorted Weights:" << endl;
  printArray(sortedWeights, cols);
  cout << endl;

  for(int i = 0; i < rows; i++) {
    matrix[i] = new int[cols];

    for(int j = 0; j < cols; j++) {
      int index = i * cols + j;
      string integer = strMatrix.substr(index, 1);
      matrix[i][j] = atoi(integer.c_str());
    }
  }

  cout << "Matrix:" << endl;
  for(int i = 0; i < rows; i++) {
    printArray(matrix[i], cols);
  }
  cout << endl;

  runAlgo(matrix, weights, sortedWeights, cols, rows);

  delete[] weights;
  delete[] sortedWeights;

  for(int i = 0; i < rows; i++) {
    delete[] matrix[i];
  }

  delete[] matrix;
}







