/**
 * GimpBus
 *   An attempt to provide PDB and other Gimp services over D-Bus.
 *
 *  Copyright (c) 2011 Samuel A. Rebelsky
 *
 *  This file is part of GimpBus.
 *
 *  GimpBus is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as 
 *  published by the Free Software Foundation, either version 3 of the 
 *  License, or (at your option) any later version.
 *
 *  GimpBus is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public 
 *  License along with GimpBus .  If not, see 
 *    <http://www.gnu.org/licenses/>.
 */

// +---------+--------------------------------------------------------
// | Headers |
// +---------+

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <gio/gio.h>

#include <libgimp/gimp.h>

#include "gimpbus.h"
#include "slog.h"


// +-------+----------------------------------------------------------
// | Types |
// +-------+

/**
 * Numbers for our wonderous errors.
 */
typedef enum
  {
    GIMPBUS_ERROR_UNIMPLEMENTED,
    GIMPBUS_ERROR_NOPDBMETHOD,
    GIMPBUS_ERROR_PARAMCOUNT,
    GIMPBUS_ERROR_PARAMTYPE,
    GIMPBUS_ERROR_MISCELLANEOUS
  } GimpBusError;


// +---------+--------------------------------------------------------
// | Globals |
// +---------+

/**
 * The "about" message
 */
static const char *GIMPBUS_ABOUT = 
  "GimpBus - An attempt to provide PDB and other Gimp services over D-Bus.\n"
  "\n"
  "Copyright (c) 2011 Samuel A. Rebelsky <rebelsky AT grinnell DOT edu>\n"
  "\n"
  "GimpBus is free software: you can redistribute it and/or modify\n"
  "it under the terms of the GNU Lesser General Public License as \n"
  "published by the Free Software Foundation, either version 3 of the \n"
  "License, or (at your option) any later version.\n"
  "\n"
  "GimpBus is distributed in the hope that it will be useful,\n"
  "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
  "GNU Lesser General Public License for more details.\n"
  "\n"
  "You should have received a copy of the GNU Lesser General Public \n"
  "License along with GimpBus .  If not, see \n"
  "  <http://www.gnu.org/licenses/>.\n"
  ;

/**
 * The mapping of error numbers to error strings.
 */
static const GDBusErrorEntry error_entries[] =
  {
    { GIMPBUS_ERROR_UNIMPLEMENTED, "org.glimmer.GimpBus.Error.Unimplemented" },
    { GIMPBUS_ERROR_NOPDBMETHOD,   "org.glimmer.GimpBus.Error.NoPDBMethod" },
    { GIMPBUS_ERROR_PARAMCOUNT,    "org.glimmer.GimpBus.Error.ParamCount" },
    { GIMPBUS_ERROR_PARAMTYPE,     "org.glimmer.GimpBus.Error.ParamType" },
    { GIMPBUS_ERROR_MISCELLANEOUS, "org.glimmer.GimpBus.Error.Miscellaneous" },
  };

/** 
 * The main event-handling loop.
 */
static GMainLoop *loop = NULL;

/**
 * The introspection data.  Whee!
 */
static GDBusNodeInfo *introspection_data = NULL;

/**
 * The error quark
 */
static GQuark quark;


// +-----------------+------------------------------------------------
// | Predeclarations |
// +-----------------+

GDBusInterfaceInfo *sample_interface (void);

static const gchar *gimpbus_pdb_type_to_signature (GimpPDBArgType type);


// +----------------+-------------------------------------------------
// | Misc Utilities |
// +----------------+

/**
 * Replace one character by another.
 */
gchar *
strrep (gchar *str, gchar target, gchar replacement)
{
  SLOG_ENTER ("strrep (\"%s\",'%c','%c')", str, target, replacement);
  gchar *tmp = str;
  while ((tmp = strchr (tmp, target)) != NULL)
    *tmp = replacement;
  SLOG_LEAVE ("returning \"%s\"", str);
  return str;
} // strrep

/**
 * Dump the introspection data for a node.
 */
void
dump_node (GDBusNodeInfo *node_data)
{
  if (node_data == NULL)
    fprintf (stderr, "NO INTROSPECTION DATA");

  GString *string = g_string_new ("");
  g_dbus_node_info_generate_xml (node_data, 0, string);
  fprintf (stderr, "%s", string->str);
  g_string_free (string, TRUE);
} // dump_node


// +----------------------------------------+-------------------------
// | Introspection Information Construction |
// +----------------------------------------+

