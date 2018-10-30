#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <mysql.h>
#include "mysqlex.h"


void mysqlex_list_init( struct mysqlex_list_entry *list )
{
	list->prev = list->next = list;
}

static inline void __mysqlex_list_add( struct mysqlex_list_entry *entry,
									   struct mysqlex_list_entry *prev,
									   struct mysqlex_list_entry *next )
{
	next->prev = entry;
	entry->next = next;
	entry->prev = prev;
	prev->next = entry;
}

void mysqlex_list_add_tail( struct mysqlex_list_entry *head,
										  struct mysqlex_list_entry *entry )
{
	__mysqlex_list_add( entry, head->prev, head );
}

void mysqlex_list_add_head( struct mysqlex_list_entry *head,
										  struct mysqlex_list_entry *entry )
{
	__mysqlex_list_add( entry, head, head->next );
}

static inline void __mysqlex_list_del( struct mysqlex_list_entry * prev,
									   struct mysqlex_list_entry * next )
{
	next->prev = prev;
	prev->next = next;
}

void mysqlex_list_del( struct mysqlex_list_entry *entry )
{
	__mysqlex_list_del( entry->prev, entry->next );
	entry->next = NULL;
	entry->prev = NULL;
}

bool mysqlex_list_empty( const struct mysqlex_list_entry *head )
{
	return head->next == head;
}

int mysqlex_list_is_last( const struct mysqlex_list_entry *list,
										const struct mysqlex_list_entry *head )
{
	return list->next == head;
}


void mysqlex_error( const char *error, const char *file, int line )
{
	printf( "FATAL ERROR:\n%s\n%s:%d\n", error, file, line );
	abort( );
}

void mysqlex_column_init( struct mysqlex_column *column )
{
	// memset( column, 0, sizeof( struct mysqlex_column ) );

	column->initialized = true;
}

static inline void *_memdup( void *buffer, size_t length )
{
	void *data = malloc( length );

	if ( data )
	{
		memcpy( data, buffer, length );
	}

	return data;
}

void mysqlex_column_format( struct mysqlex_column *output_data, MYSQL_BIND *input_data )
{
	output_data->length = *input_data->length;
	output_data->buffer = _memdup( input_data->buffer, output_data->length );
	if ( output_data->buffer == NULL )
	{
		mysqlex_error( "_memdup failed", __FILE__, __LINE__ );
	}
	output_data->buffer_type = input_data->buffer_type;
	output_data->is_unsigned = input_data->is_unsigned;
	output_data->is_null = input_data->is_null_value;
}

void mysqlex_column_close( struct mysqlex_column *column )
{
	if ( column->initialized )
	{
		if ( column->buffer )
		{
			free( column->buffer );
			column->buffer = NULL;
		}

		column->initialized = false;
	}

}

struct mysqlex_result * mysqlex_result_new( )
{
	struct mysqlex_result *result = (struct mysqlex_result *)calloc( 1, sizeof( struct mysqlex_result ) );
	result->cursor = 0;
	return result;
}

void mysqlex_clear_field( MYSQL_FIELD *field )
{
	if ( field->name )
	{
		free( field->name );
		field->name = NULL;
	}

	if ( field->org_name )
	{
		free( field->org_name );
		field->org_name = NULL;
	}

	if ( field->table )
	{
		free( field->table );
		field->table = NULL;
	}

	if ( field->org_table )
	{
		free( field->org_table );
		field->org_table = NULL;
	}

	if ( field->db )
	{
		free( field->db );
		field->db = NULL;
	}

	if ( field->catalog )
	{
		free( field->catalog );
		field->catalog = NULL;
	}

	if ( field->def )
	{
		free( field->def );
		field->def = NULL;
	}
}

