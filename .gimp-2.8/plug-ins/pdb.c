#include <libgimp/gimp.h>
#include <stdio.h>


static void query (void);

static void run (const gchar *name,
                   gint nparams,
                   const GimpParam *param,
                   gint *nreturn_vals,
                   GimpParam **return_vals);
static void modos (GimpDrawable *drawable, gint image) ;

static gboolean oval_dialog (GimpDrawable *drawable);

GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,
  NULL,
  query,
  run
};

MAIN()

static void
query (void)
{
  static GimpParamDef args[] =
  {
    {
      GIMP_PDB_INT32,
      "run-mode",
      "Run mode"
    },
    {
      GIMP_PDB_IMAGE,
      "image",
      "Input image"
    },
    {
      GIMP_PDB_DRAWABLE,
      "drawable",
      "Input drawable"
    }
  };

  gimp_install_procedure (
    "plug-in-pdb",
    "Lovely Oval!",
    "Displays \"The List of PDBs",
    "the RAMS",
    "Copyright the RAMS",
    "2012",
    "PDB",
    "RGB*, GRAY*",
    GIMP_PLUGIN,
    G_N_ELEMENTS (args), 0,
    args, NULL);

  gimp_plugin_menu_register ("plug-in-pdb",
                             "<Image>/Filters/Misc");
}

static void
run (const gchar *name,
     gint nparams,
     const GimpParam *param,
     gint *nreturn_vals,
     GimpParam **return_vals)
{
  static GimpParam values[1];
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;
  GimpRunMode run_mode;
     
  // gboolean gimp_procedural_db_dump (const gchar *filename);
 gboolean gimp_procedural_db_query();
  gint *num_matches ;
  //num_matches = 1;
  gchar ***procedure_names;
  

  /* Setting mandatory output values */
  *nreturn_vals = 1;
  *return_vals = values;

  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = status;

  /* Getting run_mode - we won't display a dialog if
* we are in NONINTERACTIVE mode */
  run_mode = param[0].data.d_int32;
  
  gchar **procnames;
  gint nprocs ;
  gimp_procedural_db_query (".*", ".*",".*",".*",".*",".*",".*",&nprocs, &procnames);

  gint i;
  for (i=0; i<nprocs; i++){
    printf("%04d: %s\n", i, procnames[i]);
}

}
