#include "../inc/minishell.h"

/**
 * @brief return 0 if everything worked out
 */
int make_prompt(t_info *info)
{
	info->prompt = NULL;
	return (0);
}


t_info	*init(char **envp)
{
	t_info *info;
	info = malloc(sizeof(t_info));
	make_env(envp, info);
	make_prompt(info);
	info->input_lexer = NULL;
	info->num_groups = 0;
	return (info);
}

char **ft_array_args(char *str, t_info *info)
{
	int	i;
	int	word_count;
	char **array;

	word_count = 0;
	i = -1;
	if(str == NULL || *str == 0)
		return NULL;
	word_count = count_parts(str);
	array = (char **)malloc((sizeof(char *) * (word_count + 1)));
	if (!array)
		error(ERR_MALLOC_ARRAY_ARGS, info);
	array[word_count] = NULL;
	return (array);
}

t_group *init_groups(t_info *info) // init everything inside to NULL
{
	t_group *groups;
	int i;

	i = -1;
	if (!(groups = ft_calloc(info->num_groups + 1, sizeof(*groups))))
	{
		error(ERR_MALLOC_INIT_GROUPS, info);
		return (NULL);
	}
	//save malloc
	//groups[info->num_groups] = NULL;
	while (++i < info->num_groups)
	{
		//groups[i] = malloc(sizeof(t_group)); // write clean_up
		//save malloc
		groups[i].arguments = NULL;
		groups[i].path = NULL;
		groups[i].builtin = 0;
		groups[i].redirect_input = 0;
		groups[i].redirect_input_filename = NULL;
		groups[i].redirect_output = 0;
		groups[i].redirect_output_filename = NULL;
		groups[i].pipe_in = 0;
		groups[i].pipe_out = 0;
		groups[i].info = info;
	}
	printf("ptr %d\n",  groups[0].redirect_output);
	return (groups);
}
