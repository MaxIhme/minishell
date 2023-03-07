
#include "../inc/minishell.h"

/**
 *  @brief the parser goes through the strings of the lexer one by one and 
 * the temporary struct t_parse_lexer is used. It holds the actual group we are in,
 * the category cat of the categoriser, the falg if the string belongs to a redirect.
 * 
 * @param info 
 */
int	parser(t_info *info) // after lexer.. groups and make space for ptr to group structs and group structs and clean_up
{
	t_parse_lexer a;
	t_parse_lexer *pl;

	pl = &a;
	pl->act_group = 0;
	int i;
	i = -1;

	add_history(info->prompt);
	info->input_lexer = ft_split_lexer(info->prompt, info);
	if (!info->input_lexer)
		return (0);			// cannot stop before executer!!!!!!
	// expand_variables(info->input_lexer, info->env, info);
	if (!expand_variables(info->input_lexer, info->env, info))
		return (0);			// cannot stop before executer!!!!!!
	delete_quotationmarks(info->input_lexer);
	info->num_groups = count_groups(info);
	info->groups = init_groups(info);
	while (++i < info->num_groups)
		info->groups[i].arguments = ft_array_args(info->prompt, info);	// double pointer for array of arguments
	i = -1;
	while (info->input_lexer[++i])
	{
		printf("string: %s\n", info->input_lexer[i]);
		if (!strncmp(info->input_lexer[0], "exit", 5)) // check for leaks and segfault
			ft_exit(info);
		if (!ft_strncmp(info->input_lexer[i], "|", 2)) //what if pipe after space??? we can use categorize here
		{
			if (ft_strncmp(info->input_lexer[0], "|", 2) || (strncmp(info->input_lexer[0], " ", 1) && ft_strncmp(info->input_lexer[1], "|", 2)))
				pl->act_group++;
		}
		printf("act_group: %d\n", pl->act_group);
		pl->cat = categorize(info->input_lexer[i]);
		printf("cat %d\n", pl->cat);

		pl-> is_red = found_save_redirect(pl, info, info->input_lexer[i]);
		printf("is_red %d\n", pl->is_red);
		pl->is_exe = found_save_executable(pl, info, info->input_lexer[i], i);
		// if (pl->is_exe == -1)
		// 	return (0);
		//if (!parser_error_check(info, pl, i))
		//	return (0);
		printf("is_exe %d\n", pl->is_exe);
		found_save_arguments(pl, info, i);
		pipe_detector(pl, info);
		//printf("pipe_in %d pipe_out %d\n", pipe_in(pl, info, i, pipe_out(pl, info, i)), pipe_out(pl, info, i));
		// printf("_%s_\n", info->input_lexer[i]);
		// printf("redirect %d\n", pl->is_red);
		// printf("executable %d\n", pl->is_exe);
		// if (pl->cat == CMD_ECHO)
		// 	ft_echo(info->groups, pl);
		//printf("break failed\n");
		//printf("cat1 %d\n", pl->cat);
	}
	// p2d(info->paths);
	
	//print_groups(info->groups, info); // möglich <> ??
	// ft_unset(info->groups[0].arguments, info);
	
	// ft_echo(info->groups[0].arguments, info->groups[0].pipe_out);
	// return (0);
	// test_env_vars(info);
	return (1);
}
