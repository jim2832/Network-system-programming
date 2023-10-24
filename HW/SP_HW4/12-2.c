#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

typedef struct Process{
    unsigned int TGID;
    unsigned int PID;
    unsigned int PPID;
    char ProcessName[200];
    struct Process *child;
    struct Process *parent;
    char IsFirstChild;
    char IsVisited;
} ProcessInfo;

char PrintPIDFlag;
char NumericSortFlag;
char PrintKernelThreadsFlag;
char DoNotDoJobFlag;

struct dirent **NameList;
struct dirent **TaskNameList;
int count;

char TreePrefix[1024];

char FilterNeq[128];

ProcessInfo all_process[1024];
ProcessInfo Root;

/* read every line of the file */
int ReadLine(FILE *fp, char s[], int limit){
    int c, i = 0;
    while((c = fgetc(fp)) != EOF && c != '\n' && i < limit - 1)
        s[i++] = c;
    s[i] = '\0';

    return i;
}

/* selector */
int Filter(const struct dirent *Directory){
    if(!strcmp(Directory->d_name, FilterNeq))
        return 0;
    for(int i = 0; i < strlen(Directory->d_name); i++){
        if(Directory->d_name[i] < '0' || Directory->d_name[i] > '9')
            return 0;
    }
    return 1;
}

/* print out the single process */
void PrintNode(ProcessInfo *CurrentNode){
    int LastTreePrefixLength = strlen(TreePrefix);
    char Buffer[512];
    int i;
    if(PrintPIDFlag)
        sprintf(Buffer, "%s(%d)", CurrentNode->ProcessName, CurrentNode->PID);
    else
        sprintf(Buffer, "%s", CurrentNode->ProcessName);

    if(CurrentNode->IsFirstChild){
        if(CurrentNode->parent == NULL){
            printf("──%s", Buffer);
            if(CurrentNode->child != NULL){
                printf("─");
                for(i = LastTreePrefixLength; i < LastTreePrefixLength + strlen(Buffer) + 3; i++){
                    TreePrefix[i] = ' ';
                }
                TreePrefix[i] = '\0';
                PrintNode(CurrentNode->child);
                TreePrefix[LastTreePrefixLength] = '\0';
            }
            else
                printf("\n");
        }
        else{
            printf("┬─%s", Buffer);
            if(CurrentNode->child != NULL){
                printf("─");
                strcat(TreePrefix, "│");
                for(i = LastTreePrefixLength + 3; i < LastTreePrefixLength + strlen(Buffer) + 5; i++){
                    TreePrefix[i] = ' ';
                }
                TreePrefix[i] = '\0';
                PrintNode(CurrentNode->child);
                TreePrefix[LastTreePrefixLength] = '\0';
            }
            else
                printf("\n");
            
            
            PrintNode(CurrentNode->parent);
        }
    }
    else{
        printf("%s", TreePrefix);
        if(CurrentNode->parent == NULL){
            printf("└─%s", Buffer);
            if(CurrentNode->child != NULL){
                printf("─");
                for(i = LastTreePrefixLength; i < LastTreePrefixLength + strlen(Buffer) + 3; i++){
                    TreePrefix[i] = ' ';
                }
                TreePrefix[i] = '\0';
                PrintNode(CurrentNode->child);
                TreePrefix[LastTreePrefixLength] = '\0';
            }
            else
                printf("\n");
        }
        else{
            printf("├─%s", Buffer);
            if(CurrentNode->child){
                printf("─");
                strcat(TreePrefix, "│");
                for(i = LastTreePrefixLength + 3; i < LastTreePrefixLength + strlen(Buffer) + 5; i++){
                    TreePrefix[i] = ' ';
                }
                TreePrefix[i] = '\0';
                PrintNode(CurrentNode->child);
                TreePrefix[LastTreePrefixLength] = '\0';
            }
            else
                printf("\n");
            PrintNode(CurrentNode->parent);
        }
    }
}

/* print out the entire proess tree */
void PrintTree(){
    TreePrefix[0] = '\0'; // TreePrefix = ""
    if(Root.child != NULL){
        PrintNode(Root.child);
    }
}

