# include "utils.hpp"

bool		if_file_exists(std::string file_name)
{
    struct stat buf;

    std::cout << file_name <<std::endl;
    if (stat(file_name.c_str(), &buf) != -1){
        return(true);
    }
    else
    {
        return(false);
    }
}

static	int		sent_len(char const *s, char c)
{
    int			word;
    int			i;

    word = 0;
    i = 0;
    while (s[i])
    {
        if (s[i] != c)
        {
            word++;
            while (s[i] != c && s[i])
                i++;
        }
        else
            i++;
    }
    return (word);
}

static	char	**free_function(char **result, int main_count)
{
    while (main_count > 0)
    {
        free(result[main_count]);
        main_count--;
    }
    free(result);
    return (0);
}

static	char	**fill_word(char const *s, char c, char **result)
{
    int			main_count;
    int			i;
    int			first;

    i = 0;
    main_count = 0;
    while (s[i])
    {
        if (s[i] != c)
        {
            first = i;
            while (s[i] != c && s[i])
                i++;
            result[main_count] = (char*)malloc(i - first + 1);
            if (result[main_count] == 0)
                return (free_function(result, main_count));
            strncpy(result[main_count], &s[first], i - first + 1);
            result[main_count][i - first] = '\0';
            main_count++;
        }
        else
            i++;
    }
    result[main_count] = 0;
    return (result);
}

char			**ft_split(char const *s, char c)
{
    int		len_sent;
    char	**result;

    if (s == 0)
        return (0);
    len_sent = sent_len(s, c);
    result = (char **)malloc(sizeof(char *) * (len_sent + 1));
    if (result == 0)
        return (0);
    return (fill_word(s, c, result));
}

std::string		get_cwd_string()
{
    char dir[1024];
    getcwd(dir, 1024);
    //Exception
    std::string cwd(dir);
    return cwd;
}