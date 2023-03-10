#include "../inc/minishell.h"

int	check_access_infile_outfile(t_group *group)
{
	if (group->redir_infile)
	{
		if (access(group->redir_infile, R_OK) == -1 && \
		group->redir_in != REDIR_INPUT_APPEND)
		{
			g_exit_status = 1;
			printf("cannot open file\n");
			return (0);
		}
	}
	return (1);
}

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

void	handle_redir_and_dup(t_group *group)
{
	if (group->redir_in)
		redir_in(group);
	else if (group->pipe_in)
		dup_fd(group->pipe_in, 0, group->info);
	if (group->redir_out)
		redir_out(group);
	else if (group->pipe_out)
		dup_fd(group->pipe_fd[WRITE], 1, group->info);
	closing_fds(group);
}