#include "../inc/minishell.h"

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

void	fds_and_pipe_after_execve(t_group *group)
{
	if (group->pipe_out)
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
	else if (group->builtin == CMD_CLEAR)
		ft_clear();
	exit(0); //changed from _Exit(3);
	if (group->pipe_in)
		close(group->pipe_in);
	if (group->pipe_out && !group->redir_out)
		replace_pipe_in_next_group(group, group->pipe_fd[READ]);
	else if (group->pipe_out && group->redir_out)
		replace_pipe_in_next_group(group, 0);
}

void	fork_and_execve(t_group *group)
{
	fork_process(group);
	if (group->pid == 0)
	{
		handle_redir_and_dup(group);
		if (group->builtin == CMD_ECHO || group->builtin == CMD_ENV \
		|| group->builtin == CMD_PWD)
			builtins_with_output(group);
		if (group->path)
		{
			if (execve(group->path, group->arguments, NULL) == -1)
			{
				g_exit_status = 1;
				perror("exec didnt work\n");
			}
		}
		exit(0);
	}
	else
		fds_and_pipe_after_execve(group);
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
	else if (group->builtin == CMD_CLEAR)
		ft_clear();
	return ;
}

int	check_access_infile_outfile(t_group *group)
void	piping_builtin_and_exec(t_group *group)
{
	if (group->pipe_out)
		make_pipe(group);
	if (!group->pipe_out && group->info->num_groups == 1)
		if (group->builtin == CMD_EXPORT || group->builtin == CMD_UNSET \
		|| group->builtin == CMD_EXIT || group->builtin == CMD_CD)
			builtin_no_piping(group);
	if (group->path || group->builtin || group->redir_out)
		fork_and_execve(group);
}

void	executer(t_group	*group)
{
	int	i;

	i = -1;
	while (++i < group->info->num_groups)
	{
		g_exit_status = 0;
		if (!group->builtin && !group->path)
		{
			input_message(STR_WRITE_ERROR, 0);
			g_exit_status = 127;
			if (group->pipe_out)
				replace_pipe_in_next_group(group, 0);
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
			|| group->builtin == CMD_EXIT || group->builtin == CMD_CD || group->builtin == CMD_CLEAR)
				builtin_no_piping(group);
		if (group->path || group->builtin || group->redir_out)
			fork_and_execve(group);
		piping_builtin_and_exec(group);
		if (i < group->info->num_groups - 1)
			group++;
	}
	while (wait(NULL) > 0)
		;
}
