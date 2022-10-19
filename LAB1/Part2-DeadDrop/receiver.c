
#include"util.h"
#include <sys/mman.h>
// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)

// TODO: define your own buffer size
#define BUFF_SIZE (1<<21) //number of bytes that are to be allocated for the mapping

#define L2_SIZE 4096
#define L2_HIT_MISS_THRESHOLD 75

// Returns true if the entire L2 cache is Primed with reciever's address space
bool initial_cache_prime_probe (void* buf) {
	volatile uint64_t evc;

	uint64_t* eviction_buffer = (uint64_t *) buf;
	uint64_t l2_latency = 0;
	uint64_t max_l2_latency = 0;

	// 8 bytes is the line size
	// L2 cache is 8 way associative
	int num_cache_lines = BUFF_SIZE / 8;

	for (int j = 0; j < 8; j++) {
		for (int k = 2; k < L2_SIZE*1.5; k++){
			evc = eviction_buffer[k*8+j];
			//evc +=  eviction_buffer[(k-2)*8+j];
			eviction_buffer[(k-2)*8+j] = evc;
		}
	}

	for (int k = 2; k < L2_SIZE*1.5; k++){ 
		l2_latency = measure_one_block_access_time((uint64_t)eviction_buffer);
		if (l2_latency > max_l2_latency) {
			max_l2_latency = l2_latency;
		} 
	}

	printf ("value for max_l2_latency is %li \n", max_l2_latency);

	if (max_l2_latency > L2_HIT_MISS_THRESHOLD) {
		return false;
	}
	else {
		return true;
	}
}

void prime_cache (){

}

int probe_cache () {
	return 0;
}

int main(int argc, char **argv)
{
	// Put your covert channel setup code here

	// Step 1: Allocate a buffer using huge page
    // See the handout for details about hugepage management
    void *buf= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);

    if (buf == (void*) - 1) {
        perror("mmap() error\n");
        exit(EXIT_FAILURE);
    }

	while (true) {
		bool l2_primed = initial_cache_prime_probe (buf);
	}

	//Step 2: Using initial_cache_prime_probe check that all the L2 cache lines are occupies by the reciever


	printf("Please press enter.\n");

	char text_buf[2];
	fgets(text_buf, sizeof(text_buf), stdin);

	printf("Receiver now listening.\n");

	bool listening = true;
	while (listening) {

		// Put your covert channel code here

	}

	printf("Receiver finished.\n");

	return 0;
}