/**
 * Allocate and build an array of pointers.
 */
gpointer *
A (gpointer p0, ...)
{
  gpointer p;           // Iterator
  gint size = 0;        // Number of pointers
  va_list args;         // The extra arguments
  int i;                // Everyone's favorite couter

  // Figure out how many pointers we have
  va_start (args, p0);
  for (p = p0; p != NULL; p = va_arg (args, gpointer))
    ++size;
  va_end (args);

  // Build the array
  gpointer *ps = g_new (gpointer, size+1);
  va_start (args, p0);
  for (i = 0, p = p0; p != NULL; i++, p = va_arg (args, gpointer))
    ps[i] = p;
  va_end (args);
  ps[size] = NULL;

  // We're done
  return ps;
} // A

GDBusArgInfo *
arg_new (gchar *name,
         const gchar *signature,
         gpointer *annotations)
{
  SLOG_ENTER ("arg_new (\"%s\", \"%s\", _)", name, signature);
  GDBusArgInfo *arg= 
    g_try_malloc (sizeof (GDBusArgInfo));
  if (arg == NULL)
    return NULL;
  
  arg->ref_count = 0;
  arg->name = name;
  arg->signature = (gchar *) signature;
  arg->annotations = (GDBusAnnotationInfo **) annotations;
  
  SLOG_LEAVE ("returning");
  return arg;
} // arg_new

GDBusInterfaceInfo *
interface_new (gchar    *name, 
               gpointer *methods,
               gpointer *signals,
               gpointer *properties,
               gpointer *annotations)
{
  SLOG_ENTER ("interface_new (%s, ...)", name);
  GDBusInterfaceInfo *interface = 
    g_try_malloc (sizeof (GDBusInterfaceInfo));
  if (interface == NULL)
    {
      SLOG_LEAVE ("returning NULL for failure");
      return NULL;
    }

  interface->ref_count = 1;
  interface->name = name;
  interface->methods = (GDBusMethodInfo **) methods;
  interface->signals = (GDBusSignalInfo **) signals;
  interface->properties = (GDBusPropertyInfo **) properties;
  interface->annotations = (GDBusAnnotationInfo **) annotations;

  SLOG_LEAVE ("returning normally");
  return interface;
} // interface_new

GDBusMethodInfo *
method_new (gchar *name,
            gpointer *in_args,
            gpointer *out_args,
            gpointer *annotations)
{
  SLOG_ENTER ("method_new (%s, ...)", name);

  GDBusMethodInfo *info = g_try_malloc (sizeof (GDBusMethodInfo));
  if (info == NULL)
    {
      SLOG_LEAVE ("returning NULL for failure");
      return NULL;
    }

  info->ref_count = 1;
  info->name = name;
  info->in_args = (GDBusArgInfo **) in_args;
  info->out_args = (GDBusArgInfo **) out_args;
  info->annotations = (GDBusAnnotationInfo **) annotations;

  SLOG_LEAVE ("returning normally");
  return info;
} // method_new

GDBusNodeInfo *
node_new (gchar *path,
          gpointer *interfaces,
          gpointer *nodes,
          gpointer *annotations)
{
  GDBusNodeInfo *node = g_try_malloc (sizeof (GDBusNodeInfo));
  if (node == NULL)
    return NULL;
  node->ref_count = 1;
  node->path = path;
  node->interfaces = (GDBusInterfaceInfo **) interfaces;
  node->nodes = (GDBusNodeInfo **) nodes;
  node->annotations = (GDBusAnnotationInfo **) annotations;
  return node;
} // node_new


// +-----------------+------------------------------------------------
// | Type Conversion |
// +-----------------+

GVariant *
gimpbus_gimp_param_to_g_variant (GimpParam value)
{
  switch (value.type)
    {
      case GIMP_PDB_INT32:
      case GIMP_PDB_DISPLAY:
      case GIMP_PDB_IMAGE:
      case GIMP_PDB_LAYER:
      case GIMP_PDB_CHANNEL:
      case GIMP_PDB_DRAWABLE:
      case GIMP_PDB_SELECTION:
      case GIMP_PDB_BOUNDARY:
      case GIMP_PDB_VECTORS:
        return g_variant_new ("i", value.data.d_int32);

      case GIMP_PDB_STRING:
        return g_variant_new ("s", value.data.d_string);

      default:
        return NULL;
    }
} // gimpbus_gimp_param_to_g_variant

