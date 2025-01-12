#ifndef CATALOG_H
#define CATALOG_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

#define PRODUCTS 30
#define ORDERS 10
#define CUSTOMERS 5

typedef struct {
    int code;
    char name[50];
    float price;
    int stock;
} catalog;

void createCatalog(catalog k[]);
void handleCustomer(catalog k[], int fd1[], int fd2[], int* profit, int* successful, int* failed, int fail[][CUSTOMERS], int range[]);

#endif // CATALOG_H
