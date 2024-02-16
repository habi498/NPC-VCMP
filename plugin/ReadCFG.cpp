/*	
    Copyright 2022 habi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
// reading a text file

#include "ReadCFG.h"
#undef int32_t
#undef uint32_t
#define int32_t int32_t__
#define uint32_t uint32_t__
#include <fstream>
#include <string>

using namespace std;
#define MAXCFGPARAMS 255
struct mystruct
{
    std::string param;
    int index;
};
mystruct getparam(string abcd)
{
    //getparam("abcd efgh") returns {"abcd",4}
    //getparam("abcd") returns {"abcd",4}
    //getparam("   abcd")returns {"abcd",7}
    //getparam(" abcd ")returns {"abcd",6} beware
    mystruct a;
    int i = abcd.find_first_not_of(' ');
    if (i == string::npos)
    {
        //string is exactly equal to "   "
        a.param = string("");
        a.index = abcd.length();
        return a;
    }
    int j = abcd.find(" ", i);
    if (j == string::npos)
    {
        //string is "abcd"
        a.param = abcd.substr(i);
        a.index = abcd.length();
        return a;
    }
    int len = j - i;
    a.param = abcd.substr(i, len);
    int h = abcd.find_first_not_of(' ', j);
    if (h == string::npos)
    {
        //there is no other paramter. Just blank space.
        //like "abcd "
        a.index = abcd.length();
    }
    else a.index = j;
    return a;
}
void cfg::freememory()
{
    if (ptr != NULL)
    {
        for (int i = 0; i < argc; i++)
        {
            if (ptr[i] != NULL)free(ptr[i]);
        }
        free(ptr);
    }
}
void cfg::read(const char* filename, const char* config)
{
    ifstream myfile(filename); string line;
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            int a = line.find(string(config) + string(" "));
            if (a == 0)
            {
                //line=ppp abc def ghi
                int c = line.find(" ");
                if (c != string::npos)
                {
                    string params = line.substr(c);
                    mystruct b = getparam(params);
                    int len; char** temp;
                    char* temp2;
                    while (b.param != string("") && argc < MAXCFGPARAMS)
                    {
                        len = b.param.length() + 1;
                        temp = (char**)realloc(ptr, (argc + 1) * sizeof(char*));
                        if (temp != NULL)
                        {
                            ptr = temp;
                            temp2 = (char*)malloc(len * sizeof(char));
                            if (temp2 != NULL)
                            {
                                ptr[argc] = temp2;
                                //*ptr[argc] = b.param.c_str();
                                for (int i = 0; i < len; i++)ptr[argc][i] = b.param.c_str()[i];

                                argc += 1;
                            }
                        }
                        //if b.index is params.length, then 
                        //there is no other paramter left. 
                        if (b.index == params.length())break;
                        params = params.substr(b.index);
                        b = getparam(params);
                    }
                }
                break;
            }
        }
        myfile.close();
    }
    else printf("Unable to open %s\n", filename);
}



