/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelabbas <zelabbas@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 10:21:23 by zelabbas          #+#    #+#             */
/*   Updated: 2024/04/24 16:34:20 by zelabbas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int g_fd;

int	ft_strlen(char *str)
{
	int i;

	i = -1;
	while (str[++i])
		;
	return (i);
}

int	print_error(char *str)
{
	int len;

	len = ft_strlen(str);
	write(2, str, len);
	return (1);
}

int ft_cd(char **av)
{
	if (av[2] && strcmp(av[2], "|") && strcmp(av[2], ";"))
		return (print_error("error: cd: bad arguments\n"));
	if (chdir(av[1]) == -1)
	{
		print_error("error: cd: cannot change directory to ");
		print_error(av[1]);
		print_error("\n");
	}
	return (0);
}

int	execution(char **av, char **env, int i)
{
	int status;
	int fd[2];
	int pid;
	int check;

	check = 0;
	if (av[i] && !strcmp(av[i], "|"))
		check = 1;
	if (av[i] == *av)
		return (0);
	if (pipe(fd) == -1)
		return (print_error("error: fatal\n"));
	pid = fork();
	if (pid == -1)
		return (print_error("error: fatal\n"));
	if (pid == 0)
	{
		dup2(g_fd, 0);
		close(fd[0]);
		if (check)
			dup2(fd[1], 1);
		if (g_fd)
			close(g_fd);
		close(fd[1]);
		av[i] = NULL;
		if (execve(*av, av, env) == -1)
		{
			print_error("error: cannot execute ");
			print_error(*av);
			print_error("\n");
			exit(1);
		}
	}
	else
	{
		close(fd[1]);
		waitpid(pid, &status, 0);
		if (g_fd)
			close(g_fd);
		if (check)
			g_fd = dup(fd[0]);
		close(fd[0]);
	}
	return (0);
}

int main(int ac, char **av, char **env)
{
	int i;

	if (ac == 1)
		return (0);
	else
	{
		i = 1;
		av[ac] = NULL;
		while (av[i - 1] && av[i])
		{
			av = av + i;
			i = 0;
			while (av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
				i++;
			if (!strcmp(*av, "cd"))
				ft_cd(av);
			else
				execution(av, env, i);
			i++;
		}
	}
}