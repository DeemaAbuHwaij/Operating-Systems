#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


// You will need two channels: 
// one for the generator to send numbers to the checkers,
// and one for the checkers to send results to the printer.
int ch_generator_to_checker;
int ch_checker_to_printer;

int is_prime (int n) {
    if (n < 2) 
        return 0;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0)
            return 0;
    }
    return 1;
}


//The generator will generate numbers starting from 2 and send them to the
//checkers via the first channel
void generator(void) {
    int num = 2;

    while (1) {
        if (channel_put(ch_generator_to_checker, num) == -1) {
            printf("PID: %d, Role: Generator, Message: Channel closed\n", getpid());  
            exit(0);
        }
        num++;
    }

    // Close the generator channel after finishing
    channel_destroy(ch_generator_to_checker);
    printf("PID: %d, Role: Generator, Message: Exiting\n", getpid());
    exit(0);
}


//Checkers will receive numbers from the generator, 
//check if they are prime, 
//and send the prime numbers to the printer via the second channel.
void checker(int id) {
    while (1) {
        int num;

        if (channel_take(ch_generator_to_checker, &num) == -1) {
            printf("PID: %d, Role: Checker %d, Message: Input channel closed\n", getpid(), id);
            exit(0);
        }

        //If the number is prime send it to the printer
        if (is_prime(num)) {
            if (channel_put(ch_checker_to_printer, num) == -1) {
                printf("PID: %d, Role: Checker %d, Message: Output channel closed\n", getpid(), id);
                exit(0);
            }
        }
    }
    
    printf("PID: %d, Role: Checker %d, Message: Exiting\n", getpid(), id);
    exit(0);
}


//The printer will receive results from the checkers
//and print them with an appropriate message
void printer(void) {
    int primes_count = 0;

    while (primes_count < 100) {
        int num;
        if (channel_take(ch_checker_to_printer, &num) == -1) {
             printf("PID: %d, Role: Printer, Message: Output channel closed\n", getpid());
            exit(0);
        }
        printf("Prime %d: %d\n", primes_count + 1, num);
        primes_count++;
    }
    
    printf("PID: %d, Role: Printer, Message: 100 prime numbers have been found \n", getpid());
    channel_destroy(ch_checker_to_printer);
    printf("PID: %d, Role: Printer, Message: Exiting\n", getpid());
    exit(0);
}

 
int main(int argc, char *argv[]) {
    int num_checkers = (argc > 1) ? atoi(argv[1]) : 3; // Accept the number of checkers = 3
                                                      // as a command-line argument

    // Create two channels: one from generator to checker, and one from checker to printer
    ch_generator_to_checker = channel_create();
    ch_checker_to_printer = channel_create();

    if (ch_generator_to_checker == -1 || ch_checker_to_printer == -1) {
        printf("Failed to create channels\n");
        exit(1);
    }

    //Forking all the processes from the same parent, create
    //the channels in the parent process and fork multiple times to create the
    //other processes.
    if (fork() == 0) {
        generator();
    }

    for (int i = 0; i < num_checkers; i++) {
        if (fork() == 0) {
            checker(i);
        }
    }

    if (fork() == 0) {
        printer();
    }

    // Wait for all child processes to exit
    wait(0);

    channel_destroy(ch_generator_to_checker);
    printf("PID: %d, Role: Main, Message: Exiting\n", getpid());
    exit(0);
}



