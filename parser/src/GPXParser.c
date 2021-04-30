#include "GPXParser.h"
#include "GPXHelper.h"
char * findPathRoute(char *filename,float slat,float slon,float elat,float elon,float delta){
    GPXdoc *d = createValidGPXdoc(filename,"gpx.xsd");
    List *routes = getRoutesBetween(d,slat,slon,elat,elon,delta);
    if(routes==NULL){
        if(d!=NULL){
            deleteGPXdoc(d);
        }
        return "[]";
    }else{
        char *s = routeListToJSON(routes);
        deleteGPXdoc(d);
        freeList(routes);
        return s;
    }
}
char * findPathTrack(char *filename,float slat,float slon,float elat,float elon,float delta){
    GPXdoc *d = createValidGPXdoc(filename,"gpx.xsd");
    List *tracks = getTracksBetween(d,slat,slon,elat,elon,delta);
    if(tracks ==NULL){
        if(d!=NULL){
            deleteGPXdoc(d);
        }
        return "[]";
    }else{
        char *s = trackListToJSON(tracks);
        deleteGPXdoc(d);
        freeList(tracks);
        return s;
    }
}
int createWay(char *filename,char* rteName,char*wayName,char *loc){
    GPXdoc *d = createValidGPXdoc(filename,"gpx.xsd");
    char *token = strtok(loc,"|");
    int lat = atoi(token);
    token = strtok(NULL,"|");
    int lon = atoi(token);
    Route* rte = getRoute(d,rteName);
    Waypoint *wpt = malloc(sizeof(Waypoint)+1);
    wpt->name = malloc(strlen(wayName)+1);
    strcpy(wpt->name,wayName);
    wpt->otherData = initializeList(&gpxDataToString,&deleteGpxData,&compareGpxData);
    wpt->latitude = lat;
    wpt->longitude = lon;
    addWaypoint(rte,wpt);
    int i = writeGPXdoc(d,filename);
    deleteGPXdoc(d);
    return i;
}
int createRte(char *filename,char *name){
    GPXdoc *d = createValidGPXdoc(filename,"gpx.xsd");
    Route *rte = malloc(sizeof(Route)+1);
    rte->name = malloc(strlen(name)+1);
    strcpy(rte->name,name);
    rte->waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
    rte->otherData = initializeList(&gpxDataToString,&deleteGpxData,&compareGpxData);
    addRoute(d,rte);
    int i = writeGPXdoc(d,filename);
    deleteGPXdoc(d);
    return i;
}
char *uploadGPXFile(char *filename){
    GPXdoc *d = createValidGPXdoc(filename,"gpx.xsd");
    if(d!=NULL){
        char * str = GPXtoJSON(d);
        deleteGPXdoc(d);
        return str;
    }
    return "{}";
}
int createGPXfile(char *filename,char* jsonString){
    GPXdoc *d = JSONtoGPX(jsonString);
    int i = writeGPXdoc(d,filename);
    deleteGPXdoc(d);
    return i;
}
char*getViewPanelTrack(char *filename){
    GPXdoc *d = createValidGPXdoc(filename,"gpx.xsd");
    char *str = trackListToJSON(d->tracks);
    deleteGPXdoc(d);
    return str;
}
char*getViewPanelRoute(char *filename){
    GPXdoc *d = createValidGPXdoc(filename,"gpx.xsd");
    char *str = routeListToJSON(d->routes);
    deleteGPXdoc(d);
    return str;
}
char *getWaypointInfo(char *filename, int rtCount){
    GPXdoc *d = createValidGPXdoc(filename,"gpx.xsd");
    ListIterator wpts = createIterator(d->routes);
    for(int i=0;i<rtCount;++i){
        nextElement(&wpts);
    }
    Route *r = (Route*)wpts.current->data;
    char *str = waypointListToJSON(r->waypoints);
    deleteGPXdoc(d);
    return str;
}
char *getDataList(char *filename,char *name,char *id){
    GPXdoc *d = createValidGPXdoc(filename,"gpx.xsd");
    char *str="[]";
    int count = 0;
    int numId = atoi(id);
    if(strcmp(name,"Track")==0){
        ListIterator trk = createIterator(d->tracks);
        while(count!=numId-1){
            nextElement(&trk);
            ++count;
        }
        Track *tr = (Track*)trk.current->data;
        str = dataListToJSON(tr->otherData);
    }else if(strcmp(name,"Route")==0){
        ListIterator rte = createIterator(d->routes);
        while(count!=numId-1){
            nextElement(&rte);
            ++count;
        }
        Route *rt = (Route*)rte.current->data;
        str = dataListToJSON(rt->otherData);
    }
    return str;
}
int changeName(char* filename,char*name,char*id,char*rename){
    GPXdoc *d = createValidGPXdoc(filename,"gpx.xsd");
    int count = 0;
    int numId = atoi(id);
    if(strcmp(name,"Track")==0){
        ListIterator trk = createIterator(d->tracks);
        while(count!=numId-1){
            nextElement(&trk);
            ++count;
        }
        Track *tr = (Track*)trk.current->data;
        strcpy(tr->name,rename);
    }
    if(strcmp(name,"Route")==0){
        ListIterator rte = createIterator(d->routes);
        while(count!=numId-1){
            nextElement(&rte);
            ++count;
        }
        Route *rt = (Route*)rte.current->data;
        strcpy(rt->name,rename);
    }
    if(writeGPXdoc(d,filename)){
        free(d);
        return 0;
    }
    free(d);
    return 1;
}
int getNumWaypointsTrack(Track *tr){
    int count=0;
    ListIterator trkseg = createIterator(tr->segments);
    while(trkseg.current!=NULL){
        TrackSegment *seg = (TrackSegment*)trkseg.current->data;
        count = count + getLength(seg->waypoints);
        nextElement(&trkseg);
    }
    return count;
}
bool validateGPXDoc(GPXdoc* doc, char* gpxSchemaFile){
    if(doc == NULL||gpxSchemaFile == NULL){
        return false;
    }
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;
    ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);
    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);
    xmlDocPtr d;
    d = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr rnode;
    xmlNodePtr wptnode;
    xmlNodePtr rtenode;
    xmlNodePtr trknode;
    xmlNodePtr segnode;
    //Make root
    rnode = xmlNewNode(NULL,BAD_CAST "gpx");
    char *ver = malloc(sizeof(ver)*10);
    sprintf(ver,"%.1f", doc->version);
    xmlNewProp(rnode,BAD_CAST "version",BAD_CAST ver);
    free(ver);
    xmlNewProp(rnode,BAD_CAST "creator",BAD_CAST doc->creator);
    xmlSetNs(rnode,xmlNewNs(rnode,BAD_CAST doc->namespace,NULL));
    xmlDocSetRootElement(d, rnode);
    //xmlSaveFormatFileEnc("file.gpx", d, "UTF-8", 1);
    ListIterator wptInt;
    wptInt = createIterator(doc->waypoints);
    while(wptInt.current!=NULL){
        //Get all wpt info
        Waypoint *w = (Waypoint*)wptInt.current->data;
        wptnode = xmlNewChild(rnode,NULL,BAD_CAST "wpt",NULL);
        char *lat = malloc(sizeof(lat)*10);
        sprintf(lat,"%.5f", w->latitude);
        xmlNewProp(wptnode,BAD_CAST "lat",BAD_CAST lat);
        char *lon = malloc(sizeof(lon)*10);
        sprintf(lon,"%.5f", w->longitude);
        xmlNewProp(wptnode,BAD_CAST "lon",BAD_CAST lon);
        free(lat);
        free(lon);
        //Add all other data name is type value is the acutal data
        ListIterator wptdata;
        wptdata = createIterator(w->otherData);
        xmlNewChild(wptnode,NULL,BAD_CAST "name",BAD_CAST w->name);
        while(wptdata.current!=NULL){
            GPXData *od = (GPXData*)wptdata.current->data;
            xmlNewChild(wptnode,NULL,BAD_CAST od->name,BAD_CAST od->value);
            nextElement(&wptdata);
        }
        nextElement(&wptInt);
    }
    ListIterator rteInt;
    rteInt = createIterator(doc->routes);
    while(rteInt.current!=NULL){
        Route *r = (Route*)rteInt.current->data;
        rtenode = xmlNewChild(rnode,NULL,BAD_CAST "rte",NULL);
        if(r->name!=NULL&&strcmp(r->name,"")!=0){
            xmlNewChild(rtenode,NULL,BAD_CAST "name",BAD_CAST r->name);
        }
        ListIterator rtedata;
        rtedata = createIterator(r->otherData);
        while(rtedata.current!=NULL){
            GPXData *od = (GPXData*)rtedata.current->data;
            xmlNewChild(rtenode,NULL,BAD_CAST od->name,BAD_CAST od->value);
            nextElement(&rtedata);
        }
        ListIterator rteptdata;
        rteptdata = createIterator(r->waypoints);
        while(rteptdata.current!=NULL){
            Waypoint *w = (Waypoint*)rteptdata.current->data;
            wptnode = xmlNewChild(rtenode,NULL,BAD_CAST "rtept",NULL);
            char *lat = malloc(sizeof(lat)*10);
            sprintf(lat,"%.5f", w->latitude);
            xmlNewProp(wptnode,BAD_CAST "lat",BAD_CAST lat);
            char *lon = malloc(sizeof(lon)*10);
            sprintf(lon,"%.5f", w->longitude);
            xmlNewProp(wptnode,BAD_CAST "lon",BAD_CAST lon);
            free(lat);
            free(lon);
            //Add all other data name is type value is the acutal data
            ListIterator wptdata;
            wptdata = createIterator(w->otherData);
            if(w->name!=NULL&&strcmp(w->name,"")!=0){
                xmlNewChild(wptnode,NULL,BAD_CAST "name",BAD_CAST w->name);
            }
            while(wptdata.current!=NULL){
                GPXData *od = (GPXData*)wptdata.current->data;
                xmlNewChild(wptnode,NULL,BAD_CAST od->name,BAD_CAST od->value);
                nextElement(&wptdata);
            }
            nextElement(&rteptdata);
        }
        nextElement(&rteInt);
    }
    ListIterator trkInt;
    trkInt = createIterator(doc->tracks);
    while(trkInt.current!=NULL){
        Track *t = (Track*) trkInt.current->data;
        trknode = xmlNewChild(rnode,NULL,BAD_CAST "trk",NULL);
        if(t->name!=NULL&&strcmp(t->name,"")!=0){
            xmlNewChild(trknode,NULL,BAD_CAST "name",BAD_CAST t->name);
        }
        ListIterator trkdata;
        trkdata = createIterator(t->otherData);
        while(trkdata.current!=NULL){
                GPXData *od = (GPXData*)trkdata.current->data;
                xmlNewChild(trknode,NULL,BAD_CAST od->name,BAD_CAST od->value);
                nextElement(&trkdata);
            }
        ListIterator trkseg;
        trkseg = createIterator(t->segments);
        while(trkseg.current!=NULL){
            TrackSegment *s = (TrackSegment*)trkseg.current->data;
            segnode = xmlNewChild(trknode,NULL,BAD_CAST "trkseg",NULL);
            ListIterator trkpt;
            trkpt = createIterator(s->waypoints);
            while(trkpt.current!=NULL){
                Waypoint *w = (Waypoint*)trkpt.current->data;
                wptnode = xmlNewChild(segnode,NULL,BAD_CAST "trkpt",NULL);
                char *lat = malloc(sizeof(lat)*10);
                sprintf(lat,"%.5f", w->latitude);
                xmlNewProp(wptnode,BAD_CAST "lat",BAD_CAST lat);
                char *lon = malloc(sizeof(lon)*10);
                sprintf(lon,"%.5f", w->longitude);
                xmlNewProp(wptnode,BAD_CAST "lon",BAD_CAST lon);
                free(lat);
                free(lon);
                //Add all other data name is type value is the acutal data
                ListIterator wptdata;
                wptdata = createIterator(w->otherData);
                if(w->name!=NULL&&strcmp(w->name,"")!=0){
                    xmlNewChild(wptnode,NULL,BAD_CAST "name",BAD_CAST w->name);
                }
                while(wptdata.current!=NULL){
                    GPXData *od = (GPXData*)wptdata.current->data;
                    xmlNewChild(wptnode,NULL,BAD_CAST od->name,BAD_CAST od->value);
                    nextElement(&wptdata);
                }
                nextElement(&trkpt);
            }
            nextElement(&trkseg);
        }
        nextElement(&trkInt);
    }
    //Close loop
    if(doc==NULL){
        return false;
    }else{
        xmlSchemaValidCtxtPtr ctxt;
        int ret;
        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        ret = xmlSchemaValidateDoc(ctxt, d);
        xmlSchemaFree(schema);
        xmlSchemaFreeValidCtxt(ctxt);
        xmlFreeDoc(d);
        xmlCleanupParser();
        if(ret ==0){
            return true;
        }
        return false;
    }
}
//Creates valid gpx doc
GPXdoc* createValidGPXdoc(char* fileName, char* gpxSchemaFile){
    if (fileName == NULL||gpxSchemaFile==NULL){
        return NULL;
    }
    xmlDocPtr doc;
    GPXdoc *d;
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;
    ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);
    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);
    doc = xmlReadFile(fileName, NULL, 0);
    if (doc == NULL||gpxSchemaFile==NULL){
        xmlSchemaFree(schema);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }else{
        xmlSchemaValidCtxtPtr ctxt;
        int ret;
        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        ret = xmlSchemaValidateDoc(ctxt, doc);
        if (ret == 0){
            d = createGPXdoc(fileName);
            xmlSchemaFree(schema);
            xmlSchemaFreeValidCtxt(ctxt);
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return d;
        }
        xmlCleanupParser();
        xmlSchemaFree(schema);
        xmlSchemaFreeValidCtxt(ctxt);
        xmlFreeDoc(doc);
        return NULL;
    } 
}
//Creates doc
GPXdoc* createGPXdoc(char* fileName){
    if(fileName==NULL){
        return NULL;
    }
    GPXdoc *doc=malloc(sizeof(GPXdoc)+1);
    xmlDoc *file;
    xmlNode *rootEle,*node,*wptEle,*rteEle,*rteWpt,*trkEle,*trkWpt,*trkWptData;
    xmlAttr *attr;
    int creCheck=0;
    char* buff;
    file = xmlReadFile(fileName, NULL, 0);
    if (file == NULL) {
        deleteGPXdoc(doc);
        xmlFreeDoc(file);
        xmlCleanupParser();
        return NULL;
    }
    rootEle = xmlDocGetRootElement(file);
    //Save GPX name,version and creator
    if(strcmp((char*)rootEle->ns->href,"")!=0){
        strcpy(doc->namespace,(char*)rootEle->ns->href);
    }else{
        deleteGPXdoc(doc);
        xmlFreeDoc(file);
        xmlCleanupParser();
        return NULL;
    }
    doc->version = 0;
    for (attr = rootEle->properties; attr != NULL; attr = attr->next){
        xmlNode *value= attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        if(strcmp(attrName,"version")==0){
            if(cont!=NULL){
                doc->version = atof(cont);
            }else{
                deleteGPXdoc(doc);
                xmlFreeDoc(file);
                xmlCleanupParser();
                return NULL;
            }
        }
        if(strcmp(attrName,"creator")==0){
            if(cont!=NULL&&strcmp(cont,"")!=0){
                doc->creator = malloc(sizeof(char)*strlen(cont)+1);
                strcpy(doc->creator,cont);
                ++creCheck;
            }else{
                deleteGPXdoc(doc);
                xmlFreeDoc(file);
                xmlCleanupParser();
                return NULL;
            }
        }
    }
    node = rootEle->children;
    //RootEle children only to restart search
    List *waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
    List *routes = initializeList(&routeToString,&deleteRoute,&compareRoutes);
    List *tracks = initializeList(&trackToString,&deleteTrack,&compareTracks);
    while(node!=NULL &&node->next != NULL){
        if (node->type == XML_ELEMENT_NODE) {
            //Check if waypoint
            if(strcmp((char*)node->name,"wpt")==0){
                Waypoint *wpt = malloc(sizeof(Waypoint)+1);
                List* wptOtherData = initializeList(&gpxDataToString,&deleteGpxData,&compareGpxData);
                wpt->name = malloc(sizeof(char*)*strlen("")+1);
                strcpy(wpt->name,"");
                wpt->latitude = 0.0;
                wpt->longitude = 0.0;
                //Add waypoint children to
                for(attr=node->properties;attr!=NULL;attr=attr->next){
                    xmlNode *value= attr->children;
                    char *attrName = (char *)attr->name;
                    char *cont = (char *)(value->content);
                    if(strcmp(attrName,"lat")==0){
                        if(cont!=NULL){
                            wpt->latitude = strtod(cont,&buff);
                        }else{
                            deleteGPXdoc(doc);
                            xmlFreeDoc(file);
                            return NULL;
                        }
                    }
                    if(strcmp(attrName,"lon")==0){
                        if(cont!=NULL){
                            wpt->longitude = strtod(cont,&buff);
                        }else{
                            deleteGPXdoc(doc);
                            xmlFreeDoc(file);
                            xmlCleanupParser();
                            return NULL;
                        }
                    }
                }
                //Get remaining data and store in otherData
                wptEle = node->children;
                while(wptEle->next!=NULL){
                    if(wptEle->children!=NULL&&strcmp((char*)wptEle->name,"name")!=0&&strcmp((char*)wptEle->name,"text")!=0){
                        GPXData* d;
                        d = malloc(sizeof(GPXData)+strlen((char*)wptEle->children->content)+1);
                        if(wptEle->name!=NULL&&wptEle->children->content!=NULL){
                            if(strcmp((char *)wptEle->name,"")!=0&&strcmp((char *)wptEle->children->content,"")!=0){
                                strcpy(d->name,(char*)wptEle->name);
                                strcpy(d->value,(char*)wptEle->children->content);
                            }else{
                                deleteGPXdoc(doc);
                                xmlFreeDoc(file);
                                xmlCleanupParser();
                                return NULL;
                            }
                        }else{
                            deleteGPXdoc(doc);
                            xmlFreeDoc(file);
                            xmlCleanupParser();
                            return NULL;
                        }
                        insertBack(wptOtherData,d);
                    }
                    if(strcmp((char*)wptEle->name,"name")==0){
                        free(wpt->name);
                        wpt->name = malloc(sizeof(char*)*strlen((char*)wptEle->children->content)+1);
                        strcpy(wpt->name,(char*)wptEle->children->content);
                    }
                    wptEle = wptEle->next;
                }
                //Add waypoint to list
                wpt->otherData = wptOtherData;
                insertBack(waypoints,wpt);
            }
            //Check for route
            if(strcmp((char*)node->name,"rte")==0){
                Route *rte = malloc(sizeof(Route)+1);
                rte->name = malloc(sizeof(char*)*strlen("")+1);
                strcpy(rte->name,"");
                List* rteOtherData = initializeList(&gpxDataToString,&deleteGpxData,&compareGpxData);
                List* rteWaypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
                rteEle = node->children;
                while(rteEle->next!=NULL){
                    //Save other Data
                    if(strcmp((char*)rteEle->name,"name")!=0&&strcmp((char*)rteEle->name,"rtept")!=0&&strcmp((char*)rteEle->name,"text")!=0){
                        GPXData* rteGpxData;
                        rteGpxData = malloc(sizeof(GPXData)*strlen((char*)rteEle->children->content)+1);
                            if(rteEle->children->content!= NULL&&strcmp((char *)rteEle->children->content,"")!=0){
                                strcpy(rteGpxData->name,(char*)rteEle->name);
                                strcpy(rteGpxData->value,(char*)rteEle->children->content);
                            }else{
                                deleteGPXdoc(doc);
                                xmlFreeDoc(file);
                                xmlCleanupParser();
                                return NULL;
                            }
                        insertBack(rteOtherData,rteGpxData);
                    }
                    if(strcmp((char*)rteEle->name,"name")==0){
                        if(rteEle->children->content!=NULL){
                            free(rte->name);
                            rte->name = malloc(sizeof(char*)*strlen((char*)rteEle->children->content)+1);
                            strcpy(rte->name,(char*)rteEle->children->content);
                        }else{
                            deleteGPXdoc(doc);
                            xmlFreeDoc(file);
                            xmlCleanupParser();
                            return NULL;
                        }
                    }
                    if(strcmp((char*)rteEle->name,"rtept")==0){
                        rteWpt=rteEle->children;
                        Waypoint* rtept;
                        rtept = malloc(sizeof(Waypoint)+1);
                        List* rtptData = initializeList(&gpxDataToString,&deleteGpxData,&compareGpxData);
                        rtept->name = malloc(strlen("")+1);
                        strcpy(rtept->name,"");
                        for(attr=rteEle->properties;attr!=NULL;attr=attr->next){
                            xmlNode *value= attr->children;
                            char *attrName = (char *)attr->name;
                            char *cont = (char *)(value->content);
                            if(strcmp(attrName,"lat")==0){
                                if(cont!=NULL){
                                    rtept->latitude= strtod(cont,&buff);
                                }else{
                                    deleteGPXdoc(doc);
                                    xmlFreeDoc(file);
                                    xmlCleanupParser();
                                    return NULL;
                                }
                            }
                            if(strcmp(attrName,"lon")==0){
                                if(cont!=NULL){
                                    rtept->longitude = strtod(cont,&buff);
                                }else{
                                    deleteGPXdoc(doc);
                                    xmlFreeDoc(file);
                                    xmlCleanupParser();
                                    return NULL;
                                }
                            }
                        }
                        if(rteWpt!=NULL){
                            while(rteWpt->next!=NULL){
                                if(strcmp((char*)rteWpt->name,"text")!=0 && strcmp((char*)rteWpt->name,"name")!=0){
                                    GPXData* rteWptGpxData;
                                    rteWptGpxData = malloc(sizeof(GPXData)*strlen((char*)rteWpt->children->content)+1);
                                    if(rteWpt->children->content!= NULL&&strcmp((char *)rteWpt->children->content,"")!=0){
                                        strcpy(rteWptGpxData->name,(char*)rteWpt->name);
                                        strcpy(rteWptGpxData->value,(char*)rteWpt->children->content);
                                    }else{
                                        deleteGPXdoc(doc);
                                        xmlFreeDoc(file);
                                        xmlCleanupParser();
                                        return NULL;
                                    }
                                    insertBack(rtptData,rteWptGpxData);
                                }
                                if(strcmp((char*)rteWpt->name,"name")==0){
                                    if(rteWpt->children->content!=NULL){
                                        free(rtept->name);
                                        rtept->name = malloc(sizeof(char*)*strlen((char*)rteWpt->children->content)+1);
                                        strcpy(rtept->name,(char*)rteWpt->children->content);
                                    }else{
                                        deleteGPXdoc(doc);
                                        xmlFreeDoc(file);
                                        xmlCleanupParser();
                                        return NULL;
                                    }
                                }
                                rteWpt=rteWpt->next;
                            }
                        }
                        rtept->otherData=rtptData;
                        insertBack(rteWaypoints,rtept);
                    }
                    rteEle =rteEle->next;
                }
                rte->waypoints=rteWaypoints;
                rte->otherData=rteOtherData;
                insertBack(routes,rte);
            }
            //Check for track
            if(strcmp((char*)node->name,"trk")==0){
                Track* trk = malloc(sizeof(Track)+1);
                trk->name = malloc(sizeof(char*)*strlen((char*)node->children->next->children->content)+1);
                strcpy(trk->name,(char*)node->children->next->children->content);
                List* trkOtherData = initializeList(&gpxDataToString,&deleteGpxData,&compareGpxData);
                List* trkSeg = initializeList(&trackSegmentToString,&deleteTrackSegment,&compareTrackSegments);
                trkEle = node->children;
                while(trkEle->next!=NULL){
                    if(strcmp((char*)trkEle->name,"name")!=0&&strcmp((char*)trkEle->name,"trkseg")!=0&&strcmp((char*)trkEle->name,"text")!=0){
                        GPXData* trkGpxData;
                        trkGpxData = malloc(sizeof(GPXData)*strlen((char*)trkEle->children->content)+1);
                        if(trkEle->children->content!= NULL&&strcmp((char *)trkEle->children->content,"")!=0){
                            strcpy(trkGpxData->name,(char*)trkEle->name);
                            strcpy(trkGpxData->value,(char*)trkEle->children->content);
                        }else{
                            deleteGPXdoc(doc);
                            xmlFreeDoc(file);
                            xmlCleanupParser();
                            return NULL;
                        }
                        insertBack(trkOtherData,trkGpxData);
                    }
                    if(strcmp((char*)trkEle->name,"trkseg")==0){
                        TrackSegment* ts;
                        ts = malloc(sizeof(TrackSegment)+1);
                        List* segwpt = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
                        trkWpt = trkEle->children;
                        while(trkWpt->next!=NULL){
                            if(strcmp((char*)trkWpt->name,"trkpt")==0){
                                Waypoint* trkpt;
                                List* trkptOtherData = initializeList(&gpxDataToString,&deleteGpxData,&compareGpxData);
                                trkpt = malloc(sizeof(Waypoint)+1);
                                trkpt->name = malloc(strlen("")*sizeof(char*)+1);
                                strcpy(trkpt->name,"");
                                for(attr=trkWpt->properties;attr!=NULL;attr=attr->next){
                                    xmlNode *value= attr->children;
                                    char *attrName = (char *)attr->name;
                                    char *cont = (char *)(value->content);
                                    if(strcmp(attrName,"lat")==0){
                                        if(cont!=NULL){
                                            trkpt->latitude= strtod(cont,&buff);
                                        }else{
                                            deleteGPXdoc(doc);
                                            xmlFreeDoc(file);
                                            xmlCleanupParser();
                                            return NULL;
                                        }
                                    }
                                    if(strcmp(attrName,"lon")==0){
                                        if(cont!=NULL){
                                            trkpt->longitude = strtod(cont,&buff);
                                        }else{
                                            deleteGPXdoc(doc);
                                            xmlFreeDoc(file);
                                            xmlCleanupParser();
                                            return NULL;
                                        }
                                    }
                                }
                                trkWptData=trkWpt->children;
                                if(trkWptData!=NULL){
                                    while(trkWptData->next!=NULL){
                                        if(strcmp((char*)trkWptData->name,"name")==0){
                                            if((char*)trkWptData->children->content!=NULL){
                                                free(trkpt->name);
                                                trkpt->name = malloc(sizeof(char*)*strlen((char*)trkWptData->children->content)+1);
                                                strcpy(trkpt->name,(char*)trkWptData->children->content);
                                            }else{
                                                deleteGPXdoc(doc);
                                                xmlFreeDoc(file);
                                                xmlCleanupParser();
                                                return NULL;
                                            }
                                        }
                                        if(strcmp((char*)trkWptData->name,"name")!=0&&strcmp((char*)trkWptData->name,"text")!=0){
                                            GPXData* trkptOD;
                                            if((char*)trkWptData->children->content!=NULL&&strcmp((char*)trkWptData->children->content,"")!=0){
                                                trkptOD = malloc(sizeof(GPXData)*strlen((char*)trkWptData->children->content)+1);
                                                strcpy(trkptOD->name,(char*)trkWptData->name);
                                                strcpy(trkptOD->value,(char*)trkWptData->children->content);
                                            }else{
                                                deleteGPXdoc(doc);
                                                xmlFreeDoc(file);
                                                xmlCleanupParser();
                                                return NULL;
                                            }
                                            insertBack(trkptOtherData,trkptOD);
                                        }
                                        trkWptData=trkWptData->next;
                                    }
                                }
                                trkpt->otherData=trkptOtherData;
                                insertBack(segwpt,trkpt);
                            }
                            trkWpt=trkWpt->next;
                        }
                        ts->waypoints=segwpt;
                        insertBack(trkSeg,ts);
                    }
                    trkEle=trkEle->next;
                }
                trk->otherData=trkOtherData;
                trk->segments=trkSeg;
                insertBack(tracks,trk);
            }
            //Go to next node
            node=node->next;
        }else{
            node = node->next;
        }
    }
    doc->routes = routes;
    doc->waypoints = waypoints;
    doc->tracks = tracks;
    //Terminate parser;
    xmlFreeDoc(file);
    xmlCleanupParser();
    return doc;
}
//Writes doc to file
bool writeGPXdoc(GPXdoc* doc, char* fileName){
    if(doc == NULL||fileName == NULL){
        return false;
    }
    xmlDocPtr d;
    d = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr rnode;
    xmlNodePtr wptnode;
    xmlNodePtr rtenode;
    xmlNodePtr trknode;
    xmlNodePtr segnode;
    //Make root
    rnode = xmlNewNode(NULL,BAD_CAST "gpx");
    char *ver = malloc(sizeof(ver)*10);
    snprintf(ver, 10, "%.1f", doc->version);
    xmlNewProp(rnode,BAD_CAST "version",BAD_CAST ver);
    free(ver);
    xmlNewProp(rnode,BAD_CAST "creator",BAD_CAST doc->creator);
    xmlSetNs(rnode,xmlNewNs(rnode,BAD_CAST doc->namespace,NULL));
    xmlDocSetRootElement(d, rnode);
    //xmlSaveFormatFileEnc("file.gpx", d, "UTF-8", 1);
    ListIterator wptInt;
    wptInt = createIterator(doc->waypoints);
    while(wptInt.current!=NULL){
        //Get all wpt info
        Waypoint *w = (Waypoint*)wptInt.current->data;
        wptnode = xmlNewChild(rnode,NULL,BAD_CAST "wpt",NULL);
        char *lat = malloc(sizeof(lat)*10);
        snprintf(lat, 10, "%.5f", w->latitude);
        xmlNewProp(wptnode,BAD_CAST "lat",BAD_CAST lat);
        char *lon = malloc(sizeof(lon)*10);
        snprintf(lon, 10, "%.5f", w->longitude);
        xmlNewProp(wptnode,BAD_CAST "lon",BAD_CAST lon);
        free(lat);
        free(lon);
        //Add all other data name is type value is the acutal data
        ListIterator wptdata;
        wptdata = createIterator(w->otherData);
        xmlNewChild(wptnode,NULL,BAD_CAST "name",BAD_CAST w->name);
        while(wptdata.current!=NULL){
            GPXData *od = (GPXData*)wptdata.current->data;
            xmlNewChild(wptnode,NULL,BAD_CAST od->name,BAD_CAST od->value);
            nextElement(&wptdata);
        }
        nextElement(&wptInt);
    }
    ListIterator rteInt;
    rteInt = createIterator(doc->routes);
    while(rteInt.current!=NULL){
        Route *r = (Route*)rteInt.current->data;
        rtenode = xmlNewChild(rnode,NULL,BAD_CAST "rte",NULL);
        if(r->name!=NULL&&strcmp(r->name,"")!=0){
            xmlNewChild(rtenode,NULL,BAD_CAST "name",BAD_CAST r->name);
        }
        ListIterator rtedata;
        rtedata = createIterator(r->otherData);
        while(rtedata.current!=NULL){
            GPXData *od = (GPXData*)rtedata.current->data;
            xmlNewChild(rtenode,NULL,BAD_CAST od->name,BAD_CAST od->value);
            nextElement(&rtedata);
        }
        ListIterator rteptdata;
        rteptdata = createIterator(r->waypoints);
        while(rteptdata.current!=NULL){
            Waypoint *w = (Waypoint*)rteptdata.current->data;
            wptnode = xmlNewChild(rtenode,NULL,BAD_CAST "rtept",NULL);
            char *lat = malloc(sizeof(lat)*10);
            snprintf(lat, 10, "%.5f", w->latitude);
            xmlNewProp(wptnode,BAD_CAST "lat",BAD_CAST lat);
            char *lon = malloc(sizeof(lon)*10);
            snprintf(lon, 10, "%.5f", w->longitude);
            xmlNewProp(wptnode,BAD_CAST "lon",BAD_CAST lon);
            free(lat);
            free(lon);
            //Add all other data name is type value is the acutal data
            ListIterator wptdata;
            wptdata = createIterator(w->otherData);
            if(w->name!=NULL&&strcmp(w->name,"")!=0){
                xmlNewChild(wptnode,NULL,BAD_CAST "name",BAD_CAST w->name);
            }
            while(wptdata.current!=NULL){
                GPXData *od = (GPXData*)wptdata.current->data;
                xmlNewChild(wptnode,NULL,BAD_CAST od->name,BAD_CAST od->value);
                nextElement(&wptdata);
            }
            nextElement(&rteptdata);
        }
        nextElement(&rteInt);
    }
    ListIterator trkInt;
    trkInt = createIterator(doc->tracks);
    while(trkInt.current!=NULL){
        Track *t = (Track*) trkInt.current->data;
        trknode = xmlNewChild(rnode,NULL,BAD_CAST "trk",NULL);
        if(t->name!=NULL&&strcmp(t->name,"")!=0){
            xmlNewChild(trknode,NULL,BAD_CAST "name",BAD_CAST t->name);
        }
        ListIterator trkdata;
        trkdata = createIterator(t->otherData);
        while(trkdata.current!=NULL){
                GPXData *od = (GPXData*)trkdata.current->data;
                xmlNewChild(trknode,NULL,BAD_CAST od->name,BAD_CAST od->value);
                nextElement(&trkdata);
            }
        ListIterator trkseg;
        trkseg = createIterator(t->segments);
        while(trkseg.current!=NULL){
            TrackSegment *s = (TrackSegment*)trkseg.current->data;
            segnode = xmlNewChild(trknode,NULL,BAD_CAST "trkseg",NULL);
            ListIterator trkpt;
            trkpt = createIterator(s->waypoints);
            while(trkpt.current!=NULL){
                Waypoint *w = (Waypoint*)trkpt.current->data;
                wptnode = xmlNewChild(segnode,NULL,BAD_CAST "trkpt",NULL);
                char *lat = malloc(sizeof(lat)*10);
                snprintf(lat, 10, "%.5f", w->latitude);
                xmlNewProp(wptnode,BAD_CAST "lat",BAD_CAST lat);
                char *lon = malloc(sizeof(lon)*10);
                snprintf(lon, 10, "%.5f", w->longitude);
                xmlNewProp(wptnode,BAD_CAST "lon",BAD_CAST lon);
                free(lat);
                free(lon);
                //Add all other data name is type value is the acutal data
                ListIterator wptdata;
                wptdata = createIterator(w->otherData);
                if(w->name!=NULL&&strcmp(w->name,"")!=0){
                    xmlNewChild(wptnode,NULL,BAD_CAST "name",BAD_CAST w->name);
                }
                while(wptdata.current!=NULL){
                    GPXData *od = (GPXData*)wptdata.current->data;
                    xmlNewChild(wptnode,NULL,BAD_CAST od->name,BAD_CAST od->value);
                    nextElement(&wptdata);
                }
                nextElement(&trkpt);
            }
            nextElement(&trkseg);
        }
        nextElement(&trkInt);
    }
    //Close loop
    if(doc==NULL){
        return false;
        xmlFreeDoc(d);
        xmlCleanupParser();
    }else{
        int check = xmlSaveFormatFileEnc(fileName, d, "UTF-8", 1);
        xmlFreeDoc(d);
        xmlCleanupParser();
        if(check == -1){
            return false;
        }
        return true;
    }
}
//Deletes gpx doc
void deleteGPXdoc(GPXdoc* doc){ 
    if(doc!=NULL){
        if(doc->creator!=NULL){
            free(doc->creator);
        }
        freeList(doc->waypoints);
        freeList(doc->routes);
        freeList(doc->tracks);
        free(doc);
    }else{
        return;
    }
}
//To string
char* GPXdocToString(GPXdoc* doc){
    if(doc==NULL){
        return "";
    }
    char *str,*temp;
    str = malloc(strlen(doc->namespace)+strlen(doc->creator)+strlen(toString(doc->waypoints))+strlen(toString(doc->routes))+strlen(toString(doc->tracks))+100);
    temp = malloc(sizeof(char*)*100);
    strcpy(str,"Name: ");
    strcat(str,doc->namespace);
    strcat(str,"\n");
    sprintf(temp, "Version: %.1f\n", doc->version);
    strcat(str,temp);
    strcat(str,"Creator: ");
    strcat(str,doc->creator);
    strcat(str,"\n");
    free(temp);
    if(getLength(doc->waypoints)!= 0){
       strcat(str,toString(doc->waypoints));
    }
    if(getLength(doc->routes)!= 0){
       strcat(str,toString(doc->routes));
    }
    if(getLength(doc->tracks)!= 0){
       strcat(str,toString(doc->tracks));
    }
    return str;
}
//Get num waypoints
int getNumWaypoints(const GPXdoc* doc){
    if(doc == NULL){
        return 0;
    }
    return getLength(doc->waypoints);
}
//Get num routes
int getNumRoutes(const GPXdoc* doc){
    if(doc == NULL){
        return 0;
    }
    return getLength(doc->routes);
}
//Get num tracks
int getNumTracks(const GPXdoc* doc){
    if(doc == NULL){
        return 0;
    }
    return getLength(doc->tracks);
}
//Get num segments
int getNumSegments(const GPXdoc* doc){
    int length = 0;
    if(doc == NULL){
        return length;
    }
    ListIterator iter = createIterator(doc->tracks);
    Track* trk;
    while(iter.current!=NULL){
        trk =(Track*)iter.current->data;
        length = length + getLength(trk->segments);
        nextElement(&iter);
    }
    return length;
}
//Get num data
int getNumGPXData(const GPXdoc* doc){
    int length = 0;
    if(doc == NULL){
        return length;
    }
    ListIterator iterRte = createIterator(doc->routes);
    ListIterator iterTrk = createIterator(doc->tracks);
    ListIterator iterWpt = createIterator(doc->waypoints);
    ListIterator iterRteWpt;
    ListIterator iterSegWpt;
    Waypoint* wpt;
    Route* rte;
    Track* trk;
    TrackSegment* seg;
    while(iterWpt.current!=NULL){
        wpt =(Waypoint*)iterWpt.current->data;
        if(strcmp(wpt->name,"")!=0){
            ++length;
        }
        length = length + getLength(wpt->otherData);
        nextElement(&iterWpt);
    }
    while(iterRte.current!=NULL){
        rte =(Route*)iterRte.current->data;
        if(strcmp(rte->name,"")!=0){
            ++length;
        }
        length = length + getLength(rte->otherData);
        iterRteWpt = createIterator(rte->waypoints);
        while(iterRteWpt.current!=NULL){
            wpt = (Waypoint*)iterRteWpt.current->data;
            if(strcmp(wpt->name,"")!=0){
                ++length;
            }
            length = length + getLength(wpt->otherData);
            nextElement(&iterRteWpt);
        }
        nextElement(&iterRte);
    }
    while(iterTrk.current!=NULL){
        trk =(Track*)iterTrk.current->data;
        if(strcmp(trk->name,"")!=0){
            ++length;
        }
        length = length + getLength(trk->otherData);
        ListIterator iterSeg = createIterator(trk->segments);
        while(iterSeg.current!=NULL){
            seg =(TrackSegment*)iterSeg.current->data;
            iterSegWpt = createIterator(seg->waypoints);
            while(iterSegWpt.current!=NULL){
                wpt = (Waypoint*)iterSegWpt.current->data;
                if(strcmp(wpt->name,"")!=0){
                    ++length;
                }
                length = length + getLength(wpt->otherData);
                nextElement(&iterSegWpt);
            }
            nextElement(&iterSeg);
        }
        nextElement(&iterTrk);
    }
    return length;
}
//Get waypoint
Waypoint* getWaypoint(const GPXdoc* doc, char* name){
    if(doc == NULL||name == NULL){
        return NULL;
    }
    Waypoint* wpt;
    Track* trk;
    Route* rte;
    TrackSegment* seg;
    ListIterator iterWpt = createIterator(doc->waypoints);
    ListIterator iterRte = createIterator(doc->routes);
    ListIterator iterTrk = createIterator(doc->tracks);
    while(iterWpt.current!=NULL){
        wpt = (Waypoint*)iterWpt.current->data;
        if(strcmp(wpt->name,name)==0){
            return wpt;
        }
        nextElement(&iterWpt);
    }
    while(iterRte.current!=NULL){
        rte =(Route*)iterRte.current->data;
        ListIterator iterRteWpt = createIterator(rte->waypoints);
        while(iterRteWpt.current!=NULL){
            wpt = (Waypoint*)iterRteWpt.current->data;
            if(strcmp(wpt->name,name)==0){
                return wpt;
            }
            nextElement(&iterRteWpt);
        }
        nextElement(&iterRte);
    }
    while(iterTrk.current!=NULL){
        trk =(Track*)iterTrk.current->data;
        ListIterator iterSeg = createIterator(trk->segments);
        while(iterSeg.current!=NULL){
            seg =(TrackSegment*)iterSeg.current->data;
            ListIterator iterSegWpt = createIterator(seg->waypoints);
            while(iterSegWpt.current!=NULL){
                wpt = (Waypoint*)iterSegWpt.current->data;
                if(strcmp(wpt->name,name)==0){
                    return wpt;
                }
                nextElement(&iterSegWpt);
            }
            nextElement(&iterSeg);
        }
        nextElement(&iterTrk);
    }
    return NULL;
}
//Get track
Track* getTrack(const GPXdoc* doc, char* name){
    if(doc == NULL||name == NULL){
        return NULL;
    }
    Track* trk;
    ListIterator iterTrk = createIterator(doc->tracks);
    while(iterTrk.current!=NULL){
        trk = (Track*)iterTrk.current->data;
        if(strcmp(trk->name,name)==0){
            return trk;
        }
        nextElement(&iterTrk);
    }
    return NULL;
}
//Get route
Route* getRoute(const GPXdoc* doc, char* name){
    if(doc == NULL||name == NULL){
        return NULL;
    }
    Route* rte;
    ListIterator iterRte = createIterator(doc->routes);
    while(iterRte.current!=NULL){
        rte = (Route*)iterRte.current->data;
        if(strcmp(rte->name,name)==0){
            return rte;
        }
        nextElement(&iterRte);
    }
    return NULL;
}
//A2 Helper and bonus
//Rounds to nearest 10th
float round10(float len){
    if(len ==0){
        return 0;
    }
    int l = (int)len;
    int num = l % 10;
    int temp = l/10;
    if(num>=5){
        return temp*10+10;
    }else{
        return temp*10;
    }
}
//Gets length of route
float getRouteLen(const Route *rt){
    if(rt ==NULL){
        return 0;
    }
    double dist=0;
    double totalDist=0;
    if(getLength(rt->waypoints)<2){
        return 0;
    }
    ListIterator rtept1 = createIterator(rt->waypoints);
    ListIterator rtept2 = createIterator(rt->waypoints);
    nextElement(&rtept2);
    while(rtept1.current!=NULL){
        Waypoint *p1 = (Waypoint*)rtept1.current->data;
        if(rtept2.current!=NULL){
            Waypoint *p2 = (Waypoint*)rtept2.current->data;
            double dx, dy, dz;
            double p1lon = p1->longitude- p2->longitude;
            p1lon = p1lon*M_PI/180.0;
            dz = sin(p1->latitude*M_PI/180.0) - sin(p2->latitude*M_PI/180.0);
	        dx = cos(p1lon) * cos(p1->latitude*M_PI/180.0) - cos(p2->latitude*M_PI/180.0);
	        dy = sin(p1lon) * cos(p1->latitude*M_PI/180.0);
	        dist = asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * 6371;
            totalDist=dist+totalDist;
            nextElement(&rtept2);
            nextElement(&rtept1);
        }else{
            return totalDist*1000;
        }
    }
    return totalDist*1000;
}
//Gets length of track
float getTrackLen(const Track *tr){
    Waypoint *p1;
    Waypoint *p2;
    if(tr == NULL){
        return 0;
    }
    double dist=0;
    double totalDist=0;
    int wayLen = 0;
    int check = 0;
    ListIterator trkseg = createIterator(tr->segments);
    ListIterator temp = trkseg;
    while(trkseg.current!=NULL){
        TrackSegment *s = (TrackSegment*)trkseg.current->data;
        wayLen = wayLen+getLength(s->waypoints);
        nextElement(&trkseg);
    }
    if(wayLen<2){
        return 0;
    }
    trkseg = temp;
    while(trkseg.current!=NULL){
        TrackSegment *s = (TrackSegment*)trkseg.current->data;
        ListIterator rtept1 = createIterator(s->waypoints);
        ListIterator rtept2 = createIterator(s->waypoints);
        if(check ==1&&rtept1.current!=NULL){
            p2 = (Waypoint*)rtept1.current->data;
            double dx, dy, dz;
            double p1lon = p1->longitude- p2->longitude;
            p1lon = p1lon*M_PI/180.0;
            dz = sin(p1->latitude*M_PI/180.0) - sin(p2->latitude*M_PI/180.0);
	        dx = cos(p1lon) * cos(p1->latitude*M_PI/180.0) - cos(p2->latitude*M_PI/180.0);
	        dy = sin(p1lon) * cos(p1->latitude*M_PI/180.0);
	        dist = asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * 6371;
            totalDist=dist+totalDist;
        }
        nextElement(&rtept2);
        while(rtept1.current!=NULL){
            p1 = (Waypoint*)rtept1.current->data;
            if(rtept2.current!=NULL){
                p2 = (Waypoint*)rtept2.current->data;
                double dx, dy, dz;
                double p1lon = p1->longitude- p2->longitude;
                p1lon = p1lon*M_PI/180.0;
                dz = sin(p1->latitude*M_PI/180.0) - sin(p2->latitude*M_PI/180.0);
	            dx = cos(p1lon) * cos(p1->latitude*M_PI/180.0) - cos(p2->latitude*M_PI/180.0);
	            dy = sin(p1lon) * cos(p1->latitude*M_PI/180.0);
	            dist = asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * 6371;
                totalDist=dist+totalDist;
                nextElement(&rtept2);
                nextElement(&rtept1);
            }else{
                nextElement(&rtept1);
            }
        }
        nextElement(&trkseg);
        check = 1;
    }
    return totalDist*1000;
}
//Gets num routes with length
int numRoutesWithLength(const GPXdoc*doc,float len,float delta){
    if(doc==NULL||len<0||delta<0){
        return 0;
    }
    int count=0;
    float dif = 0;
    ListIterator rte = createIterator(doc->routes);
    while(rte.current!=NULL){
        Route *r = (Route*)rte.current->data;
        dif = len-getRouteLen(r);
        if(dif<0){
            dif = dif*-1.0;
        }
        dif = round10(dif);
        if(dif<delta){
            ++count;
        }
        nextElement(&rte);
    }
    return count;
}
//Gets num routes with length
int numTracksWithLength(const GPXdoc* doc, float len, float delta){
    if(doc==NULL||len<0||delta<0){
        return 0;
    }
    int count=0;
    float dif = 0;
    ListIterator trk = createIterator(doc->tracks);
    while(trk.current!=NULL){
        Track *t = (Track*) trk.current->data;
        dif = len-getTrackLen(t);
        if(dif<0){
            dif = dif*-1.0;
        }
        dif = round10(dif);
        if(dif<delta){
            ++count;
        }
        nextElement(&trk);
    }
    return count;
}
//Checks for loop
bool isLoopRoute(const Route* route, float delta){
    if(route==NULL||delta<0||getLength(route->waypoints)<4){
        return false;
    }
    ListIterator wpt = createIterator(route->waypoints);
    Waypoint *p1;
    Waypoint *p2;
    int check = 0;
    float dist;
    while(wpt.current!=NULL){
        if(check==0){
            p1 = (Waypoint*)wpt.current->data;
            check =1;
        }
        if(wpt.current->next==NULL){
            p2 = (Waypoint*)wpt.current->data;
        }
        nextElement(&wpt);
    }
    double dx, dy, dz;
    double p1lon = p1->longitude- p2->longitude;
    p1lon = p1lon*M_PI/180.0;
    dz = sin(p1->latitude*M_PI/180.0) - sin(p2->latitude*M_PI/180.0);
	dx = cos(p1lon) * cos(p1->latitude*M_PI/180.0) - cos(p2->latitude*M_PI/180.0);
	dy = sin(p1lon) * cos(p1->latitude*M_PI/180.0);
	dist = asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * 6371*100;
    if(dist<0){
        dist = dist*-1.0;
    }
    if(dist<=delta){
        return true;
    }
    return false;
}
//Checks for track
bool isLoopTrack(const Track *tr, float delta){
    if(tr==NULL||delta<0){
        return false;
    }
    float dist;
    Waypoint *p1;
    Waypoint *p2;
    ListIterator trkseg = createIterator(tr->segments);
    ListIterator temp = trkseg;
    int check = 0;
    int wptlength=0;
    while(trkseg.current!=NULL){
        TrackSegment* ts = (TrackSegment*)trkseg.current->data;
        wptlength = wptlength+getLength(ts->waypoints);
        nextElement(&trkseg);
    }
    if(wptlength<4){
        return false;
    }
    trkseg = temp;
    while(trkseg.current!=NULL){
        TrackSegment* ts = (TrackSegment*)trkseg.current->data;
        ListIterator trkpt = createIterator(ts->waypoints);
        if(check==0){
            p1 = (Waypoint*)trkpt.current->data;
            //printf("%f\n",p1->latitude);
            check =1;
        }
        if(trkseg.current->next!=NULL){
            nextElement(&trkseg);
        }else{
            while(trkpt.current!=NULL){
                if(trkpt.current->next==NULL){
                    p2 = (Waypoint*)trkpt.current->data;
                    //printf("%f\n",p2->latitude);
                }
                nextElement(&trkpt);
            }
            nextElement(&trkseg);
        }
    }
    double dx, dy, dz;
    double p1lon = p1->longitude- p2->longitude;
    p1lon = p1lon*M_PI/180.0;
    dz = sin(p1->latitude*M_PI/180.0) - sin(p2->latitude*M_PI/180.0);
	dx = cos(p1lon) * cos(p1->latitude*M_PI/180.0) - cos(p2->latitude*M_PI/180.0);
	dy = sin(p1lon) * cos(p1->latitude*M_PI/180.0);
	dist = asin(sqrt(dx * dx + dy * dy + dz * dz) / 2.0) * 2.0 * 6371.0*100.0;
    if(dist<0){
        dist = dist*-1.0;
    }
    if(dist<=delta){
        return true;
    }
    return false;
}
//List of routes between two points
List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){
    if(doc==NULL){
        return NULL;
    }
    ListIterator rte = createIterator(doc->routes);
    //Add custom function to not delete doc
    List* routeList = initializeList(&routeToString,&deleteRouteDum,&compareRoutes);
    while(rte.current!=NULL){
        Route *r = (Route*)rte.current->data;
        ListIterator wpt = createIterator(r->waypoints);
        Waypoint *p1;
        Waypoint *p2;
        int check = 0;
        float dist;
        while(wpt.current!=NULL){
            if(check==0){
                p1 = (Waypoint*)wpt.current->data;
                check =1;
            }
            if(wpt.current->next==NULL){
                p2 = (Waypoint*)wpt.current->data;
            }
            nextElement(&wpt);
        }
        double dx, dy, dz;
        double p1lon = p1->longitude-sourceLong;
        p1lon = p1lon*M_PI/180.0;
        dz = sin(p1->latitude*M_PI/180.0) - sin(sourceLat*M_PI/180.0);
	    dx = cos(p1lon) * cos(p1->latitude*M_PI/180.0) - cos(sourceLat*M_PI/180.0);
	    dy = sin(p1lon) * cos(p1->latitude*M_PI/180.0);
	    dist = asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * 6371*100;
        if(dist<delta){
            double dx, dy, dz;
            double p1lon = p2->longitude-destLong;
            p1lon = p1lon*M_PI/180.0;
            dz = sin(p2->latitude*M_PI/180.0) - sin(destLat*M_PI/180.0);
	        dx = cos(p1lon) * cos(p2->latitude*M_PI/180.0) - cos(destLat*M_PI/180.0);
	        dy = sin(p1lon) * cos(p2->latitude*M_PI/180.0);
	        dist = asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * 6371*100;
            if(dist<delta){
                insertBack(routeList,r);
            }
        }
        nextElement(&rte);
    }
    if(getLength(routeList)==0){
        freeList(routeList);
        return NULL;
    }
    return routeList;
}
//List of tracks between two points
List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){
    if(doc==NULL){
        return NULL;
    }
    float dist;
    Waypoint *p1;
    Waypoint *p2;
    List* trackList = initializeList(&trackToString,&deleteTrackDum,&compareTracks);
    ListIterator trk = createIterator(doc->tracks);
    while(trk.current!=NULL){
        Track *t = (Track*)trk.current->data;
        ListIterator trkseg = createIterator(t->segments);
        int check = 0;
        while(trkseg.current!=NULL){
            TrackSegment* ts = (TrackSegment*)trkseg.current->data;
            ListIterator trkpt = createIterator(ts->waypoints);
            if(trkpt.current==NULL){
                return NULL;
            }
            if(check==0){
                p1 = (Waypoint*)trkpt.current->data;
                check =1;
            }
            if(trkseg.current->next!=NULL){
                nextElement(&trkseg);
            }else{
                while(trkpt.current!=NULL){
                    if(trkpt.current->next==NULL){
                        p2 = (Waypoint*)trkpt.current->data;
                        break;
                    }
                nextElement(&trkpt);
                }
            nextElement(&trkseg);
            }
        }
        double dx, dy, dz;
        double p1lon = p1->longitude-sourceLong;
        p1lon = p1lon*M_PI/180.0;
        dz = sin(p1->latitude*M_PI/180.0) - sin(sourceLat*M_PI/180.0);
	    dx = cos(p1lon) * cos(p1->latitude*M_PI/180.0) - cos(sourceLat*M_PI/180.0);
	    dy = sin(p1lon) * cos(p1->latitude*M_PI/180.0);
	    dist = asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * 6371*100;
        if(dist<delta){
            double dx, dy, dz;
            double p1lon = p2->longitude-destLong;
            p1lon = p1lon*M_PI/180.0;
            dz = sin(p2->latitude*M_PI/180.0) - sin(destLat*M_PI/180.0);
	        dx = cos(p1lon) * cos(p2->latitude*M_PI/180.0) - cos(destLat*M_PI/180.0);
	        dy = sin(p1lon) * cos(p2->latitude*M_PI/180.0);
	        dist = asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * 6371*100;
            if(dist<delta){
                insertBack(trackList,t);
            }
        }
        nextElement(&trk);
    }
    if(getLength(trackList)==0){
        freeList(trackList);
        return NULL;
    }
    return trackList;
}
char * waypointToJSON(const Waypoint *wpt){
    if(wpt==NULL){
        return "{}";
    }
    char* string = malloc(sizeof(char)*100000);
    strcpy(string,"{\"name\":");
    if(strcmp(wpt->name,"")==0||wpt->name==NULL){
        strcat(string,"\"None\",");
    }else{
        strcat(string,"\"");
        strcat(string,wpt->name);
        strcat(string,"\",");
    }
    strcat(string,"\"lat\":");
    char *temp = malloc(10000);
    sprintf(temp,"%lf",wpt->latitude);
    strcat(string,temp);
    free(temp);
    strcat(string,",\"lon\":");
    temp = malloc(10000);
    sprintf(temp,"%lf",wpt->longitude);
    strcat(string,temp);
    free(temp);
    strcat(string,"}");
    return string;
}
char * waypointListToJSON(const List *list){
    if(list==NULL||getLength((List*)list)==0){
        return "[]";
    }
    char *string = malloc(1000000);
    strcpy(string,"[");
    ListIterator wpts = createIterator((List*)list);
    while(wpts.current!=NULL){
        Waypoint *wpt = (Waypoint*)wpts.current->data;
        char *str = waypointToJSON(wpt);
        strcat(string,str);
        if(wpts.current->next!=NULL){
            strcat(string,",");
        }
        nextElement(&wpts);
    }
    strcat(string,"]");
    return string;
}
char* routeToJSON(const Route *rt){
    if(rt==NULL){
        return "{}";
    }
    char* string = malloc(sizeof(char)*100000);
    strcpy(string,"{\"name\":");
    if(strcmp(rt->name,"")==0||rt->name==NULL){
        strcat(string,"\"None\",");
    }else{
        strcat(string,"\"");
        strcat(string,rt->name);
        strcat(string,"\",");
    }
    strcat(string,"\"numPoints\":");
    char* temp = malloc(100);
    sprintf(temp,"%d",getLength(rt->waypoints));
    strcat(string,temp);
    free(temp);
    strcat(string,",\"len\":");
    temp = malloc(100);
    sprintf(temp,"%.1f",round10(getRouteLen(rt)));
    strcat(string,temp);
    free(temp);
    strcat(string,",\"loop\":");
    if(isLoopRoute(rt,10)){
        strcat(string,"true}");
    }else{
        strcat(string,"false}");
    }
    return string;
    
}
char *otherDataToJSON(const GPXData *d){
    if(d==NULL){
        return "{}";
    }
    char* string = malloc(sizeof(char)*100000);
    strcpy(string,"{\"eleName\":\"");
    strcat(string,strtok((char*)d->name,"\n"));
    strcat(string,"\",\"value\":\"");
    strcat(string,strtok((char*)d->value,"\n"));
    strcat(string,"\"}");
    return string;
}
char* trackToJSON(const Track *tr){
    char *temp;
    if(tr==NULL){
        return "{}";
    }
    char* string = malloc(sizeof(char)*100000);
    strcpy(string,"{\"name\":");
    if(strcmp(tr->name,"")==0||tr->name==NULL||strcmp(tr->name,"\n")==0||strcmp(tr->name,"\t")==0||strcmp(tr->name,"\n\t")==0){
        strcat(string,"\"None\",");
    }else{
        strcat(string,"\"");
        strcat(string,tr->name);
        strcat(string,"\",");
    }
    strcat(string,"\"points\":");
    temp = malloc(100);
    sprintf(temp,"%d",getNumWaypointsTrack((Track*)tr));
    strcat(string,temp);
    free(temp);
    strcat(string,",\"len\":");
    temp = malloc(100);
    sprintf(temp,"%.1f",round10(getTrackLen(tr)));
    strcat(string,temp);
    free(temp);
    strcat(string,",\"loop\":");
    if(isLoopTrack(tr,10)){
        strcat(string,"true}");
    }else{
        strcat(string,"false}");
    }
    return string;
}
char* routeListToJSON(const List *list){
    if(list==NULL||getLength((List*)list)==0){
        return "[]";
    }
    char *string = malloc(1000000);
    strcpy(string,"[");
    ListIterator rts = createIterator((List*)list);
    while(rts.current!=NULL){
        Route *r = (Route*)rts.current->data;
        char *str = routeToJSON(r);
        strcat(string,str);
        free(str);
        if(rts.current->next!=NULL){
            strcat(string,",");
        }
        nextElement(&rts);
    }
    strcat(string,"]");
    return string;
}
char* trackListToJSON(const List *list){
    if(list==NULL||getLength((List*)list)==0){
        return "[]";
    }
    char *string = malloc(1000000);
    strcpy(string,"[");
    ListIterator trks = createIterator((List*)list);
    while(trks.current!=NULL){
        Track *t = (Track*)trks.current->data;
        char *str = trackToJSON(t);
        strcat(string,str);
        free(str);
        if(trks.current->next!=NULL){
            strcat(string,",");
        }
        nextElement(&trks);
    }
    strcat(string,"]");
    return string;
}
char* GPXtoJSON(const GPXdoc* gpx){
    if(gpx==NULL){
        return "{}";
    }
    char *string = malloc(1000000);
    strcpy(string,"{\"version\":");
    char* temp = malloc(100);
    sprintf(temp,"%.1f",gpx->version);
    strcat(string,temp);
    free(temp);
    strcat(string,",\"creator\":\"");
    strcat(string,gpx->creator);
    strcat(string,"\",\"numWaypoints\":");
    temp = malloc(100);
    sprintf(temp,"%d",getLength(gpx->waypoints));
    strcat(string,temp);
    free(temp);
    strcat(string,",\"numRoutes\":");
    temp = malloc(100);
    sprintf(temp,"%d",getLength(gpx->routes));
    strcat(string,temp);
    free(temp);
    strcat(string,",\"numTracks\":");
    temp = malloc(100);
    sprintf(temp,"%d",getLength(gpx->tracks));
    strcat(string,temp);
    free(temp);
    strcat(string,"}");
    return string;
}
char* dataListToJSON(const List *list){
    if(list==NULL||getLength((List*)list)==0){
        return "[]";
    }
    char *string = malloc(100000);
    strcpy(string,"[");
    ListIterator data = createIterator((List*)list);
    while(data.current!=NULL){
        GPXData *d = (GPXData*)data.current->data;
        char *str = otherDataToJSON(d);
        strcat(string,str);
        free(str);
        if(data.current->next!=NULL){
            strcat(string,",");
        }
        nextElement(&data);
    }
    strcat(string,"]");
    return string;
}
void addWaypoint(Route *rt, Waypoint *pt){
    if(rt==NULL||pt==NULL){
        return;
    }
    insertBack(rt->waypoints,pt);
}
void addRoute(GPXdoc*doc,Route*rt){
    if(doc==NULL||rt==NULL){
        return;
    }
    insertBack(doc->routes,rt);
}
GPXdoc* JSONtoGPX(const char*gpxString){
    if(gpxString==NULL){
        return NULL;
    }
    char *temp = malloc(strlen(gpxString)+1);
    GPXdoc* d = malloc(sizeof(GPXdoc)+1);
    strcpy(temp,(char*)gpxString);
    char *token = strtok(temp,"}{,:\"");
    while(token!=NULL){
        if(strcmp(token,"version")==0){
            token = strtok(NULL,"}{,:\"");
            d->version = atof(token); 
        }
        if(strcmp(token,"creator")==0){
            token = strtok(NULL,"}{,:\"");
            d->creator = malloc(strlen(token)+1);
            strcpy(d->creator,token); 
        }
        token = strtok(NULL,"}{,:\"");
    }
    List *waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
    List *routes = initializeList(&routeToString,&deleteRoute,&compareRoutes);
    List *tracks = initializeList(&trackToString,&deleteTrack,&compareTracks);
    d->waypoints = waypoints;
    d->routes = routes;
    d->tracks = tracks;
    strcpy(d->namespace,"http://www.topografix.com/GPX/1/1");
    free(temp);
    return d;
}
Waypoint* JSONtoWaypoint(const char* gpxString){
    if(gpxString==NULL){
        return NULL;
    }
    char *temp = malloc(strlen(gpxString)+1);
    Waypoint *w = malloc(sizeof(Waypoint)+1);
    strcpy(temp,(char*)gpxString);
    char *token = strtok(temp,"}{,:\"");
    while(token!=NULL){
        if(strcmp(token,"lat")==0){
            token = strtok(NULL,"}{,:\"");
            w->latitude = atof(token); 
        }
        if(strcmp(token,"lon")==0){
            token = strtok(NULL,"}{,:\"");
            w->longitude = atof(token); 
        }
        token = strtok(NULL,"}{,:\"");
    }
    List *otherData = initializeList(&gpxDataToString,&deleteGpxData,&compareGpxData);
    w->otherData = otherData;
    w->name = malloc(strlen("")+1);
    strcpy(w->name,"");
    free(temp);
    return w;
}
Route* JSONtoRoute(const char* gpxString){
    if(gpxString==NULL){
        return NULL;
    }
    char *temp = malloc(strlen(gpxString)+1);
    Route *r = malloc(sizeof(Route)+1);
    strcpy(temp,(char*)gpxString);
    char *token = strtok(temp,"}{,:\"");
    while(token!=NULL){
        if(strcmp(token,"name")==0){
            token = strtok(NULL,"}{,:\"");
            r->name = malloc(strlen(token)+1);
            strcpy(r->name,token);
        }
        token = strtok(NULL,"}{,:\"");
    }
    List *otherData = initializeList(&gpxDataToString,&deleteGpxData,&compareGpxData);
    List *waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
    r->otherData = otherData;
    r->waypoints = waypoints;
    free(temp);
    return r;
}
//******************Helper Functions***********************//
//Delete gpx Data
void deleteGpxData( void* data){
    if(data==NULL){
        return;
    }
    GPXData *d = (GPXData*)data;
    free(d);
}
//To string
char* gpxDataToString(void* data){
    char* str;
    GPXData *d;
    if (data == NULL){
		return NULL;
	}
    d = (GPXData*)data;
    str = malloc(sizeof(d->name)*strlen((char*)d->name)+sizeof(char)*strlen((char*)d->value)+10);
    sprintf(str, "Name: %s Value: %s\n", d->name, d->value);
    return str;
}
//Compare
int compareGpxData(const void *first, const void *second){
    GPXData *d1,*d2;
    if (first == NULL || second == NULL){
		return 0;
	}
    d1 = (GPXData*)first;
    d2 = (GPXData*)second;
    return strcmp((char*)d1->name,(char*)d2->name);
}
//Delete
void deleteWaypoint(void* data){
    if(data==NULL){
        return;
    }
    Waypoint *w = (Waypoint*)data;
    freeList(w->otherData);
    free(w->name);
    free(w);
}
//To string
char* waypointToString( void* data){
    char* str,*temp;
    Waypoint* w;
    w = (Waypoint*)data;
    str = malloc(strlen(w->name)+strlen(toString(w->otherData))+100);
    temp = malloc(100);
    strcpy(str,"Name: ");
    strcat(str,w->name);
    strcat(str,"\n");
    sprintf(temp, "Longitude: %f\n", w->longitude);
    strcat(str,temp);
    free(temp);
    temp = malloc(100);
    sprintf(temp, "Latitude: %f\n", w->latitude);
    strcat(str,temp);
    free(temp);
    if(getLength(w->otherData)!=0){
        strcat(str,toString(w->otherData));
    }
    return str;
}
//Compare
int compareWaypoints(const void *first, const void *second){
    Waypoint *w1,*w2;
    if (first == NULL || second == NULL){
		return 0;
	}
    w1 = (Waypoint*)first;
    w2 = (Waypoint*)second;
    return strcmp((char*)w1->name,(char*)w2->name);
}
//Delete
void deleteRoute(void* data){
    if(data==NULL){
        return;
    }
    Route* r;
    r = (Route*)data;
    freeList(r->otherData);
    freeList(r->waypoints);
    free(r->name);
    free(r);
}
//To string
char* routeToString(void* data){
    char* str;
    Route*r;
    r = (Route*)data;
    str = malloc(strlen(r->name)+strlen(toString(r->waypoints))+strlen(toString(r->otherData))+100);
    strcpy(str,"Name: ");
    strcat(str,r->name);
    strcat(str,"\n");
    if(getLength(r->waypoints)!=0){
        strcat(str,toString(r->waypoints));
    }
    if(getLength(r->otherData)!=0){
        strcat(str,toString(r->otherData));
    }
    return str;
}
//Compare
int compareRoutes(const void *first, const void *second){
    Route* r1,*r2;
    if (first == NULL || second == NULL){
		return 0;
	}
    r1 = (Route*)first;
    r2 = (Route*)second;
    return strcmp((char*)r1->name,(char*)r2->name);
}
//Delete
void deleteTrack(void* data){
    if(data==NULL){
        return;
    }
    Track* t;
    t = (Track*)data;
    free(t->name);
    freeList(t->segments);
    freeList(t->otherData);
    free(t);
}
//To string
char* trackToString(void* data){
    char* str;
    Track*t;
    t = (Track*)data;
    str = malloc(strlen(toString(t->otherData))+strlen(toString(t->segments))+100);
    strcpy(str,"Name: ");
    strcat(str,t->name);
    strcat(str,"\n");
    if(getLength(t->otherData)!=0){
        strcat(str,toString(t->otherData));
    }
    if(getLength(t->segments)!=0){
        strcat(str,toString(t->segments));
    }
    return str;
}
//Compare
int compareTracks(const void *first, const void *second){
    Track *t1,*t2;
    if (first == NULL || second == NULL){
		return 0;
	}
    t1 = (Track*)first;
    t2 = (Track*)second;
    return strcmp((char*)t1->name,(char*)t2->name);
}
//Delete
void deleteTrackSegment(void* data){
    if(data==NULL){
        return;
    }
    TrackSegment* t;
    t = (TrackSegment*)data;
    freeList(t->waypoints);
    free(t);
}
//To string
char* trackSegmentToString(void* data){
    char*str;
    TrackSegment* t;
    t = (TrackSegment*)data;
    str= malloc(sizeof(char*)*strlen(toString(t->waypoints))+1);
    strcpy(str,toString(t->waypoints));
    return str;  
}
//Compare
int compareTrackSegments(const void *first, const void *second){
    TrackSegment *t1,*t2;
    if (first == NULL || second == NULL){
		return 0;
	}
    t1 = (TrackSegment*)first;
    t2 = (TrackSegment*)second;
    return strcmp((char*)toString(t1->waypoints),(char*)toString(t2->waypoints));
}

