

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

int global = 0;

int mul(int x, int y){
    int local_mul = x * y;
    return local_mul;
}

void add (int p, int q, int s){
    int local_add = p + q;
    s = local_add;
}


int main()
{
    

    int ad = 0, mu = 0;
    init_platform();

    mu = mul(8, 8);
    add(8, 8, ad);

    xil_printf("Value of add : %0d\r\n", ad);
    xil_printf("Value of mul : %0d\r\n", mu);

    
    cleanup_platform();
    return 0;
}