GVariant *
gimpbus_gimp_params_to_g_variant (GimpParam *values, int nvalues)
{
  SLOG_ENTER ("gimpbus_gimp_params_to_g_variant (...)");

  GVariantBuilder  builder;   // Something to help build the variant
  int              i;         // Counter variable
  GVariant        *val;       // One of the parameters

  g_variant_builder_init (&builder, G_VARIANT_TYPE_TUPLE);
  for (i = 0; i < nvalues; i++)
    {
      SLOG_MESSAGE ("processing GimpParam %d", i);
      val = gimpbus_gimp_param_to_g_variant (values[i]);
      if (val == NULL)
        {
          SLOG_LEAVE ("failed to add GimpParam %d", i);
          return NULL;
        } // if (val == NULL)

      SLOG_MESSAGE ("adding value %s", g_variant_get_type_string (val));
      g_variant_builder_add_value (&builder, val);
    } // for each value

  SLOG_LEAVE ("finished");
  return g_variant_builder_end (&builder);
} // gimpbus_gimp_params_to_g_variant

/**
 * Convert a single GVariant to a GimpParam, which gets stored in the
 * already allocated param.  Returns success/failure.
 */
gboolean
gimpbus_g_variant_to_gimp_param (GVariant         *parameter,
                                 GimpPDBArgType    type,
                                 GimpParam        *param)
{
  SLOG_ENTER ("gimpbus_b_variant_to_gimp_param (%p, %d, %p)",
              parameter, type, param);

  // Make sure that types match
  const gchar *paramtype = gimpbus_pdb_type_to_signature (type);
  if (! g_variant_type_equal ((GVariantType *) paramtype,
                              g_variant_get_type (parameter)))
    {
      SLOG_LEAVE ("type mismatch");
      return FALSE;
    } // g_variant_type_equal

  param->type = type;

  switch (type)
    {
      case GIMP_PDB_INT32:
      case GIMP_PDB_DISPLAY:
      case GIMP_PDB_IMAGE:
      case GIMP_PDB_LAYER:
      case GIMP_PDB_CHANNEL:
      case GIMP_PDB_DRAWABLE:
      case GIMP_PDB_SELECTION:
      case GIMP_PDB_BOUNDARY:
      case GIMP_PDB_VECTORS:
        param->data.d_int32 = g_variant_get_int32 (parameter);
        SLOG_LEAVE ("returning an integer");
        return TRUE;
 
      case GIMP_PDB_STRING:
        param->data.d_string = g_variant_dup_string (parameter, NULL);
        SLOG_LEAVE ("returning a string");
        return TRUE;

      default:
        SLOG_LEAVE ("unimplemented type");
        return FALSE;
    } // switch
} // gimpbus_g_variant_to_gimp_param

/**
 * Convert a GVariant to a a newly allocated array of GimpParams.
 */
gboolean
gimpbus_g_variant_to_gimp_params (GVariant       *parameters, 
                                  GimpParamDef   *types,
                                  GimpParam     **actuals)
{
  SLOG_ENTER ("gimpbus_g_variant_to_gimp_params (...)");
  int  nparams = g_variant_n_children (parameters);
  GimpParam *result = g_new (GimpParam, nparams);
  int i;

  for (i = 0; i < nparams; i++)
    {
      GVariant *param = g_variant_get_child_value (parameters, i);
      if (! gimpbus_g_variant_to_gimp_param (param, 
                                             types[i].type, 
                                             &(result[i])))
        {
          SLOG_LEAVE ("incorrect type for parameter %d", i);
          g_free (result);
          return FALSE;
        }
    } // for each parameter

  SLOG_LEAVE ("returning");
  *actuals = result;
  return TRUE;
} // gimpbus_g_variant_to_gimp_params

/**
 * Convert a GimpParamDef to a GVariant signature
 */
