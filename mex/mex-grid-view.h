/* mex-grid-view.h */

#ifndef _MEX_GRID_VIEW_H
#define _MEX_GRID_VIEW_H

#include <mex/mex.h>

G_BEGIN_DECLS

#define MEX_TYPE_GRID_VIEW mex_grid_view_get_type()

#define MEX_GRID_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  MEX_TYPE_GRID_VIEW, MexGridView))

#define MEX_GRID_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  MEX_TYPE_GRID_VIEW, MexGridViewClass))

#define MEX_IS_GRID_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  MEX_TYPE_GRID_VIEW))

#define MEX_IS_GRID_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  MEX_TYPE_GRID_VIEW))

#define MEX_GRID_VIEW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  MEX_TYPE_GRID_VIEW, MexGridViewClass))

typedef struct _MexGridView MexGridView;
typedef struct _MexGridViewClass MexGridViewClass;
typedef struct _MexGridViewPrivate MexGridViewPrivate;

struct _MexGridView
{
  MxWidget parent;

  MexGridViewPrivate *priv;
};

struct _MexGridViewClass
{
  MxWidgetClass parent_class;
};

GType mex_grid_view_get_type (void) G_GNUC_CONST;

ClutterActor *mex_grid_view_new (MexModel *model);

G_END_DECLS

#endif /* _MEX_GRID_VIEW_H */
