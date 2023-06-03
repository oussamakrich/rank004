#include "microshell.h"
char **envp;
int	count_tab(char **av)
{
	int i = 0;
	int count = 0;
	 while (av[i])
	 {
		 if (strcmp(av[i], "|") && strcmp(av[i], ";"))
		 {
			 count++;
			 while (av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
				 i++;
		 }
		 else
			 i++;
	 }
		return (count);
}

t_tab get_arg(char **av, int *ii)
{
	int i = *ii;
	int j = 0;
	int count = 0;
	t_tab tab;

	 while (av[i] && strcmp(av[i], "|") && strcmp(av[i++], ";"))
		 count++;
	i = *ii;
	tab.args = malloc(sizeof(char *) * (count + 1));
	tab.args[count] = NULL;
	tab.out = 1;
	while (av[i])
	{
		if (!strcmp(av[i], "|") || !strcmp(av[i], ";"))
		{
			tab.out = 1;
			if (!strcmp(av[i], "|"))
				tab.out = 2;
			break ;
		}
		tab.args[j++] = av[i++];
	}
	j = 0;
	*ii = i;
	return (tab);
}

t_tab *get_tab(char **av, int *counter)
{
	int i= 0;
	int j = 0;
	int next_in;
	t_tab *tab;


	*counter = count_tab(av);
	tab = malloc(sizeof(t_tab) * *counter);
	next_in = 0;
	while (av[i])
	{
		tab[j] = get_arg(av, &i);
		tab[j].in = next_in;
		j++;
		next_in = 0;
		if (!av[i])
			break ;
		if (!strcmp(av[i], "|"))
			next_in = 2;
		i++;
	 	while (av[i] && (!strcmp(av[i], "|") || !strcmp(av[i], ";")))
			i++;
	}
	return (tab);
}

int ft_strlen(char *l)
{
	int i = 0;
	if (!l)
		return (0);
	while (l[i])
		i++;
	return (i);
}

void	exec_cd(t_tab tab)
{
	if (!tab.args[1])
		write (2, "error: cd: bad arguments\n", 25); 
	else if (chdir(tab.args[1])==-1)
	{
		write (2, "error: cd: cannot change directory to ", 38);
		write(2, tab.args[1], ft_strlen(tab.args[1]));
		write(2, "\n", 1);
	}
	return ;
}

void	exec(t_tab tab, int *fd, int *tmp_in)
{
	close(fd[0]);
	if (tab.out == 2)
		dup2(fd[1], 1);
	close(fd[1]);
	if (tab.in == 2)
		dup2(*tmp_in,0);
	close (*tmp_in);
	execve(tab.args[0], tab.args, envp);
	write (2, "error: cannot execute ", 22);
	write(2, tab.args[0], ft_strlen(tab.args[0]));
	write(2, "\n", 1);
	exit(1);
}

void	exec_cmd(t_tab *tab, int counter)
{
	int i = 0;
	int pid;
	int tmp_in = dup(0);
	int	fd[2];
	while (i < counter)
	{
		if (tab[i].in == 0)
			while (wait(NULL) != -1);
		if (!strcmp(tab[i].args[0], "cd"))	
			exec_cd(tab[i]);
		else
		{	
			pipe(fd);
			pid = fork();
			if (pid == 0)
				exec(tab[i], fd, &tmp_in);
			close(fd[1]);
			if (tab[i].out == 2)
				dup2(fd[0], tmp_in);
			close(fd[0]);
		}
		i++;
	}
	close(tmp_in);
	while (wait(NULL) != -1);
}

int main(int ac, char **av, char **env)
{
	envp = env;
	t_tab *tab = NULL;		
	int i = 1;
	int counter = 0;
	
	if (ac == 1)
		return 0;
	while (av[i] && (!strcmp(av[i], "|") || !strcmp(av[i], ";")))
			i++;
	if (!av[i])
		return 0;
	tab = get_tab(av + i, &counter);
	exec_cmd(tab, counter);
}
