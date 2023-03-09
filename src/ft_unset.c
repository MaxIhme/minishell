#include "../inc/minishell.h"

/**
 * @return the position in the env array of arg, if no return -1
 */
int	var_pos_in_env(char *arg, char	***env)
{
	int		i;

	i = -1;
	while (env[++i])
	{
		// printf("\n\nenv %s\n", env[i][0]);
		// printf("arg %s\n", arg);
		// printf("len %d\n\n", ft_strlen(arg));
		if (!ft_strncmp(env[i][0], arg, ft_strlen(arg) + 1))
			return (i);
	}
	return (-1);
}

/**
 * @brief copy old ptrs to new
 * leave_out one variable
 *  if -1 leave out nothing
 */
void	populate_env(char ***new_env, char ***old_env, int leave_out)
{
	int i;
	int j;

	i = 0;
	j = 0;
	while (old_env[i])
	{
		if (j == leave_out)
			j++;
		new_env[i] = old_env[j];
		i++;
		j++;
	}
}

/**
 * @brief '=' is an error
 * @return 1 if there is an error
 */
int	error_check_unset(char **args)
{
	int i;
	int j;

	i = 0;
	while (args[++i])
	{
		j = -1;
		while (args[i][++j])
		{
			if (args[i][j] == '=')
				return (1);
		}
	}
	return (0);
}

/**
 * @brief 
 * errocheck -> is there a = sing in there -> then error
 * while through the arguments
 * 		on which position is the var in the env
 * 		if position_env is -1 then error "not a valid identifier" and return 1
 * 		free var str and content string
 * 		free array of 3 ptrs to the strings
 * 		how many vars do we have?
 * 		calloc new *env array with one position less then we have vars
 * 		- 1 + 1 because add one NULL and delete one -> want one less
 * 		copy the ptrs to the new *env array, but not the one we want to delete
 * 		free the old env* array
 * @return 0
 */
int	ft_unset(char **args, t_info *info)
{
	int		var_pos;
	int		num_vars;
	char	***env;
	// int i;

	//error_check "not a valid identifier" and return 1
	
	// if (!ft_strncmp(args[1], "PATH", 5))
	// {
	// 	i = -1;
	// 	while (info->paths[++i])
	// 		info->paths[i] = NULL;
	// }
	if (error_check_unset(args))
		return (error(ERR_EQUAL_IN_UNSET_ARG, NULL));
	args++;
	while (*args)
	{
		env = info->env;
		var_pos = var_pos_in_env(*args, env);
		if (var_pos == -1)
		{
			args++;
			continue;
		}
		if (!ft_strncmp(*args, "PATH", 5))
			clean_up_paths(info);
		free(env[var_pos][0]);
		free(env[var_pos][1]);
		free(env[var_pos]);
		num_vars = num_env_args(env);
		info->env = ft_calloc((num_vars - 1 + 1), sizeof(char *));
		populate_env(info->env, env, var_pos);
		free(env);
		args++;
	}
	return (0);
}