void mysqlex_result_free( struct mysqlex_result *result )
{
	my_ulonglong i;
	if ( result->elements )
	{
		for ( i = 0; i < result->num_elements; i++ )
		{
			mysqlex_column_close( &result->elements[i] );
		}

		free( result->elements );
		result->elements = NULL;
	}

	if ( result->rows )
	{
		free( result->rows );
		result->rows = NULL;
	}

	if ( result->fields )
	{
		for ( i = 0; i < result->field_count; i++ )
		{
			mysqlex_clear_field( &result->fields[i] );
		}

		free( result->fields );
		result->fields = NULL;
	}

	free( result );
}



struct mysqlex_resultset * mysqlex_resultset_new( )
{
	struct mysqlex_resultset *rs = malloc( sizeof( struct mysqlex_resultset ) );

	if ( rs == NULL )
	{
		mysqlex_error( "mysqlex_resultset_new rs == NULL", __FILE__, __LINE__ );
	}

	mysqlex_list_init( &rs->results );
	rs->num_results = 0;
	rs->current = NULL;

	return rs;
}

void mysqlex_resultset_free( struct mysqlex_resultset *rs )
{
	struct mysqlex_result *r;

	while ( !mysqlex_list_empty( &rs->results ) )
	{
		r = (struct mysqlex_result *)rs->results.next;

		mysqlex_list_del( &r->entry );

		mysqlex_result_free( r );
	}

	free( rs );
}


void mysqlex_resultset_add_result( struct mysqlex_resultset *rs, struct mysqlex_result *r )
{
	mysqlex_list_add_tail( &rs->results, &r->entry );
	rs->num_results++;
}

enum enum_field_types mysqlex_f2t( char type, bool *is_unsigned )
{
	enum enum_field_types type2 = MYSQL_TYPE_NULL;

	if ( is_unsigned )
	{
		*is_unsigned = false;
	}

	switch ( type )
	{
		case 'c':
			type2 = MYSQL_TYPE_TINY;
			break;
		case 'x':
			type2 = MYSQL_TYPE_TINY;
			if ( is_unsigned ) *is_unsigned = true;
			break;
		case 'o':
			type2 = MYSQL_TYPE_SHORT;
			break;
		case 'z':
			type2 = MYSQL_TYPE_SHORT;
			if ( is_unsigned ) *is_unsigned = true;
			break;
		case 'u':
			type2 = MYSQL_TYPE_LONG;
			if ( is_unsigned ) *is_unsigned = true;
			break;
		case 'i':
			type2 = MYSQL_TYPE_LONG;
			break;
		case 's':
			type2 = MYSQL_TYPE_STRING;
			break;
		case 'I':
			type2 = MYSQL_TYPE_LONGLONG;
			break;
		case 'U':
			type2 = MYSQL_TYPE_LONGLONG;
			if ( is_unsigned ) *is_unsigned = true;
			break;
		case 'f':
			type2 = MYSQL_TYPE_FLOAT;
			break;
		case 'r':
			type2 = MYSQL_TYPE_DOUBLE;
			break;
		case 'b':
			type2 = MYSQL_TYPE_BLOB;
			break;
		case 'd':
			type2 = MYSQL_TYPE_DATETIME;
			break;
		default:
			break;
	}

	return type2;
}


