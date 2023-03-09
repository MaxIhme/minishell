#include "../inc/minishell.h"

/**
 * @return num of arguments minus one, because the first argument
 * is the command itself
 */
int	num_args(char **args)
{
	int num;

	num = 0;
	while (*args)
	{
		args++;
		num++;
	}
	return (num - 1);
}

int	num_env_args(char ***args)
{
	int num;

	num = 0;
	while (*args)
	{
		args++;
		num++;
	}
	return (num);
}

void	populate_new_env(char ***new_env, char ***old_env)
{
	while (*old_env)
	{
		*new_env = *old_env;
		new_env++;
		old_env++;
	}
}

int	num_var_chars(char *str)
{
	int i;

	i = 0;
	while (str[i] != '=')
		i++;
	return (i);
}

int	num_cont_chars(char *str)
{
	int i;
	int j;

	j = 0;
	i = 0;
	while (str[i] != '=')
		i++;
	while (str[++i])
		j++;
	return (j);
}

void populate_var(char *str, char *arg)
{
	int i;

	i = 0;
	while (arg[i] != '=')
	{
		str[i] = arg[i];
		i++;
	}
}

void populate_cont(char *str, char *arg)
{
	int i;
	int	j;

	i = 0;
	j = -1;
	while (arg[i] != '=')
		i++;
	while (arg[++i])
	{
		str[++j] = arg[i];
	}
}

/**
 * @brief looks for a '=' sign in every argument
 * start at i = 0 because first argument is export
 * @returns 0 if everything is ok, 1 if at least one arg has no '=' or invalid identifier
 * besides the first one (that is "export")
 */
int	error_check_export(char **args)
{
	int equal_flag;
	int i;
	int j;

	i = 0;
	while (args[++i])
	{
		equal_flag = 0;
		j = -1;
		while (args[i][++j])
		{
			if (args[i][j] == '=')
				equal_flag = 1;
			if (!equal_flag && !(ft_isalnum(args[i][j]) ||  args[i][j] == '_'))
			{
				input_message(INVALID_IDENTIFIER, 0);
				return (1);
			}
		}
		if (equal_flag == 0)
			return (1);
	}
	return (0);		// we return 0 for error in paser and 1 for sucess
}

/**
 * @brief expects something like ab=cd as arg
 * @return the position in the env array of arg (array starts with 0),
 *  if no return -1
 */
int	var_pos_in_env_export(char *arg, char	***env)
{
	int		i;

	i = -1;
	while (env[++i])
	{
		// printf("\n\nenv %s\n", env[i][0]);
		// printf("arg %s\n", arg);
		// printf("len %d\n\n", ft_strlen(arg));
		if (!ft_strncmp(env[i][0], arg, num_var_chars(arg)) &&
			env[i][0][num_var_chars(arg)] == '\0')
			return (i);
	}
	return (-1);
}

/**
 * @brief 
 * buf old content
 * malloc memory for the content into env ptr at the right spot
 * copy content
 * free the old content
 */
void	replace_cont_of_var(char *arg, char ***env)
{
	char	*buf;
	int		var_pos_in_env;

	var_pos_in_env = var_pos_in_env_export(arg, env);
	buf = env[var_pos_in_env][1];
	env[var_pos_in_env][1] = ft_calloc(num_cont_chars(arg) + 1, sizeof(char));
	populate_cont(env[var_pos_in_env][1], arg);
	free (buf);
}

void export_with_no_args(t_info *info)
{
	int i;

	i = -1;
	while (info->env[++i])
		printf("declare -x %s=\"%s\"\n", info->env[i][0], info->env[i][1]);
}

/**
 * @brief give it a 2D char array -> arguments 
 * in the form of:{"export", "some_variable_name=some_content", "2ndvar=second_content", NULL}
 * It takes the info->env and makes it bigger (the first array)
 * 
 * how many arguments do we have?
 * // how many arguments did we already find in the env - don't do this
 * calloc space for the array of ptrs to the environment variables 
 * // - num that we already found - don't do this
 * plus the new ones + zeros at the end
 * copy old ptrs to the new space, free the old array
 * step one argument forward, because the first one is "export"
 * search the end of the environment variables
 * while loop through the new args
 * 		malloc memory for an array of 3; [2] is (NULL)
 * 		with its ptr at the end of env
 * 		malloc memory for the variable name of 
 * 		the first argument -> put its ptr to env[i][0]
 * 		the same for the content
 * 		copy variable name into malloced memory 
 * 		-> populate_var()
 * 		same for content
 * 		
 * @return 0
 */
int	ft_export(char **args, t_info *info)
{
	int	num_new_args;
	char	***env_buf;
	int	i;

	if (!args[1])
		export_with_no_args(info);
	num_new_args = num_args(args);
	if (num_new_args == 0)	
		return (0);
	if (error_check_export(args))
		return (0);
	i = 0;
	env_buf = info->env;
	info->env = ft_calloc((num_env_args(info->env) + num_new_args + 1), sizeof(char *));
	if (!info->env)
		return (2); // save really please !!!
	populate_new_env(info->env, env_buf);
	free (env_buf);
	env_buf = info->env;
	args++;
	while (env_buf[i])
			i++;
	while (*args) // I expect it to have an = sign
	{
		if (var_pos_in_env_export(*args, info->env) != -1)
		{
			replace_cont_of_var(*args, info->env);
			args++;
			continue;
		}
		env_buf[i] = malloc(sizeof(char *) * 3);
		if (!env_buf[i])
			return (2);
		env_buf[i][2] = NULL;
		env_buf[i][0] = ft_calloc((num_var_chars(*args) + 1), sizeof(char));
		if (!env_buf[i][0])
			return (2);
		env_buf[i][1] = ft_calloc((num_cont_chars(*args) + 1), sizeof(char));
		if (!env_buf[i][1])
			return (2);
		populate_var(env_buf[i][0], *args);
		populate_cont(env_buf[i][1], *args);
		if (!ft_strncmp(*args, "PATH=", 5))
			info->paths = ft_split(*args + 5, ':');
		args++;
		i++;
	}
	return (0);
}