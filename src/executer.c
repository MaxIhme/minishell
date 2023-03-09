#include "../inc/minishell.h"

// void	execute_builtins(t_info	*info)
// {
// 	//find the macro interger and pass param to eg ft_echo
// }

void	fork_process(t_group *group)
{
	group->pid = fork();
	if (group->pid == -1)
	{
		perror("fork error");
		g_exit_status = 1;
		exit(1);
	}
}

// #include <errno.h>

void	dup_fd(int fd_new, int fd_old, t_info *info)
{
	if (dup2(fd_new, fd_old) < 0)
	{
		printf("dup_func() error new %d old %d\n", fd_new, fd_old);
		clean_up(CLEAN_UP_FOR_NEW_PROMPT, info);
		g_exit_status = 1;
		exit(1);
	}
}

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

/*Open an infile when "<infile" is present and overwrites STDIN*/
int	redir_in(t_group *group)
{
	if (group->redir_in == REDIR_INPUT || group->redir_in == REDIR_INPUT_APPEND)
	{
		if (group->redir_in == REDIR_INPUT)
			group->redir_in = open(group->redir_infile, O_RDONLY);
		else
			group->redir_in = heredoc(group);
		if (group->redir_in < 0)
		{
			//clean_up(CLEAN_UP_FOR_NEW_PROMPT, group->info); //is this nessecary
			perror("open_infile() cannot open file");
			g_exit_status = 1;
			exit(1);
		}
		dup_fd(group->redir_in, 0, group->info);
		close(group->redir_in);
	}
	return (1);
}

