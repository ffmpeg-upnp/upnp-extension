/* $Id: upnpreplyparse.c,v 1.3 2009/05/10 03:24:29 jmaggard Exp $ */
/* MiniUPnP project
 * http://miniupnp.free.fr/ or http://miniupnp.tuxfamily.org/
 * (c) 2006 Thomas Bernard 
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "upnpreplyparse.h"
#include "minixml.h"

static void
NameValueParserStartElt(void * d, const char * name, int l)
{
    struct NameValueParserData * data = (struct NameValueParserData *)d;
    if(l>511)
        l = 511;
    memcpy(data->curelt, name, l);
    data->curelt[l] = '\0';
}

static void
NameValueParserGetData(void * d, const char * datas, int l)
{
    struct NameValueParserData * data = (struct NameValueParserData *)d;
    struct NameValue * nv;
    nv = malloc(sizeof(struct NameValue));
    if(l>511)
        l = 511;
    strncpy(nv->name, data->curelt, 512);
    nv->name[63] = '\0';
    memcpy(nv->value, datas, l);
    nv->value[l] = '\0';
    LIST_INSERT_HEAD( &(data->head), nv, entries);
}

void
ParseNameValue(const char * buffer, int bufsize,
                    struct NameValueParserData * data)
{
    struct xmlparser parser;
    LIST_INIT(&(data->head));
    /* init xmlparser object */
    parser.xmlstart = buffer;
    parser.xmlsize = bufsize;
    parser.data = data;
    parser.starteltfunc = NameValueParserStartElt;
    parser.endeltfunc = 0;
    parser.datafunc = NameValueParserGetData;
	parser.attfunc = 0;
    parsexml(&parser);
}

void
ClearNameValueList(struct NameValueParserData * pdata)
{
    struct NameValue * nv;
    while((nv = pdata->head.lh_first) != NULL)
    {
        LIST_REMOVE(nv, entries);
        free(nv);
    }
}

char * 
GetValueFromNameValueList(struct NameValueParserData * pdata,
                          const char * Name)
{
    struct NameValue * nv;
    char * p = NULL;
    for(nv = pdata->head.lh_first;
        (nv != NULL) && (p == NULL);
        nv = nv->entries.le_next)
    {
        if(strcmp(nv->name, Name) == 0)
            p = nv->value;
    }
    return p;
}

/* debug all-in-one function 
 * do parsing then display to stdout */
#ifdef DEBUG
void
DisplayNameValueList(char * buffer, int bufsize)
{
    struct NameValueParserData pdata;
    struct NameValue * nv;
    ParseNameValue(buffer, bufsize, &pdata);
    for(nv = pdata.head.lh_first;
        nv != NULL;
        nv = nv->entries.le_next)
    {
        printf("%s = %s\n", nv->name, nv->value);
    }
    ClearNameValueList(&pdata);
}
#endif

