#include "kernel/types.h"
#include "user/user.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "kernel/fcntl.h"
#include "kernel/crypto.h"

void xor_encrypt_decrypt(uchar *data, uchar *key, int data_size, int key_size) {
    for (int i = 0; i < data_size; i++) {
        data[i] ^= key[i % key_size];
    }
}

int main(void) {
  if(open("console", O_RDWR) < 0){
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  printf("crypto_srv: starting\n");

  // TODO: implement the cryptographic server here

  //Check if the PID of the server process and exit if its not 2
  if(getpid() != 2){
    exit(1);
  }

  struct crypto_op *op;
    uchar *data = 0;
    uchar *key = 0; 
    uint64 size = 0;
    void *ptr = 0; 

  while(1) { 
    //obtain cryptographic operation requests in an infinite loop
    take_shared_memory_request(&ptr, &size);
    op = (struct crypto_op *)ptr; 

    if(op->state != CRYPTO_OP_STATE_INIT) {
      op->state = CRYPTO_OP_STATE_ERROR;
      remove_shared_memory_request(ptr, size);
      exit(1);
    }

    data = malloc(op->data_size);
    key = malloc(op->key_size);
    if (data == 0 || key == 0) {
      op->state = CRYPTO_OP_STATE_ERROR;
      remove_shared_memory_request(ptr, size);
      exit(1);
    }

    // Copy data and key from op->payload
    memcpy(data, op->payload + op->key_size, op->data_size);
    memcpy(key, op->payload, op->key_size);
    // Perform XOR encryption/decryption
    xor_encrypt_decrypt(data, key, op->data_size, op->key_size);
    // Copy encrypted/decrypted data back to payload
    memcpy(op->payload + op->key_size, data, op->data_size);

    // Use fence instruction before changing the state
    asm volatile("fence rw,rw" : : : "memory");

    // Set the state to done
    op->state = CRYPTO_OP_STATE_DONE;
    memcpy(ptr, op, sizeof(struct crypto_op));
        
    //release cryptographic operation requests in an infinite loop
    remove_shared_memory_request(ptr, size);
    free(data);
    free(key);
  }
  exit(0);
}