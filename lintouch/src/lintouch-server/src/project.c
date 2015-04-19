/* $Id: project.c 1774 2006-05-31 13:17:16Z hynek $
 *
 *   FILE: project.c --
 * AUTHOR: Martin Man <mman@swac.cz>
 *   DATE: 20 April 2005
 *
 * Copyright (c) 2001-2005 S.W.A.C. GmbH, Germany.
 * Copyright (c) 2001-2005 S.W.A.C. Bohemia s.r.o., Czech Republic.
 *
 * This application is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this application; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <apr_hash.h>
#include <apr_strings.h>
#include <apr_xml.h>

#include "lt/variables/variables.h"
#include "lt/server/project.h"
#include "lt/server/logger.h"

#include "ziparchive.h"

struct lt_project_t {
    apr_pool_t * pool;

    /* Project Info */
    const char * author;
    const char * shortdesc;
    const char * longdesc;
    const char * version;
    const char * date;

    /* Project Connections */
    apr_hash_t * connections;

    /* Project Views */
    apr_hash_t * views;

    /* Project Template Libraries */
    apr_hash_t * template_libraries;

    /* Project Bindings */
    apr_hash_t * bindings;

    /* Project Logging */
    apr_hash_t * logging;
};

struct lt_project_connection_t {
    apr_pool_t * pool;

    /* Connection Type - to determine what plugin should handle it */
    const char * type;

    /* Variables handled by this connection */
    lt_var_set_t * varset;

    /* Connection Properties: [ const char * prop -> const char * val ] */
    apr_hash_t * properties;

    /* Variable Properties: [ const char * var -> apr_hash_t * varprops ] */
    /*                varprops: [ const char * prop -> const char * val ] */
    apr_hash_t * varproperties;
};

struct lt_project_logging_t {
    apr_pool_t * pool;

    /* Logging Type - to determine what plugin should handle it */
    const char * type;

    /* Logging Properties: [ const char * prop -> const char * val ] */
    apr_hash_t * properties;
};

/* LOCAL METHODS */

/**
 * Parse out attributes of given XML element.
 *
 * @param elem The element whose attributes should be parsed.
 * @param attr1 The attribute name. Can be null.
 * @param val1 The parsed attribute value.
 * @param attr2 The attribute name. Can be null.
 * @param val2 The parsed attribute value.
 * @param attr3 The attribute name. Can be null.
 * @param val3 The parsed attribute value.
 * @param pool The pool to allocate strings out of.
 * @param val1 The parsed attribute value.
 * @return 1 if the attributes are there, 0 otherwise.
 */
static int _lt_project_xml_elem_attrs_parse( apr_xml_elem * elem,
        const char * attr1, const char ** val1,
        const char * attr2, const char ** val2,
        const char * attr3, const char ** val3,
        apr_pool_t * pool );

/**
 * Parse out CDATA of given XML elem.
 *
 * @param elem The element whose cdata should be parsed.
 * @param pool The pool to allocate strings out of.
 * @return The parsed and merged cdata of a element.
 */
static const char * _lt_project_xml_elem_cdata_parse( apr_xml_elem * elem,
        apr_pool_t * pool );

/**
 * Parse out connection data.
 *
 * @param connection The parsed connection.
 * @param elem The root element containing the connection descriptor.
 * @param pool The pool to allocate connection out of.
 * @return LT_PROJECT_SUCCESS when everything goes well.
 */
lt_project_status_t  _lt_project_connection_parse(
        lt_project_connection_t ** connection,
        apr_xml_elem * elem, apr_pool_t * pool );

/**
 * Parse out logging data.
 *
 * @param l The parsed logging plugin.
 * @param elem The root element containing the logging descriptor.
 * @param pool The pool to allocate logging out of.
 * @return LT_PROJECT_SUCCESS when everything goes well.
 */
lt_project_status_t  _lt_project_logging_parse(
        lt_project_logging_t ** l,
        apr_xml_elem * elem, apr_pool_t * pool );

apr_pool_t * lt_project_pool_get( const lt_project_t * src )
{
    if( src == NULL ) return NULL;
    return src->pool;
}

#define CREATE_POOL_IF_NULL(p)  if((p)==NULL)apr_pool_create(&(p),NULL)

