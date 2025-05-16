/* file_search_shard.c ─ Child·Thread 조합 테스트용 (락 X, 샤딩 O) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include "file_search.h"

#define MAX_DIRS 256
#define PATH_STR 1024

/* 1-depth 디렉터리 수집 */
static int list_dirs(const char* root, char arr[][PATH_STR]){
    int n=0; DIR* d=opendir(root); struct dirent* e;
    while((e=readdir(d))){
        if(e->d_type==DT_DIR && e->d_name[0]!='.')
            snprintf(arr[n++],PATH_STR,"%s/%s",root,e->d_name);
    } closedir(d); return n;
}

/* 스레드 인자 */
typedef struct{
    const char* kw; const char* ext; int maxd;
    char (*dirs)[PATH_STR]; int dir_cnt; int tid; int threads;
    result_list_t* rlist;
} thr_arg;

/* 스레드 함수: 자신 몫 디렉터리만 큐에 넣어 검색 */
static void* run_thread(void* vp){
    thr_arg* a=(thr_arg*)vp;
    task_queue_t q; init_queue(&q);
    for(int i=0;i<a->dir_cnt;i++)
        if(i % a->threads == a->tid)
            traverse_directory_to_queue(a->dirs[i],&q,a->ext,1,a->maxd);

    thread_arg w = {0};
    strncpy(w.keyword,a->kw,256);
    w.task_queue=&q; w.result_list=a->rlist; w.thread_id=a->tid;
    worker_thread(&w);
    return NULL;
}

int main(int ac,char**av){
    if(ac<2){fprintf(stderr,"사용법: %s <kw> [--threads=N] [--procs=M] [--ext=.txt] [--max-depth=N]\n",av[0]);return 1;}

    const char* kw=av[1],*root="./test_root",*ext=NULL;
    int threads=1, procs=0, maxd=10;          /* procs=0 ▶ Child 없음(가) */

    for(int i=2;i<ac;i++){
        if(!strncmp(av[i],"--threads=",10))  threads=atoi(av[i]+10);
        else if(!strncmp(av[i],"--procs=",8)) procs=atoi(av[i]+8);
        else if(!strncmp(av[i],"--ext=",6))   ext=av[i]+6;
        else if(!strncmp(av[i],"--max-depth=",12)) maxd=atoi(av[i]+12);
    }
    if(threads<1)threads=1; if(procs<0)procs=0;

    /* 1-depth 목록 */
    char dirlist[MAX_DIRS][PATH_STR]; int nd=list_dirs(root,dirlist);

    /* Child 생성 (procs==0 → 가조건) */
    int child_total = procs;
    for(int cid=0; cid<(procs?procs:1); cid++){
        pid_t pid = (procs?fork():0);
        if(pid<0){perror("fork");exit(1);}
        if(pid==0){                     /* Child 또는 단일-모드 실행 */
            result_list_t rlist={0};

            /* 스레드 풀 */
            pthread_t tid[threads];
            thr_arg a = {.kw=kw,.ext=ext,.maxd=maxd,.dirs=dirlist,
                          .dir_cnt=nd,.threads=threads,.rlist=&rlist};

            for(int t=0;t<threads;t++){a.tid=t;pthread_create(&tid[t],0,run_thread,&a);}
            for(int t=0;t<threads;t++)  pthread_join(tid[t],0);

            char out[64];
            if(procs==0) sprintf(out,"results.csv");
            else sprintf(out,"results_P%d.csv",cid);
            save_results_to_csv(out,&rlist);
            exit(0);
        }
        if(!procs) break;   /* Child 없음(가) 경로면 fork 안 했으니 루프 한번만 */
    }
    /* 부모: Child wait */
    for(int i=0;i<child_total;i++) wait(NULL);
    return 0;
}
