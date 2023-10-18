#include <stdio.h>
#include <stdlib.h>

int main(){
    system("echo -n \"hostname: \" ; hostname ; uname -r ; echo -n \"hostid: \" ; hostid");

    return 0;
}