#ifndef MICROSHELL_H
# define MICROSHELL_H


#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

typedef struct s_tab
{
	char **args;
	int in;
	int out;
} t_tab;

#endif
