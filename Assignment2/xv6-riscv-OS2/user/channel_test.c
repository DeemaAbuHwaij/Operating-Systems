#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
    int cd = channel_create();
    if (cd < 0) {
        printf("Failed to create channel\n");
        exit(1);
    }

    if (fork() == 0) { 
        if (channel_put(cd, 42) < 0) {
            printf("Failed to put data in channel\n");
            exit(1);
        }
        if (channel_put(cd, 43) < 0) { // Sleeps until cleared
            printf("Failed to put data in channel\n");
            exit(1);
        }
        if (channel_destroy(cd) < 0) {
            printf("Failed to destroy channel\n");
            exit(1);
        }
        exit(0);
    } else { 
        int data;
        if (channel_take(cd, &data) < 0) { // 42
            printf("Failed to take data from channel\n");
            exit(1);
        } 
        else {
            printf("Parent received: %d\n", data);
        }
        if (channel_take(cd, &data) < 0) { // 43
            printf("Failed to take data from channel\n");
            exit(1); 
        }
        else {
            printf("Parent received: %d\n", data);
        }
        // Sleep until the child destroys the channel
        if (channel_take(cd, &data) < 0) {
            printf("Failed to take data from channel (expected after destroy)\n");
        }
        else {
            printf("Parent received: %d (unexpected)\n", data);
        }

        wait(0); // Wait for child to finish
    }

    exit(0);
}