static const gchar *
gimpbus_pdb_type_to_signature (GimpPDBArgType type)
{
  SLOG_ENTER ("gimpbus_pdb_type_to_signature (%d)", type);

  const gchar *result;

  switch (type)
    {
      case GIMP_PDB_INT32:
        result = (const gchar *) G_VARIANT_TYPE_INT32;
        break;
      case GIMP_PDB_INT16:
        result = (const gchar *) G_VARIANT_TYPE_INT16;
        break;
      case GIMP_PDB_INT8:
        result = (const gchar *) G_VARIANT_TYPE_BYTE;
        break;
      case GIMP_PDB_FLOAT:
        result = (const gchar *) G_VARIANT_TYPE_DOUBLE;
        break;
      case GIMP_PDB_STRING:
        result = (const gchar *) G_VARIANT_TYPE_STRING;
        break;

      case GIMP_PDB_DISPLAY:
        result = (const gchar *) G_VARIANT_TYPE_INT32;
        break;
      case GIMP_PDB_IMAGE:
        result = (const gchar *) G_VARIANT_TYPE_INT32;
        break;
      case GIMP_PDB_LAYER:
        result = (const gchar *) G_VARIANT_TYPE_INT32;
        break;

      default:
        result = (const gchar *) G_VARIANT_TYPE_INT32;
        break;
    } // switch

  SLOG_LEAVE ("returning \"%s\"", result);
  return result;
} // gimpbus_pdb_param_to_signature

/**
 * Convert a GimpParamDef to a GDBusArgInfo.
 */
GDBusArgInfo *
gimpbus_pdb_param_to_arginfo (GimpParamDef param)
{
  SLOG_ENTER ("gimpbus_pdb_param_to_arginfo (...)");
  gchar *name = strrep (g_strdup (param.name), '-', '_');
  const gchar *sig = gimpbus_pdb_type_to_signature (param.type);
  GDBusArgInfo *result = arg_new (name, sig, NULL);
  SLOG_LEAVE ("returning");
  return result;
} // gimpbus_pdb_param_to_arginfo


// +----------------------------+-------------------------------------
// | GimpBus Introspection Data |
// +----------------------------+

/**
 * Create information about one PDB method.
 */
static GDBusMethodInfo *
gimpbus_pdb_method (gchar *proc_name)
{
  SLOG_ENTER ("gimpbus_pdb_method (\"%s\")", proc_name);

  // Lots and lots and lots of fields for getting info.
  gchar           *proc_blurb;
  gchar           *proc_help;
  gchar           *proc_author;
  gchar           *proc_copyright;
  gchar           *proc_date;
  GimpPDBProcType  proc_type;
  GimpParamDef    *formals;       
  gint             nparams;
  GimpParamDef    *return_types;
  gint             nreturn_vals;

  // Parts of our return information
  gint i;                        // Counter variable
  GDBusArgInfo   **args = NULL;         // Argument info
  GDBusArgInfo   **returns = NULL;      // Return value info

  // Get the information 
  SLOG_MESSAGE ("getting PDB info");
  if (! gimp_procedural_db_proc_info (proc_name,
                                      &proc_blurb,
                                      &proc_help,
                                      &proc_author,
                                      &proc_copyright,
                                      &proc_date,
                                      &proc_type,
                                      &nparams, &nreturn_vals,
                                      &formals, &return_types))
    {
      SLOG_LEAVE ("no such procedure, returning NULL");
      return NULL;
    }
  SLOG_MESSAGE ("got PDB info");

  // Process the parameters
  if (nparams > 0)
    {
      args = g_new (GDBusArgInfo *, nparams+1);
      for (i = 0; i < nparams; i++)
        {
          SLOG_MESSAGE ("processing parameter type %d", i);
          args[i] = gimpbus_pdb_param_to_arginfo (formals[i]);
          g_assert (args[i] != NULL);
        } // for
      // Terminate the array
      args[nparams] = NULL;
    } // if (nparams > 0)

  // Process the return values
  if (nreturn_vals > 0)
    {
      returns = g_new (GDBusArgInfo *, nreturn_vals);
      for (i = 0; i < nreturn_vals ; i++)
        {
          SLOG_MESSAGE ("processing return value %d", i);
          returns[i] = gimpbus_pdb_param_to_arginfo (return_types[i]);
          g_assert (returns[i] != NULL);
        } // for
      // Terminate the array
      returns[nreturn_vals] = NULL;
    } // if (nreturn_vals > 0)

  GDBusMethodInfo * result =
    method_new (strrep (g_strdup (proc_name), '-', '_'),
                (gpointer *) args,
                (gpointer *) returns,
                NULL);

  SLOG_LEAVE ("returning normally");
  return result;
} // gimpbus_pdb_method

/**
 * Add methods to the array of methods based on a pattern.  Returns the 
 * number of methods added.
 */