static size_t mysqlex_format_params( const char *fmt, va_list args, MYSQL_BIND **output )
{
	int bnd_count = 0;
	MYSQL_BIND *bnd;
	unsigned long *lengths;
	int i;
	struct mysqlex_blob *blob = NULL;

	if ( fmt == NULL )
	{
		goto ignore_binds;
	}

	bnd_count = (int)strlen( fmt );

	if ( bnd_count == 0 )
	{
		goto ignore_binds;
	}


	bnd = (MYSQL_BIND *)calloc( bnd_count, sizeof( unsigned long ) + sizeof( MYSQL_BIND ) );

	if ( bnd == NULL )
	{
		mysqlex_error( "bnd == NULL", __FILE__, __LINE__ );
	}

	lengths = (unsigned long *)&bnd[bnd_count];

	for ( i = 0; i < bnd_count; i++ )
	{
		bnd[i].buffer_type = mysqlex_f2t( fmt[i], &bnd[i].is_unsigned );
		bnd[i].length = &lengths[i];

		lengths[i] = 0;

		switch ( fmt[i] )
		{
			case 'c':
				bnd[i].buffer_length = sizeof( char );
				*bnd[i].length = sizeof( char );
				bnd[i].buffer = args;
				va_arg( args, char );
				break;
			case 'x':
				bnd[i].buffer_length = sizeof( unsigned char );
				*bnd[i].length = sizeof( unsigned char );
				bnd[i].buffer = args;
				va_arg( args, unsigned char );
				break;
			case 'o':
				bnd[i].buffer_length = sizeof( int16_t );
				*bnd[i].length = sizeof( int16_t );
				bnd[i].buffer = args;
				va_arg( args, int16_t );
				break;
			case 'z':
				bnd[i].buffer_length = sizeof( uint16_t );
				*bnd[i].length = sizeof( uint16_t );
				bnd[i].buffer = args;
				va_arg( args, uint16_t );
				break;
			case 'u':
				bnd[i].buffer_length = sizeof( uint32_t );
				*bnd[i].length = sizeof( uint32_t );
				bnd[i].buffer = args;
				va_arg( args, uint32_t );
				break;
			case 'i':
				bnd[i].buffer_length = sizeof( int32_t );
				*bnd[i].length = sizeof( int32_t );
				bnd[i].buffer = args;
				va_arg( args, int32_t );
				break;
			case 's':
				bnd[i].buffer = args;
				bnd[i].buffer_length = (unsigned long)strlen( va_arg( args, char * ) );
				*bnd[i].length = bnd[i].buffer_length;
				break;
			case 'I':
				bnd[i].buffer_length = sizeof( int64_t );
				*bnd[i].length = sizeof( int64_t );
				bnd[i].buffer = args;
				va_arg( args, int64_t );

				break;
			case 'U':
				bnd[i].buffer_length = sizeof( uint64_t );
				*bnd[i].length = sizeof( uint64_t );
				bnd[i].buffer = args;
				va_arg( args, uint64_t );
				break;
			case 'f':
				bnd[i].buffer_length = sizeof( float );
				*bnd[i].length = sizeof( float );
				bnd[i].buffer = args;
				va_arg( args, float );
				break;
			case 'r':
				bnd[i].buffer_length = sizeof( double );
				*bnd[i].length = sizeof( double );
				bnd[i].buffer = args;
				va_arg( args, double );
				break;
			case 'b':
				blob = va_arg( args, struct mysqlex_blob * );
				bnd[i].buffer_length = (unsigned long)blob->length;
				*bnd[i].length = blob->length;
				bnd[i].buffer = blob->data;
				break;
			case 'd':
				bnd[i].buffer_length = sizeof( MYSQL_TIME );
				*bnd[i].length = sizeof( MYSQL_TIME );
				bnd[i].buffer = args;
				va_arg( args, MYSQL_TIME * );
				break;
			default:
				break;
		}
	}

	*output = bnd;
	return bnd_count;

ignore_binds:
	*output = NULL;
	return 0;
}


void mysqlex_clone_fields( unsigned int field_count, MYSQL_FIELD *from, MYSQL_FIELD **to )
{
	unsigned int i;
	MYSQL_FIELD *targets = calloc( field_count, sizeof( MYSQL_FIELD ) );

	if ( targets == NULL )
	{
		mysqlex_error( "mysqlex_clone_fields calloc failed", __FILE__, __LINE__ );
	}

	for ( i = 0; i < field_count; i++ )
	{
		if ( from[i].name )
		{
			targets[i].name = _strdup( from[i].name );
		}

		if ( from[i].org_name )
		{
			targets[i].org_name = _strdup( from[i].org_name );
		}
		if ( from[i].table )
		{
			targets[i].table = _strdup( from[i].table );
		}
		if ( from[i].org_table )
		{
			targets[i].org_table = _strdup( from[i].org_table );
		}
		if ( from[i].db )
		{
			targets[i].db = _strdup( from[i].db );
		}
		if ( from[i].catalog )
		{
			targets[i].catalog = _strdup( from[i].catalog );
		}
		if ( from[i].def )
		{
			targets[i].def = _strdup( from[i].def );
		}

		//targets[i].name
		targets[i].length = from[i].length;
		targets[i].max_length = from[i].max_length;
		targets[i].name_length = from[i].name_length;
		targets[i].org_name_length = from[i].org_name_length;
		targets[i].table_length = from[i].table_length;
		targets[i].org_table_length = from[i].org_table_length;
		targets[i].db_length = from[i].db_length;
		targets[i].catalog_length = from[i].catalog_length;
		targets[i].def_length = from[i].def_length;
		targets[i].flags = from[i].flags;
		targets[i].decimals = from[i].decimals;
		targets[i].charsetnr = from[i].charsetnr;
		targets[i].type = from[i].type;
	}

	*to = targets;
}

