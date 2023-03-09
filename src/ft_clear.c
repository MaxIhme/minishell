#include "../inc/minishell.h"

void	ft_clear(void)
{
/*	int i;

	i = 0;
	while (i++ < 50)
		printf("\n");*/
	printf("\033[2J\033[1;1H");
}