lt_project_status_t lt_project_parse( lt_project_t ** project,
        const char * archive, int what_to_parse,
        apr_pool_t * pool )
{
    apr_file_t * file = NULL;

    /* variables needed for XML parsing */
    apr_pool_t * ppool = NULL;
    apr_xml_doc * pd = NULL;
    apr_xml_parser * parser = NULL;

    CREATE_POOL_IF_NULL( pool );

    *project = ( lt_project_t * ) apr_pcalloc( pool,
            sizeof( lt_project_t ) );

    /* allocate & setup output structure */
    ( *project )->pool = pool;
    ( *project )->connections = apr_hash_make( pool );
    ( *project )->views = apr_hash_make( pool );
    ( *project )->template_libraries = apr_hash_make( pool );
    ( *project )->bindings = apr_hash_make( pool );
    ( *project )->logging = apr_hash_make( pool );

    /* nothing to parse, return the empty project */
    if( what_to_parse == 0 ) return LT_PROJECT_SUCCESS;

    /* prepare XML parsing related data */
    apr_pool_create( &ppool, pool );

    lt_server_logger_print( PRIO_PROJECT,
            "Going to parse project-descriptor.xml\n" );

    /* try to extract & open project-descriptor.xml */
    file = lt_server_zip_extract_file(
            archive, "project-descriptor.xml", ppool );
    if( file == NULL ) {
        lt_server_logger_print( PRIO_DEFAULT,
                "Unable to extract project-descriptor.xml from archive %s\n",
                archive );
        apr_pool_destroy( ppool );
        return LT_PROJECT_INVALID_ARG;
    }

    /* try to parse project-descriptor.xml */
    if( APR_SUCCESS != apr_xml_parse_file( ppool, &parser,
                    &pd, file, 2048 ) ) {
        lt_server_logger_print_apr_xml( PRIO_DEFAULT, parser,
                "Unable to parse project-descriptor.xml from archive %s\n",
                archive );
        apr_pool_destroy( ppool );
        return LT_PROJECT_INVALID_ARG;
    }

    /* close project-descriptor.xml */
    apr_file_close( file ); file = NULL;

    /* parse out Project Info */
    if( what_to_parse & LT_PROJECT_INFO ) {
        /* try to find info node */
        apr_xml_elem * i = NULL;
        for( i = pd->root->first_child; i != NULL; i = i->next ) {
            if( apr_strnatcmp( "project-info", i->name ) == 0 ) break;
        }
        /* found it */
        if( i != NULL ) {
            apr_xml_elem * j = NULL;
            for( j = i->first_child; j != NULL; j = j->next ) {
                if( apr_strnatcmp( "author", j->name ) == 0 ) {
                    ( *project )->author =
                        _lt_project_xml_elem_cdata_parse( j, pool );
                }
                if( apr_strnatcmp( "version", j->name ) == 0 ) {
                    ( *project )->version =
                        _lt_project_xml_elem_cdata_parse( j, pool );
                }
                if( apr_strnatcmp( "date", j->name ) == 0 ) {
                    ( *project )->date =
                        _lt_project_xml_elem_cdata_parse( j, pool );
                }
                if( apr_strnatcmp( "shortdesc", j->name ) == 0 ) {
                    ( *project )->shortdesc =
                        _lt_project_xml_elem_cdata_parse( j, pool );
                }
                if( apr_strnatcmp( "longdesc", j->name ) == 0 ) {
                    ( *project )->longdesc =
                        _lt_project_xml_elem_cdata_parse( j, pool );
                }
            }
        }
    }

    /* parse out Project Connections */
    if( what_to_parse & LT_PROJECT_CONNECTIONS ) {
        /* try to find connections node */
        apr_xml_elem * i = NULL;
        apr_xml_elem * j = NULL;
        for( i = pd->root->first_child; i != NULL; i = i->next ) {
            if( apr_strnatcmp( "project-connections", i->name ) == 0 ) break;
        }
        if( i == NULL ) {
            lt_server_logger_print( PRIO_PROJECT,
                    "Project %s does not define any connections\n", archive );
            apr_pool_destroy( ppool );
        } else {
            /* found it */
            for( j = i->first_child; j != NULL; j = j->next ) {
                if( apr_strnatcmp( "connection", j->name ) == 0 ) {
                    const char * name = NULL, * src = NULL;
                    apr_xml_doc * cd = NULL;
                    lt_project_connection_t * connection = NULL;


                    if( ! _lt_project_xml_elem_attrs_parse(
                                j, "name", &name, "src", &src,
                                NULL, NULL, ppool ) ) {
                        lt_server_logger_print( PRIO_PROJECT,
                                "<connection> element does not "
                                "define required "
                                "``name`` and ``src`` attributes\n" );
                        apr_pool_destroy( ppool );
                        return LT_PROJECT_INVALID_ARG;
                    }

                    lt_server_logger_print( PRIO_PROJECT,
                            "Going to parse connection %s defined in %s\n",
                            name, src );

                    /* try to extract given connection-descriptor.xml */
                    file = lt_server_zip_extract_file(
                            archive, src, ppool );
                    if( file == NULL ) {
                        lt_server_logger_print( PRIO_DEFAULT,
                                "Unable to extract %s from archive %s\n",
                                src, archive );
                        apr_pool_destroy( ppool );
                        return LT_PROJECT_INVALID_ARG;
                    }

                    /* try to parse connection-descriptor.xml */
                    if( APR_SUCCESS !=
                                apr_xml_parse_file( ppool, &parser,
                                    &cd, file, 2048 ) ) {
                        lt_server_logger_print_apr_xml( PRIO_DEFAULT, parser,
                                "Unable to parse %s from archive %s\n",
                                src, archive );
                        apr_pool_destroy( ppool );
                        return LT_PROJECT_INVALID_ARG;
                    }

                    /* close connection-descriptor.xml */
                    apr_file_close( file ); file = NULL;

                    /* extract interesting information out of connection
                     * descriptor */
                    if( LT_PROJECT_SUCCESS != _lt_project_connection_parse(
                                &connection, cd->root, pool ) ) {
                        lt_server_logger_print( PRIO_DEFAULT,
                                "Unable to parse connection %s\n",
                                name );
                        apr_pool_destroy( ppool );
                        return LT_PROJECT_INVALID_ARG;
                    }

                    /* register newly created connection */
                    apr_hash_set( ( *project )->connections,
                            apr_pstrdup( pool, name),
                            APR_HASH_KEY_STRING, connection );

                }
            } /* connection */
        }
    }

    /* parse out Project Logging */
    if( what_to_parse & LT_PROJECT_LOGGING ) {
        /* try to find logging node */
        apr_xml_elem * i = NULL;
        apr_xml_elem * j = NULL;
        for( i = pd->root->first_child; i != NULL; i = i->next ) {
            if( apr_strnatcmp( "project-logging", i->name ) == 0 ) break;
        }
        if( i == NULL ) {
            lt_server_logger_print( PRIO_PROJECT,
                    "Project %s does not define any logging\n", archive );
            apr_pool_destroy( ppool );
        } else {
        /* found it */
            for( j = i->first_child; j != NULL; j = j->next ) {
                if( apr_strnatcmp( "logging", j->name ) == 0 ) {
                    const char * name = NULL, * src = NULL;
                    apr_xml_doc * cd = NULL;
                    lt_project_logging_t * l = NULL;


                    if( ! _lt_project_xml_elem_attrs_parse(
                                j, "name", &name, "src", &src, 
                                NULL, NULL, ppool ) ) {
                        lt_server_logger_print( PRIO_PROJECT,
                                "<logging> element does not define required "
                                "``name`` and ``src`` attributes\n" );
                        apr_pool_destroy( ppool );
                        return LT_PROJECT_INVALID_ARG;
                    }

                    lt_server_logger_print( PRIO_PROJECT,
                            "Going to parse logging %s defined in %s\n",
                            name, src );

                    /* try to extract given logging-descriptor.xml */
                    file = lt_server_zip_extract_file(
                            archive, src, ppool );
                    if( file == NULL ) {
                        lt_server_logger_print( PRIO_DEFAULT,
                                "Unable to extract %s from archive %s\n",
                                src, archive );
                        apr_pool_destroy( ppool );
                        return LT_PROJECT_INVALID_ARG;
                    }

                    /* try to parse logging-descriptor.xml */
                    if( APR_SUCCESS !=
                                apr_xml_parse_file( ppool, &parser,
                                    &cd, file, 2048 ) ) {
                        lt_server_logger_print_apr_xml( PRIO_DEFAULT, parser,
                                "Unable to parse %s from archive %s\n",
                                src, archive );
                        apr_pool_destroy( ppool );
                        return LT_PROJECT_INVALID_ARG;
                    }

                    /* close logging-descriptor.xml */
                    apr_file_close( file ); file = NULL;

                    /* extract interesting information out of logging
                     * descriptor */
                    if( LT_PROJECT_SUCCESS != _lt_project_logging_parse(
                                &l, cd->root, pool ) ) {
                        lt_server_logger_print( PRIO_DEFAULT,
                                "Unable to parse logging %s\n",
                                name );
                        apr_pool_destroy( ppool );
                        return LT_PROJECT_INVALID_ARG;
                    }

                    /* register newly created logging */
                    apr_hash_set( ( *project )->logging,
                            apr_pstrdup( pool, name),
                            APR_HASH_KEY_STRING, l );

                } /* logging */
            }
        }
    }

    apr_pool_destroy( ppool );
    return LT_PROJECT_SUCCESS;
}

