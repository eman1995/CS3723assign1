#include <stdio.h>
#include <stdlib.h>

#define PRECHECK 0xAEAEAEAEAEAEAEAEL
#define POSTCHECK 0xEAEAEAEAEAEAEAEAL

// +--------------------------------------+
// +    Size of User Data, in bytes       +
// +--------------------------------------+
//      PRECHECK sentinel
// +--------------------------------------+
//          User Data
//
//
// +--------------------------------------+
//      POSTCHECK sentinel
// +--------------------------------------+

//Grab some memory
void *getMem(int numBytes) 
{
    long *blockPtr;
    //blockPtr = (long *)malloc((sizeof(long)*3) + numBytes);
    while(numBytes % 8 != 0)
    {
        numBytes++;
    }
    blockPtr = (long *)malloc((sizeof(long)*3) + numBytes);
    //printf("%d\n", numBytes);

    *blockPtr = numBytes;
    *(blockPtr + sizeof(long)) = PRECHECK;
    *(blockPtr + sizeof(long) +sizeof(long) + numBytes) = POSTCHECK;

    return (void *)(blockPtr + 2*sizeof(long));

    //: Allocation memory, fill in the two blocks at the top, and the block at the bottom
    //Return a pointer to the User Data
}   //End of getMem

//Free memory alloated by getMem
void freeMem(void *ptr) 
{
    long *newptr = (long *)ptr - 2*sizeof(long);
    long sb = *(newptr);
    printf("Bytes: %ld\n", *(newptr));
    printf("Precheck Sent: %x\n", *(newptr + sizeof(long)));
    //printf("User Data: %d\n", *(newptr + 2*sizeof(long))); //debug
    printf("Postcheck Sent: %x\n", *(newptr + 2*sizeof(long) + sb));
    if (*(newptr + sizeof(long)) != PRECHECK)
    {
        fprintf(stderr, "Memory Corruption at %p, bytes = %ld, sentinel = %x instead of %ld.\n", (newptr + (sizeof(long))), sb, *(newptr + sizeof(long)), PRECHECK);
    }
    if (*(newptr + 2*sizeof(long) + sb) != POSTCHECK)
    {
        fprintf(stderr, "Memory Corruption at %p, bytes = %ld, sentinel = %x instead of %ld.\n", (newptr + 2*sizeof(long) + sb), sb, *(newptr + 2*sizeof(long) + sb), POSTCHECK);
    }
    free(newptr);
    //: Check the two sentinels. If there are any problems, print an error message to stderr.
    //release (free) the memory
}   //End of freeMem 

int main(int argc, char *argv[]) {
    char *ptr;

    printf("===== Test 1\n");
    ptr = getMem(10);
    ptr[0] = 'x';
    ptr[9] = 'y';
    freeMem(ptr);

    printf("===== Test 2\n");
    ptr = getMem(1);
    ptr[0] = 'x';
    ptr[1] = 'y';
    freeMem(ptr);

    printf("===== Test 3\n");
    ptr = getMem(100);
    ptr[-100] = 'a';
    ptr[200] = 'b';
    freeMem(ptr);

    printf("===== Test 4\n");
    ptr = getMem(1000);
    ptr[-1] = 'a';
    ptr[1000] = 'b';
    freeMem(ptr);

    return 0;
}