int
gimpbus_pdb_methods_add (GArray *builder, gchar *name, gchar *proc_type)
{
  SLOG_ENTER ("gimpbus_pdb_methods_add (%p, \"%s\", \"%s\"",
              builder, name, proc_type);
  gchar           **procnames; // Array of procedure names
  gint              nprocs;    // Size of that array
  int               i;         // Counter variable
  GDBusMethodInfo  *info;      // Information on one procedure

  // Get the list of method names
  gimp_procedural_db_query (name, ".*", ".*", ".*", ".*", ".*", proc_type,
                            &nprocs, &procnames);

  // Add each method to the list
  for (i = 0; i < nprocs; i++)
    {
#ifdef OBSERVE
      fprintf (stderr, "Getting method %d: %s (%p)\n", i, procnames[i], procnames[i]);
#endif
      SLOG_MESSAGE ("Getting method %d: %s", i, procnames[i]);
      info = gimpbus_pdb_method (procnames[i]);
      g_assert (info != NULL);
      g_array_append_val (builder, info);
    } // for

  SLOG_LEAVE ("returning %d", nprocs);
  return nprocs;
} // gimpbus_pdb_methods_add

/**
 * Make an array of information about PDB methods.
 */
static GDBusMethodInfo **
gimpbus_pdb_methods (void)
{
  SLOG_ENTER ("gimpbus_pdb_methods ()");
  GArray           *builder;   // For building the array of methods
  GDBusMethodInfo **methods;   // What we're going to return
  GDBusMethodInfo  *info;      // Information on one procedure

  // Set up the builder
  builder = g_array_new (TRUE, TRUE, sizeof (GDBusMethodInfo *));

  // Add procedures
#if 0
  gimpbus_pdb_methods_add (builder, 
                           "gimp-image-get.*", 
                           "Internal GIMP procedure");
  gimpbus_pdb_methods_add (builder, 
                           ".*new.*", 
                           "Internal GIMP procedure");
  gimpbus_pdb_methods_add (builder, 
                           "gimp-image-add-layer", 
                           "Internal GIMP procedure");
  gimpbus_pdb_methods_add (builder, 
                           "gimp-edit-bucket-fill", 
                           "Internal GIMP procedure");
  gimpbus_pdb_methods_add (builder, 
                           "gimp-display-new", 
                           "Internal GIMP procedure");
  gimpbus_pdb_methods_add (builder, 
                           "gimp-floating-sel-to-layer",
                           "Internal GIMP procedure");
#else
  gimpbus_pdb_methods_add (builder, 
                           ".*",
                           "Internal GIMP procedure");
#endif

  // Terminate the array
  info = NULL;
  g_array_append_val (builder, info);

  // Clean up
  methods = (GDBusMethodInfo **) builder->data;
  g_array_free (builder, FALSE);

  // And we're done
  SLOG_LEAVE ("returning");
  return methods;
} // gimpbus_pdb_methods

/**
 * Build introspection data for GimpBus administration.
 */
static GDBusInterfaceInfo *
gimpbus_admin_interface (void)
{
  // Build the interface
  return
    interface_new (GIMPBUS_ADMIN_INTERFACE,
      A (method_new ("about", 
                     NULL, 
                     A (arg_new ("description", "s", NULL), NULL),
                     NULL),
         method_new ("dump", NULL, NULL, NULL),
         method_new ("quit", 
                     NULL, 
                     A (arg_new ("message", "s", NULL), NULL),
                     NULL),
         NULL),
      NULL,
      NULL,
      NULL);
} // gimpbus_admin_interface

/**
 * Build introspection data for PDB.
 */
static GDBusInterfaceInfo *
gimpbus_pdb_interface (void)
{
  // Build the interface
  return
    interface_new (GIMPBUS_PDB_INTERFACE,
      (gpointer *) gimpbus_pdb_methods (),
      NULL,
      NULL,
      NULL);
} // gimpbus_pdb_interface

GDBusNodeInfo *
gimpbus_node (void)
{
  return 
    node_new (GIMPBUS_PATH,
      A (gimpbus_admin_interface (),
         gimpbus_pdb_interface (), 
         NULL),
      NULL,
      NULL);
} // gimpbus_node


// +-----------------+------------------------------------------------
// | Error Reporting |
// +-----------------+

/**
 * Note that a method is unimplemented.
 */
static void
gimpbus_error_unimplemented  (GDBusConnection       *connection,
                              const gchar           *method_name,
                              GVariant              *parameters,
                              GDBusMethodInvocation *invocation)
{
  g_dbus_method_invocation_return_error (invocation,
                                         quark,
                                         GIMPBUS_ERROR_UNIMPLEMENTED,
                                         "%s is unimplemented",
                                         method_name);
} // gimpbus_error_unimplemented


