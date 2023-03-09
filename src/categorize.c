#include "../inc/minishell.h"

int	only_separator_in_string(char *str)
{
	int i;

	i = -1;
	while (str[++i])
	{
		if (!sep(str[i]))
			return (0);
	}
	return (1);
}

int categorize(char *str)
{
	if (!ft_strncmp(str, "<<", 3))
		return (REDIR_INPUT_APPEND);
	if (!ft_strncmp(str, "<", 2))
		return (REDIR_INPUT);
	if (!ft_strncmp(str, ">", 2))
		return (REDIR_OUTPUT);
	if (!ft_strncmp(str, ">>", 3))
		return (REDIR_OUTPUT_APPEND);
	if (!ft_strncmp(str, "|", 2))
		return (PIPE);
	if (!ft_strncmp(str, "echo", 5))
		return (CMD_ECHO);
	if (!ft_strncmp(str, "exit", 5))
		return (CMD_EXIT);
	if (!ft_strncmp(str, "cd", 3))
		return (CMD_CD);
	if (!ft_strncmp(str, "pwd", 4))
		return (CMD_PWD);
	if (!ft_strncmp(str, "export", 7))
		return (CMD_EXPORT);
	if (!ft_strncmp(str, "unset", 6))
		return (CMD_UNSET);
	if (!ft_strncmp(str, "env", 4))
		return (CMD_ENV);
	if (!ft_strncmp(str, "clear", 5))
		return (CMD_CLEAR);
	if (!ft_strncmp(str, "-", 1))
		return (FLAG);
	if (only_separator_in_string(str))
		return (SEPARATOR);
	return (WORD);
}
