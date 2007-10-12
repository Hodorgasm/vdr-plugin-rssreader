/*
 * tools.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <stdlib.h>
#include <iconv.h>
#include "tools.h"
#include "common.h"

// --- Static -----------------------------------------------------------

#define ELEMENTS(x) (sizeof(x) / sizeof(x[0]))

struct conv_table {
  char *from;
  char *to;
};

static struct conv_table pre_conv_table[] =
{
  // 'to' field must be smaller than 'from'
  {"<br />",   "\n"}
};

static struct conv_table post_conv_table[] =
{
  // 'to' field must be smaller than 'from'
  {"&#228;",   "�"},
  {"&auml;",   "�"},
  {"&#196;",   "�"},
  {"&Auml;",   "�"},
  {"&#246;",   "�"},
  {"&ouml;",   "�"},
  {"&#214;",   "�"},
  {"&Ouml;",   "�"},
  {"&#229;",   "�"},
  {"&�uml;",   "�"},
  {"&#197;",   "�"},
  {"&�uml;",   "�"},
  {"&#220;",   "�"},
  {"&Uuml;",   "�"},
  {"&#252;",   "�"},
  {"&uuml;",   "�"},
  {"&#223;",   "�"},
  {"&szlig;",  "�"},
  {"&#234;",   "�"},
  {"&#8211;",  "-"},
  {"&ndash;",  "-"},
  {"&#38;",    "&"},
  {"&amp;",    "&"},
  {"&#58;",    ":"},
  {"&#91;",    "["},
  {"&#93;",    "]"},
  {"&#40;",    "'"},
  {"&#41;",    "'"},
  {"&#039;",   "'"},
  {"&#180;",   "'"},
  {"&acute;",  "'"},
  {"&#8216;",  "'"},
  {"&#8217;",  "'"},
  {"&#231;",   "�"},
  {"&ccedil;", "�"},
  {"&#233;",   "�"},
  {"&eacute;", "�"},
  {"&#226;",   "�"},
  {"&acirc;",  "�"},
  {"&#8364;",  "�"},
  {"&euro;",   "�"},
  {"&quot;",   "\""},
  {"&#8220;",  "\""},
  {"&#8221;",  "\""},
  {"&#8222;",  "\""},
  {"&#160;",   " "},
  {"&nbsp;",   " "},
  {"&lt;",     "<"},
  {"&gt;",     ">"},
  {"\n\n",     "\n"}, // let's also strip multiple linefeeds
};

static char *htmlcharconv(char *str, struct conv_table *conv, unsigned int elem)
{
  if (str && conv) {
     for (unsigned int i = 0; i < elem; ++i) {
        char *ptr = strstr(str, conv[i].from);
        while (ptr) {
           int of = ptr - str;
           int l  = strlen(str);
           int l1 = strlen(conv[i].from);
           int l2 = strlen(conv[i].to);
           if (l2 > l1) {
              error("htmlcharconv(): cannot reallocate string");
              return str;
              }
           if (l2 != l1)
              memmove(str + of + l2, str + of + l1, l - of - l1 + 1);
           strncpy(str + of, conv[i].to, l2);
           ptr = strstr(str, conv[i].from);
           }
        }
     return str;
     }
  return NULL;
}

// --- General functions ------------------------------------------------

int charsetconv(const char *buffer, int buf_len, const char *str, int str_len, const char *from, const char *to)
{
  if (to && from) {
     iconv_t ic = iconv_open(to, from);
     if (ic >= 0) {
        size_t inbytesleft = str_len;
        size_t outbytesleft = buf_len;
        char *out = (char*)buffer;
        int ret;
        if ((ret = iconv(ic, (char**)&str, &inbytesleft, &out, &outbytesleft)) >= 0) {
           iconv_close(ic);
           return buf_len - outbytesleft;
           }
        iconv_close(ic);
        }
     }
  else
     error("charsetconv(): charset is not valid");
  return -1;
}

char *striphtml(char *str)
{
  if (str) {
     char *c, t = 0, *r;
     str = htmlcharconv(str, pre_conv_table, ELEMENTS(pre_conv_table));
     c = str;
     r = str;
     while (*str != '\0') {
       if (*str == '<')
          t++;
       else if (*str == '>')
          t--;
       else if (t < 1)
          *(c++) = *str;
       str++;
       }
     *c = '\0';
     return htmlcharconv(r, post_conv_table, ELEMENTS(post_conv_table));
     }
  return NULL;
}

void *myrealloc(void *ptr, size_t size)
{
  /* There might be a realloc() out there that doesn't like reallocing
     NULL pointers, so we take care of it here */
  if (ptr)
     return realloc(ptr, size);
  else
     return malloc(size);
}
