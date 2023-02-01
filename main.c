#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

#define print(...) do{printf(__VA_ARGS__); fflush(stdout); } while(0)


void how_many() {


    for(int i=0; i<2; i++) {
        int x = fork();
        sleep(1);
        print("X\n");
    }
    exit(1);
}

void how_many_with_pid() {
    for(int i=0; i<2; i++) {
        int pid = fork();
        //int status;
        //waitpid(pid, &status,0);
        if(pid == 0) print("X");
    }
    exit(1);
}

void how_many_with_pid_parent() {
    for(int i=0; i<2; i++) {
        int pid = fork();
        if(pid != 0) print("X");
    }
    exit(1);
}

void fork_mem_not_shared() {
    int val = 20;
    int pid = fork();

    if(pid == 0) val = 30;

    print("Prints: %d\n",val);
}

struct l_node {
    int nb;
    struct l_node* next;
};

#define l_node struct l_node*

l_node link_node(l_node current, l_node next) {
    current->next = next;
    return next;
}

l_node new_node(int val) {
    l_node node = malloc(sizeof(l_node));
    node->nb = val;
    node->next = NULL; //bonne pratique!
    return node;
}

l_node free_node(l_node node) {
    l_node next = node->next;
    print("%d\n", node->nb);
    free(node);
    return next;
}

void free_l_list(l_node head) {
    //Tres laid! ne faites pas ça! Mais, free n'est pas si compliqué: en une seule for-loop, sans corps, on peut libérer
    //toute une liste chaînée!
    for(; head != NULL; head = free_node(head));
}

void fork_pre_fork() {
    l_node head = new_node(0);
    l_node current = head;

    //for loop sur une seule ligne
    for(int i=1; i<10; i++) current = link_node(current, new_node(i)); //current = current.link_node(new Node(i));

    int pid = fork();
    int child_code = 0;
    if(pid == 0) {
        free_l_list(head);
        exit(40);
    } else {
        wait(&child_code);
        child_code = WEXITSTATUS(child_code);
        print("\nChild code: %d\n\n", child_code);
        free_l_list(head);
    }

    //DONC! Tout ce qui est malloc avant le fork doit être free deux fois: dans le parent et dans l'enfant
    //L'enfant partage tout ce que le parent avait avant le fork
}

void fork_pid_signals() {
    int pid = fork();
    if(pid == 0) {
        while(1) print("CHILD RUNNING FREE\n");
    } else {
        kill(pid, 18);
        print("CHILD KILLED!\n");
    }

    //Race conditions.....
}

void exec_kills() {
    char *x[] = {"ls", NULL};
    execvp("ls", x);
}


int main() {

    //how_many();
    //how_many_with_pid();
    //how_many_with_pid_parent();
    fork_mem_not_shared();
    //fork_pre_fork();
    //fork_pid_signals();
    return 0;
}
