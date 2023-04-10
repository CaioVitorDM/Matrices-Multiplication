#include <iostream>
#include <ctime>
#include <math.h>
#include <vector>
#include <chrono>
#include <fstream>

using namespace std;

void SalvarMatriz(vector<vector<int>> &mat, int linha, int coluna, string nome) {
  ofstream resultado(nome);
  resultado << nome << endl;
  resultado << endl;
  for (int i = 0; i < linha; i++) {
    for (int j = 0; j < coluna; j++) {
      if (resultado.is_open()) {
        resultado << mat[i][j] << " ";
      } else {
        cout << "Erro ao salvar!";
      }
    }
    resultado << endl;
  }
  resultado.close();
}

int main(int argc, char* argv[]) {
  
  //Recebe os argumentos via linha de comando
  int linhas1 = atoi(argv[1]);
  int colunas1 = atoi(argv[2]);
  int linhas2 = atoi(argv[3]);
  int colunas2 = atoi(argv[4]);
  
  //cria as matrizes
  vector<vector<int>> mat_A(linhas1, vector<int>(colunas1));
  vector<vector<int>> mat_B(linhas2, vector<int>(colunas2));
  vector<vector<int>> Resultado(linhas1, vector<int>(colunas2));

  // Preenche as matrizes A e B com valores aleatórios
  srand(time(NULL));
  for(int i = 0; i < linhas1; i++) {
        for(int j = 0; j < colunas1; j++) {
            mat_A[i][j] = rand() % 10;
        }
    }
    for(int i = 0; i < linhas2; i++) {
        for(int j = 0; j < colunas2; j++) {
            mat_B[i][j] = rand() % 10;
        }
    }
  
  //inicia a contagem do tempo
  auto start = chrono::high_resolution_clock::now();

  // Faz a multiplicação de matrizes
  for(int i = 0; i < linhas1; i++){
    for(int j = 0; j < colunas2; j++){
      int soma = 0;
      for(int k = 0; k < colunas1; k++){
        soma += mat_A[i][k] * mat_B[k][j];
      }
      Resultado[i][j] = soma;
    }
  }

  //imprime o tempo de execução
  auto end = chrono::high_resolution_clock::now();
  auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
  cout << "Tempo de execução: " << duration.count() << " microssegundos" << endl;

  //Salva a matriz resultado em um arquivo .txt
  SalvarMatriz(ref(Resultado), linhas1, colunas2, "Matriz Resultado - Sequencial.txt");

  
}