/**
Description: Plug-in is an oval-shaped picture frame of adjustable size on the thickness of the frame and color/pattern
Author : Hart Russell, Mani Tiawree and Zarni Htet @ The RAMS
Finalized and Documented : 06/18/2012
**/

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

// declaring structures and functions
typedef struct 
{
  gint xcord;
  gint ycord;
  gint framecolor;
} MyOval;

static void query (void);
static void run (const gchar *name,
		 gint nparams,
		 const GimpParam *param,
		 gint *nreturn_vals,
		 GimpParam **return_vals);
static void modos (GimpDrawable *drawable, gint image) ;
static gboolean oval_dialog (GimpDrawable *drawable);


// default settings for the frame
static MyOval vals =
  {
    10, // xcord (thickness of the frame horizontally)
    10, // ycord (thickness of the frame vertically)
    1, //frame design (different channels (mentioned later))
  };


// typedef struct that is called when the gimp application initially starts up
GimpPlugInInfo PLUG_IN_INFO =
  {
    NULL,
    NULL,
    query,
    run
  };
// A macro for the main() function of gimp. gimp_main() which has the usual parameters to read from the terminal and PLUG_IN_INFO parameter.
// this calls in query and run in this case and put in the necsseary parameters.
MAIN()


// the query function installs the plug-in for the first time and as the plug-in changes
static void
query (void)
{

  // saving in constant memory an array with info on 
  static GimpParamDef args[] =
    {
      {

	GIMP_PDB_INT32, // determines the run-mode whether it is interactive or non-interactive
	"run-mode",
	"RM"        
      },
      {
	GIMP_PDB_IMAGE, // Input image
	"image",
	"Input image"
      },
      {
	GIMP_PDB_DRAWABLE,// Input drawable
	"drawable",
	"Input drawable"
      }
    };

  gimp_install_procedure (
			  "plug-in-oval",                        // Name of the plug-in
			  "Lovely Oval!",                        // plug-in title
			  "Displays \"An Oval \" in the pane", // description   
			  "the RAMS",           // Author
			  "Copyright the RAMS", // Copyright
			  "2012",               // year
			  "_Oval",              // name, we guess
			  "RGB*, GRAY*",        // image types  
			  GIMP_PLUGIN,          // procedure type
			  G_N_ELEMENTS (args), 0, // int nparams, int nreturn vals, 
			  args, NULL);            //GimpParamDef* params,GimpParamDef* return_values

  gimp_plugin_menu_register ("plug-in-oval",          // the path of where this is installed
                             "<Image>/Filters/Misc");
}



//
static void
run (const gchar *name,
     gint nparams,
     const GimpParam *param,
     gint *nreturn_vals,
     GimpParam **return_vals)
{
 
  // GimpParam contains PDBArg type and ParamData
  // PDBArg type contains GIMP_PDB_DRAWABLE, GIMP_PDB_COLOR, CHANNEL etc
  // ParamData is a union datastructure of all the different data types such as d_drawable,d_channel, d_image of gint32, gint18 etc
  static GimpParam values[1];                  // allocating an array for return type
  GimpPDBStatusType status = GIMP_PDB_SUCCESS; // we successful connected with the 
  GimpRunMode run_mode;

  GimpDrawable *drawable;
  gint image;

  /* Setting mandatory output values */
  *nreturn_vals = 1;
  *return_vals = values;

  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = status;

  /* Getting run_mode - we won't display a dialog if
   * we are in NONINTERACTIVE mode */
  run_mode = param[0].data.d_int32; // Calling from the GimpParamDef (refers back to the static struct in query


  //we will get the specified drawable
  drawable = gimp_drawable_get(param[2].data.d_drawable);
  image= param[1].data.d_image;
  // where are we setting the value of the run_mode?
  switch (run_mode)
    {
    case GIMP_RUN_INTERACTIVE:
      // Get options last values if needed
      //gimp_get_data calls gimp_procedural_db_get_data()
      // which takes in the identifier and the gpointer data (a byte array containing data which is the values we have from the GTK GUI)
      gimp_get_data ("plug-in-oval", &vals);

      // Display the dialog (We think this part is confusing)
       if (! oval_dialog (drawable))
       return;
      break;

    case GIMP_RUN_NONINTERACTIVE:
      if (nparams != 4)                             // why is this not 4?
	gimp_get_data ("plug-in-oval", &vals);
      
      //Display the dialog
      if (! oval_dialog (drawable))
	return;
      break;
  
     

    case GIMP_RUN_WITH_LAST_VALS:
      //  Get options last values if needed
      gimp_get_data ("plug-in-oval", &vals);
      break;

    default:
      break;
    }
   
  //dumping changes made in PDB, into the GIMP core

  modos (drawable, image);
  gimp_displays_flush();                                                                // update all the changes we have made to the user interface                                                       // 
  
  
  return;
}

