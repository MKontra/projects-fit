#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#include "pthread.h"
#include "unistd.h"

#define READ_TURN 0
#define PROCESS_TURN 1

char buff[513];
int end;

int turn = READ_TURN;
//int data_rdy;
//int data_processed;

volatile sig_atomic_t fgtaskid;
volatile sig_atomic_t is_fg_executing = 0;

//pthread_mutex_t init_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t buffer_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_cond = PTHREAD_COND_INITIALIZER;
//pthread_cond_t exec_cond = PTHREAD_COND_INITIALIZER;

pthread_t buffer_reading_thread;
pthread_t buffer_processing_thread;

struct sigaction oldsigint;
struct sigaction oldsigchld;

void sigint_sig_handler(int sigdata)
{
  //notify user if there was fg task kill
  if ( is_fg_executing == 1 ) printf("Foreground task interrupted %d\n", fgtaskid);
}

void sigchld_sig_handler(int sigdata)
{
  int status;
  int chld = waitpid(-1, &status, 0);
  //printf("child terminated %d %d %d\n",getpid(), chld,is_fg_executing);
  if ( is_fg_executing == 1 && chld == fgtaskid )
     {//printf("unlocking\n");
      is_fg_executing = 0;
     } else
     {
       printf("Background task terminated: %d\n", chld); 
     }
}

void * buffer_reading_thread_func( void * params )
{
  int readcnt;
  char prmpt[] = "prompt:";
  while ( 1 )
  {
    pthread_mutex_lock(&buffer_lock);
    while ( turn == PROCESS_TURN && end != 1) pthread_cond_wait(&buffer_cond, &buffer_lock);
    if ( end == 1 ) break;
    write(STDOUT_FILENO, prmpt, strlen(prmpt));
    while ( 1 )
    {
      readcnt = read(STDIN_FILENO, buff, 513);
      if ( readcnt < 513 )
      {
        buff[readcnt-1] = 0;
        break;
      } else
      {
        char c;
        while ( (read(STDIN_FILENO, &c, 1) == 1) && (c != '\n') );
        printf("Input buffer too long...\n");
      }
    }
    
    turn = PROCESS_TURN;
    pthread_cond_signal(&buffer_cond);

    //while ( data_processed == 0 ) pthread_cond_wait(&exec_cond, &buffer_lock);
    //data_processed = 0;

    pthread_mutex_unlock(&buffer_lock);
  }
  pthread_exit(0);
}

int parse_command_line( char * input, char ** command, char * (*params)[], char ** ofilename, char ** ifilename, int * isbg )
{
  char * tmp = NULL;
  if ( (tmp = strpbrk( input, "&")) != NULL )
  {
    *tmp = '\0';
    *isbg = 1;
  }
  tmp = strtok( input, " " );
  if ( tmp == NULL )
  {
    return -1;
  } else
  {
    *command = tmp;
    (*params)[0] = tmp;
  }
  int i = 1;
  while ( 1 )
  {
    tmp = strtok( NULL, " " );
    if(tmp == NULL ) break;
    if (*tmp == '>')
    {
      if (strlen( tmp ) == 1) 
      {
        *ofilename = strtok( NULL, " " );
      } else
      {
        *ofilename = tmp+1;
      }
    } else 
    if (*tmp == '<')
    {
      if (strlen( tmp ) == 1) 
      {
        *ifilename = strtok( NULL, " " );
      } else
      {
        *ifilename = tmp+1;
      }
    } else 
    {
      (*params)[i] = tmp;
      i++;
    }
  }
  (*params)[i] = NULL;
  return 0;

}

