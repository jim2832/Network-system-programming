#include <stdio.h>
#include <time.h>

void printCurrentDate() {
    time_t currentTime;
    struct tm *localTime;

    // Get the current time
    currentTime = time(NULL);

    // Convert the current time to the local time
    localTime = localtime(&currentTime);

    // Print the current date (including time information)
    printf("%d/%d/%d %d:%d:%d\n", localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_year + 1900, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
    
}

int main() {
    printCurrentDate();
    return 0;
}