static void modos (GimpDrawable *drawable ,gint image)
{
  gint width, height;

  // getting the image size
  width = gimp_image_width(image);
  height = gimp_image_height(image);

  // filling in the various parameters of rect_select which is image, x-cord, y-cord, width, height, Operation, feather, feather_radius
  gimp_rect_select(image, 0, 0, width, height, 0, 0, 0);
  // filling the various parameters of ellipse_select which is image, x-cord, y-cord (in this thickness of the frame), adjusted width and height with regards to thickness, SUBTRACT operation, Making the pic clear, feather, feather-radius
  gimp_ellipse_select (image, vals.xcord,vals.ycord, width - (2*vals.xcord), height - (2*vals.ycord), 1, TRUE, vals.xcord/4, vals.ycord/8);
  //Two parameters: drawable id, and fill type. (0-Foreground, 1-Background, 2-Whitefill, 3-Transparentfill, 4-Patterfill)
  gimp_edit_fill (drawable->drawable_id, vals.framecolor);
}

static gboolean
oval_dialog (GimpDrawable *drawable)
{
  GtkWidget *dialog;
  GtkWidget *main_vbox;
  GtkWidget *main_hbox;
  GtkWidget *frame;
  GtkWidget *width_label;
  GtkWidget *alignment;
  GtkWidget *spinbutton;
  GtkWidget *spinbutton2;
  GtkWidget *spinbutton3;
  GtkObject *spinbutton_adj;
  GtkWidget *frame_label;
  gboolean   run;


  //This function initializes GTK+ with gtk_init() and initializes GDK's image rendering subsystem (GdkRGB) to follow the GIMP main program's colormap allocation/installation policy. 
  gimp_ui_init ("oval", FALSE);

  //Creates a new GimpDialog widget
  dialog = gimp_dialog_new ("My Oval", "myoval",
			    NULL, 0,
			    gimp_standard_help_func, "plug-in-oval",

			    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			    GTK_STOCK_OK,     GTK_RESPONSE_OK,

			    NULL);

  //returns a new gtkvbox
  main_vbox = gtk_vbox_new (FALSE, 6);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), main_vbox); //struct to contain widgets
  gtk_widget_show (main_vbox);

  //Creates a new GtkFrame, with optional label label. If label is NULL, the label is omitted. 
  frame = gtk_frame_new (NULL);
  gtk_widget_show (frame);
  gtk_box_pack_start (GTK_BOX (main_vbox), frame, TRUE, TRUE, 0); //Adds widgets to box, packed with reference to the start of box
  gtk_container_set_border_width (GTK_CONTAINER (frame), 6);

  alignment = gtk_alignment_new (0.5, 0.5, 1, 1); //params are xalign, yalign, xscale, yscale
  gtk_widget_show (alignment);
  gtk_container_add (GTK_CONTAINER (frame), alignment);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 6, 6, 6, 6); //Gets the padding on the different sides of the widget.

  main_hbox = gtk_hbox_new (FALSE, 0); //creates a new gtkhbox
  gtk_widget_show (main_hbox);
  gtk_container_add (GTK_CONTAINER (alignment), main_hbox);

  width_label = gtk_label_new_with_mnemonic ("Xcord/Ycord:"); // Creates a new GtkLabel, containing the text in str. 
  gtk_widget_show (width_label);
  gtk_box_pack_start (GTK_BOX (main_hbox), width_label, FALSE, FALSE, 6);
  gtk_label_set_justify (GTK_LABEL (width_label), GTK_JUSTIFY_RIGHT);

  spinbutton_adj = gtk_adjustment_new (10, 0, 100, 1, 5, 5);
  spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_adj), 1, 0);
  gtk_widget_show (spinbutton);
  gtk_box_pack_start (GTK_BOX (main_hbox), spinbutton, FALSE, FALSE, 6);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton), TRUE);

  frame_label = gtk_label_new ("<b>Modify oval</b>");
  gtk_widget_show (frame_label);
  gtk_frame_set_label_widget (GTK_FRAME (frame), frame_label);
  gtk_label_set_use_markup (GTK_LABEL (frame_label), TRUE);

  g_signal_connect (spinbutton_adj, "value_changed",    //Connects a GCallback function to a signal for a particular object. Params are instance, detailed_signal, c_handler, data
                    G_CALLBACK (gimp_int_adjustment_update),
                    &vals.xcord);

  spinbutton_adj = gtk_adjustment_new (10, 0, 100, 1, 5, 5); //initial value, min value, max value, step increment, page increment, page size 
  spinbutton2 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_adj), 1, 0);
  gtk_widget_show (spinbutton2);
  gtk_box_pack_start (GTK_BOX (main_hbox), spinbutton2, FALSE, FALSE, 6);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton2), TRUE);

  g_signal_connect (spinbutton_adj, "value_changed",
		    G_CALLBACK (gimp_int_adjustment_update),
		    &vals.ycord);

  spinbutton_adj = gtk_adjustment_new (0, 0, 4, 1, 5, 5); 
  spinbutton3 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_adj), 1, 0);
  gtk_widget_show (spinbutton3);
  gtk_box_pack_start (GTK_BOX (main_hbox), spinbutton3, FALSE, FALSE, 6);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton3), TRUE);

  g_signal_connect (spinbutton_adj, "value_changed",
		    G_CALLBACK (gimp_int_adjustment_update),
		    &vals.framecolor);
  gtk_widget_show (dialog);

  run = (gimp_dialog_run (GIMP_DIALOG (dialog)) == GTK_RESPONSE_OK);

  gtk_widget_destroy (dialog);

  return run; 

}
