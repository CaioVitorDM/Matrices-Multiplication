#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <fstream>

using namespace std;

// Função que calcula uma linha da matriz resultado
void multiply(const vector<vector<int>>& a, const vector<vector<int>>& b, vector<vector<int>>& c, int row) {
    for (size_t j = 0; j < b[0].size(); j++) {
        int sum = 0;
        for (size_t k = 0; k < a[0].size(); k++) {
            sum += a[row][k] * b[k][j];
        }
        c[row][j] = sum;
    }
}

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

    //checa a quantidade de argumentos na linha de comando
    if (argc != 5) {
        cerr << "Erro: número de argumentos inválido." << endl;
        return 1;
    }

    // Recebe os argumentos via linha de comando
    int linhasA = stoi(argv[1]);
    int colunasA = stoi(argv[2]);
    int linhasB = stoi(argv[3]);
    int colunasB = stoi(argv[4]);

    // Inicialização das matrizes com valores aleatórios
    vector<vector<int>> a(linhasA, vector<int>(colunasA));
    vector<vector<int>> b(linhasB, vector<int>(colunasB));
    vector<vector<int>> c(linhasA, vector<int>(colunasB));

    // Checa se as matrizes são compatíveis para multiplicação
    if(colunasA != linhasB) {
        cerr << "Erro: as matrizes não são compatíveis para multiplicação." << endl;
        return 1;
    }

    // Preenche as matrizes com números aleatórios
    srand(time(NULL));
    for(int i = 0; i < linhasA; i++) {
        for(int j = 0; j < colunasA; j++) {
            a[i][j] = rand() % 10;
        }
    }
    for(int i = 0; i < linhasB; i++) {
        for(int j = 0; j < colunasB; j++) {
            b[i][j] = rand() % 10;
        }
    }
    
    //inicialização do tempo
    auto start = chrono::high_resolution_clock::now();

    // Criação de um vetor de threads para executar as funções de multiplicação em paralelo
    vector<thread> threads(linhasA);
    for (int i = 0; i < linhasA; i++) {
        threads[i] = thread(multiply, ref(a), ref(b), ref(c), i);
    }

    // Espera todas as threads terminarem de executar antes de continuar
    for (auto& t : threads) {
        t.join();
    }
    
    //imprime o tempo de execução
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "Tempo de execução: " << duration.count() << " microssegundos" << endl;

    //Salva a matriz resultado em um arquivo .txt
    SalvarMatriz(ref(c), linhasA, colunasB, "Matriz Resultado - Line.txt");
    

    return 0;
}