/*Makes a pipe and overwrites STDOU with write end, so execve outputs to pipe instead of STDOUT*/
void	redir_out(t_group *group) //make remove return of int
{
	if (group->redir_out == REDIR_OUTPUT || \
	group->redir_out == REDIR_OUTPUT_APPEND)
	{
		if (group->redir_out == REDIR_OUTPUT)
		{
				group->redir_out = open(group->redir_outfile, \
				O_WRONLY | O_CREAT | O_TRUNC, 0644);
		}
		else
			group->redir_out = open(group->redir_outfile, \
			O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (group->redir_out < 0)
		{
			perror("cannot open file\n");
			g_exit_status = 1;
			//clean_up(CLEAN_UP_FOR_NEW_PROMPT, group->info);
			exit(0);
		}
		dup_fd(group->redir_out, 1, group->info);
		close(group->redir_out);
	}
}

void	make_pipe(t_group *group)
{
	//------------------pipe------------------
	if (pipe(group->pipe_fd) == -1)
	{
		//clean_up(CLEAN_UP_FOR_NEW_PROMPT, group->info);
		perror("pipe error");
		g_exit_status = 1;
		exit(1);
	}
}

void	replace_pipe_in_next_group(t_group *group, int new_pipe_in)
{
	t_group	*temp;

	temp = group;
	temp++;
	temp->pipe_in = new_pipe_in;
}

int	next_have_pipe_out(t_group *group)
{
	t_group	*temp;

	temp = group;
	temp++;
	if (temp->pipe_out)
		return (1);
	return (0);
}

void	closing_fds(t_group *group)
{
	if (group->redir_out)
		close(group->redir_out);
	if (group->redir_in)
		close(group->redir_in);
	if (group->pipe_in)
		close(group->pipe_in);
	if (group->pipe_out)
	{
		close(group->pipe_fd[READ]);
		close(group->pipe_fd[WRITE]);
	}
}

void	builtins_with_output(t_group *group)
{
	if (group->builtin == CMD_ECHO)
		ft_echo(group->arguments, group->pipe_out);
	// else if (group->builtin == CMD_PWD)
	// 	ft_pwd();
	else if (group->builtin == CMD_ENV)
		ft_env(group->info);
	else if (group->builtin == CMD_PWD)
		ft_pwd(group);


	exit(0); //changed from _Exit(3);
}

void	fork_and_execve(t_group *group)
{
	fork_process(group);
	if (group->pid == 0) //CHILD
	{
		//-------INPUT-----------------------------------
		if (group->redir_in)
			redir_in(group); //open infile
		else if (group->pipe_in) //if pipe_in + no redir_in
			dup_fd(group->pipe_in, 0, group->info);
		if (group->redir_out)//open outfile
			redir_out(group);
		//-------OUTPUT-----------------------------------
		else if (group->pipe_out) //if pipe_out + no redir_out
			dup_fd(group->pipe_fd[WRITE], 1, group->info);
		//-------CLOSING-----------------------------------
		 closing_fds(group);
		//-------EXECVE-----------------------------------
		if (group->builtin == CMD_ECHO || group->builtin == CMD_ENV \
		|| group->builtin == CMD_PWD)
			builtins_with_output(group);
		if (group->path)
			if (execve(group->path, group->arguments, NULL) == -1)
			{
				g_exit_status = 1;
				perror("exec didnt work\n"); //no need to free anything here
			}
		exit(0); //exiting export cd etc..
	}
	else //PARENT
	{
		if (group->pipe_out)
			close(group->pipe_fd[WRITE]);
		if (group->pipe_in)
			close(group->pipe_in);
		if (group->pipe_out && !group->redir_out)
			replace_pipe_in_next_group(group, group->pipe_fd[READ]);
		else if (group->pipe_out && group->redir_out)
			replace_pipe_in_next_group(group, 0);
	}
}

void	builtin_no_piping(t_group *group)
{
	printf("args: %s and %s\n", group->arguments[0], group->arguments[1]);
	if (group->builtin == CMD_EXPORT)
		ft_export(group->arguments, group->info);
	else if (group->builtin == CMD_UNSET)
		ft_unset(group->arguments, group->info);
	else if (group->builtin == CMD_EXIT)
		ft_exit(group->info);
	else if (group->builtin == CMD_CD)
		ft_cd(group->arguments, group->info);
	return ;
}

int	check_access_infile_outfile(t_group *group)
{
	if (group->redir_infile)
	{
		if (access(group->redir_infile, R_OK) == -1 && group->redir_in != REDIR_INPUT_APPEND)
		{
			g_exit_status = 1;
			printf("cannot open file\n");
			return (0);
		}
	}
	return (1);
}

void	executer(t_group	*group)
{
	int	i;

	i = -1;
	// print_groups(group, group->info);
	//ft_env(group->info); //for testing
	while (++i < group->info->num_groups) //sadfdsa | cat infile | cat
	{
		g_exit_status = 0;
		// if (!executer_error_check(group->info, group))
		// 	break;
		if (!group->builtin && !group->path)
		{
			input_message(STR_WRITE_ERROR, 0);
			g_exit_status = 127; 
			if (group->pipe_out)
				replace_pipe_in_next_group(group, 0);
			//mabe do access check also if nessecary?
			if (i < group->info->num_groups - 1)
				group++;
			continue ;
		}
		if (!check_access_infile_outfile(group))
			break ;
		if (group->pipe_out) //Must be made even though pipe out
			make_pipe(group);
		if (!group->pipe_out && group->info->num_groups == 1)
			if (group->builtin == CMD_EXPORT || group->builtin == CMD_UNSET \
			|| group->builtin == CMD_EXIT || group->builtin == CMD_CD)
				builtin_no_piping(group);
		if (group->path || group->builtin || group->redir_out)
			fork_and_execve(group);
		if (i < group->info->num_groups - 1)
			group++;
		// g_exit_status = 0;
	}
	while (wait(NULL) > 0)
		;
	// int	status_;
	// int	counter;
	// counter = 0;
	// waitpid(-1, &status_, 0);
	// counter = WEXITSTATUS(status_);
	// 	if (counter != 0)
	// 	{
	// 		g_exit_status = counter;
	// 		//clean up all filedescriptors??
	// 		return ;
	// 	}
}
//printf("i = %d arg[0] %s pipe in = %d\n", i, group->arguments[0], group->pipe_in);