// +------------------+-----------------------------------------------
// | Message Handlers |
// +------------------+

static void
gimpbus_handle_admin_method_call (GDBusConnection       *connection,
                                  const gchar           *method_name,
                                  GVariant              *parameters,
                                  GDBusMethodInvocation *invocation)
{
  // A hack to make it easier to rearrange the gimpbus_handlers
  if (FALSE)
    ;
  
  // Return the "about" message
  else if (g_strcmp0 (method_name, "about") == 0)
    {
      g_dbus_method_invocation_return_value (invocation,
                                             g_variant_new ("(s)", 
                                                            GIMPBUS_ABOUT));
    } // about

  // Dump the XML
  else if (g_strcmp0 (method_name, "dump") == 0)
    {
      dump_node (introspection_data);
      g_dbus_method_invocation_return_value (invocation, NULL);
    } // dump

  // Quit the program
  else if (g_strcmp0 (method_name, "quit") == 0)
    {
      g_dbus_method_invocation_return_value (invocation,
                                             g_variant_new ("(s)", "goodbye"));
      g_main_loop_quit (loop);
    } // quit

  // Everything else is unimplemented
  else
    {
      gimpbus_error_unimplemented (connection, 
                                   method_name, 
                                   parameters, 
                                   invocation);
    } // else
} // gimpbus_handle_admin_method_call
    
static void
gimpbus_handle_pdb_method_call (GDBusConnection       *connection,
                                const gchar           *method_name,
                                GVariant              *parameters,
                                GDBusMethodInvocation *invocation)
{
  SLOG_ENTER ("gimpbus_handle_pdb_method_call (%p, %s, %s, %p)",
              connection,
              method_name,
              g_variant_get_type_string (parameters),
              invocation);

  // Lots and lots and lots of fields for getting info on the procedure.
  gchar           *proc_name;
  gchar           *proc_blurb;
  gchar           *proc_help;
  gchar           *proc_author;
  gchar           *proc_copyright;
  gchar           *proc_date;
  GimpPDBProcType  proc_type;
  GimpParamDef    *formals;
  gint             nparams;
  GimpParamDef    *return_types;
  gint             nreturn_vals;
  GimpParam       *actuals = NULL;   // The arguments to the call.
  GimpParam       *values = NULL;    // The return values from the call.
  gint             nvalues;          // Number of return values.
  GVariant        *result;

  // Build the procname
  proc_name = strrep (g_strdup (method_name), '_', '-');

  // Look up the information on the procedure in the PDB
  if (! gimp_procedural_db_proc_info (proc_name,
                                      &proc_blurb,
                                      &proc_help,
                                      &proc_author,
                                      &proc_copyright,
                                      &proc_date,
                                      &proc_type,
                                      &nparams, &nreturn_vals,
                                      &formals, &return_types))
    {
      SLOG_LEAVE ("exiting: %s does not exist", proc_name);
      g_dbus_method_invocation_return_error (invocation,
                                             quark,
                                             GIMPBUS_ERROR_NOPDBMETHOD,
                                             "%s is not a PDB procedure",
                                             method_name);
      return;
    } // if we can't get the information

  // Check the number of parameters
  if (nparams != g_variant_n_children (parameters))
    {
      SLOG_LEAVE ("exiting: incorrect number of parameters for %s "
                  "(expected %d, got %d)", 
                  proc_name, nparams, g_variant_n_children (parameters));
      g_dbus_method_invocation_return_error (invocation,
        quark,
        GIMPBUS_ERROR_PARAMCOUNT,
        "Incorrect number of parameters for %s (expeted %d, got %d)",
        proc_name, nparams, g_variant_n_children (parameters));
      return;
    } // if the number of parameters does not match

  // Okay, we're ready to build the parameters
  if (! gimpbus_g_variant_to_gimp_params (parameters, formals, &actuals))
    {
      SLOG_LEAVE ("exiting: unable to convert parameters");
      g_dbus_method_invocation_return_error (invocation,
        quark,
        GIMPBUS_ERROR_PARAMTYPE,
        "Unable to convert parameters");
      return;
    } // if we could not build the parameter list

  // Do the call
  values = gimp_run_procedure2 (proc_name, &nvalues, nparams, actuals);

  // Check for potential errors.  TODO
  if ( (values == NULL) 
       || (values[0].data.d_status != GIMP_PDB_SUCCESS) )
    {
      SLOG_LEAVE ("exiting: PDB error %d", values[0].data.d_status);
      g_dbus_method_invocation_return_error (invocation,
        quark,
        GIMPBUS_ERROR_MISCELLANEOUS,
        "PDB call failed with error %d",
        values[0].data);
      return;
    } 

  // Convert the values back to a GVariant
  result = gimpbus_gimp_params_to_g_variant (values+1, nvalues-1);
  if (result == NULL)
    {
      SLOG_LEAVE ("exiting: could not convert return values");
      g_dbus_method_invocation_return_error (invocation,
        quark,
        GIMPBUS_ERROR_MISCELLANEOUS,
        "could not convert return values");
      return;
    } // if (result == NULL)

  // Return via DBus
  g_dbus_method_invocation_return_value (invocation, result);

  // Cleanup: TODO
  g_variant_unref (result);
} // gimpbus_handle_pdb_method_call
    
