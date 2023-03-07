#include "../inc/minishell.h"

int	is_redirect(int num)
{
	if (num == REDIR_INPUT_APPEND)
		return (1);
	if (num == REDIR_INPUT)
		return (1);
	if (num == REDIR_OUTPUT_APPEND)
		return (1);
	if (num == REDIR_OUTPUT)
		return (1);
	return (0);
}

/**
 * @brief only call ONCE in the loop? it changes status it is called
 * it returns 1 if it identifies the given str as an redirect i.e. (">", " ", "filename")
 * else returns 0
 * it saves the redirect in the struct groups
 */
int	found_save_redirect(t_parse_lexer *pl, t_info *info, char *act_input_lexer_str) // in init set everything to NULL or 0
{
	int ret;
	static int before_cat = 0;

	ret = 0;
	if (pl->cat == REDIR_INPUT_APPEND || pl->cat == REDIR_INPUT)
	{
		ret = 1;
		before_cat = pl->cat;
		info->groups[pl->act_group].redir_in = pl->cat;
	}
	if (pl->cat == REDIR_OUTPUT_APPEND || pl->cat == REDIR_OUTPUT)
	{
		ret = 1;
		before_cat = pl->cat;
		info->groups[pl->act_group].redir_out = pl->cat;
	}
	if (pl->cat == SEPARATOR && is_redirect(before_cat))
		ret = 1;
	if (is_redirect(before_cat))
	{
		if (pl->cat == WORD || pl->cat == FLAG || (pl->cat > BUILTIN_START && pl->cat < BUILTIN_END))
		{
			ret = 1;
			if (before_cat == REDIR_INPUT_APPEND || before_cat == REDIR_INPUT)
				info->groups[pl->act_group].redir_infile = act_input_lexer_str;
			if (before_cat == REDIR_OUTPUT_APPEND || before_cat == REDIR_OUTPUT)
				info->groups[pl->act_group].redir_outfile = act_input_lexer_str;
			before_cat = 0;
		}
	}
	return (ret);
}

int	is_builtin(char *str)
{
	if (!ft_strncmp(str, "echo\0", 5))
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
	return (0);
}

int	found_save_executable(t_parse_lexer *pl, t_info *info, char *act_input_lexer_str, int i) // malloc so groß wie der lexer string ist
{
	static int already_found_exe = 0;
	char		*path_to_executable;

	if (pl->cat == PIPE || i == 0)
		already_found_exe = 0;
	if (already_found_exe == 0 && !pl->is_red && pl->cat != PIPE && pl->cat != SEPARATOR)// (pl->cat == WORD || pl->cat == FLAG || (pl->cat > BUILTIN_START && pl->cat < BUILTIN_END)))
	{
		// info->groups[pl->act_group].cmd = act_input_lexer_str;
		pl->is_exe = 1;
		already_found_exe = 1;
		path_to_executable = is_an_executable(act_input_lexer_str, info);
		if (is_builtin(act_input_lexer_str))
		{
			info->groups[pl->act_group].builtin = is_builtin(act_input_lexer_str);
			info->groups[pl->act_group].arguments[0] = act_input_lexer_str;
			free(path_to_executable);
			path_to_executable = NULL;
		}
		else if (path_to_executable)
		{
			info->groups[pl->act_group].path = path_to_executable; // and needs to be the first arg
			info->groups[pl->act_group].arguments[0] = act_input_lexer_str; // and needs to be the first arg
		}
		//else
		//	pl->is_exe = -1;
		return (pl->is_exe);
	}
	// printf("%p\n", act_input_lexer_str);	// prints out storage adress
	if (act_input_lexer_str == NULL)
		already_found_exe = 0;
	pl->is_exe = 0;
	return (pl->is_exe);
}

/**
 * @brief The arguments are only ptrs that point tho the right lexer strings
 */
