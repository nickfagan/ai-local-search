#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

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

  cout << "Initial Assignment:" << endl;
  printArray(array, cols);
  cout << endl;

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
        if(matrix[j][i] == 1) rowsAssigned[j] = 1;
      }
    }
  }

  int unassigned = rows;
  for(int i = 0; i < rows; i++) {
    unassigned -= rowsAssigned[i];
  }

  for(int i = 0; i < unassigned; i++) {
    cost += 1 + sortedWeights[cols - 1 - i];
  }

  return cost;
}

int* getNeighbour(int **matrix, int *weights, int *sortedWeights, int* s, int cols, int rows) {
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

  cout << "Neighbours:" << endl;
  for(int i = 0; i < cols; i++) {
    for(int j = 0; j < cols; j++) {
      cout << neighbours[i][j] << " ";
    }
    cout << "cost: " << cost[i] << endl;
  }
  cout << endl;

  return s;
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
      string integer = strMatrix.substr(index, index + 1);
      matrix[i][j] = atoi(integer.c_str());
    }
  }

  int* s = getInitialAssignment(cols);
  int* r = getNeighbour(matrix, weights, sortedWeights, s, cols, rows);

  delete[] weights;
  for(int i = 0; i < rows; i++) {
    delete[] matrix[i];
  }
  delete[] matrix;
}







