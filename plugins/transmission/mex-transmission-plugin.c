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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib/gi18n-lib.h>
#include <gmodule.h>
#include <libsoup/soup.h>
#include <json-glib/json-glib.h>

#include <mex/mex-plugin.h>
#include <mex/mex-model-provider.h>
#include <mex/mex-action-provider.h>
#include <mex/mex-generic-model.h>
#include <mex/mex-utils.h>

#include "mex-transmission-plugin.h"
#include "mex-torrent.h"

#define RPC_URL               "http://localhost:9091/transmission/rpc"
#define TRANSMISSION_SESSION  "X-Transmission-Session-Id"

static void model_provider_iface_init (MexModelProviderInterface *iface);
G_DEFINE_TYPE_WITH_CODE (MexTransmissionPlugin,
                         mex_transmission_plugin,
                         G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (MEX_TYPE_MODEL_PROVIDER,
                                                model_provider_iface_init))

#define GET_PRIVATE(o)                                          \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o),                            \
                                MEX_TYPE_TRANSMISSION_PLUGIN,   \
                                MexTransmissionPluginPrivate))

struct _MexTransmissionPluginPrivate {
  GList *models;
  MexModel *transmission_model;

  SoupSession *session;
  gchar *session_id;
};

static void
decode_response (MexTransmissionPlugin *plugin,
                 const gchar           *data)
{
  MexTransmissionPluginPrivate *priv = plugin->priv;
  JsonParser *parser;
  JsonReader *reader;
  JsonNode *root;
  gint nb_torrents, i;
  GError *error = NULL;

  parser = json_parser_new ();
  json_parser_load_from_data (parser, data, -1, &error);
  if (error)
    {
      g_warning ("could not parser response: %s\nmessage was %s",
                 error->message, data);
      goto parser_error;
    }

  root = json_parser_get_root (parser);
  reader = json_reader_new (root);

  json_reader_read_member (reader, "arguments");
  json_reader_read_member (reader, "torrents");
  nb_torrents = json_reader_count_elements (reader);
  for (i = 0; i < nb_torrents; i++)
    {
      gint64 id, total_size;
      const gchar *name;
      MexTorrent *torrent;

      json_reader_read_element (reader, i);

      json_reader_read_member (reader, "id");
      id = json_reader_get_int_value (reader);
      json_reader_end_member (reader);

      json_reader_read_member (reader, "name");
      name = json_reader_get_string_value (reader);
      json_reader_end_member (reader);

      json_reader_read_member (reader, "totalSize");
      total_size = json_reader_get_int_value (reader);
      json_reader_end_member (reader);

      json_reader_end_element (reader);

      torrent = g_object_new (MEX_TYPE_TORRENT,
                              "name", name,
                              "size", total_size,
                              NULL);
      mex_model_add_content (priv->transmission_model, MEX_CONTENT (torrent));
    }
  json_reader_end_member (reader); /* torrents */
  json_reader_end_member (reader); /* arguments */

  g_object_unref (reader);
parser_error:
  g_object_unref (parser);
}

static void
on_response_received (SoupSession *session,
                      SoupMessage *message,
                      gpointer     user_data)
{
  MexTransmissionPlugin *plugin = MEX_TRANSMISSION_PLUGIN (user_data);
  MexTransmissionPluginPrivate *priv = plugin->priv;

  if (message->status_code == SOUP_STATUS_CONFLICT)
    {
      const gchar *session_id;

      session_id = soup_message_headers_get_one (message->response_headers,
                                                 TRANSMISSION_SESSION);
      if (session_id == NULL)
        {
          g_warning ("could not retrieve session header");
          return;
        }

      priv->session_id = g_strdup (session_id);

      //g_message ("updated session id is %s", priv->session_id);

      /* resend the message with the updated session id */
      soup_message_headers_append (message->request_headers,
                                   TRANSMISSION_SESSION,
                                   priv->session_id);
      g_object_ref (message);
      soup_session_queue_message (priv->session, message,
                                  on_response_received, plugin);
    }
  else if (message->status_code == SOUP_STATUS_OK)
    {
      decode_response (plugin, message->response_body->data);
    }
  else
    {
      g_warning ("Error %d while talking to the transmission daemon",
                 message->status_code);
    }
}