const char * lt_project_author_get( lt_project_t * project )
{
    if( project == NULL ) return NULL;
    return project->author;
}

const char * lt_project_version_get( lt_project_t * project )
{
    if( project == NULL ) return NULL;
    return project->version;
}

const char * lt_project_date_get( lt_project_t * project )
{
    if( project == NULL ) return NULL;
    return project->date;
}

const char * lt_project_shortdesc_get( lt_project_t * project )
{
    if( project == NULL ) return NULL;
    return project->shortdesc;
}

const char * lt_project_longdesc_get( lt_project_t * project )
{
    if( project == NULL ) return NULL;
    return project->longdesc;
}

apr_hash_t * lt_project_connections_get( lt_project_t * project )
{
    if( project == NULL ) return NULL;
    return project->connections;
}

const char * lt_project_connection_type_get(
        lt_project_connection_t * conn )
{
    if( conn == NULL ) return NULL;
    return conn->type;
}

apr_hash_t * lt_project_connection_properties_get(
        lt_project_connection_t * conn )
{
    if( conn == NULL ) return NULL;
    return conn->properties;
}

apr_hash_t * lt_project_connection_varproperties_get(
        lt_project_connection_t * conn )
{
    if( conn == NULL ) return NULL;
    return conn->varproperties;
}

