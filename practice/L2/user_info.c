#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <stdio.h>

int main(){
    struct passwd *pw;
    pw = getpwuid( getuid() );
    printf("Logged in as %s\n", pw->pw_name);
}