static void
mex_transmission_send_request (MexTransmissionPlugin *plugin,
                               const gchar           *method,
                               JsonNode              *arguments)
{
  MexTransmissionPluginPrivate *priv = plugin->priv;
  JsonGenerator *generator;
  SoupMessage *message;
  JsonBuilder *builder;
  JsonNode *root;
  gsize length = 0;
  gchar *data;

  /* build the json data */
  builder = json_builder_new ();
  json_builder_begin_object (builder);
  json_builder_set_member_name (builder, "method");
  json_builder_add_string_value (builder, method);
  json_builder_set_member_name (builder, "arguments");
  json_builder_add_value (builder, arguments);
  json_builder_end_object (builder);

  root = json_builder_get_root (builder);
  g_object_unref (builder);

  generator = json_generator_new ();
  json_generator_set_root (generator, root);
  data = json_generator_to_data (generator, &length);
  g_object_unref (generator);

  /* send the message */
  message = soup_message_new ("POST", RPC_URL);
  soup_message_set_request (message, "application/json", SOUP_MEMORY_TAKE,
                            data, length);
  soup_session_queue_message (priv->session, message,
                              on_response_received, plugin);
}

static void
mex_transmission_update_model (MexTransmissionPlugin *self)
{
  JsonBuilder *builder;
  JsonNode *arguments;

  g_message ("update model");

  builder = json_builder_new ();
  json_builder_begin_object (builder);
  json_builder_set_member_name (builder, "fields");
  json_builder_begin_array (builder);
  json_builder_add_string_value (builder, "id");
  json_builder_add_string_value (builder, "name");
  json_builder_add_string_value (builder, "totalSize");
  json_builder_end_array (builder);
  json_builder_end_object (builder);

  arguments = json_builder_get_root (builder);
  g_object_unref (builder);

  mex_transmission_send_request (self, "torrent-get", arguments);
}

/*
 * MexModelProvider implementation
 */

static const GList *
mex_transmission_plugin_get_models (MexModelProvider *model_provider)
{
  MexTransmissionPlugin *plugin = MEX_TRANSMISSION_PLUGIN (model_provider);
  MexTransmissionPluginPrivate *priv = plugin->priv;

  return priv->models;
}

static void
model_provider_iface_init (MexModelProviderInterface *iface)
{
  iface->get_models = mex_transmission_plugin_get_models;
}

/*
 * GObject implementation
 */

static void
mex_transmission_plugin_finalize (GObject *object)
{
  MexTransmissionPlugin *plugin = MEX_TRANSMISSION_PLUGIN (object);
  MexTransmissionPluginPrivate *priv = plugin->priv;

  while (priv->models)
    {
      mex_model_info_free (priv->models->data);
      priv->models = g_list_delete_link (priv->models, priv->models);
    }

  if (priv->session)
    g_object_unref (priv->session);

  g_free (priv->session_id);

  G_OBJECT_CLASS (mex_transmission_plugin_parent_class)->finalize (object);
}

static void
mex_transmission_plugin_class_init (MexTransmissionPluginClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (MexTransmissionPluginPrivate));

  object_class->finalize = mex_transmission_plugin_finalize;
}

static void
mex_transmission_plugin_init (MexTransmissionPlugin *self)
{
  MexTransmissionPluginPrivate *priv;
  MexModelManager *manager;
  MexModelInfo *model_info;
  MexModelCategoryInfo downloads =
    {
      "downloads", _("Downloads"), "icon-apps", 60,
      _("Add torrents with the Tranmission web interface, monitor and play "
        "them from here.")
    };

  self->priv = priv = GET_PRIVATE (self);

  priv->session = soup_session_async_new ();

  priv->transmission_model = mex_generic_model_new (_("Downloads"),
                                                     "icon-apps");

  model_info = mex_model_info_new_with_sort_funcs (priv->transmission_model,
                                                   "downloads", 0);
  g_object_unref (priv->transmission_model);

  priv->models = g_list_append (priv->models, model_info);

  manager = mex_model_manager_get_default ();
  mex_model_manager_add_category (manager, &downloads);
  mex_model_manager_add_model (manager, model_info);

  mex_transmission_update_model (self);
}

static GType
mex_transmission_get_type (void)
{
  return MEX_TYPE_TRANSMISSION_PLUGIN;
}

MEX_DEFINE_PLUGIN ("Transmission",
		   "Transmission BitTorrent integration",
		   PACKAGE_VERSION,
		   "LGPLv2.1+",
                   "Damien Lespiau <damien.lespiau@intel.com>",
		   MEX_API_MAJOR, MEX_API_MINOR,
		   mex_transmission_get_type,
		   MEX_PLUGIN_PRIORITY_DEBUG)