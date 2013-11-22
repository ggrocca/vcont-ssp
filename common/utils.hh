#ifndef _UTILS_HH
#define _UTILS_HH

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include <string>
#include <vector>

#include "strlcat.h"

 #define ___STR1(x)  #x
 #define GGSTR(x)  ___STR1(x)

static inline char* get_ext (char* fn)
{
    char* p;
    char* r = 0;
    for (p = fn; *p != '\0'; p++)
	if (*p == '.')
	    r = p + 1;
    return r;
}

static inline std::string get_ext (std::string const& s)
{
    if(s.find_last_of (".") != std::string::npos)
        return s.substr (s.find_last_of (".") + 1);
    return "";
}

static inline std::string get_base (std::string const& s)
{
    if(s.find_last_of ("/") != std::string::npos)
        return s.substr (s.find_last_of ("/") + 1);
    return "";
}



static inline bool ends_with (std::string const &s, std::string const &e)
{
    if (s.length() >= e.length())
        return 0 == s.compare (s.length() - e.length(), e.length(), e);
    return false;
}


// // cp utils.hh  prova.cc && g++ prova.cc && ./a.out && rm a.out prova.cc

// int main (void)
// {
//     std::string s;
    
//     s = std::string ("");
//     printf ("%s --> %s\n", s.c_str(), get_ext(s).c_str());

//     s = std::string ("gne");
//     printf ("%s --> %s\n", s.c_str(), get_ext(s).c_str());

//     s = std::string ("gne.bih.buh");
//     printf ("%s --> %s\n", s.c_str(), get_ext(s).c_str());

//     s = std::string ("gne.bi h");
//     printf ("%s --> %s\n", s.c_str(), get_ext(s).c_str());

//     s = std::string ("gne...bih");
//     printf ("%s --> %s\n", s.c_str(), get_ext(s).c_str());

//     s = std::string ("gne.bih");
//     printf ("%s --> %s\n", s.c_str(), get_ext(s).c_str());
// }


// int main (void)
// {
//     std::string s;
    
//     s = std::string ("");
//     printf ("%s --> %s\n", s.c_str(), get_base(s).c_str());

//     s = std::string ("gne");
//     printf ("%s --> %s\n", s.c_str(), get_base(s).c_str());

//     s = std::string ("gne/bih/buh");
//     printf ("%s --> %s\n", s.c_str(), get_base(s).c_str());

//     s = std::string ("gne/bi h");
//     printf ("%s --> %s\n", s.c_str(), get_base(s).c_str());

//     s = std::string ("gne///bih");
//     printf ("%s --> %s\n", s.c_str(), get_base(s).c_str());

//     s = std::string ("gne/bih");
//     printf ("%s --> %s\n", s.c_str(), get_base(s).c_str());

//     return 0;
// }


static inline void list_dir (const char *name, std::vector<std::string>* entries)
{
#define __FNLEN 512

    DIR *pDIR;
    struct dirent *entry;
    
    char dir_name[__FNLEN] = {'\0'};
    size_t dir_siz = strlcat (dir_name, name, __FNLEN);
    if (dir_name[dir_siz - 1] == '/')
	dir_name[dir_siz - 1] = '\0';

    entries->clear();

    if (pDIR = opendir (dir_name))
    {
	while (entry = readdir(pDIR))
	{
	    if ((strlen (entry->d_name) == 0) ||
		(strlen (entry->d_name) >= 1 && entry->d_name[0] == '.') ||
		(strlen (entry->d_name) >= 2 && entry->d_name[0] == '.' && entry->d_name[1] == '.'))
		continue;

	    //printf ("%s\n", entry->d_name);
	    //char rel_path[__FNLEN] = {'\0'};
	    //snprintf (rel_path, __FNLEN, "%s/%s", dir_name, entry->d_name);
	    //printf ("%s\n", rel_path);

	    std::string sp (dir_name);
	    entries->push_back (sp + "/" + entry->d_name);
	}
    }
    else
    {
	fprintf (stderr, "Could not open directory  %s.\n", dir_name);
	exit (2);
    }

    closedir(pDIR);    
#undef __FNLEN
}

// // test list_dir 
// int main(int argc, char *argv[])
// {
//     if (argc != 2)
//     {
// 	fprintf (stderr, "Usage: %s dirname.\n", argv[0]);
// 	exit (1);
//     }

//     std::vector<std::string> es;

//     list_dir (argv[1], &es);

//     for (int i = 0; i < es.size(); i++)
// 	printf ("%s\n", es[i].c_str());

//     exit (0);
// }




#endif
