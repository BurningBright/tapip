#include "lib.h"
#include "netif.h"
#include "arp.h"

extern void shell_master(char *);
extern void *shell_worker(void *);
extern void shell_init(void);

/*
 * 0 timer for ip and arp
 * 1 timer for tcp
 * 2 core stack
 * 3 shell worker
 */
pthread_t threads[4];

pthread_t newthread(pfunc_t thread_func)
{
    pthread_t tid;
    if (pthread_create(&tid, NULL, thread_func, NULL))
        perrx("pthread_create");
    return tid;
}

void net_stack_init(void)
{
    netdev_init();
    arp_cache_init();
    // rt_init();
    // socket_init();
    shell_init();
}

void net_stack_run(void)
{
    /* create timer thread */
    threads[0] = newthread((pfunc_t)net_timer);
    dbg("thread 0: net_timer");

    /* tcp timer */
    // threads[1] = newthread((pfunc_t)tcp_timer);
    // dbg("thread 1: tcp_timer");

    threads[2] = newthread((pfunc_t)netdev_interrupt);
    dbg("thread 2: netdev_interrupt");

    threads[3] = newthread((pfunc_t)shell_worker);
    dbg("thread 3: shell worker");

    /* net shell runs! */
    shell_master(NULL);
}

void net_stack_exit(void)
{
    if (pthread_cancel(threads[0]))
        perror("kill child 0");

    //if (pthread_cancel(threads[1]))
    //    perror("kill child 1");

    if (pthread_cancel(threads[2]))
        perror("kill child 2");

    /* shell work will be killed by shell master */
    if (pthread_join(threads[3], NULL))
        perror("kill child 3");

    netdev_exit();
}

int main(int argc, char **argv)
{
/*
    unsigned int ip = 16777343;
    printfs(16, "%d.%d.%d.%d", (ip) & 0xff, ((ip) >> 8) & 0xff, ((ip) >> 16) & 0xff, ((ip) >> 24) & 0xff);

    printf("\n");
    unsigned int ip_1;
    str2ip("127.0.0.1", &ip_1);
    printf("%d\n", ip_1);

//    char *c = "hello world";
//    c[1] = 'H';
*/
    net_stack_init();
    net_stack_run();
    sleep(1);
    net_stack_exit();
    dbg("wait system exit");

    return 0;
}
