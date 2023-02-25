
#include "../inc/minishell.h"

void	clean_up_group_structs(t_info *info)
{
	free(info->groups);
	info->groups = NULL;
}

/**
 * @brief free info at last!!!
 */
void	clean_up_info_struct(t_info *info)
{
	free(info);
	info = NULL;
}

void	clean_up_paths(t_info *info)
{
	int i;
	char 	**paths_;

	paths_ = info->paths;
	i = -1;
	while (paths_[++i])
	{
		free(paths_[i]);
	}
	free(paths_);
	paths_ = NULL;
	info->paths = NULL;
}

void	clean_up_prompt(t_info *info)
{
	free(info->prompt);
	info->prompt = NULL;
}

void clean_up_env(t_info *info)
{
	char ***env_;
	int i;
	int j;

	i = -1;
	env_ = info->env;
	while (env_[++i])
	{
		j = -1;
		while (env_[i][++j])
		{
			free(env_[i][j]);
		}
		free(env_[i]);
	}
	free(env_);
	env_ = NULL;
	info->env = NULL;
}

void clean_up_lexer(t_info *info)
{
	char	**lexer;
	int i;

	i = -1;
	lexer = info->input_lexer;
	while (lexer[++i])
	{
		free(lexer[i]);
	}
	free(lexer);
	lexer = NULL;
	info->input_lexer = NULL;
}

void clean_up_arguments(t_info *info)
{
	int		i;

	i = -1;
	while (++i < info->num_groups)
	{
		free(info->groups[i].arguments);
		info->groups[i].arguments = NULL;
	}
}

void	clean_up_path_to_executable(t_info *info)
{
	int	i;

	i = -1;
	while (i++ < info->num_groups)
	{
		if (info->groups[i].path)
		{
			free(info->groups[i].path);
			info->groups[i].path = NULL;
		}
	}
}

void	clean_up(int clean_up_code, t_info *info)
{
	if (clean_up_code == CTRL_D_PRESSED) 
		exit(0); //should we use rrl_clear_history here to free history? maybe we can check with valgrind? please correct me if i dont understand (nick)
	if (clean_up_code == ERR_MALLOC_INIT_GROUPS)
	{
		clean_up_group_structs(info);
	}
	
	if (clean_up_code == CLEAN_UP_REST_BEFORE_EXIT)
	{
		clean_up_env(info);
		clean_up_paths(info);
		clean_up_info_struct(info);
	}
	if (clean_up_code == CLEAN_UP_FOR_NEW_PROMPT)
	{
		clean_up_lexer(info);
		clean_up_prompt(info);
		clean_up_arguments(info);
		clean_up_path_to_executable(info);
		clean_up_group_structs(info);
	}
	if (clean_up_code == ERR_WRONG_AMOUNT_QUOTATION_MARKS)
	{
		clean_up_prompt(info);
	}
	
	return;
}
