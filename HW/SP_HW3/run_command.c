/*
 * run_command.c :    do the fork, exec stuff, call other functions
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include "shell.h"

/*
waitpid()會暫時停止目前程序的執行, 直到有訊號來到或子程序結束. 如果在呼叫wait()時子程序
已經結束, 則wait()會立即傳回子程序結束狀態值. 子程序的結束狀態值會由參數status 傳回, 而
子進程的進程識別碼也會一快回傳. 如果不在意結束狀態值, 則參數status 可以設為NULL. 參數pid
為欲等待的子程序識別碼, 其他數值意義如下：
1、pid<-1 等待進程組識別碼為pid絕對值的任何子程序.
2、pid=-1 等待任何子程序, 相當於wait().
3、pid=0 等待進程組識別碼與目前進程相同的任何子進程.
4、pid>0 等待任何子程序識別碼為pid 的子程序.
*/

/*
參數option 可以為0 或下面的OR 組合：
1、WNOHANG：如果沒有任何已經結束的子進程則馬上返回, 不予以等待；
2、WUNTRACED：如果子進程進入暫停執行情況則馬上返回, 但結束狀態不予以理會. 子進程的結束狀態返回後存於status, 底下有幾個宏可判別結束情況；
3、WIFEXITED(status)：若子程序正常結束則為非0 值；
4、WEXITSTATUS(status)：取得子程序exit()回傳的結束程式碼, 一般會先用WIFEXITED 來判斷是否正常結束才能使用此巨集；
5、WIFSIGNALED(status)：如果子程序是因為訊號而結束則此宏值為真；
6、WTERMSIG(status)：取得子程序因訊號而中止的訊號碼, 一般會先用WIFSIGNALED 來判斷後才使用此巨集；
7、WIFSTOPPED(status)：若子程序處於暫停執行情況則此巨集值為真. 一般只有使用WUNTRACED時才會有此情況；
8、WSTOPSIG(status)：取得引發子程序暫停的訊號代碼, 一般會先用；
9、WIFSTOPPED 來判斷後才使用此巨集。
*/

void waiting(pid_t pid){

}

void run_command(char **myArgv) {
    pid_t pid;
    int stat;

    /* Create a new child process. */
    pid_t pid = fork();

    switch (pid) {

        /* Error. */
        case -1 :
            perror("fork");
            exit(errno);

        /* Child. */
        case 0 :
            /* Run command in child process. */
            execvp(myArgv[0], myArgv);

            /* Handle error return from exec */
			exit(errno);
        
        /* Parent. */
        default :
            /* Wait for child to terminate. */
            if(do_wait)
                _parent_wait(pid);
                break;

            /* Optional: display exit status.  (See wstat(5).)
             * Fill in code.
			 */

            return;
    }
}