void	found_save_arguments(t_parse_lexer *pl, t_info *info, int i)
{
	int	arg_j;
	int	temp;

	arg_j = -1;
	temp = pl->cat;
	//printf("i %d\n", i);
	if (pl->is_exe == 1)
	{
		// printf("is_exe %d\n", pl->is_exe);
		i--; // change that a bit nicer
		while (info->input_lexer[++i])
		{
			//printf("str: %s\n", info->input_lexer[i]);
			pl->cat = categorize(info->input_lexer[i]); // pl->cat steht wenn wir zurück gehen immernoch auf der PIPE oder so wichtig?
			//printf("cat %d\n", pl->cat);
			pl->is_red = found_save_redirect(pl, info, info->input_lexer[i]);
			if (pl->cat == SEPARATOR || pl->is_red)
				continue;
			else if (pl->cat == PIPE)
			{
				info->groups[pl->act_group].arguments[arg_j + 1] = NULL; //
				//arg_j = 0;
				break;
			}
			else
			{
				++arg_j;
				//printf("arg_j %d\n", arg_j);
				info->groups[pl->act_group].arguments[arg_j] = info->input_lexer[i];	// creates seg fault for > 1 group
				//printf("arg_string %s\n", info->groups[pl->act_group].arguments[arg_j]);				// creates seg fault for > 1 group
				//printf("adress_arg %p\n", info->groups[pl->act_group].arguments[arg_j]);	// creates seg fault for > 1 group	
				// printf("word\n");
			}	
			//printf("i2 %d\n", i);
		}
		info->groups[pl->act_group].arguments[arg_j + 1] = NULL;
		//printf("arg_string %s\n", info->groups[pl->act_group].arguments[arg_j + 1]);
		pl->cat = temp;
	}
}

void	pipe_detector(t_parse_lexer *pl, t_info *info)
{
	//info->groups->pipe_in = 0;		init in init file!!!
	//info->groups->pipe_out = 0;
	// printf("act_group %d num_groups %d\n", pl->act_group, info->num_groups);
	if (info->num_groups > 1 && pl->act_group == 0)	// why gets this one overwritten when going to the next group and why doesn't it understand group 0?
	{
		info->groups[pl->act_group].pipe_in = 0;
		info->groups[pl->act_group].pipe_out = 1;
	}
	else if (pl->act_group > 0 && pl->act_group < info->num_groups - 1)
	{
		info->groups[pl->act_group].pipe_in = 1;
		info->groups[pl->act_group].pipe_out = 1;
	}
	else if (info->num_groups > 1 && pl->act_group == info->num_groups - 1)
	{
		info->groups[pl->act_group].pipe_in = 1;
		info->groups[pl->act_group].pipe_out = 0;
	}
	// printf("act pipe in %d act pipe out %d\n", info->groups[pl->act_group].pipe_in, info->groups[pl->act_group].pipe_out);
	// printf("pipe_in[0] %d pipe_out[0] %d\n", info->groups[0].pipe_in, info->groups[0].pipe_out);
}

int	count_groups(t_info *info)
{
	int num_groups;
	int run;

	num_groups = 1;
	run = -1;
	while (info->input_lexer[++run])
	{
		if (!strncmp(info->input_lexer[0], "|", 2) || (!strncmp(info->input_lexer[0], " ", 2) && !strncmp(info->input_lexer[1], "|", 2))) // delete second part, if we don't wanna implement that case
			return (1);
		if (!ft_strncmp(info->input_lexer[run], "|", 2))
			num_groups++;
	}
	return (num_groups);
}


void	shift_str_left(char *str)
{
	int i;

	i = -1;
	while (str[++i])
		str[i] = str[i + 1];
}

void	delete_quotationmarks(char	**array)
{
	int i;
	int j;
	int d_quote;
	int s_quote;
	char buf;

	i = -1;
	d_quote = 0;
	s_quote = 0;
	while (array[++i])
	{
		j = -1;
		while (array[i][++j])
		{
			buf = array[i][j];
			if (!d_quote && buf == '\'')
			{
				shift_str_left(&array[i][j]); // the same with \", the rest li: 265 - 281 put in a function -> lexer uses that too
				j--;

			}

			if (!d_quote && buf == '\'')
				s_quote = !s_quote;
			// {
				
			// 	if (s_quote == 0)
			// 		s_quote = 1;
			// 	else
			// 		s_quote = 0;
			// }
			else if (!s_quote && buf == '\"')
			{
				shift_str_left(&array[i][j]);
				j--;
				d_quote = !d_quote;  // better
				// if (d_quote == 0)
				// 	d_quote = 1;
				// else
				// 	d_quote = 0;
			}

		}
	}
}
