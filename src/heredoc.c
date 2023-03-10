#include "../inc/minishell.h"

int heredoc_sub(t_group *group, char *str[3], int fd[2]) //temp str[0] , buf str[1]
{
	while (!str[0][0] || ft_strncmp(str[0], group->redir_infile, ft_strlen(group->redir_infile) + 1))
	{
		free (str[0]);
		str[0] = readline("> "); // ctrtl D händeln  also return wert von 0
		if (!str[0])
			break;
		str[2] = str[1];
		str[1] = ft_strjoin(str[1], str[0]);
		free(str[2]);
		str[2] = str[1];
		str[1] = ft_strjoin(str[1], "\n");
		free (str[2]);
	}
	if (!str[0])
		write(fd[1], str[1], ft_strlen(str[1]));
	else
		write(fd[1], str[1], ft_strlen(str[1]) - ft_strlen(group->redir_infile) - 1);
	close(fd[1]);
	return (fd[0]);

	// while (!temp[0] || ft_strncmp(temp, group->redir_infile, ft_strlen(group->redir_infile) + 1))
	// {
	// 	free (temp);
	// 	temp = readline("> "); // ctrtl D händeln  also return wert von 0
	// 	if (!temp)
	// 		break;
	// 	buf_ = buf;
	// 	buf = ft_strjoin(buf, temp);
	// 	free(buf_);
	// 	buf_ = buf;
	// 	buf = ft_strjoin(buf, "\n");
	// 	free (buf_);
	// }
	// if (!temp)
	// 	write(fd[1], buf, ft_strlen(buf));
	// else
	// 	write(fd[1], buf, ft_strlen(buf) - ft_strlen(group->redir_infile) - 1);
	// close(fd[1]);
}

/**
 * @brief while readline not EOF
 * join from readline
 * 
 * @return int 
 */
int	heredoc(t_group *group) // check fd's closed // save mallocs ? 
{
	int fd[2];
	char	*str[3];
	
	str[0] = malloc(1);
	str[0][0] = 0;
	str[1] = malloc(1);
	str[1][0] = 0;
	if (!group->redir_infile)
	{
		perror("syntax error near unexpected token");
		g_exit_status = 1;
		free (str[0]);
		free (str[1]);
		exit(1);
	}
	if (pipe(fd) == -1)
	{
		perror("pipe fail"); // do we need to save pipes?
		return (-1);
	}
	fd[0] = heredoc_sub(group, str, fd);
	return (fd[0]);
}