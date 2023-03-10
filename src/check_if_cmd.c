#include "../inc/minishell.h"

char	*check_and_return_path(char *slash_added, \
char *slash_not_added, char *cmd)
{
	if (!access(slash_added, X_OK))
	{
		return (ft_strdup(slash_added));
	}
	if (!access(slash_not_added, X_OK))
	{
		return (ft_strdup(slash_not_added));
	}
	if (!access(cmd, X_OK))
	{
		return (ft_strdup(cmd));
	}
	return (NULL);
}

char	*add_slash(char *cmd, char *path)
{
	char	*slash_added;
	char	*temp;

	temp = ft_strjoin(path, "/");
	slash_added = ft_strjoin(temp, cmd);
	free(temp);
	return (slash_added);
}

/**
 * @brief checks if cmd is (only command like cat or full path)
 * an executable on the system (installed in one of the dirs listed in env)
 * 
 * @return NULL if not an executable, char ptr to fully path f.e. "..../cat"
 * to the executable. You have to free the space!!
 */

char	*check_if_access_in_parser(char *cmd, t_info *t_info, char	*path_to_executable)
{
	if (!t_info->paths)
	{
		if (!access(cmd, X_OK))
		{
			path_to_executable = ft_strdup(cmd);
			return (path_to_executable);
		}
	}
	return (NULL);
}

void	free_slash(char *slash_added, char *slash_not_added)
{
	free(slash_added);
	free(slash_not_added);
}

char	*is_an_executable(char *cmd, t_info *t_info)
{
	int		i;
	char	*slash_added;
	char	*slash_not_added;
	char	*path_to_executable;

	i = -1;
	path_to_executable = NULL;
	path_to_executable = check_if_access_in_parser(cmd, \
	t_info, path_to_executable);
	while (t_info->paths[++i])
	{
		slash_added = add_slash(cmd, t_info->paths[i]);
		slash_not_added = ft_strjoin(slash_added, cmd);
		path_to_executable = check_and_return_path(slash_added, \
		slash_not_added, cmd);   //leak!!! stdup inside
		if (path_to_executable)
		{
			t_info->nb_root_cmd++;
			free_slash(slash_added, slash_not_added);
			return (path_to_executable);
		}
		free_slash(slash_added, slash_not_added);
	}
	return (NULL);
}