static void
gimpbus_handle_method_call (GDBusConnection       *connection,
                            const gchar           *sender,
                            const gchar           *object_path,
                            const gchar           *interface_name,
                            const gchar           *method_name,
                            GVariant              *parameters,
                            GDBusMethodInvocation *invocation,
                            gpointer               user_data)
{
  SLOG_ENTER ("gimpbus_handle_method_call (CONNECTION, %s, %s, %s, %s, %s, INFOCATION, USER_DATA)",
              sender, object_path, interface_name, method_name,
              g_variant_get_type_string (parameters));

  // Administrative methods
  if (! g_strcmp0 (interface_name, GIMPBUS_ADMIN_INTERFACE))
    {
      gimpbus_handle_admin_method_call (connection, 
                                        method_name, 
                                        parameters, 
                                        invocation);
    }

  // PDB methods
  else if (! g_strcmp0 (interface_name, GIMPBUS_PDB_INTERFACE))
    {
      gimpbus_handle_pdb_method_call (connection, 
                                      method_name, 
                                      parameters, 
                                      invocation);
    }

  // Everything else 
  else
    {
      gimpbus_error_unimplemented (connection, 
                                   method_name, 
                                   parameters, 
                                   invocation);
    } // default

  SLOG_LEAVE ("finished");
} // gimpbus_handle_method_call

static GVariant *
gimpbus_handle_get_property (GDBusConnection  *connection,
                             const gchar      *sender,
                             const gchar      *object_path,
                             const gchar      *interface_name,
                             const gchar      *property_name,
                             GError          **error,
                             gpointer          user_data)
{
  return NULL;
} // gimpbus_handle_get_property

static gboolean
gimpbus_handle_set_property (GDBusConnection  *connection,
                             const gchar      *sender,
                             const gchar      *object_path,
                             const gchar      *interface_name,
                             const gchar      *property_name,
                             GVariant         *value,
                             GError          **error,
                             gpointer          user_data)
{
  return FALSE;
} // gimpbus_handle_set_property

/**
 * The table of DBus gimpbus_handlers.
 */
static const GDBusInterfaceVTable interface_vtable =
{
  gimpbus_handle_method_call,
  gimpbus_handle_get_property,
  gimpbus_handle_set_property
};


// +----------+-------------------------------------------------------
// | Handlers |
// +----------+

static void
on_bus_acquired (GDBusConnection *connection,
                 const gchar     *name,
                 gpointer         user_data)
{
  SLOG_ENTER ("on_bus_acquired (%p, %s, %p)", connection, name, user_data);
  guint registration_id;

  registration_id = 
    g_dbus_connection_register_object (connection,
                                       GIMPBUS_ADMIN_PATH,
                                       introspection_data->interfaces[0],
                                       &interface_vtable,
                                       NULL,  /* user_data */
                                       NULL,  /* user_data_free_func */
                                       NULL); /* GError** */
  g_assert (registration_id > 0);

  registration_id = 
    g_dbus_connection_register_object (connection,
                                       GIMPBUS_PDB_PATH,
                                       introspection_data->interfaces[1],
                                       &interface_vtable,
                                       NULL,  /* user_data */
                                       NULL,  /* user_data_free_func */
                                       NULL); /* GError** */
  g_assert (registration_id > 0);

  SLOG_LEAVE ("finished");
} // on_bus_acquired

static void
on_name_acquired (GDBusConnection *connection,
                  const gchar     *name,
                  gpointer         user_data)
{
  SLOG_ENTER ("on_name_acquired (%p, %s, %p)", connection, name, user_data);
  SLOG_LEAVE ("done");
} // on_name_acquired