lt_var_set_t * lt_project_connection_variables_get(
        lt_project_connection_t * conn )
{
    if( conn == NULL ) return NULL;
    return conn->varset;
}

apr_hash_t * lt_project_logging_get( lt_project_t * project )
{
    if( project == NULL ) return NULL;
    return project->logging;
}

const char * lt_project_logging_type_get( lt_project_logging_t * l )
{
    if( l == NULL ) return NULL;
    return l->type;
}

apr_hash_t * lt_project_logging_properties_get( lt_project_logging_t * l )
{
    if( l == NULL ) return NULL;
    return l->properties;
}


/* LOCAL METHODS */

int _lt_project_xml_elem_attrs_parse( apr_xml_elem * elem,
        const char * attr1, const char ** val1,
        const char * attr2, const char ** val2,
        const char * attr3, const char ** val3,
        apr_pool_t * pool )
{
    apr_xml_attr * i = NULL;
    char at1 = 0, at2 = 0, at3 = 0;

    if( elem == NULL ) return 0;

    for( i = elem->attr; i != NULL; i = i->next ) {
        if( attr1 != NULL ) {
            if( apr_strnatcmp( attr1, i->name ) == 0 ) {
                *val1 = apr_pstrdup( pool, i->value );
                at1 = 1;
            }
        } else {
            at1 = 1;
        }
        if( attr2 != NULL ) {
            if( apr_strnatcmp( attr2, i->name ) == 0 ) {
                *val2 = apr_pstrdup( pool, i->value );
                at2 = 1;
            }
        } else {
            at2 = 1;
        }
        if( attr3 != NULL ) {
            if( apr_strnatcmp( attr3, i->name ) == 0 ) {
                *val3 = apr_pstrdup( pool, i->value );
                at3 = 1;
            }
        } else {
            at3 = 1;
        }
    }

    return at1 && at2 && at3;
}

const char * _lt_project_xml_elem_cdata_parse( apr_xml_elem * elem,
        apr_pool_t * pool )
{
    apr_pool_t * tmppool = NULL;
    const char * result = "";

    if( elem == NULL ) return NULL;
    if( elem->first_cdata.first == NULL ) return NULL;
    if( elem->first_cdata.first->text == NULL ) return NULL;

    /* FIXME: this algo is not very efficient for elements whose cdata
     * sections contain a lot of entities that are splitted into text
     * fragments by the parser, figure out how to speed this all up without
     * eating too much memory */

    /* create dummy pool used for concatenation of text fragments */
    apr_pool_create( &tmppool, pool );

    /* concat fragments while allocating result string from a temporary pool
     * */
    {
        apr_text * i = elem->first_cdata.first;
        for( ; i != NULL; i = i->next ) {
            result = apr_pstrcat( tmppool, result, i->text, NULL );
        }
    }

    result = apr_pstrdup( pool, result );
    apr_pool_destroy( tmppool );
    return result;
}

