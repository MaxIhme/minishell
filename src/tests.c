#include "../inc/minishell.h"

void	p2d(char **ptr)
{
	printf("Start Content of **ptr: %p\n", ptr);
	printf("Content of *ptr: %p\n", *ptr);
	int i = -1;
	while(ptr[++i])
	{
		printf("%p : _%s_\n", ptr[i], ptr[i]);

	}
	printf("%p\n", ptr[i]);
}
void	print_groups(t_group *groups, t_info *info)
{
	int i = -1;
	int j;
	printf("num groups %d\n", groups->info->num_groups);
	while (++i < info->num_groups)
	{
		j = -1;
		printf("GROUP %d\n", i);
		while (groups[i].arguments[++j])
			printf("\targument %d:\t_%s_\n", j, groups[i].arguments[j]);
		printf("\targument %d:\t_%s_\n", j, groups[i].arguments[j]);
		printf("path: \t\t\t_%s_\n", groups[i].path);
		printf("builtin: \t\t_%d_\n", groups[i].builtin);
		printf("redir_in: \t\t_%d_\n", groups[i].redir_in);
		printf("redir_infile: \t_%s_\n", groups[i].redir_infile);
		printf("redir_out: \t\t_%d_\n", groups[i].redir_out);
		printf("redir_outfile:\t_%s_\n", groups[i].redir_outfile);
		// printf("argument_filename\t_%s_\n\n", groups[i].argument_filename);
		printf("pipe_in: \t\t_%d_\n", groups[i].pipe_in);
		printf("pipe_out: \t\t_%d_\n", groups[i].pipe_out);
	}
	printf("----------------------------------------\n");
	// printf("Last group pointer %p\n", &groups[i]);
}
