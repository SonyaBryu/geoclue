/* Geomap - A DBus api and wrapper for getting geography pictures
 * Copyright (C) 2006 Garmin
 * 
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2 as published by the Free Software Foundation; 
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include <geoclue_map_server_yahoo.h>
#include <geoclue_map_server_glue.h>
#include <geoclue_map_signal_marshal.h>
#include <dbus/dbus-glib-bindings.h>



typedef enum _geoclue_map_returncode
{
    GEOCLUE_MAP_SUCCESS                  = 0,
    GEOCLUE_MAP_NOT_INITIALIZED          = -1, 
    GEOCLUE_MAP_DBUS_ERROR               = -2,
    GEOCLUE_MAP_SERVICE_NOT_AVAILABLE    = -3,
    GEOCLUE_MAP_HEIGHT_TOO_BIG           = -4,
    GEOCLUE_MAP_HEIGHT_TOO_SMALL         = -5,
    GEOCLUE_MAP_WIDTH_TOO_BIG            = -6,
    GEOCLUE_MAP_WIDTH_TOO_SMALL          = -7,
    GEOCLUE_MAP_ZOOM_TOO_BIG             = -8,
    GEOCLUE_MAP_ZOOM_TOO_SMALL           = -9,
    GEOCLUE_MAP_INVALID_LATITUDE         = -10,
    GEOCLUE_MAP_INVALID_LONGITUDE        = -11   
   
} GEOCLUE_MAP_RETURNCODE;



#include <libxml/xmlreader.h>
#include <libsoup/soup.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>



#define GEOCLUE_MAP_MIN_HEIGHT 100
#define GEOCLUE_MAP_MAX_HEIGHT 2000
#define GEOCLUE_MAP_MIN_WIDTH 100
#define GEOCLUE_MAP_MAX_WIDTH 3000
#define GEOCLUE_MAP_MIN_ZOOM 1
#define GEOCLUE_MAP_MAX_ZOOM 12


//This is latitude per pixel at zoom level 5 on yahoo
#define MAGIC_NUMBER 0.000138



typedef struct {
gdouble IN_latitude;
gdouble IN_longitude;
gint IN_width;
gint IN_height;
gint IN_zoom;
GeoclueMap* server; 
} params;


G_DEFINE_TYPE(GeoclueMap, geoclue_map, G_TYPE_OBJECT)


/* Filter signals and args */
enum {
  /* FILL ME */
  GET_MAP_FINISHED,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];

//Default handler
void geoclue_map_get_map_finished(GeoclueMap* obj, gint returncode, GArray* map_buffer, gchar* buffer_mime_type)
{   

printf("finished sending map\n");
}






static void
geoclue_map_init (GeoclueMap *obj)
{


	GError *error = NULL;
	DBusGProxy *driver_proxy;
	GeoclueMapClass *klass = GEOCLUE_MAP_GET_CLASS(obj);
	guint request_ret;
	
	/* Register DBUS path */
	dbus_g_connection_register_g_object (klass->connection,
			GEOCLUE_MAP_DBUS_PATH ,
			G_OBJECT (obj));


	/* Register the service name, the constant here are defined in dbus-glib-bindings.h */
	driver_proxy = dbus_g_proxy_new_for_name (klass->connection,
			DBUS_SERVICE_DBUS,
			DBUS_PATH_DBUS,
			DBUS_INTERFACE_DBUS);


	if(!org_freedesktop_DBus_request_name (driver_proxy,
			GEOCLUE_MAP_DBUS_SERVICE,
			0, &request_ret,    /* See tutorial for more infos about these */
			&error))
	{
		g_printerr("Unable to register geomap service: %s", error->message);
		g_error_free (error);
        exit(1);
	}	

if (request_ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
{ 
    g_printerr("Yahoo maps service already running!\n");
}

  
    g_print("registered mapping interface \n");

}



static void
geoclue_map_class_init (GeoclueMapClass *klass)
{
	GError *error = NULL;

    klass->connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);

	signals[GET_MAP_FINISHED] =
        g_signal_new ("get_map_finished",
                TYPE_GEOCLUE_MAP,
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET (GeoclueMapClass, get_map_finished),
                NULL, /* accumulator */
                NULL, /* accumulator data */
                _geoclue_map_VOID__INT_BOXED_STRING,
                G_TYPE_NONE, 3 ,G_TYPE_INT,  DBUS_TYPE_G_UCHAR_ARRAY , G_TYPE_STRING);
  
    klass->get_map_finished = geoclue_map_get_map_finished;
  

   
    
    
	if (klass->connection == NULL)
	{
		g_printerr("Unable to connect to dbus: %s", error->message);
		g_error_free (error);
		return;
	}	
	/* &dbus_glib__object_info is provided in the server-bindings.h file */
	/* OBJECT_TYPE_SERVER is the GType of your server object */
	dbus_g_object_type_install_info (TYPE_GEOCLUE_MAP, &dbus_glib_geoclue_map_object_info);	
    
}