lt_project_status_t  _lt_project_connection_parse(
        lt_project_connection_t ** connection,
        apr_xml_elem * elem, apr_pool_t * pool )
{
    const char * type = NULL;
    if( elem == NULL ) return LT_PROJECT_INVALID_ARG;

    /* allocate & setup output structure */
    *connection = ( lt_project_connection_t * ) apr_pcalloc( pool,
            sizeof( lt_project_connection_t ) );

    ( *connection )->pool = pool;
    ( *connection )->properties = apr_hash_make( pool );
    ( *connection )->varproperties = apr_hash_make( pool );
    ( *connection )->varset = lt_var_set_create( 0, pool );

    /* check top-level tag name */
    if( apr_strnatcmp( "project-connection", elem->name ) != 0 ) {
        lt_server_logger_print( PRIO_PROJECT,
                "Connection does not define required top-level"
                " element <project-connection>\n" );
        return LT_PROJECT_INVALID_ARG;
    }

    /* get connection type */
    if( ! _lt_project_xml_elem_attrs_parse( elem, "type", &type,
                NULL, NULL, NULL, NULL, pool ) ) {
        lt_server_logger_print( PRIO_PROJECT,
                "<project-connection> element does not define required "
                "``type`` attribute\n" );
        return LT_PROJECT_INVALID_ARG;
    }
    ( *connection )->type = type;

    /* parse out Connection Properties */
    {
        /* try to find properties node */
        apr_xml_elem * i = NULL;
        for( i = elem->first_child; i != NULL; i = i->next ) {
            if( apr_strnatcmp( "properties", i->name ) == 0 ) break;
        }
        /* found it */
        if( i != NULL ) {
            apr_xml_elem * j = NULL;
            for( j = i->first_child; j != NULL; j = j->next ) {
                if( apr_strnatcmp( "property", j->name ) == 0 ) {
                    const char * name = NULL, * value = NULL;
                    if( ! _lt_project_xml_elem_attrs_parse( j,
                                "name", &name, "value", &value,
                                NULL, NULL, pool ) ) {
                        lt_server_logger_print( PRIO_PROJECT,
                                "<property> element does not define required "
                                "``name`` and ``value`` attributes\n" );
                        return LT_PROJECT_INVALID_ARG;
                    }

                    lt_server_logger_print( PRIO_PROJECT,
                            "Connection property %s set to %s\n",
                            name, value );

                    apr_hash_set( ( *connection )->properties,
                            name, APR_HASH_KEY_STRING, value );
                }
            }
        }
    }

    /* parse out Variables */
    {
        /* try to find variables node */
        apr_xml_elem * i = NULL;
        apr_xml_elem * j = NULL;
        for( i = elem->first_child; i != NULL; i = i->next ) {
            if( apr_strnatcmp( "variables", i->name ) == 0 ) break;
        }

        if( i == NULL ) {
            lt_server_logger_print( PRIO_PROJECT,
                    "Connection does not define any variables\n" );
            return LT_PROJECT_SUCCESS;
        }

        /* found it */
        for( j = i->first_child; j != NULL; j = j->next ) {
            if( apr_strnatcmp( "variable", j->name ) == 0 ) {
                const char * name = NULL, * type = NULL;
                lt_var_t * var = NULL;
                apr_hash_t * varprops = NULL;
                apr_xml_elem * k = NULL;

                /* parse out variable name & type */
                if( ! _lt_project_xml_elem_attrs_parse( j,
                            "name", &name, "type", &type,
                            NULL, NULL, pool ) ) {
                    lt_server_logger_print( PRIO_PROJECT,
                            "<variable> element does not define required "
                            "``name`` and ``type`` attributes\n" );
                    return LT_PROJECT_INVALID_ARG;
                }

                lt_server_logger_print( PRIO_PROJECT,
                        "Going to parse variable %s of type %s\n",
                        name, type );

                if( apr_strnatcmp( "bit", type ) == 0 ) {
                    var = lt_var_create( LT_IO_TYPE_BIT, 1, pool );
                } else if( apr_strnatcmp( "number", type ) == 0 ) {
                    var = lt_var_create( LT_IO_TYPE_NUMBER, 1, pool );
                } else if( apr_strnatcmp( "string", type ) == 0 ) {
                    var = lt_var_create( LT_IO_TYPE_STRING, 1, pool );
                } else {
                    lt_server_logger_print( PRIO_PROJECT,
                            "<variable> element defines unknown "
                            "variable type %s, using null instead\n", type );
                    var = lt_var_create( LT_IO_TYPE_NULL, 1, pool );
                }

                /* register newly created variable to a varset */
                if( LT_VAR_SUCCESS != lt_var_set_variable_register(
                            ( *connection )->varset, name, var ) ) {
                    lt_server_logger_print( PRIO_PROJECT,
                            "Unable to register newly created "
                            "variable %s of type %s "
                            "to the connection varset\n", name, type );
                    return LT_PROJECT_INVALID_ARG;
                }

                varprops = apr_hash_make( pool );
                apr_hash_set( ( *connection )->varproperties,
                        name, APR_HASH_KEY_STRING, varprops );

                /* parse out variable properties */
                for( k = j->first_child; k != NULL; k = k->next ) {
                    if( apr_strnatcmp( "property", k->name ) == 0 ) {
                        const char * pname = NULL, * pvalue = NULL;
                        if( ! _lt_project_xml_elem_attrs_parse( k,
                                    "name", &pname, "value", &pvalue,
                                    NULL, NULL, pool ) ) {
                            lt_server_logger_print( PRIO_PROJECT,
                                    "<property> element does not define"
                                    " required "
                                    "``name`` and ``value`` attributes\n" );
                            return LT_PROJECT_INVALID_ARG;

                        }

                        lt_server_logger_print( PRIO_PROJECT,
                                "Variable %s property %s set to %s\n",
                                name, pname, pvalue );

                        apr_hash_set( varprops,
                                pname, APR_HASH_KEY_STRING, pvalue );
                    }
                } /* variable properties */
            } /* variable */
        }
    }
    return LT_PROJECT_SUCCESS;
}

