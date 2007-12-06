/*
 * Geoclue
 * geocode-example.c - Example using the Geocode client API
 *
 * Author: Jussi Kukkonen <jku@openedhand.com>
 */

#include <glib.h>
#include <geoclue/geoclue-geocode.h>

int main (int argc, char** argv)
{
	gchar *service, *path;
	GeoclueGeocode *geocoder = NULL;
	GeocluePositionFields fields;
	GHashTable *address = NULL;
	double lat, lon;
	GeoclueAccuracy *accuracy = NULL;
	GError *error = NULL;
	
	g_type_init();
	
	if (argc != 2) {
		g_printerr ("Usage:\n  geocode-example <provider_name>\n");
		return 1;
	}
	g_print ("Using provider '%s'", argv[1]);
	service = g_strdup_printf ("org.freedesktop.Geoclue.Providers.%s", argv[1]);
	path = g_strdup_printf ("/org/freedesktop/Geoclue/Providers/%s", argv[1]);
	
	address = g_hash_table_new (g_str_hash, g_str_equal);
	g_hash_table_insert (address, "postalcode", "00330");
	g_hash_table_insert (address, "countrycode", "FI");
	g_hash_table_insert (address, "street", "Solnantie 24");
	
	
	/* Create new GeoclueGeocode */
	geocoder = geoclue_geocode_new (service, path);
	g_free (service);
	g_free (path);
	if (geocoder == NULL) {
		g_printerr ("Error while creating GeoclueGeocode object.\n");
		return 1;
	}
	
	
	/* Geocode. We're not interested in altitude 
	   this time, so leave it NULL. */
	fields = geoclue_geocode_address_to_position (geocoder, address, 
	                                              &lat, &lon, NULL, 
	                                              &accuracy, &error);
	if (error) {
		g_printerr ("Error while geocoding: %s\n", error->message);
		g_error_free (error);
		g_free (geocoder);
		return 1;
	}
	
	/* Print out coordinates if they are valid */
	if (fields & GEOCLUE_POSITION_FIELDS_LATITUDE &&
	    fields & GEOCLUE_POSITION_FIELDS_LONGITUDE) {
		
		GeoclueAccuracyLevel level;
		double horiz_acc;
		
		geoclue_accuracy_get_details (accuracy, &level, &horiz_acc, NULL);
		g_print ("Geocoded position:\n");
		g_print ("\t%f, %f\n", lat, lon);
		g_print ("\tAccuracy level %d (%.0f meters)\n", level, horiz_acc);
		
	} else {
		g_print ("Latitude and longitude not available.\n");
	}
	
	geoclue_accuracy_free (accuracy);
	g_free (geocoder);
	return 0;
	
}