gboolean geoclue_map_version (GeoclueMap *obj, gint* OUT_major, gint* OUT_minor, gint* OUT_micro, GError **error)
{
        printf("Yahoo!!!\n");
    *OUT_major = 1;
    *OUT_minor = 0;
    *OUT_micro = 0;
    
    return TRUE;
}


gboolean geoclue_map_service_provider(GeoclueMap *obj, char** name, GError **error)
{
    *name = "Yahoo Maps";
    printf("Yahoo!!!\n");
    return TRUE;
}

gboolean geoclue_map_max_zoom(GeoclueMap *obj, int* max_zoom, GError **error)
{
        printf("Yahoo!!!\n");
    *max_zoom = GEOCLUE_MAP_MAX_ZOOM;
    return TRUE;
}


gboolean geoclue_map_min_zoom(GeoclueMap *obj, int* min_zoom, GError **error)
{
   *min_zoom = GEOCLUE_MAP_MIN_ZOOM;
    return TRUE;
}

gboolean geoclue_map_max_height(GeoclueMap *obj, int* max_height, GError **error)
{
    *max_height = GEOCLUE_MAP_MAX_HEIGHT;
    return TRUE;
}

gboolean geoclue_map_min_height(GeoclueMap *obj, int* min_height, GError **error)
{
    *min_height = GEOCLUE_MAP_MIN_HEIGHT;
    return TRUE;
}

gboolean geoclue_map_max_width(GeoclueMap *obj, int* max_width, GError **error)
{
    *max_width = GEOCLUE_MAP_MAX_WIDTH;
    return TRUE;
}

gboolean geoclue_map_min_width(GeoclueMap *obj, int* min_width, GError **error)
{
    *min_width = GEOCLUE_MAP_MIN_WIDTH;
    return TRUE;
}




void geoclue_map_map_thread(params *obj)
{

gdouble IN_latitude = obj->IN_latitude;
gdouble IN_longitude = obj->IN_longitude;
gint IN_width = obj->IN_width;
gint IN_height = obj->IN_height;
gint IN_zoom = obj->IN_zoom;
    

    SoupSession *session;
    SoupMessage *msg;
    SoupMessage *msg2;
    const char *cafile = NULL;
    SoupUri *proxy = NULL;
    SoupUri *base_uri = NULL;
    int fd;
    
    base_uri = soup_uri_new ("http://api.local.yahoo.com/MapsService/V1/mapImage?appid=libgeomap&latitude=38.0&longitude=-122.0&imagetype=png&image_height=800&image_width=800&zoom=7");
    
    
    char* proxy_env;
    
    proxy_env = getenv ("http_proxy");
    
    printf("found proxy %s:end\n", proxy_env); 
        
    
    if (proxy_env != NULL) {  
        printf("added proxy %s\n", proxy_env); 
        proxy = soup_uri_new (proxy_env);
        session = soup_session_sync_new_with_options (
            SOUP_SESSION_SSL_CA_FILE, cafile,
            SOUP_SESSION_PROXY_URI, proxy,
            NULL);
    }
    else
    {
        session = soup_session_sync_new ();
     
        
    }
    
    
    char url[5000];

    snprintf(url, 5000,"http://api.local.yahoo.com/MapsService/V1/mapImage?appid=YahooDemo&latitude=%f&longitude=%f&imagetype=png&image_height=%d&image_width=%d&zoom=%d", IN_latitude,  IN_longitude,  IN_height, IN_width, IN_zoom);
    
    g_print(url);
    
    msg = soup_message_new ("GET", url);
    soup_session_send_message(session, msg);

    char *name, *value;
    xmlTextReaderPtr reader;
    int ret;
    char* pngurl;

    //reader = xmlReaderForFile(argv[1], NULL, 0);
    
    reader = xmlReaderForMemory (msg->response.body, 
                         msg->response.length, 
                         NULL, 
                         NULL, 
                         0);
    
    

        ret = xmlTextReaderRead(reader);
        
        //FIXME: super hack because I don't know how to use the XML libraries.  This just works for now
        while (ret == 1) {
            
                name = (char*)xmlTextReaderConstName(reader);
                   if (!strcmp(name,"#text"))
                   {
                        value = (char*)xmlTextReaderConstValue(reader);
                        printf("%s %s\n", name, value);
                        int size = strlen(value);
                        pngurl = malloc(size);
                        strcpy(pngurl, value);
                        
                        
                   }
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);




    // A very bad rough sanity check
    if(pngurl != NULL)
    {
        if(pngurl[0] == 'h')
        {
    
            printf("Trying to grab image %s\n", pngurl);
            msg2 = soup_message_new ("GET", pngurl);
            soup_session_send_message(session, msg2);
            printf("got message\n, parsing\n");
        
        
        
        
                                                     
            GArray *mydata;
            mydata = g_array_new(FALSE,FALSE, sizeof(guint8));
            mydata->data = msg2->response.body;
            mydata->len =  msg2->response.length;
        
             g_signal_emit_by_name (obj->server,
                                       "get_map_finished",
                                       0, mydata, "image/png"); 
        }
    } 
  
  

};