lt_project_status_t  _lt_project_logging_parse(
        lt_project_logging_t ** l,
        apr_xml_elem * elem, apr_pool_t * pool )
{
    const char * type = NULL;
    if( elem == NULL ) return LT_PROJECT_INVALID_ARG;

    /* allocate & setup output structure */
    *l = ( lt_project_logging_t * ) apr_pcalloc( pool,
            sizeof( lt_project_logging_t ) );

    ( *l )->pool = pool;
    ( *l )->properties = apr_hash_make( pool );

    /* check top-level tag name */
    if( apr_strnatcmp( "project-logging", elem->name ) != 0 ) {
        lt_server_logger_print( PRIO_PROJECT,
                "Logging does not define required top-level"
                " element <project-logging>\n" );
        return LT_PROJECT_INVALID_ARG;
    }

    /* get logging type */
    if( ! _lt_project_xml_elem_attrs_parse( elem, "type", &type,
                NULL, NULL, NULL, NULL, pool ) ) {
        lt_server_logger_print( PRIO_PROJECT,
                "<project-logging> element does not define required "
                "``type`` attribute\n" );
        return LT_PROJECT_INVALID_ARG;
    }
    ( *l )->type = type;

    /* parse out Logging Properties */
    {
        /* try to find properties node */
        apr_xml_elem * i = NULL;
        for( i = elem->first_child; i != NULL; i = i->next ) {
            if( apr_strnatcmp( "properties", i->name ) == 0 ) break;
        }
        /* found it */
        if( i != NULL ) {
            apr_xml_elem * j = NULL;
            for( j = i->first_child; j != NULL; j = j->next ) {
                if( apr_strnatcmp( "property", j->name ) == 0 ) {
                    const char * name = NULL, * value = NULL;
                    if( ! _lt_project_xml_elem_attrs_parse( j,
                                "name", &name, "value", &value,
                                NULL, NULL, pool ) ) {
                        lt_server_logger_print( PRIO_PROJECT,
                                "<property> element does not define required "
                                "``name`` and ``value`` attributes\n" );
                        return LT_PROJECT_INVALID_ARG;
                    }

                    lt_server_logger_print( PRIO_PROJECT,
                            "Logging property %s set to %s\n",
                            name, value );

                    apr_hash_set( ( *l )->properties,
                            name, APR_HASH_KEY_STRING, value );
                }
            }
        }
    }
    return LT_PROJECT_SUCCESS;
}

/* vim: set et ts=4 sw=4 tw=76:
 * $Id: project.c 1774 2006-05-31 13:17:16Z hynek $
 */
