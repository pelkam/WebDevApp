#ifndef GPX_HELPER_H
#define GPX_HELPER_H
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlschemastypes.h>
#include "LinkedListAPI.h"
#include "GPXParser.h"
void deleteRouteDum(void *data);
void deleteTrackDum(void *data);
#endif