int mysqlex_get_fields( MYSQL_STMT *stmt, struct mysqlex_result *r )
{
	MYSQL_RES *res;

	res = mysql_stmt_result_metadata( stmt );

	if ( res == NULL )
	{
		mysqlex_error( "mysql_stmt_result_metadata return NULL", __FILE__, __LINE__ );
	}

	r->field_count = mysql_stmt_field_count( stmt );

	if ( r->field_count > 0 )
	{
		mysqlex_clone_fields( r->field_count, res->fields, &r->fields );
	}

	mysql_free_result( res );

	return 0;
}

void mysqlex_build_result_bnd( struct mysqlex *db, struct mysqlex_result *result, MYSQL_BIND **output )
{
	unsigned int i;
	unsigned char *dataseg;
	unsigned long totalsize;
	MYSQL_BIND *bindparams;
	unsigned long *lengths;

	totalsize = (sizeof( unsigned long ) + sizeof( MYSQL_BIND )) * result->field_count;

	for ( i = 0; i < result->field_count; i++ )
	{
		totalsize += result->fields[i].length;
	}

	bindparams = (MYSQL_BIND *)malloc( totalsize );
	if ( !bindparams )
	{
		mysqlex_error( "mysqlex_build_result_bnd malloc failed", __FILE__, __LINE__ );
	}

	memset( bindparams, 0, totalsize );

	lengths = (unsigned long *)&bindparams[result->field_count];
	dataseg = (unsigned char *)&lengths[result->field_count];

	for ( i = 0; i < result->field_count; i++ )
	{
		bindparams[i].buffer_type = result->fields[i].type;
		bindparams[i].length = &lengths[i];
		bindparams[i].buffer_length = result->fields[i].length;
		bindparams[i].buffer = dataseg;
		bindparams[i].is_unsigned = (result->fields[i].flags & UNSIGNED_FLAG) != 0;

		dataseg += bindparams[i].buffer_length;
	}

	*output = bindparams;
	db->eno = 0;
}

int mysqlex_get_rows( struct mysqlex *db, MYSQL_STMT *stmt, struct mysqlex_result *result )
{
	MYSQL_BIND *bnd = NULL;
	my_ulonglong i;
	unsigned int j;
	struct mysqlex_column *currow;

	if ( !result->field_count )
	{
		db->eno = 0;
		return 0;
	}

	result->num_rows = mysql_stmt_num_rows( stmt );

	mysqlex_build_result_bnd( db, result, &bnd );

	db->eno = mysql_stmt_bind_result( stmt, bnd );
	if ( db->eno )
	{
		mysqlex_error( "mysql_stmt_bind_result failed", __FILE__, __LINE__ );
	}

	result->num_elements = result->num_rows * result->field_count;
	result->elements = (struct mysqlex_column *)calloc( result->num_elements, sizeof( struct mysqlex_column ) );
	result->rows = (struct mysqlex_column **)calloc( result->num_rows, sizeof( struct mysqlex_column * ) );


	if ( result->elements == NULL )
	{
		mysqlex_error( "result->elements calloc failed", __FILE__, __LINE__ );
	}

	if ( result->rows == NULL )
	{
		mysqlex_error( "result->rows calloc failed", __FILE__, __LINE__ );
	}

	for ( i = 0; i < result->num_rows; i++ )
	{
		currow = &result->elements[i * result->field_count];
		result->rows[i] = currow;

		db->eno = mysql_stmt_fetch( stmt );

		if ( db->eno )
		{
			goto failed;
		}

		for ( j = 0; j < result->field_count; j++ )
		{
			db->eno = mysql_stmt_fetch_column( stmt, &bnd[j], j, 0 );

			if ( db->eno )
			{
				goto failed;
			}

			mysqlex_column_init( &currow[j] );
			mysqlex_column_format( &currow[j], &bnd[j] );
		}
	}

	if ( bnd )
	{
		free( bnd );
	}

	return db->eno;

failed:

	if ( bnd )
	{
		free( bnd );
	}

	if ( result->elements )
	{
		free( result->elements );
		result->elements = NULL;
	}

	if ( result->rows )
	{
		free( result->rows );
		result->rows = NULL;
	}

	return db->eno;
}