/* build the process node */
void BuildNode(ProcessInfo *ParentNode){
    int proc_iter;
    ProcessInfo *child_iter;
    for(proc_iter = 0; proc_iter < count; proc_iter++){
        if(!PrintKernelThreadsFlag && all_process[proc_iter].PID == 2)
            continue;
        if(all_process[proc_iter].IsVisited)
            continue;
        if(all_process[proc_iter].PPID == ParentNode->PID){
            if(NumericSortFlag){
                child_iter = ParentNode->child;
                if(child_iter == NULL){
                    all_process[proc_iter].IsFirstChild = 1;
                    all_process[proc_iter].parent = NULL;
                    ParentNode->child = &(all_process[proc_iter]);
                }
                while(child_iter != NULL){
                    if(all_process[proc_iter].PID > child_iter->PID){
                        if(child_iter->parent != NULL){
                            if(all_process[proc_iter].PID <= child_iter->parent->PID){
                                all_process[proc_iter].parent = child_iter->parent;
                                child_iter->parent = &(all_process[proc_iter]);
                                break;
                            }
                            else{
                                child_iter = child_iter->parent;
                                continue;
                            }
                        }
                        else{
                            child_iter->parent = &(all_process[proc_iter]);
                            break;
                        }
                    }
                    else{
                        all_process[proc_iter].IsFirstChild = 1;
                        all_process[proc_iter].parent = child_iter;
                        child_iter->IsFirstChild = 0;
                        ParentNode->child = &(all_process[proc_iter]);
                        break;
                    }
                }
                BuildNode(&(all_process[proc_iter]));
            }
            else{
                all_process[proc_iter].parent = ParentNode->child;
                if(all_process[proc_iter].parent != NULL)
                    all_process[proc_iter].parent->IsFirstChild = 0;
                all_process[proc_iter].IsFirstChild = 1;
                ParentNode->child = &(all_process[proc_iter]);
                BuildNode(ParentNode->child);
            }
            all_process[proc_iter].IsVisited = 1;
        } 
    }
}

/* init the hierarchical process tree */
void BuildTree(){
    Root.TGID = 0;
    Root.PID = 0;
    Root.PPID = 0;
    Root.child = NULL;
    Root.parent = NULL;
    BuildNode(&Root);
}

void ScanTask(int name_cnt, int *process_iter){
    int start, end, len, iter;
    char Path[256];
    char Buffer[256];
    char ID_Buffer[256];
    FILE *file;
    for(iter = 0; iter < name_cnt;(*process_iter)++, iter++){
        strcpy(Path, "/proc/");
        strcat(Path,(TaskNameList[iter])->d_name);
        strcat(Path, "/status");

        file = fopen(Path, "r");
        all_process[*process_iter].child = NULL;
        all_process[*process_iter].parent = NULL;
        all_process[*process_iter].IsFirstChild = 0;
        all_process[*process_iter].IsVisited = 0;
        while(!feof(file)){
            ReadLine(file, Buffer, 200);
            if(strncmp(Buffer, "Pid", 3) == 0){
                len = strlen(Buffer) + 1; // Include '\0'
                start = strlen("Pid:\t");
                for(end = 0; end < len - start; end++){
                    ID_Buffer[end] = Buffer[start + end];
                }
                all_process[*process_iter].PID = atoi(ID_Buffer);
            }
            else if(strncmp(Buffer, "PPid", 4) == 0){
                len = strlen(Buffer) + 1; // Include '\0'
                start = strlen("PPid:\t");
                for(end = 0; end < len - start; end++){
                    ID_Buffer[end] = Buffer[start + end];
                }
                all_process[*process_iter].PPID = atoi(ID_Buffer);
            }
            else if(strncmp(Buffer, "Name", 4) == 0){
                len = strlen(Buffer) + 1; // Include '\0'
                start = strlen("Name:\t");
                for(end = 0; end < len - start; end++){
                    ID_Buffer[end] = Buffer[start + end];
                }
                strcpy(all_process[*process_iter].ProcessName, ID_Buffer);
            }
            else if(strncmp(Buffer, "Tgid", 3) == 0){
                len = strlen(Buffer) + 1; // Include '\0'
                start = strlen("Tgid:\t");
                for(end = 0; end < len - start; end++){
                    ID_Buffer[end] = Buffer[start + end];
                }
                all_process[*process_iter].TGID = atoi(ID_Buffer);
            }
        }
        if(all_process[*process_iter].TGID != all_process[*process_iter].PID){
            all_process[*process_iter].PPID = all_process[*process_iter].TGID;
        }
    }
}

