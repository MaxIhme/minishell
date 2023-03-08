#include "libft.h"

static void	ft_fill_array(char const *s, char c, char **newstrings)
{
	unsigned int	i;
	unsigned int	j;

	i = -1;
	while (s[++i] != c);
	newstrings[0] = ft_calloc(i + 1, sizeof(char));
	ft_strlcpy(newstrings[0], s, i + 1);
	j = i++;
	while (s[++j]);
	newstrings[1] = ft_calloc(j - i + 1, sizeof(char));
	ft_strlcpy(newstrings[1], s + i, j - i + 1);
	newstrings[2] = NULL;
}

char	**ft_one_split(char const *s, char c)
{
	char			**newstrings;

	newstrings = ft_calloc(3, sizeof(char *));
	if (!s || !newstrings)
		return (NULL);
	ft_fill_array(s, c, newstrings);
	return (newstrings);
}