gboolean geoclue_map_get_map (GeoclueMap *obj, const gdouble IN_latitude, const gdouble IN_longitude, const gint IN_width, const gint IN_height, const gint IN_zoom, int* return_code, GError **error)
{

    if( IN_latitude > 180.0 || IN_latitude < -180.0)
    {
        *return_code = GEOCLUE_MAP_INVALID_LATITUDE;
        return TRUE;
    }
    if( IN_longitude > 180.0 || IN_longitude < -180.0)
    {
        *return_code = GEOCLUE_MAP_INVALID_LONGITUDE;
        return TRUE;
    }
    if( IN_height > GEOCLUE_MAP_MAX_HEIGHT)
    {
        *return_code = GEOCLUE_MAP_HEIGHT_TOO_BIG;
        return TRUE;
    }   
    if( IN_height < GEOCLUE_MAP_MIN_HEIGHT)
    {
        *return_code = GEOCLUE_MAP_HEIGHT_TOO_SMALL;
        return TRUE;
    }
    if( IN_zoom > GEOCLUE_MAP_MAX_ZOOM)
    {
        *return_code = GEOCLUE_MAP_ZOOM_TOO_BIG;
        return TRUE;
    }   
    if( IN_zoom < GEOCLUE_MAP_MIN_ZOOM)
    {
        *return_code = GEOCLUE_MAP_ZOOM_TOO_SMALL;
        return TRUE;
    } 
    if( IN_width > GEOCLUE_MAP_MAX_WIDTH)
    {
        *return_code = GEOCLUE_MAP_WIDTH_TOO_BIG;
        return TRUE;
    }   
    if( IN_width < GEOCLUE_MAP_MIN_WIDTH)
    {
        *return_code = GEOCLUE_MAP_WIDTH_TOO_SMALL;
        return TRUE;
    }        

    params* parameters = malloc(sizeof(params));
    parameters->IN_latitude = IN_latitude;
    parameters->IN_longitude = IN_longitude;
    parameters->IN_width = IN_width;
    parameters->IN_height = IN_height;
    parameters->IN_zoom = IN_zoom;
    parameters->server = obj;

    GThread* mythread = g_thread_create((GThreadFunc)geoclue_map_map_thread,  parameters,  FALSE, NULL);

      
    *return_code = 0;
    
    return TRUE;
}