static void
on_name_lost (GDBusConnection *connection,
              const gchar     *name,
              gpointer         user_data)
{
  SLOG_ENTER ("on_name_lost (%p, %s, %p)", connection, name, user_data);
  SLOG_LEAVE ("done");
  exit (1);
} // on_name_lost


// +---------------+--------------------------------------------------
// | Miscellaneous |
// +---------------+

/**
 * Set up the quark
 */
void
quark_setup (void)
{
  static volatile gsize quark_volatile = 0;
  g_dbus_error_register_error_domain ("org-glimmer-error-quark",
                                      &quark_volatile,
                                      error_entries,
                                      G_N_ELEMENTS (error_entries));
  quark = quark_volatile;
} // quark_setup


// +----------------------------------+-------------------------------
// | Standard Gimp Plugin Boilerplate |
// +----------------------------------+

static void gimpbus_query (void);
static void gimpbus_quit (void);
static void gimpbus_run   (const gchar      *name,
                           gint              nparams,
                           const GimpParam  *param,
                           gint             *nreturn_vals,
                           GimpParam       **return_vals);

GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,
  gimpbus_quit,
  gimpbus_query,
  gimpbus_run
};

MAIN ()


// +-------------------------------+----------------------------------
// | The Key Gimp Plugin Functions |
// +-------------------------------+

static void
gimpbus_query (void)
{
  SLOG_ENTER ("gimpbus_query ()");
  // The arguments that the plug-in expects
  static GimpParamDef args[] =
  {
    { GIMP_PDB_INT32, "run-mode", "Run mode" }
  };

  // Install this plug-in.
  gimp_install_procedure (
    "gimpbus-pdb",
    "A D-Bus PDB Server",
    "Provides access to PDB procedures through D-Bus",
    "Samuel A. Rebelsky",
    "Copyright (c) Samuel A. Rebelsky",
    "2011",
    "<Toolbox>/Xtns/GimpBus/PDB",
    NULL, 
    GIMP_PLUGIN,
    G_N_ELEMENTS (args), 0,
    args, NULL);

  SLOG_LEAVE ("finished");
} // gimpbus_query

static void
gimpbus_quit (void)
{
  SLOG_ENTER ("gimpbus_quit");
  if (loop != NULL)
    {
      g_main_loop_quit (loop);
      loop = NULL;
    }
  else
    {
    }
  SLOG_LEAVE ("returning");
} // gimpbus_quit

static void
gimpbus_run (const gchar      *name,
             gint              nparams,
             const GimpParam  *param,
             gint             *nreturn_vals,
             GimpParam       **return_vals)
{
  SLOG_ENTER ("gimpbus_run (...)");

  // Standard Plugin params
  static GimpParam  values[1];
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;
  GimpRunMode       run_mode;

  // Setting mandatory output values
  *nreturn_vals = 1;
  *return_vals  = values;

  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = status;

  // Getting run_mode
  run_mode = param[0].data.d_int32;

  if (run_mode != GIMP_RUN_NONINTERACTIVE)
    {
      SLOG_MESSAGE ("Interactive!");
      guint owner_id;
      GBusNameOwnerFlags flags;
      g_type_init ();

      // Set up the quark
      quark_setup ();

      // Build the introspection data
      introspection_data = gimpbus_node ();
    #ifdef DUMP
      dump_node (introspection_data);
    #endif // DUMP

      // Get the bus
      flags = G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT 
            | G_BUS_NAME_OWNER_FLAGS_REPLACE;

      SLOG_MESSAGE ("getting bus");
      owner_id = g_bus_own_name (G_BUS_TYPE_SESSION,
                                 GIMPBUS_SERVICE,
                                 flags,
                                 on_bus_acquired,
                                 on_name_acquired,
                                 on_name_lost,
                                 NULL,
                                 NULL);
      SLOG_MESSAGE ("got bus");
    
      // Main loop
      SLOG_MESSAGE ("building main loop");
      loop = g_main_loop_new (NULL, FALSE);
      SLOG_MESSAGE ("running main loop");
      g_main_loop_run (loop);
    
      // Clean up
      SLOG_MESSAGE ("loop seems to be done running");
      g_bus_unown_name (owner_id);
      SLOG_MESSAGE ("unowned name");
    } // if interactive
  SLOG_MESSAGE ("run is done");
} // gimpbus_run
