/*
The command tail [ –n num ] file prints the last num lines (ten by default) of the
named file. Implement this command using I/O system calls (lseek(), read(), write(),
and so on). Keep in mind the buffering issues described in this chapter, in order to
make the implementation efficient.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define DEFAULT_LINES_NUMBER 10
#define BUFFER_SIZE 2048

void tail(int fd, int lines){
    off_t file_offset = lseek(fd, 0, SEEK_END);
    char buffer[BUFFER_SIZE];
    int lines_count = 0;
    int read_bytes;
    int total_read_bytes = 0;

    while(file_offset > 0){
        //若要讀取的大小超過 buffer size，則分次讀取
        if(file_offset >= BUFFER_SIZE){
            read_bytes = BUFFER_SIZE;
            file_offset -= BUFFER_SIZE;
        }
        //若要讀取的大小沒有超過 buffer size，則一次讀取完
        else{
            read_bytes = file_offset;
            file_offset = 0;
        }

        //將讀寫位置移動到檔案尾端
        lseek(fd, file_offset, SEEK_SET);

        //讀取檔案
        int read_bytes = read(fd, buffer, read_bytes);
        if(read_bytes < 0){
            perror("read");
            exit(EXIT_FAILURE);
        }
        
        //紀錄目前已經讀取了多少
        total_read_bytes += read_bytes;

        //反向印出每一行
        for(int i=read_bytes-1; i>=0; i--){
            if(buffer[i] == '\n'){
                lines_count++;
                if(lines_count >= lines){
                    write(STDOUT_FILENO, buffer+i+1, total_read_bytes-(i+1));
                    return;
                }
            }
        }
    }

    //如果 -n 的參數要求印出的行大於總行數，則印出整個文件
    lseek(fd, 0, SEEK_SET);
    while((read_bytes = read(fd, buffer, BUFFER_SIZE)) > 0){
        write(STDOUT_FILENO, buffer, read_bytes);
    }
}

int main(int argc, char *argv[]){
    int lines_printed = DEFAULT_LINES_NUMBER;
    int fd;

    if(argc < 2 || argc > 4){
        printf("Error: Invalid command\n");
        printf("Usage: ./tail [-n num] file\n");
        exit(EXIT_FAILURE);
    }

    if(argc == 2 && !(strcmp(argv[1], "-h"))){
        printf("Usage: ./tail [-n num] file\n");
        exit(EXIT_SUCCESS);
    }

    if(argc == 4 && !(strcmp(argv[1], "-n"))){
        lines_printed = atoi(argv[2]);
    }

    // 打開文件
    fd = open(argv[argc-1], O_RDONLY);
    if(fd < 0){
        perror("open");
        exit(EXIT_FAILURE);
    }

    // 印出最後 "lines_printed" 行
    tail(fd, lines_printed);

    close(fd);

    return 0;
}
