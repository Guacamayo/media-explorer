/*
 * Mex - a media explorer
 *
 * Copyright © 2011 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses>
 */

#ifndef _MEX_BLIPTV_PLUGIN
#define _MEX_BLIPTV_PLUGIN

#include <glib-object.h>
#include <gmodule.h>
#include <mex/mex.h>

G_BEGIN_DECLS

#define MEX_TYPE_BLIPTV_PLUGIN mex_bliptv_plugin_get_type()

#define MEX_BLIPTV_PLUGIN(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MEX_TYPE_BLIPTV_PLUGIN, MexBliptvPlugin))

#define MEX_BLIPTV_PLUGIN_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), MEX_TYPE_BLIPTV_PLUGIN, MexBliptvPluginClass))

#define MEX_IS_BLIPTV_PLUGIN(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MEX_TYPE_BLIPTV_PLUGIN))

#define MEX_IS_BLIPTV_PLUGIN_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), MEX_TYPE_BLIPTV_PLUGIN))

#define MEX_BLIPTV_PLUGIN_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), MEX_TYPE_BLIPTV_PLUGIN, MexBliptvPluginClass))

typedef struct _MexBliptvPluginPrivate MexBliptvPluginPrivate;

typedef struct {
  GObject parent;

  MexBliptvPluginPrivate *priv;
} MexBliptvPlugin;

typedef struct {
  GObjectClass parent_class;
} MexBliptvPluginClass;

GType mex_bliptv_plugin_get_type (void);

G_END_DECLS

#endif
