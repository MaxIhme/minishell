#include "../inc/minishell.h"
/*
Saves all environment variables in a 3d array
*/
void	make_env(char **envp, t_info *info)
{
	int	i;

	i = 0;
	while (envp[i])
		i++;
	info->env =ft_calloc(i + 1, sizeof(char *));
	if (!(info->env))
	{
		write(2, "Error: malloc failed", 20);
		exit (0);
	}
	while (--i >= 0)
	{
		info->env[i] = ft_one_split(envp[i], '='); //save malloc !!!!;
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			info->paths = ft_split(envp[i] + 5, ':');
	}
}

/**
 * @brief 
 * set path ... -> (see if works with one path, more and no paths and emptysapces)
 * in export: call renew_paths():-> 
 * 	free old malloc
 * 	split new paths
 * 
 * unset path ... -> free old memory (but maybe leave a "" malloced -> for that see check_if_cmd and cleanup)
 * 
 * handle info->paths == NULL
 * 
 */