void * buffer_processing_thread_func( void * params )
{
  sigset_t unblck_child;

  pthread_mutex_lock(&buffer_lock);
  //pthread_mutex_unlock(&init_lock);

  sigemptyset(&unblck_child);
  sigaddset(&unblck_child, SIGCHLD);
  pthread_sigmask(SIG_UNBLOCK, &unblck_child, NULL);  

  while ( 1 )
  {
    while ( turn == READ_TURN  && end != 1) pthread_cond_wait(&buffer_cond, &buffer_lock);
    if ( end == 1 ) break;
    //data_rdy = 0;

    //process input here

    char * redirectin = NULL;
    char * redirectout = NULL;
    char * command = NULL;
    char * params[10];
    int isbgtask = 0;

    if( 0 == strcmp(buff, "exit") ) 
    { 
      end = 1; 
    } else
    if ( parse_command_line( buff, &command, &params, &redirectout, &redirectin, &isbgtask ) != -1 )
    {
      //( char * input, char ** command, char *** params, char ** ofilename, char ** ifilename, int * isbg )
      //printf("in pid: %d\n", getpid() );make

      //printf ( "command: %s\n", command );
      
      //int i = 0;
      //while ( 1 )
      //{
      //  if ( params[i] == NULL ) break;
      //  printf("param %d: %s\n", i, params[i] );
      //  i++;
      //}
  
      //printf("new output: %s\n", redirectout ); 
      //printf("new input: %s\n", redirectin );
  
      //printf("isbgtask %d\n", isbgtask);

      //zmen streamy

      int oldstdin = dup(STDIN_FILENO);
      int oldstdout = dup(STDOUT_FILENO);

      if ( redirectin != NULL )
      {
          close (STDIN_FILENO);  
          if ( open(redirectin, O_RDONLY) == -1 )
          { 
            dup2(oldstdin, 0);
            printf("Cannot open file for input, using stdin\n");
            perror(NULL);
          }
      }
      if ( redirectout != NULL )
      {
          close (STDOUT_FILENO);
          if ( open(redirectout, O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR | S_IWUSR) == -1 )
          { 
            dup2(oldstdout, 1);
            printf("Cannot open file for output, using stdout\n");
            perror(NULL);
          }
      }

      if ( isbgtask == 0 ) // fgtask
      {
        pthread_sigmask(SIG_BLOCK, &unblck_child, NULL); 
        is_fg_executing = 1;
        pid_t newpid;
        fgtaskid = fork();
        newpid = fgtaskid;
        if ( newpid == -1 )
        {
          perror("Cannot spawn child: ");
        } else
        if ( newpid == 0 ) //decko
        { 
            sigaction(SIGCHLD, &oldsigchld, NULL);
            sigaction(SIGINT, &oldsigint, NULL);
            sigprocmask(SIG_UNBLOCK, &unblck_child, NULL);
            if ( execvp(command, params) == -1 )
            {
              perror("Execvp failed: "); 
            }   
            _exit(0);        
        } else
        {
          //rodic
          sigset_t allow_chld;
          sigfillset(&allow_chld);
          sigdelset(&allow_chld,SIGCHLD);
          //printf("waiting on fg task\n");
          while ( is_fg_executing == 1 ) sigsuspend (&allow_chld);
          //printf("fg task done\n");
        }
        pthread_sigmask(SIG_UNBLOCK, &unblck_child, NULL); 

       } else //isbgtask == 1
        {

          //bgtask - do not wait
          pid_t newpid;
          newpid = fork();
          if ( newpid == -1 )
          {
            perror("Cannot spawn child: ");
          } else
          if ( newpid == 0 ) //decko
          {
              sigaction(SIGCHLD, &oldsigchld, NULL);
              sigaction(SIGINT, &oldsigint, NULL);
              sigprocmask(SIG_UNBLOCK, &unblck_child, NULL);
              if ( execvp(command, params) == -1 )
              {
                perror("Execvp failed: "); 
              }   
              _exit(0);        
          } else
          {
            //rodic
            //do nothing
          }

        }
        
          dup2(oldstdin, 0);        
          dup2(oldstdout, 1);
          close(oldstdin);
          close(oldstdout);
        //sigsuspend()
      } else
      {
        printf("Bad Command line\n");
      }

    turn = READ_TURN;
    pthread_cond_signal(&buffer_cond);      
    //data_processed = 1;
    //pthread_cond_signal(&exec_cond);

  }

  pthread_mutex_unlock(&buffer_lock);
  pthread_exit(0);
}

int main(int arcg, char * argv[])
{
  //printf("pid: %d\n", getpid());
  struct sigaction sigchld_handler;
  sigchld_handler.sa_handler = sigchld_sig_handler;
  sigchld_handler.sa_flags = 0;
  sigfillset(&sigchld_handler.sa_mask);
  sigaction(SIGCHLD, &sigchld_handler, &oldsigchld);

  struct sigaction sigint_handler;
  sigint_handler.sa_handler = sigint_sig_handler;
  sigint_handler.sa_flags = 0;
  sigfillset(&sigint_handler.sa_mask);
  sigaction(SIGINT, &sigint_handler, &oldsigint);

  sigset_t blck_child;
  sigemptyset(&blck_child);
  sigaddset(&blck_child, SIGCHLD);
  sigprocmask(SIG_BLOCK, &blck_child, NULL);

  int rc;
  //data_rdy = 0;
  //data_processed = 0;
  //pthread_mutex_lock(&init_lock);
  rc = pthread_create(&buffer_processing_thread, NULL, buffer_processing_thread_func, NULL);
  //pthread_mutex_lock(&init_lock);

  rc = pthread_create(&buffer_reading_thread, NULL, buffer_reading_thread_func, NULL);

  rc = pthread_join(buffer_processing_thread, NULL);
  rc = pthread_join(buffer_reading_thread, NULL);

  kill(0, SIGHUP);

  return 0;
}
