#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1 /* XPG 4.2 - needed for WCOREDUMP() */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "unistd.h"

void dumpProcessInfo(char * label)
{
  printf("%s identification: \n", label); /*grandparent/parent/child */
  printf("pid = %d,ppid = %d,pgrp = %d\n", (int)getpid(), (int)getppid(), (int)getpgrp() );
  printf("uid = %d,gid = %d\n", (int)getuid(), (int)getgid() );
  printf("euid = %d,egid = %d\n", (int)geteuid(), (int)getegid() );
}

void dumpTerminationInfo(char* label, int pid, int status)
{
  printf("%s exit (pid = %d):", label, pid);
  if ( WIFEXITED(status) )
  {
    printf("normal termination (exit code = %d)\n", WEXITSTATUS(status) );
  } else
  if ( WIFSIGNALED(status) )
  {
    #ifdef WCOREDUMP
    if ( WCOREDUMP(status) )
    {
      printf("signal termination %s(signal = %d)\n", "with core dump ", WTERMSIG(status) );
    } else
    {
      printf("signal termination %s(signal = %d)\n", "", WTERMSIG(status) );
    }
    #else
    printf("signal termination %s(signal = %d)\n", "", WTERMSIG(status) );
    #endif
  } else
  {
    printf("unknown type of termination\n");
  }
}

/* ARGSUSED */
int main(int argc, char *argv[])
{
  int parentpid;
  int childpid;
  int statvalprnt;
  int statvalchld;
  if ( argc <= 1 )
  {
    printf("Missing parameters\n");
    exit(1);
  }
  dumpProcessInfo("grandparent");
  if ( (parentpid = fork()) == 0 ){
        /*parent code*/
        dumpProcessInfo("parent");
    if ( (childpid = fork()) == 0 ){
      /*child code*/
      dumpProcessInfo("child");
      execv(argv[1], &argv[1]);
    } else {
      waitpid(childpid, &statvalchld, 0);
      dumpTerminationInfo("child", childpid, statvalchld);
    }
  } else {
    waitpid(parentpid, &statvalprnt, 0);
    dumpTerminationInfo("parent", parentpid, statvalprnt);
  }
  return 0;
  }

