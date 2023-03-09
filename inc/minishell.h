#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libs/libft/libft.h"
# include <sys/stat.h>
# include <sys/stat.h>
# include <sys/ioctl.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <signal.h>
# include <fcntl.h>
# include <dirent.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/wait.h>
# include <termios.h>

//*********************************************************//
//**                FLAGS                               **//

//**                PARSER CATEGORIES                    **//

# define REDIR_INPUT 4
# define REDIR_OUTPUT 5
# define REDIR_OUTPUT_APPEND 6
# define REDIR_INPUT_APPEND 7
# define PIPE 8
# define WORD 9
# define FLAG 11 // we don't need to cat it if the ft_order says ok to that ( > -n) then we need to check that again, when using open() it will return a -1 I think if it's not a valid file name.
# define SEPARATOR 12

# define SEPARATOR_ONE " " // this is not so nice
# define SEPARATOR_TWO "\t"
# define SEPARATOR_THREE "\n"

# define BUILTIN_START 30
# define CMD_ECHO 31
# define CMD_EXIT 32
# define CMD_CD 33
# define CMD_PWD 34
# define CMD_EXPORT 35
# define CMD_UNSET 36
# define CMD_ENV 37
# define BUILTIN_END 40

//**                ERROR FLAGS                          **//

# define ERR_MALLOC_SPLIT 41
# define ERR_MALLOC_INIT_GROUPS 42
# define ERR_CMD_NOT_FOUND 43
# define ERR_MALLOC_ARRAY_ARGS 44
# define ERR_WRONG_AMOUNT_QUOTATION_MARKS 45
# define ERR_MALLOC_SPLIT_ONE 46
# define ERR_NO_EQUAL_IN_EXPORT_ARG 47
# define ERR_EQUAL_IN_UNSET_ARG 48
# define ERR_MALLOC_EXPAND_VARIABLES 49
# define ERR_CD_NO_DIRECTORY 50

//**                OTHER FLAGS                          **//

# define CTRL_D_PRESSED 60
# define CLEAN_UP_REST_BEFORE_EXIT 61
# define CLEAN_UP_FOR_NEW_PROMPT 62

//**				TEXT OUTPUT							**//

# define STR_PROG_NAME "minishell:"
# define ERROR "error\n"
# define STR_EXIT_MULTIPLE "Word exit is enough!\n"
# define STR_EXIT_SUCCESSFULL "Exit successfull!\n"
# define MALLOC_FAIL "While memory allocation\n"
# define PIPE_ERROR_1 "-bash: syntax error near unexpected token `|'\n"
# define PIPE_ERROR_2 "This input is not allowed!\n"
# define STR_WRITE_ERROR "Command not found\n"
# define REDIRECT_ERROR_1 "No such file or directory\n"
# define INVALID_IDENTIFIER "Not a valid identifier\n"
# define QUOTATION_MARKS "Wrong amount of quotation marks\n"
# define EQUAL_SIGN "No '=' sign in export argument\n"

//**				pipes							**//

# define WRITE 1
# define READ 0

//*********************************************************//
//**                STRUCTURES                          **//

/**
 * @brief  info struct holds all informations
 * @prompt:	stores the whole command line the user types in 
 * -> has to be freed in the main while loop before user can
 *  enter the next commands
 * @input_lexer: holds a 2d char array, all the commands, 
 * arguments, emptyspacec, pipes and so on are separated
 * @env: 3d array to all the environment variables.
 * *env is an array of ptrs for the different vars.
 * each of the ptrs point to an array of 2 ptrs. [0] points to
 * the string of the env variables name.
 * [1] points to the string of the variables value.
 * @paths: 2d char array that holds all the paths to the bin folders
 * that are saved in the env variable PATH
 * @root_cmd: ask Nick
 * @nb_root_cmd: ask Nick
 * @num_groups: number of groups in the prompt (separated by pipes | )
 * @groups: ptr to all the groups. All groups are behind each other
 * in the memory. There are no pointers in between that point to 
 * every group. The last group is 0. Cannot check in while loop
 * like this while(group[i]) -> has to be done like this
 * while(++i < info->num_groups) don't know why.
 */
typedef struct s_info
{
	char			*prompt;
	char			**input_lexer;
	char			***env;
	char			**paths;
	// char			*root_cmd;
	int				nb_root_cmd;
	int				num_groups;
	struct s_group	*groups;
}	t_info;

