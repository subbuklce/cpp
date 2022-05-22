//#include "inc/ThreadPool_void_type.h"
#include "include/script_launch.h"

int main(){

 pid_t pid = cScript::proc_find("code");
        if (pid == -1) {
            printf("%s: not found\n", "code");
        } else {
            printf("The pid is: %d\n",  pid);
        }

    
    return 0;
}