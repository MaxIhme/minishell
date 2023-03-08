#include "../inc/minishell.h"

// int	slash_at_end(char *str)
// {
// 	int	len;

// 	len = ft_strlen(str);
// 	if (str[len - 1] == '/')
// 		return (1);
// 	return (0);
// }

/**
 * @brief gets a path
 * decides if that path is relative, absolute or ~/
 * there can be a '/' or not in path
 * 
 * @return malloced space with the name inside
 * if couldn't malloc returns NULL
 * return a path without / at the end
 */
char *get_absolute_path(char *path, t_info *info) // test path with " kdsjf/  "
{
	char	*new_wd;
	char	*buf;

	buf = NULL;	
	if (!path)
	{
		new_wd = value_of_variable_from_env(info->env, "HOME", 4);
		new_wd = ft_strdup(new_wd);
		if (!new_wd)
			return (NULL);
	}
	else if (*path == '~' && path[1] == '/')
	{
		new_wd = value_of_variable_from_env(info->env, "HOME", 4);
		// buf = ft_strjoin(new_wd, "/");
		new_wd = ft_strjoin(new_wd, ft_strchr(path, '/'));
		// free (buf);
	}
	else if (*path == '/')
	{
		new_wd = ft_strdup(path);
		// if (slash_at_end(new_wd))
		// 	new_wd[ft_strlen(new_wd) - 1] = '\0';
	}
	else
	{
		new_wd = getcwd(NULL, 0);
		buf = ft_strjoin(new_wd, "/");
		free (new_wd);
		new_wd = ft_strjoin(buf, path);
		free(buf);
		// if (slash_at_end(new_wd))
		// 	new_wd[ft_strlen(new_wd) - 1] = '\0';
	}
	return (new_wd);
}

/**
 * calls ft_export, so first create 2d array
 * and array[1] should be "var=value"
 * @returns NULL if malloc fails, else ptr to env
 *
*/
char	***set_env_value(char *var, char *value, t_info *info)
{
	char	**args;
	char	*buf;

	args = malloc(sizeof(char*) * 3);
	if (!args)
		return (NULL);
	args[2] = NULL;
	args[0] = ft_strdup("export");
	if (!args[0])
		return (NULL);
	buf = ft_strjoin(var, "=");
	if (!buf)
		return (NULL);
	args[1] = ft_strjoin(buf, value);
	free (buf);
	ft_export(args, info);
	free (args[0]);
	free (args[1]);
	free (args);
	return (info->env);
}

/**
 * @brief expects and 2d array of strings{"cd", "/somedir", "not important"...}
 * only takes argument one as the new path
 * 
 * Decide if argument is a absolut path, a relative path or an ~/path
 * get the absolute path
 * 
 * check if the path is available
 * put the actual path to OLDPWD in env
 * change to new path with
 * put the new path to PWD in env
 * 
 * acess() returns 0 if the path exists. If not returns -1
 * 
 */
void	ft_cd(char **args, t_info *info)
{
	char	*absolute_path;
	char	*buf;

	buf = NULL;
	absolute_path = get_absolute_path(args[1], info);
	if (access(absolute_path, F_OK) != 0)
	{
		error(ERR_CD_NO_DIRECTORY, info);
		free (absolute_path);
		return ;
	}
	buf = getcwd(NULL, 0);
	set_env_value("OLDPWD", buf, info);
	free (buf);
	chdir(absolute_path);
	buf = getcwd(NULL, 0);
	set_env_value("PWD", buf, info);
	//test
	printf("%s\n", buf);
	//test
	free (buf);
	free (absolute_path);
}