/**
 * @brief there is one group for every command that is typed into the minishell
 * we assume, that every command is separated by a | . 
 * For example if is typed in> echo Hallo | cat
 * there will be 2 groups. in group[0] echo is the command and Hallo the argument. 
 * We put both into **arguments: {"echo", "Hallo", NULL}. The command itself 
 * will allways be the first argument. group[1].arguments: {"cat", NULL}.
 * This for example: <infile echo -n <infile_2 Hallo >>outfile du | cat >out_2
 * will give us :
 * group[0].arguments: {"echo", "-n", "Hallo", "du", NULL}
 * group[0].path: NULL -> we shall code echo ourselfs, so no path to the executable
 * group[0].builtin: 31 -> CMD_ECHO is defined to 31, see line 38 in this Headerfile
 * group[0].redir_in: 4 -> REDIR_INPUT is defined to 4, see line 24 in this file
 * group[0].redir_infile: "infile_2" -> only the last redirect 
 * in the category input will be saved. The redirect from infile will be ignored.
 * group[0].redir_out: 6 -> # define REDIR_OUTPUT_APPEND 6
 * group[0].redir_outfile: "outfile"
 * group[0].pipe_in: 0
 * group[0].pipe_out: 1
 * *pipe_fd_in, *pipe_fd_out, pid: will be later used in the executer
 * info ist a pointer to the info struct.
 * For group[1] there will only be showed the variables 
 * group[1].arguments: {"cat", NULL}
 * group[1].path: "usr/bin/cat" -> the path will be something like this (including the executable)
 * group[1].builtin: 0
 * group[1].redir_out: 5 -> # define REDIR_OUTPUT 5
 * group[1].redir_outfile: "out_2"
 * group[1].pipe_in: 1
 * 
 */
typedef struct s_group
{
	char	**arguments;
	char	*path;
	int		builtin;
	int		redir_in;
	char 	*redir_infile;
	int		redir_out;
	char 	*redir_outfile;
	int		pipe_in;
	int		pipe_out;
	int		pipe_fd[2];
	pid_t	pid;
	t_info	*info;
}	t_group;

/**
 * @brief temp struct for the while loop in the parser
 * 
 * @cat: 	the int value the categorizer returns f.e. (REDIRECT_OUTPUT)
 * @act_group:	actual group of commands f.e. (echo Hallo | cat) before the | it's 0 after it's 1
 * @is_red:		is redirect -> the int value that found_save_redirect() returns -> 1 for yes, 0 no redirect
 * @is_exe:		is executable -> int value that found_save_executable() returns -> 1 for yes, 0 no exe
 */
typedef struct s_parse_lexer
{
	int		cat;
	int		act_group;
	int		is_red;
	int		is_exe;

}	t_parse_lexer;


//*********************************************************//
//**                FUNCTIONS                           **//
//*******************************************************//

//**** categorize.c ****//

int categorize(char *str);

//**** check_if_cmd.c ****//

char	*is_an_executable(char *cmd, t_info *t_info);

//**** input_check.c ****//

int	parser_error_check(t_info *info, t_parse_lexer *pl, int i);
int	executer_error_check(t_info *info, t_group *groups);

//**** clean_up.c ****//

void	clean_up_group_structs(t_info *info);
void	clean_up_paths(t_info *info);
void	clean_up_prompt();
void	clean_up_lexer(t_info *info);
void	clean_up(int clean_up_code, t_info *info);

//**** ft_message.c ****//

int input_message(char *str, int exit_no);

//**** error.c ****//

int	error(int err, t_info *info);

//**** executer.c ****//

void	executer(t_group	*group);

//**** expand_variables.c.c ****//

char	*value_of_variable_from_env(char ***env, char *var, int len);
int		expand_variables(char	**array, char ***env, t_info *info);

//**** ft_pwd.c ***//
void	ft_pwd(t_group *group);

//**** ft_cd.c ****//

void	ft_cd(char **args, t_info *info);

//**** ft_echo.c ****//

void	ft_echo(char	**args, int pipe_out);

//**** ft_exit.c ****//

void	ft_exit(t_info *info);

//**** ft_env.c ****//
void	ft_env(t_info *info);

//**** ft_export.c ****//

int	num_env_args(char ***args);
int	ft_export(char **args, t_info *info);

//**** ft_split_lexer.c ****//

int		sep(char c);
int		count_parts(char *str);
char	**ft_split_lexer(char *str, t_info *info);

//**** ft_unset.c ****//

int	ft_unset(char **args, t_info *info);

//**** init.c ****//
t_info	*init(char **envp);
char	**ft_array_args(char *str, t_info *info);
t_group *init_groups(t_info *info);


//**** make_env_arr.c ****//

void	make_env(char **envp, t_info *info);

//**** parser_utils.c ****//

int		found_save_redirect(t_parse_lexer *pl, t_info *info, char *act_input_lexer_str, int i);
int		found_save_executable(t_parse_lexer *pl, t_info *info, char *act_input_lexer_str, int i);
void	found_save_arguments(t_parse_lexer *pl, t_info *info, int i);
void	pipe_detector(t_parse_lexer *pl, t_info *info);
int		count_groups(t_info *info);
void	delete_quotationmarks(char	**array);

//**** parser.c ****//

int		parser(t_info *info);

void	test_env_vars(t_info *info);

//**** tests.c ****//

void	p2d(char **ptr);
void	print_groups(t_group *groups, t_info *info);

void	remove_ctrl_c_feed(void);

extern int	g_exit_status;

#endif
