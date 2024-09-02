#include <cassert>
#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;

using HR = high_resolution_clock;
using HRTimer = HR::time_point;

#define N (4096)

void matmul_ijk(const uint32_t *A, const uint32_t *B, uint32_t *C, const int SIZE) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      uint32_t sum = 0.0;
      for (int k = 0; k < SIZE; k++) {
        sum += A[i * SIZE + k] * B[k * SIZE + j];
      }
      C[i * SIZE + j] += sum;
    }
  }
}

void matmul_ijk_blocking(const uint32_t *A, const uint32_t *B, uint32_t *C, const int SIZE, int BLK) {
  for (int i = 0; i < SIZE; i+=BLK){
     for (int j = 0; j < SIZE; j+=BLK){
        for (int k = 0; k < SIZE; k+=BLK){
            for (int i1 = i; i1 < i+BLK && i1<SIZE; i1++){
              for (int j1 = j; j1 < j+BLK && j1<SIZE; j1++){
                for (int k1 = k; k1 < k+BLK && k1<SIZE; k1++){
                    C[i1*SIZE+j1] += A[i1*SIZE + k1]*B[k1*SIZE + j1];
                }
              }
            }
        }
     }
  } 
}

void init(uint32_t *mat, const int SIZE) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      mat[i * SIZE + j] = 1;
    }
  }
}

void print_matrix(const uint32_t *mat, const int SIZE) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      cout << mat[i * SIZE + j] << "\t";
    }
    cout << "\n";
  }
}

void check_result(const uint32_t *ref, const uint32_t *opt, const int SIZE) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (ref[i * SIZE + j] != opt[i * SIZE + j]) {
        assert(false && "Diff found between sequential and blocked versions!\n");
      }
    }
  }
}

int main() {
  uint32_t *A = new uint32_t[N * N];
  uint32_t *B = new uint32_t[N * N];
  uint32_t *C_seq = new uint32_t[N * N];

  init(A, N);
  init(B, N);
  init(C_seq, N);

  HRTimer start = HR::now();
  // matmul_ijk(A, B, C_seq, N);
  HRTimer end = HR::now();
  auto duration = duration_cast<microseconds>(end - start).count();
  cout << "Time without blocking (us): " << duration << "\n";

  uint32_t *C_blk = new uint32_t[N * N];
  // init(C_blk, N);

  // start = HR::now();
  // matmul_ijk_blocking(A, B, C_blk, N,1);
  // end = HR::now();
  // duration = duration_cast<microseconds>(end - start).count();
  // cout << "Time with blocking on blk=1 (us): " << duration << "\n";
  // check_result(C_seq, C_blk, N);
  
  // init(C_blk, N);

  // start = HR::now();
  // matmul_ijk_blocking(A, B, C_blk, N,2);
  // end = HR::now();
  // duration = duration_cast<microseconds>(end - start).count();
  // cout << "Time with blocking on blk=2 (us): " << duration << "\n";

  // check_result(C_seq, C_blk, N);
  // init(C_blk, N);

  // start = HR::now();
  // matmul_ijk_blocking(A, B, C_blk, N,4);
  // end = HR::now();
  // duration = duration_cast<microseconds>(end - start).count();
  // cout << "Time with blocking on blk=4 (us): " << duration << "\n";

  // check_result(C_seq, C_blk, N);

  // init(C_blk, N);

  // start = HR::now();
  // matmul_ijk_blocking(A, B, C_blk, N,8);
  // end = HR::now();
  // duration = duration_cast<microseconds>(end - start).count();
  // cout << "Time with blocking on blk=8 (us): " << duration << "\n";

  // check_result(C_seq, C_blk, N);

  // init(C_blk, N);

  // start = HR::now();
  // matmul_ijk_blocking(A, B, C_blk, N,16);
  // end = HR::now();
  // duration = duration_cast<microseconds>(end - start).count();
  // cout << "Time with blocking on blk=16 (us): " << duration << "\n";

  // check_result(C_seq, C_blk, N);
  init(C_blk, N);

  start = HR::now();
  matmul_ijk_blocking(A, B, C_blk, N,32);
  end = HR::now();
  duration = duration_cast<microseconds>(end - start).count();
  cout << "Time with blocking on blk=32 (us): " << duration << "\n";

  init(C_blk, N);

  start = HR::now();
  matmul_ijk_blocking(A, B, C_blk, N,64);
  end = HR::now();
  duration = duration_cast<microseconds>(end - start).count();
  cout << "Time with blocking on blk=64 (us): " << duration << "\n";

  init(C_blk, N);

  start = HR::now();
  matmul_ijk_blocking(A, B, C_blk, N,128);
  end = HR::now();
  duration = duration_cast<microseconds>(end - start).count();
  cout << "Time with blocking on blk=128 (us): " << duration << "\n";

  init(C_blk, N);

  start = HR::now();
  matmul_ijk_blocking(A, B, C_blk, N,256);
  end = HR::now();
  duration = duration_cast<microseconds>(end - start).count();
  cout << "Time with blocking on blk=256 (us): " << duration << "\n";

  init(C_blk, N);

  start = HR::now();
  matmul_ijk_blocking(A, B, C_blk, N,512);
  end = HR::now();
  duration = duration_cast<microseconds>(end - start).count();
  cout << "Time with blocking on blk=512 (us): " << duration << "\n";

  return EXIT_SUCCESS;
}
