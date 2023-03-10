#include "../inc/minishell.h"

int	sep(char c)
{
	if (c == ' ' || c == '\t' || c == '\n')
		return (1);
	return (0);
}

int	red(char c)
{
	if (c == '<' || c == '>')
		return (1);
	return (0);
}

int quote(char c)
{
	if (c == '\"' || c == '\'')
		return (1);
	return (0);
}

int pipesign(char c)
{
	if (c == '|')
		return (1);
	return (0);
}

/**
 * @brief returns ptr to place after the redirect signs
 * saves num of signs in buf if buf is not NULL
 */
char	*after_red(char *str, int *buf)
{
	int i = 0;
	while(red(*str) && *str != '\0')
	{
		i++;
		str++;
	}
	if (buf)
		*buf = i;
	return (str);
}

char	*after_sep(char *str, int *buf)
{
	int i = 0;
	while(sep(*str) && *str != '\0')
	{
		i++;
		str++;
	}
	if (buf)
		*buf = i;
	return (str);
}

char	*after_pipe(char *str, int *buf)
{
	if (buf)
		*buf = 1;
	return (++str);
}

char	*after_word(char *str, int *buf);

char	*after_quote(char *str, int *buf)
{
	int i;
	int i_word = 0;
	char start_quote;

	start_quote = *str;
	i = 1;
	str++;
	while(*str != start_quote)
	{
		i++;
		str++;
	}
	i++;
	str++;
	if (!sep(*str) && !red(*str) && *str != '\0')
		str = after_word(str, &i_word);
	if (buf)
		*buf = i + i_word;
	return (str);
}

char	*after_word(char *str, int *buf)
{
	int i;
	int i_quote;

	i = 0;
	i_quote = 0;
	while(!sep(*str) && !red(*str) && !pipesign(*str) && *str != '\0') // test if o"hallo <outf"ile // test in bash "hallo<outfile"<outfile
	{
		if (quote(*str))
			str = after_quote(str, &i_quote);
		else
		{
			i++;
			str++;
		}
	}
	if (buf)
		*buf = i + i_quote;
	return (str);
}

/**
 * @brief shall count how many character, separator, 
 * redirect, etc. parts we have
 */
int	count_parts(char *str)
{
	int i = 0;

	while(*str != '\0')
	{
		if(sep(*str))
			str = after_sep(str, NULL);
		else if(red(*str))
			str = after_red(str, NULL);
		else if(quote(*str))
			str = after_quote(str, NULL);
		else if(pipesign(*str))
			str = after_pipe(str, NULL);
		else// if(!sep(*str) && !red(*str)) // you can write an else
			str = after_word(str, NULL);
		i++;
	}
	return (i);
}	

/**
 * @brief runs though string and saves the parts into array[i].
 * gets with the subfuncs after_.... the amount of characters
 * in the next part and saves the value in part_len. buf points 
 * to the char after the part. 
 * Mallocs memory with part_len + 1 and saves the ptr in array[]
 * puts at the end of the new memory a \0
 * copies the chars of the part into the new memory. Sets str to
 * the char after the part. 
 * 
 * @param array 
 * @param str 
 */
int	fill_array(char **array, char *str, t_info *info)
{
	int part_i = 0;
	int part_len = 0;
	int i;
	char *buf;

	while(*str)
	{
		if (sep(*str))
			buf = after_sep(str, &part_len);
		else if (red(*str))
			buf = after_red(str, &part_len);
		else if (quote(*str))
			buf = after_quote(str, &part_len);
		else if (pipesign(*str))
			buf = after_pipe(str, &part_len);
		else
			buf = after_word(str, &part_len);
		array[part_i] = malloc(sizeof(char) * (part_len + 1));
		if (array[part_i] == NULL)
		{
			return (!error(ERR_MALLOC_SPLIT, info)); // ATTENTION!!!!!!!!  have to hand over info, or info will be a global
			//return (0);
		}
		array[part_i][part_len] = '\0';
		i = -1;
		while (++i < part_len)
			array[part_i][i] = str[i];
		str = buf;
		part_i++;
	}
	return (1);
}

/**
 * @brief counts amount of quotation marks ("" and '')
 * "skdhfjh skjdhf" -> return 1
 * "skdhfjh skjdhf -> return 0
 * "skdhfjh ' skjdhf" -> return 1 ' is inside " ", that's ok
 * @returns 1 if the amount is correct
 */
int	correct_amount_of_quot_marks(char *str)
{
	int	double_quote;
	int	single_quote;

	double_quote = 0;
	single_quote = 0;
	while (*str)
	{
		if (*str == '\"' && !single_quote)
			double_quote = !double_quote;
		// {
		// 	if (double_quote == 0)
		// 		double_quote = 1;
		// 	else
		// 		double_quote = 0;
		// }
		else if (*str == '\'' && !double_quote)
			single_quote = !single_quote;
		// {
		// 	if (single_quote == 0)
		// 		single_quote = 1;
		// 	else
		// 		single_quote = 0;
		// }
		str++;
	}
	if (double_quote == 0 && single_quote == 0)
		return (1);
	return (0);
}

char	**ft_split_lexer(char *str, t_info *info)
{
	int	word_count = 0;
	char **array;

	if(str == NULL || *str == 0) // maybe return a double pointer with malloced space -> better to handle in clean_up?
		return NULL;
	if(!correct_amount_of_quot_marks(str))
	{
		error(ERR_WRONG_AMOUNT_QUOTATION_MARKS, info); //info and return NULL?
		return (NULL);
	}
	word_count = count_parts(str);
	array = malloc((sizeof(char *) * (word_count + 1)));
	if (!array)
	{
		error(ERR_MALLOC_SPLIT_ONE, info);
		return (NULL);
	}
	array[word_count] = 0;
	if (!fill_array(array, str, info))
		return (NULL);
	return (array);
}
