#include "shell.h"
#include <stddef.h>
#include "clib.h"
#include <string.h>
#include "fio.h"
#include "filesystem.h"

#include "FreeRTOS.h"
#include "task.h"
#include "host.h"

typedef struct {
	const char *name;
	cmdfunc *fptr;
	const char *desc;
} cmdlist;

void ls_command(int, char **);
void man_command(int, char **);
void cat_command(int, char **);
void ps_command(int, char **);
void host_command(int, char **);
void help_command(int, char **);
void host_command(int, char **);
void mmtest_command(int, char **);
void test_command(int, char **);
void new_command(int, char **);
void _command(int, char **);
void vTask1(); 

#define MKCL(n, d) {.name=#n, .fptr=n ## _command, .desc=d}

cmdlist cl[]={
	MKCL(ls, "List directory"),
	MKCL(man, "Show the manual of the command"),
	MKCL(cat, "Concatenate files and print on the stdout"),
	MKCL(ps, "Report a snapshot of the current processes"),
	MKCL(host, "Run command on host"),
	MKCL(mmtest, "heap memory allocation test"),
	MKCL(help, "help"),
	MKCL(test, "test new function"),
	MKCL(new, "test new task"),
	MKCL(, ""),
};
/* string to integer */
int stringToInt(char inputString[]) {
    int i, sign, offset, n = 0;
    if (inputString[0] == '-') {  
        sign = -1;
    }
    if (sign == -1) {  
        offset = 1;
    }
    else {
        offset = 0;
    } 
    for (i = offset; inputString[i] != '\0'; i++) {
        n = n * 10 + inputString[i] - 48;
    } 
    if (sign == -1) {
        n = -n;
    } 
    return n;  
}
/* fibonacci */
int fib(int n){
    if(n == 0 || n == 1){
        return n;
    }
    else{
        return fib(n - 1)  + fib(n - 2);
    }
}
/* is prime */
int isPrime(int n){
    int flag = 0;		// if flag == 0, n is prime
    for(int i = 2; i * i < n ;i++){
	if(n % i == 0){
	    flag++;
        }
    }
    if(flag == 0){
	return 1;
    }
    else{
	return 0;
    }
}
void newTask(void *pvParameters)     
{
	
	int *pcTask = (int *)pvParameters;
	for(;;){
            fio_printf(1, "\n\r[%d] is running!..\r\n",pcTask);
	    vTaskDelay(1000/portTICK_RATE_MS); 
	}
	vTaskDelete(NULL);
}
  
int parse_command(char *str, char *argv[]){
	int b_quote=0, b_dbquote=0;
	int i;
	int count=0, p=0;
	for(i=0; str[i]; ++i){
		if(str[i]=='\'')
			++b_quote;
		if(str[i]=='"')
			++b_dbquote;
		if(str[i]==' '&&b_quote%2==0&&b_dbquote%2==0){
			str[i]='\0';
			argv[count++]=&str[p];
			p=i+1;
		}
	}
	/* last one */
	argv[count++]=&str[p];

	return count;
}

void ls_command(int n, char *argv[]){
    fio_printf(1,"\r\n"); 
    int dir;
    if(n == 0){
        dir = fs_opendir("");
    }else if(n == 1){
        dir = fs_opendir(argv[1]);
        //if(dir == )
    }else{
        fio_printf(1, "Too many argument!\r\n");
        return;
    }
(void)dir;   // Use dir
}

int filedump(const char *filename){
	char buf[128];

	int fd=fs_open(filename, 0, O_RDONLY);

	if( fd == -2 || fd == -1)
		return fd;

	fio_printf(1, "\r\n");

	int count;
	while((count=fio_read(fd, buf, sizeof(buf)))>0){
		fio_write(1, buf, count);
    }
	
    fio_printf(1, "\r");

	fio_close(fd);
	return 1;
}

void ps_command(int n, char *argv[]){
	signed char buf[1024];
	vTaskList(buf);
        fio_printf(1, "\n\rName          State   Priority  Stack  Num\n\r");
        fio_printf(1, "*******************************************\n\r");
	fio_printf(1, "%s\r", buf + 2);	
}

void cat_command(int n, char *argv[]){
	if(n==1){
		fio_printf(2, "\r\nUsage: cat <filename>\r\n");
		return;
	}

    int dump_status = filedump(argv[1]);
	if(dump_status == -1){
		fio_printf(2, "\r\n%s : no such file or directory.\r\n", argv[1]);
    }else if(dump_status == -2){
		fio_printf(2, "\r\nFile system not registered.\r\n", argv[1]);
    }
}

void man_command(int n, char *argv[]){
	if(n==1){
		fio_printf(2, "\r\nUsage: man <command>\r\n");
		return;
	}

	char buf[128]="/romfs/manual/";
	strcat(buf, argv[1]);

    int dump_status = filedump(buf);
	if(dump_status < 0)
		fio_printf(2, "\r\nManual not available.\r\n");
}

void host_command(int n, char *argv[]){
    int i, len = 0, rnt;
    char command[128] = {0};

    if(n>1){
        for(i = 1; i < n; i++) {
            memcpy(&command[len], argv[i], strlen(argv[i]));
            len += (strlen(argv[i]) + 1);
            command[len - 1] = ' ';
        }
        command[len - 1] = '\0';
        rnt=host_action(SYS_SYSTEM, command);
        fio_printf(1, "\r\nfinish with exit code %d.\r\n", rnt);
    } 
    else {
        fio_printf(2, "\r\nUsage: host 'command'\r\n");
    }
}

void help_command(int n,char *argv[]){
	int i;
	fio_printf(1, "\r\n");
	for(i = 0;i < sizeof(cl)/sizeof(cl[0]) - 1; ++i){
		fio_printf(1, "%s - %s\r\n", cl[i].name, cl[i].desc);
	}
}

void test_command(int n, char *argv[]) {
    /* Hello Meassage for test command */
    fio_printf(1, "Hello!Test!\r\n");

    if(n == 1 || n == 2){
        fio_printf(2, "\rUsage: test <options> <number>\r\n");
        fio_printf(2, "\roptions : 0 for fibonacci, 1 for isPrime.\r\n");
    }
    else if(n == 3){
	if(stringToInt(argv[1]) == 0)
	    fio_printf(1, "\r%d\r\n",fib(stringToInt(argv[2])));
        else if (stringToInt(argv[1]) == 1){
            if(isPrime(stringToInt(argv[2])) == 0)
	        fio_printf(1, "\r%s is NOT prime number!\r\n",argv[2]);
            else
	        fio_printf(1, "\r%s is prime number!\r\n",argv[2]);
        }
        else{
            fio_printf(2, "\rError!\r\n");
            fio_printf(2, "\rUsage: test <options> <number>\r\n");
            fio_printf(2, "\roptions : 0 for fibonacci, 1 for isPrime.\r\n");
        }
    }
    else{
        fio_printf(2, "Too many argument!\r\n");
    }
}

void _command(int n, char *argv[]){
    (void)n; (void)argv;
    fio_printf(1, "\r\n");
}
void new_command( int n, char *argv[]){
    	static int task_count = 1;  // begin -> 1
	fio_printf(1, "\r\n");
	xTaskCreate(newTask,(signed portCHAR *)"newTask",100,(void*)task_count,5, NULL);
    	task_count++;
}

cmdfunc *do_command(const char *cmd){

	int i;

	for(i=0; i<sizeof(cl)/sizeof(cl[0]); ++i){
		if(strcmp(cl[i].name, cmd)==0)
			return cl[i].fptr;
	}
	return NULL;	
}