void ScanProcess(){
    int process_iter, start, end, len, name_iter, name_cnt;
    char Path[100];
    char TaskPath[100];
    char Buffer[200];
    char ID_Buffer[200];
    
    FILE *file; // FNMDP
    for(process_iter = 0, name_iter = 0; name_iter < count; process_iter++, name_iter++){
        strcpy(Path, "/proc/");
        strcat(Path, NameList[name_iter]->d_name);
        strcat(Path, "/status");
        
        strcpy(FilterNeq, NameList[name_iter]->d_name);
        strcpy(TaskPath, "/proc/");
        strcat(TaskPath, NameList[name_iter]->d_name);
        strcat(TaskPath, "/task");
        name_cnt = scandir(TaskPath, &TaskNameList, Filter, alphasort);
        if(name_cnt != 0){
            ScanTask(name_cnt, &process_iter);
            free(TaskNameList);
        }

        file = fopen(Path, "r");
        all_process[process_iter].child = NULL;
        all_process[process_iter].parent = NULL;
        all_process[process_iter].IsFirstChild = 0;
        all_process[process_iter].IsVisited = 0;
        while(!feof(file)){
            ReadLine(file, Buffer, 200);
            if(strncmp(Buffer, "Pid", 3) == 0){
                len = strlen(Buffer) + 1; // Include '\0'
                start = strlen("Pid:\t");
                for(end = 0; end < len - start; end++){
                    ID_Buffer[end] = Buffer[start + end];
                }
                all_process[process_iter].PID = atoi(ID_Buffer);
            }
            else if(strncmp(Buffer, "PPid", 4) == 0){
                len = strlen(Buffer) + 1; // Include '\0'
                start = strlen("PPid:\t");
                for(end = 0; end < len - start; end++){
                    ID_Buffer[end] = Buffer[start + end];
                }
                all_process[process_iter].PPID = atoi(ID_Buffer);
            }
            else if(strncmp(Buffer, "Name", 4) == 0){
                len = strlen(Buffer) + 1; // Include '\0'
                start = strlen("Name:\t");
                for(end = 0; end < len - start; end++){
                    ID_Buffer[end] = Buffer[start + end];
                }
                strcpy(all_process[process_iter].ProcessName, ID_Buffer);
            }
            else if(strncmp(Buffer, "Tgid", 3) == 0){
                len = strlen(Buffer) + 1; // Include '\0'
                start = strlen("Tgid:\t");
                for(end = 0; end < len - start; end++){
                    ID_Buffer[end] = Buffer[start + end];
                }
                all_process[process_iter].TGID = atoi(ID_Buffer);
            }
        }
    }
    count = process_iter;
}

int main(int argc, char **argv){
    int i;
    PrintPIDFlag = 0;
    NumericSortFlag = 0;
    PrintKernelThreadsFlag = 0;
    DoNotDoJobFlag = 0;
    FilterNeq[0] = '\0';
    for(i = 1; i < argc; i++){
        if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--show-pids") == 0){
            PrintPIDFlag = 1;
        }
        else if(strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--numeric-sort") == 0){
            NumericSortFlag = 1;
        }
        else if(strcmp(argv[i], "-k") == 0 || strcmp(argv[i], "--kernel-thread") == 0){
            PrintKernelThreadsFlag = 1;
        }
        else if(strcmp(argv[i], "-V") == 0 || strcmp(argv[i], "--version") == 0){
            DoNotDoJobFlag = 1;
        }
        else{
            printf("Unknown argument: %s\n", argv[i]);
            DoNotDoJobFlag = 1;
        }
    }
    if(!DoNotDoJobFlag){
        count = scandir("/proc", &NameList, Filter, alphasort);

        if(count < 0)
            perror("FATAL: Procedure scandir failed.\n");
        else{
            ScanProcess();
            BuildTree();
            PrintTree();
            free(NameList);
        }
    }
    return 0;
}
