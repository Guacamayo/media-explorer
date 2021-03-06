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

#include "mex-background-video.h"

#include <clutter-gst/clutter-gst.h>

#include <mx/mx.h>

#include <mex/mex-background.h>
#include <mex/mex-utils.h>

static void mex_background_video_background_iface_init (MexBackgroundIface *iface);

G_DEFINE_TYPE_WITH_CODE (MexBackgroundVideo,
                         mex_background_video,
                         MX_TYPE_STACK,
                         G_IMPLEMENT_INTERFACE (MEX_TYPE_BACKGROUND,
                                                mex_background_video_background_iface_init))

#define BACKGROUND_VIDEO_PRIVATE(o)                             \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o),                            \
                                MEX_TYPE_BACKGROUND_VIDEO,      \
                                MexBackgroundVideoPrivate))

struct _MexBackgroundVideoPrivate
{
  gchar        *video_url;
  const gchar  *name;
  ClutterMedia *media;
};

static void eos_cb (ClutterMedia *media, MexBackgroundVideo *background);

static void
mex_background_video_get_property (GObject    *object,
                                   guint       property_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  switch (property_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
mex_background_video_set_property (GObject      *object,
                                   guint         property_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  switch (property_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
mex_background_video_finalize (GObject *object)
{
  MexBackgroundVideoPrivate *priv = MEX_BACKGROUND_VIDEO (object)->priv;

  g_free (priv->video_url);

  G_OBJECT_CLASS (mex_background_video_parent_class)->finalize (object);
}

static void
mex_background_video_constructed (GObject *object)
{
  MexBackgroundVideoPrivate *priv = MEX_BACKGROUND_VIDEO (object)->priv;

  if (G_OBJECT_CLASS (mex_background_video_parent_class)->constructed)
    G_OBJECT_CLASS (mex_background_video_parent_class)->constructed (object);

  clutter_actor_add_child (CLUTTER_ACTOR (object), CLUTTER_ACTOR (priv->media));
  clutter_texture_set_keep_aspect_ratio (CLUTTER_TEXTURE (priv->media), TRUE);
  clutter_container_child_set (CLUTTER_CONTAINER (object),
                               CLUTTER_ACTOR (priv->media),
                               "crop", TRUE, NULL);

  g_signal_connect (priv->media, "eos", G_CALLBACK (eos_cb), object);
}

static void
mex_background_video_class_init (MexBackgroundVideoClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (MexBackgroundVideoPrivate));

  object_class->get_property = mex_background_video_get_property;
  object_class->set_property = mex_background_video_set_property;
  object_class->finalize = mex_background_video_finalize;
  object_class->constructed = mex_background_video_constructed;
}

static void
mex_background_video_init (MexBackgroundVideo *self)
{
  MexBackgroundVideoPrivate *priv;
  GFile *video;
  gchar *path;

  self->priv = priv = BACKGROUND_VIDEO_PRIVATE (self);

  priv->name = "video";

  path = g_build_filename (mex_get_data_dir (),
                           "style",
                           "background-loop.mkv",
                           NULL);
  video = g_file_new_for_path (path);
  g_free (path);
  priv->video_url = g_file_get_uri (video);
  g_object_unref (video);

  priv->media = CLUTTER_MEDIA (clutter_gst_video_texture_new ());
  clutter_media_set_uri (priv->media, priv->video_url);
  /* Don't start playing until we're active */
  clutter_media_set_playing (priv->media, FALSE);
}


MexBackgroundVideo *
mex_background_video_new (void)
{
  return g_object_new (MEX_TYPE_BACKGROUND_VIDEO, NULL);
}

static void
mex_background_video_set_active (MexBackground *self,
                                 gboolean       active)
{
  MexBackgroundVideoPrivate *priv = MEX_BACKGROUND_VIDEO (self)->priv;

  clutter_media_set_playing (CLUTTER_MEDIA (priv->media), active);
}

static const gchar*
mex_background_video_get_name (MexBackground *self)
{
  MexBackgroundVideoPrivate *priv = MEX_BACKGROUND_VIDEO (self)->priv;

  return priv->name;
}

static void
mex_background_video_background_iface_init (MexBackgroundIface *iface)
{
  iface->set_active = mex_background_video_set_active;
  iface->get_name = mex_background_video_get_name;
}

static void
eos_cb (ClutterMedia *media, MexBackgroundVideo *background)
{
  clutter_media_set_progress (media, 0);
  clutter_media_set_playing (media, TRUE);
}
