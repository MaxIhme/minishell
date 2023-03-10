#include "../inc/minishell.h"

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
			perror("open_infile() cannot open file");
			g_exit_status = 1;
			exit(1);
		}
		dup_fd(group->redir_in, 0, group->info);
		close(group->redir_in);
	}
	return (1);
}

//Makes a pipe and overwrites STDOU with write end, 
//so execve outputs to pipe instead of STDOUT
void	redir_out(t_group *group)
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
			exit(0);
		}
		dup_fd(group->redir_out, 1, group->info);
		close(group->redir_out);
	}
}

void	make_pipe(t_group *group)
{
	if (pipe(group->pipe_fd) == -1)
	{
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