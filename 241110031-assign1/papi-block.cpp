#include <cstdlib>
#include <iostream>
#include <papi.h>

using std::cerr;
using std::cout;
using std::endl;
using std::exit;

#define T (1024 * 1024)
#define N (4096)
void init(uint32_t *mat, const int SIZE) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      mat[i * SIZE + j] = 1;
    }
  }
}
void handle_error(int retval) {
    cout << "PAPI error: " << retval << ": " << PAPI_strerror(retval) << "\n";
    exit(EXIT_FAILURE);
}

int main() {
    int EventSet = PAPI_NULL;
    long long values[2];
    int BLK=8;
    uint32_t *A = new uint32_t[N * N];
    uint32_t *B = new uint32_t[N * N];
    uint32_t *C_blk = new uint32_t[N * N];

    init(A, N);
    init(B, N);
    init(C_blk, N);


    // Initialize PAPI library
    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
        handle_error(PAPI_ESYS);
    }

    // Start high-level region
    int retval = PAPI_hl_region_begin("PAPI-HL");
    if (retval != PAPI_OK) {
        handle_error(retval);
    }

    // Create event set
    int status = PAPI_create_eventset(&EventSet);
    if (status != PAPI_OK) {
        fprintf(stderr, "Error creating event set\n");
        handle_error(status);
    }

    // Add events
    status = PAPI_add_event(EventSet, PAPI_TOT_INS);
    if (status != PAPI_OK) {
        fprintf(stderr, "Error adding total instructions event to event set\n");
        handle_error(status);
    }

    status = PAPI_add_event(EventSet, PAPI_L2_DCM);
    if (status != PAPI_OK) {
        fprintf(stderr, "Error adding L2 D-Cache miss event to event set\n");
        handle_error(status);
    }

    // Start counting
    status = PAPI_start(EventSet);
    if (status != PAPI_OK) {
        fprintf(stderr, "Error starting event counter\n");
        handle_error(status);
    }

    // Computation
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

    // Stop counting
    if (PAPI_stop(EventSet, values) != PAPI_OK) {
        fprintf(stderr, "Error stopping event counter\n");
        exit(1);
    }

    // Read values
    if (PAPI_read(EventSet, values) != PAPI_OK) {
        fprintf(stderr, "Error reading event counters\n");
        exit(1);
    }

    cout << "Total load-store instructions = " << values[0] << endl;
    cout << "Total L2 D-Cache misses = " << values[1] << endl;

    // End high-level region
    retval = PAPI_hl_region_end("PAPI-HL");
    if (retval != PAPI_OK) {
        handle_error(retval);
    }

    cout << "\nPASSED\n";
    exit(EXIT_SUCCESS);
}

