/*
 * Geoclue
 * client.h - Master process client
 *
 * Authors: Iain Holmes <iain@openedhand.com>
 * Copyright 2007-2008 by Garmin Ltd. or its subsidiaries
 */

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <glib-object.h>
#include <geoclue/geoclue-accuracy.h>

#include "master.h"

#define GEOCLUE_TYPE_MASTER_CLIENT (geoclue_master_client_get_type ())
#define GEOCLUE_MASTER_CLIENT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEOCLUE_TYPE_MASTER_CLIENT, GeoclueMasterClient))

typedef struct {
	GObject parent;

	GeoclueAccuracy *desired_accuracy;
	int min_time;
	gboolean require_updates;

	guint32 update_id;
	ProviderDetails *provider;
} GeoclueMasterClient;

typedef struct {
	GObjectClass parent_class;
} GeoclueMasterClientClass;

GType geoclue_master_client_get_type (void);

#endif