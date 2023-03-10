#include "../inc/minishell.h"

void	builtins_with_output(t_group *group)
{
	if (group->builtin == CMD_ECHO)
		ft_echo(group->arguments, group->pipe_out);
	else if (group->builtin == CMD_ENV)
		ft_env(group->info);
	else if (group->builtin == CMD_PWD)
		ft_pwd(group);
	exit(0);
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