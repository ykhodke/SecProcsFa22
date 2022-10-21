
#include"util.h"
#include <sys/mman.h>
// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)

// TODO: define your own buffer size
#define BUFF_SIZE (1<<21)

int main(int argc, char **argv)
{
    // Allocate a buffer using huge page
    // See the handout for details about hugepage management
    void *buf= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);

    if (buf == (void*) - 1) {
        perror("mmap() error\n");
        exit(EXIT_FAILURE);
    }
    // The first access to a page triggers overhead associated with
    // page allocation, TLB insertion, etc.
    // Thus, we use a dummy write here to trigger page allocation
    // so later access will not suffer from such overhead.
    *((char *)buf) = 1; // dummy write to trigger page allocation

    bool test_transmission_bit = false;
    char datum_line0, datum_line1;

    int str_to_num;

    // TODO: Put your covert channel setup code here
    // Creating a function to map the transmission bit to an address in the buffer allocated via mmap ?

    //Create a datum line with 2 consecutive eviction from the cache
    datum_line0 = *((char *) buf);
    datum_line1 = *((char *) buf+64);

    printf("\n the lines evicted are %p, %p", (void *)buf, (void*)(buf+64));

    printf("Please type a message.\n");

    bool sending = true;
    while (sending) {
        char text_buf[128];
        fgets(text_buf, sizeof(text_buf), stdin);

        // TODO: Put your covert channel code here
        // use the decode function to convert the string into interger

        //convert the char we obtained into a format ideal for converting to strided access
        str_to_num = string_to_int(text_buf)
        printf ("text_buffer: %s, its size: %li, the integer conversion %i \n", text_buf, strlen(text_buf), str_to_num);




    }

    printf("Sender finished.\n");
    return 0;
}