int mysqlex_get_result_data( struct mysqlex *db, MYSQL_STMT *stmt, struct mysqlex_result *result )
{
	int eno;

	result->field_count = mysql_stmt_field_count( stmt );
	result->num_rows = mysql_stmt_num_rows( stmt );
	result->insert_id = mysql_stmt_insert_id( stmt );
	result->affected_rows = mysql_stmt_affected_rows( stmt );

	eno = mysqlex_get_fields( stmt, result );
	if ( eno )
	{
		return eno;
	}

	eno = mysqlex_get_rows( db, stmt, result );
	if ( eno )
	{
		return eno;
	}

	return 0;
}

void mysqlex_init( struct mysqlex *dbex )
{
	memset( dbex, 0, sizeof( struct mysqlex ) );

	mysql_init( &dbex->db );
}

void mysqlex_close( struct mysqlex *dbex )
{
	mysql_close( &dbex->db );
	memset( dbex, 0, sizeof( struct mysqlex ) );
}

bool mysqlex_connect( struct mysqlex *dbex, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, const char *charset )
{
	bool yes = 1;
	int timeout = 10;

	mysql_options( &dbex->db, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&timeout );
	mysql_options( &dbex->db, MYSQL_OPT_RECONNECT, (const char*)&yes );

	if ( charset )
	{
		mysql_options( &dbex->db, MYSQL_SET_CHARSET_NAME, charset );
	}

	if ( !mysql_real_connect( &dbex->db, host, user,
		 passwd, db, port, unix_socket, CLIENT_BASIC_FLAGS ) )
	{
		strcpy( dbex->error, mysql_error( &dbex->db ) );
		return false;
	}

	return true;
}

//transaction
bool mysqlex_begin_transaction( struct mysqlex *dbex )
{
	dbex->eno = mysql_query( &dbex->db, "START TRANSACTION;" );

	if ( dbex->eno )
	{
		strcpy( dbex->error, mysql_error( &dbex->db ) );
	}

	return dbex->eno == 0;
}

bool mysqlex_commit( struct mysqlex *dbex )
{
	dbex->eno = mysql_commit( &dbex->db );

	if ( dbex->eno )
	{
		strcpy( dbex->error, mysql_error( &dbex->db ) );
	}

	return dbex->eno == 0;
}

bool mysqlex_rollback( struct mysqlex *dbex )
{
	dbex->eno = mysql_rollback( &dbex->db );

	if ( dbex->eno )
	{
		strcpy( dbex->error, mysql_error( &dbex->db ) );
	}

	return dbex->eno == 0;
}


struct mysqlex_resultset* mysqlex_query( struct mysqlex *dbex, const char *sql, const char *fmt, ... )
{
	va_list args;
	MYSQL_STMT *stmt;
	MYSQL_BIND *bnd = NULL;
	size_t params_count;
	unsigned int stmt_param_count;
	struct mysqlex_resultset *resultset = NULL;
	struct mysqlex_result *result = NULL;

