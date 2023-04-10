#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <mutex>
#include <semaphore.h>
#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/ipc.h>
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
    
    //checa se a quantidade de elemetos via linha de comando está correta
    if(argc != 5) {
        cerr << "Erro: número de argumentos inválido." << endl;
        return 1;
    }

    // Recebe os argumentos via linha de comando
    int linhasA = stoi(argv[1]);
    int colunasA = stoi(argv[2]);
    int linhasB = stoi(argv[3]);
    int colunasB = stoi(argv[4]);
    int *memoria_compartilhada;
    sem_t* sem;
    const char* SEM_NAME = "/semaphore";
    int memoria_compartilhada_id;

  
    // Checa se as matrizes são compatíveis para multiplicação
    if(colunasA != linhasB) {
        cerr << "Erro: as matrizes não são compatíveis para multiplicação." << endl;
        return 1;
    }

    // Inicializa as matrizes com números randômicos
    vector<vector<int>> A(linhasA, vector<int>(colunasA));
    vector<vector<int>> B(linhasB, vector<int>(colunasB));
    vector<vector<int>> C(linhasA, vector<int>(colunasB));
  
    //inicializa as matrizes com números aleatórios
    srand(time(NULL));
    for(int i = 0; i < linhasA; i++) {
        for(int j = 0; j < colunasA; j++) {
            A[i][j] = rand() % 10;
        }
    }
    for(int i = 0; i < linhasB; i++) {
        for(int j = 0; j < colunasB; j++) {
            B[i][j] = rand() % 10;
        }
    }

    // Cria semáforo e mutex para controlar o acesso à memória compartilhada
    const int num_processos = linhasA * colunasB;
    sem_t *semaphores[num_processos];
    mutex mtx;

    for (int i = 0; i < num_processos; i++) {
        semaphores[i] = sem_open("semaphore", O_CREAT, 0644, 1);
    }

    // Inicia a medição de tempo
    auto start = chrono::high_resolution_clock::now();

    // Cria um processo para cada elemento da matriz resultado
    vector<thread> threads;
    int thread_idx = 0;
    for(int i = 0; i < linhasA; i++) {
        for(int j = 0; j < colunasB; j++) {
            threads.push_back(thread([&A, &B, &C, i, j, &semaphores, &mtx, thread_idx, colunasA]() mutable {
                // Calcula o elemento da matriz resultado
                int sum = 0;
                for(int k = 0; k < colunasA; k++) {
                    sum += A[i][k] * B[k][j];
                }
                sem_wait(semaphores[thread_idx]);
                mtx.lock();
                C[i][j] = sum;
                sem_post(semaphores[thread_idx]);
                mtx.unlock();
                thread_idx++;
            }));
        }
    }
  
    // Espera todos os processos terminarem
    for(auto& thread : threads) {
        thread.join();
    }

    // Libera os semáforos
    for (int i = 0; i < num_processos; i++) {
        sem_close(semaphores[i]);
        sem_unlink("semaphore");
    }

    // Desanexa e remove a memória compartilhada
    shmdt(memoria_compartilhada);
    shmctl(memoria_compartilhada_id, IPC_RMID, NULL);

    // Deleta o semáforo
    sem_close(sem);
    sem_unlink(SEM_NAME);

    // Imprime o tempo de execução
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "Tempo de execução: " << duration.count() << " microssegundos" << endl;

    //salva a matriz resultado em um arquivo .txt
    SalvarMatriz(ref(C), linhasA, colunasB, "Matriz Resultado - Element.txt");
 
    return 0;
}
  