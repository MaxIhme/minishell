#include "libft.h"
/**
 @brief mallocs count * size no extra zero at the end
**/
void	*ft_calloc(size_t count, size_t size)
{
	char	*ptr;
	size_t	memory;
	size_t	i;

	i = 0;
	memory = size * count;
	ptr = (void *)malloc(count * size);
	if (!ptr)
		return (NULL);
	while (i < memory)
	{
		ptr[i] = 0;
		i++;
	}
	return (ptr);
}