	va_start( args, fmt );

	stmt = mysql_stmt_init( &dbex->db );

	if ( stmt == NULL )
	{
		mysqlex_error( "mysql_stmt_init == NULL", __FILE__, __LINE__ );
	}


	dbex->eno = mysql_stmt_prepare( stmt, sql, (unsigned long)strlen( sql ) );
	if ( dbex->eno )
	{
		goto failed;
	}

	stmt_param_count = mysql_stmt_param_count( stmt );

	if ( stmt_param_count > 0 )
	{
		params_count = mysqlex_format_params( fmt, args, &bnd );

		if ( stmt_param_count != params_count )
		{
			mysqlex_error( "mysql_stmt_param_count ( stmt ) != params_count", __FILE__, __LINE__ );
		}

		dbex->eno = mysql_stmt_bind_param( stmt, bnd );
		if ( dbex->eno )
		{
			mysqlex_error( "mysql_stmt_bind_param failed", __FILE__, __LINE__ );
		}
	}

	dbex->eno = mysql_stmt_execute( stmt );

	if ( dbex->eno )
	{
		goto failed;
	}

	resultset = mysqlex_resultset_new( );

	do
	{
		dbex->eno = mysql_stmt_store_result( stmt );

		if ( dbex->eno )
		{
			goto failed;
		}

		result = mysqlex_result_new( );

		dbex->eno = mysqlex_get_result_data( dbex, stmt, result );

		if ( dbex->eno )
		{
			mysqlex_result_free( result );
			goto failed;
		}

		mysqlex_resultset_add_result( resultset, result );

		dbex->eno = mysql_stmt_next_result( stmt );
	}
	while ( dbex->eno == 0 );

	if ( dbex->eno > 0 )
	{
		goto failed;
	}

	if ( bnd )
	{
		free( bnd );
	}

	mysql_stmt_reset( stmt );
	mysql_stmt_close( stmt );

	return resultset;

failed:
	if ( resultset )
	{
		mysqlex_resultset_free( resultset );
	}

	if ( bnd )
	{
		free( bnd );
	}
	mysql_stmt_reset( stmt );
	mysql_stmt_close( stmt );
	return NULL;
}

bool mysqlex_execute( struct mysqlex *db, const char *sql, const char *fmt, ... )
{
	va_list args;
	MYSQL_STMT *stmt;
	MYSQL_BIND *bnd = NULL;
	size_t parameters_count;
	unsigned int stmt_param_count;

	va_start( args, fmt );

	stmt = mysql_stmt_init( &db->db );

	if ( stmt == NULL )
	{
		mysqlex_error( "mysql_stmt_init == NULL", __FILE__, __LINE__ );
	}

	db->eno = mysql_stmt_prepare( stmt, sql, (unsigned long)strlen( sql ) );

	if ( db->eno )
	{
		goto failed;
	}

	stmt_param_count = mysql_stmt_param_count( stmt );

	if ( stmt_param_count > 0 )
	{
		parameters_count = mysqlex_format_params( fmt, args, &bnd );

		if ( stmt_param_count != parameters_count )
		{
			mysqlex_error( "mysql_stmt_param_count ( stmt ) != parameters_count", __FILE__, __LINE__ );
		}

		db->eno = mysql_stmt_bind_param( stmt, bnd );

		if ( db->eno )
		{
			mysqlex_error( "mysql_stmt_bind_param failed", __FILE__, __LINE__ );
		}
	}

	db->eno = mysql_stmt_execute( stmt );

	db->insert_id = mysql_stmt_insert_id( stmt );
	db->affected_rows = mysql_stmt_affected_rows( stmt );

	mysql_stmt_reset( stmt );
	mysql_stmt_close( stmt );

	if ( bnd )
	{
		free( bnd );
	}

	return (db->eno == 0);

failed:

	if ( bnd )
	{
		free( bnd );
	}

	mysql_stmt_close( stmt );
	return false;
}