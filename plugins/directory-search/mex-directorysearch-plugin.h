/*
 * Mex - a media explorer
 *
 * Copyright © 2010, 2011 Intel Corporation.
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


#ifndef __MEX_DIRECTORYSEARCHMANAGER_H__
#define __MEX_DIRECTORYSEARCHMANAGER_H__

#include <glib-object.h>
#include <gmodule.h>
#include <mex/mex.h>

G_BEGIN_DECLS

#define MEX_TYPE_DIRECTORY_SEARCH_MANAGER mex_directory_search_manager_get_type()

#define MEX_DIRECTORY_SEARCH_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MEX_TYPE_DIRECTORY_SEARCH_MANAGER, MexDirectorySearchManager))

#define MEX_DIRECTORY_SEARCH_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), MEX_TYPE_DIRECTORY_SEARCH_MANAGER, MexDirectorySearchManagerClass))

#define MEX_IS_DIRECTORY_SEARCH_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MEX_TYPE_DIRECTORY_SEARCH_MANAGER))

#define MEX_IS_DIRECTORY_SEARCH_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), MEX_TYPE_DIRECTORY_SEARCH_MANAGER))

#define MEX_DIRECTORY_SEARCH_MANAGER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), MEX_TYPE_DIRECTORY_SEARCH_MANAGER, MexDirectorySearchManagerClass))

typedef struct _MexDirectorySearchManager MexDirectorySearchManager;
typedef struct _MexDirectorySearchManagerClass MexDirectorySearchManagerClass;
typedef struct _MexDirectorySearchManagerPrivate MexDirectorySearchManagerPrivate;

struct _MexDirectorySearchManager
{
  GObject parent;

  MexDirectorySearchManagerPrivate *priv;
};

struct _MexDirectorySearchManagerClass
{
  GObjectClass parent_class;
};

GType           mex_directory_search_manager_get_type         (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __MEX_DIRECTORY_SEARCH_MANAGER_H__ */