gboolean geoclue_map_latlong_to_offset(GeoclueMap *obj, const gdouble IN_latitude, const gdouble IN_longitude,  const gint IN_zoom, const gdouble IN_center_latitude, const gdouble IN_center_longitude, int* OUT_x_offset, int* OUT_y_offset, GError **error)
{
      printf("Long offset \n");  

    double cos_value;
    double two_pi_over_360 = 0.017453278;
    cos_value = cos(IN_center_latitude * two_pi_over_360);
    
    // Yeah for 0.000138 the magic number
    double zoom_factor = (MAGIC_NUMBER * pow(2.0,(double)IN_zoom - 5.0));
     

    *OUT_x_offset =  (IN_latitude - IN_center_latitude) / zoom_factor;
    *OUT_y_offset =  (IN_longitude - IN_center_longitude ) / zoom_factor * cos_value;
 
    
    return TRUE;
}

gboolean geoclue_map_offset_to_latlong(GeoclueMap *obj, const int IN_x_offset,const int IN_y_offset, const gint IN_zoom, const gdouble IN_center_latitude, const gdouble IN_center_longitude, gdouble* OUT_latitude, gdouble* OUT_longitude,  GError **error )
{

    /* Higher zoom levels it doubles per level */  
    //printf("Offset to Long \n");
    
    double cos_value;
    double two_pi_over_360 = 0.017453278;
    cos_value = cos(IN_center_latitude * two_pi_over_360);
    
    // Yeah for 0.000138 the magic number
    double zoom_factor = (MAGIC_NUMBER * pow(2.0,(double)IN_zoom - 5.0));
     
     //printf("cos %f sin %f\n", sin_value, cos_value);
     // printf("x %d y %d\n", IN_x_offset, IN_y_offset);
     //      printf("zoomfactor %f \n", zoom_factor); 
    *OUT_latitude =  zoom_factor * (IN_y_offset )  + IN_center_latitude;
    *OUT_longitude =  zoom_factor * (IN_x_offset / cos_value) + IN_center_longitude;

    //printf("Sending %f %f\n",*OUT_latitude, *OUT_longitude);


    return TRUE;
}

gboolean geoclue_map_find_zoom_level (GeoclueMap *obj, const gdouble IN_latitude_top_left, const gdouble IN_longitude_top_left, const gdouble IN_latitude_bottom_right, const gdouble IN_longitude_bottom_right, const gint IN_width, const gint IN_height,  gint* OUT_zoom, GError** error)
{

    // Do Y first as it is easy and will be a good guess to start
    double lat_delta = IN_latitude_top_left - IN_latitude_bottom_right;
    double center_latitude = IN_latitude_top_left - lat_delta/2;
    double cos_value;
    double two_pi_over_360 = 0.017453278;
    cos_value = cos(center_latitude * two_pi_over_360);
    
    //Start at 1
    *OUT_zoom = 1;
    
    double zoom_factor = (MAGIC_NUMBER * pow(2.0,(double)*OUT_zoom - 5.0));
    double make_me_one = zoom_factor * (IN_height) / lat_delta;
    
    while( *OUT_zoom  <= GEOCLUE_MAP_MAX_ZOOM && make_me_one < 1)
    {
        *OUT_zoom++;
        make_me_one *= 2.0;       
    }
   


    double lon_delta = IN_longitude_top_left - IN_longitude_bottom_right;
    
    zoom_factor = (MAGIC_NUMBER * pow(2.0,(double)*OUT_zoom - 5.0));
    make_me_one = zoom_factor * (IN_width / cos_value) / lon_delta;
    
    while( *OUT_zoom  <= GEOCLUE_MAP_MAX_ZOOM && make_me_one < 1)
    {
        *OUT_zoom++;
        make_me_one *= 2.0;       
    }

   
   
      printf("Long offset \n");  


    return TRUE;       
}


gboolean geoclue_map_service_available(GeoclueMap *obj, gboolean* OUT_available, char** OUT_reason, GError** error)
{
    return TRUE;  
}

gboolean geoclue_map_shutdown(GeoclueMap *obj, GError** error)
{
    g_main_loop_quit (obj->loop);
    return TRUE;
}




int main( int   argc,
          char *argv[] )
{
    guint request_name_result;
    
    g_type_init ();
    g_thread_init (NULL);

    
   
    

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION
      

    GeoclueMap* obj = NULL; 
  
    obj = GEOCLUE_MAP(g_type_create_instance (geoclue_map_get_type()));
  
  
    obj->loop = g_main_loop_new(NULL,TRUE);
        



    g_main_loop_run(obj->loop);
    
    g_object_unref(obj);   
    g_main_loop_unref(obj->loop);
    
    
    return 0;
}







