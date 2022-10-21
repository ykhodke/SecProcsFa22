
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

	uint64_t  l2_latency 	  = 0;
	uint64_t  max_l2_latency  = 0;

	// 8 bytes is the line size
	// L2 cache is 8 way associative
	int num_cache_lines = BUFF_SIZE / 8;

	for (int j = 0; j < 8; j++) {
		for (int k = 0; k < L2_SIZE; k++) {
			evc = buf[k*64+j];
			buf[(L2_SIZE - 1 - k)*64+j] = evc;
		}
	}

	for (int k = 0; k < L2_SIZE; k++) { 
		l2_latency = measure_one_block_access_time((uint64_t)(buf+k*64));
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


int probe_cache (void* buf, int* evicted_indices) {

	uint64_t  l2_latency;
	int i;
	int flag = 0;
	
	for (int k = 0; k < L2_SIZE; k++) { 
		l2_latency = measure_one_block_access_time((uint64_t)(buff+k*64));
		if (l2_latency > L2_HIT_MISS_THRESHOLD) {
			evicted_indices[i] == k;
			i++;
			if (i>10) {
				flag = 1;
				break;
			}
		} 
	}
	return flag;
}

int main(int argc, char **argv)
{
	// Put your covert channel setup code here

	// Step 1: Allocate a buffer using huge page
    // See the handout for details about hugepage management
    void *buf= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);

	int* evicted_indices = (int *)malloc(10*sizeof(int *));
	int check_initial_prime;

    if (buf == (void*) - 1) {
        perror("mmap() error\n");
        exit(EXIT_FAILURE);
    }

	//Step 2: Using initial_cache_prime_probe check that all the L2 cache lines are occupies by the reciever
	while (true) {
		bool l2_primed = initial_cache_prime_probe (buf);
		if (l2_primed) {
			break;
		}
	}

	//Step 3: Double check for cache occupany :: Could be removed later since initial conditions take care of it
	check_initial_prime = probe_cache (buf, evicted_indices);
	
	if (check_initial_prime) {
		printf ("This ain't working.\n");
	}
	else {
		printf ("This is working.\n");
	}

	printf("Please press enter.\n");

	char text_buf[2];
	fgets(text_buf, sizeof(text_buf), stdin);

	printf("Receiver now listening.\n");

	bool listening = true;
	while (listening) {



		//Step 4: Put your covert channel code here
		//Checks for the cache entries populated by the reciever and finds out eviction indices to

	}

	printf("Receiver finished.\n");

	return 0;
}


