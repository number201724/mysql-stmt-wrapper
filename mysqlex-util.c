#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <mysql.h>
#include "mysqlex.h"
#include "mysqlex-util.h"


int16_t mysqlex_column_get_int16( struct mysqlex_column *col )
{
	if ( col->buffer_type == MYSQL_TYPE_SHORT )
	{
		return *(int16_t *)col->buffer;
	}
	return 0;
}

uint16_t mysqlex_column_get_uint16( struct mysqlex_column *col )
{
	if ( col->buffer_type == MYSQL_TYPE_SHORT )
	{
		return *(uint16_t *)col->buffer;
	}

	return 0;
}

int32_t mysqlex_column_get_int32( struct mysqlex_column *col )
{
	if ( col->buffer_type == MYSQL_TYPE_INT24 || col->buffer_type == MYSQL_TYPE_LONG )
	{
		return *(int32_t *)col->buffer;
	}
	return 0;
}

uint32_t mysqlex_column_get_uint32( struct mysqlex_column *col )
{
	if ( col->buffer_type == MYSQL_TYPE_INT24 || col->buffer_type == MYSQL_TYPE_LONG )
	{
		return *(uint32_t *)col->buffer;
	}

	return 0;
}

int64_t mysqlex_column_get_int64( struct mysqlex_column *col )
{
	if ( col->buffer_type == MYSQL_TYPE_LONGLONG )
	{
		return *(int64_t *)col->buffer;
	}
	return 0;
}

uint64_t mysqlex_column_get_uint64( struct mysqlex_column *col )
{
	if ( col->buffer_type == MYSQL_TYPE_LONGLONG )
	{
		return *(uint64_t *)col->buffer;
	}

	return 0;
}

char mysqlex_column_get_char( struct mysqlex_column *col )
{
	if ( col->buffer_type == MYSQL_TYPE_TINY )
	{
		return *(char *)col->buffer;
	}
	return 0;
}

unsigned char mysqlex_column_get_uchar( struct mysqlex_column *col )
{
	if ( col->buffer_type == MYSQL_TYPE_TINY )
	{
		return *(unsigned char *)col->buffer;
	}
	return 0;
}

MYSQL_TIME mysqlex_column_get_time( struct mysqlex_column *col )
{
	MYSQL_TIME tm = { 0 };


	if ( col->buffer_type == MYSQL_TYPE_TIME ||
		 col->buffer_type == MYSQL_TYPE_DATE ||
		 col->buffer_type == MYSQL_TYPE_DATETIME ||
		 col->buffer_type == MYSQL_TYPE_TIMESTAMP )
	{
		tm = *(MYSQL_TIME *)col->buffer;
	}

	return tm;
}

float mysqlex_column_get_float( struct mysqlex_column *col )
{
	if ( col->buffer_type == MYSQL_TYPE_FLOAT )
	{
		return *(float *)col->buffer;
	}

	return 0;
}

double mysqlex_column_get_double( struct mysqlex_column *col )
{
	if ( col->buffer_type == MYSQL_TYPE_DOUBLE )
	{
		return *(double *)col->buffer;
	}

	return 0;
}

bool mysqlex_column_get_string( struct mysqlex_column *col, char *buffer, size_t length )
{
	if ( col->buffer_type == MYSQL_TYPE_STRING ||
		 col->buffer_type == MYSQL_TYPE_VAR_STRING ||
		 col->buffer_type == MYSQL_TYPE_TINY_BLOB ||
		 col->buffer_type == MYSQL_TYPE_BLOB ||
		 col->buffer_type == MYSQL_TYPE_MEDIUM_BLOB ||
		 col->buffer_type == MYSQL_TYPE_LONG_BLOB ||
		 col->buffer_type == MYSQL_TYPE_BIT )
	{
		if ( length >= col->length + 0x1 )
		{
			memcpy( buffer, col->buffer, col->length );
			buffer[col->length] = 0;
		}

		return true;
	}

	return false;
}

size_t mysqlex_column_get_blob( struct mysqlex_column *col, void *buffer, size_t blobsize )
{
	if ( col->buffer_type == MYSQL_TYPE_STRING ||
		 col->buffer_type == MYSQL_TYPE_VAR_STRING ||
		 col->buffer_type == MYSQL_TYPE_TINY_BLOB ||
		 col->buffer_type == MYSQL_TYPE_BLOB ||
		 col->buffer_type == MYSQL_TYPE_MEDIUM_BLOB ||
		 col->buffer_type == MYSQL_TYPE_LONG_BLOB ||
		 col->buffer_type == MYSQL_TYPE_BIT )
	{
		if ( blobsize >= col->length )
		{
			memcpy( buffer, col->buffer, col->length );
			return col->length;
		}
	}

	return 0;
}

char *mysqlex_column_get_string2( struct mysqlex_column *col )
{
	char *data = NULL;

	if ( col->buffer_type == MYSQL_TYPE_STRING ||
		 col->buffer_type == MYSQL_TYPE_VAR_STRING ||
		 col->buffer_type == MYSQL_TYPE_TINY_BLOB ||
		 col->buffer_type == MYSQL_TYPE_BLOB ||
		 col->buffer_type == MYSQL_TYPE_MEDIUM_BLOB ||
		 col->buffer_type == MYSQL_TYPE_LONG_BLOB ||
		 col->buffer_type == MYSQL_TYPE_BIT )
	{
		data = malloc( col->length + 0x1 );
		memcpy( data, col->buffer, col->length );
		data[col->length] = 0;
		return data;
	}

	return NULL;
}

void *mysqlex_column_get_blob2( struct mysqlex_column *col, size_t *blobsize )
{
	void *data = NULL;

	if ( col->buffer_type == MYSQL_TYPE_STRING ||
		 col->buffer_type == MYSQL_TYPE_VAR_STRING ||
		 col->buffer_type == MYSQL_TYPE_TINY_BLOB ||
		 col->buffer_type == MYSQL_TYPE_BLOB ||
		 col->buffer_type == MYSQL_TYPE_MEDIUM_BLOB ||
		 col->buffer_type == MYSQL_TYPE_LONG_BLOB ||
		 col->buffer_type == MYSQL_TYPE_BIT )
	{
		data = malloc( col->length );
		*blobsize = col->length;
		memcpy( data, col->buffer, col->length );

		return data;
	}

	return NULL;
	return NULL;
}

bool mysqlex_first_result( struct mysqlex_resultset *rs )
{
	if ( rs->num_results > 0 )
	{
		if ( !mysqlex_list_empty( &rs->results ) )
		{
			rs->current = (struct mysqlex_result *)rs->results.next;
			return true;
		}
	}

	return false;
}

bool mysqlex_next_result( struct mysqlex_resultset *rs )
{
	if ( rs->num_results > 0 )
	{
		if ( rs->current == NULL )
		{
			if ( !mysqlex_list_empty( &rs->results ) )
			{
				rs->current = (struct mysqlex_result *)rs->results.next;
				return true;
			}
		}
		else
		{
			if ( rs->current->entry.next == &rs->results )
			{
				return false;
			}

			rs->current = (struct mysqlex_result *)rs->current->entry.next;
			return true;
		}
	}

	return false;
}

bool mysqlex_move_first( struct mysqlex_resultset *rs )
{
	if ( rs->current == NULL )
	{
		if ( !mysqlex_first_result( rs ) )
			return false;
	}

	if ( rs->current->field_count == 0 ||
		 rs->current->num_rows == 0 )
	{
		return false;
	}

	if ( rs->current->fields && rs->current->rows )
	{
		rs->current->cursor = 0;

		return true;
	}

	return false;
}

bool mysqlex_move_next( struct mysqlex_resultset *rs )
{
	if ( rs->current == NULL )
		return false;

	if ( rs->current->field_count == 0 ||
		 rs->current->num_rows == 0 )
	{
		return false;
	}

	if ( rs->current->fields && rs->current->rows )
	{
		if ( rs->current->num_rows > rs->current->cursor + 1 )
		{
			rs->current->cursor++;
			return true;
		}
	}

	return false;
}

bool mysqlex_get_field_index( struct mysqlex_resultset *rs, const char *field_name, unsigned int *index )
{
	unsigned int i;

	if ( !rs->current )
		return false;

	if ( !rs->current->field_count )
		return false;

	for ( i = 0; i < rs->current->field_count; i++ )
	{
		if ( !strcmp( rs->current->fields[i].name, field_name ) )
		{
			*index = i;
			return true;
		}
	}

	return false;
}

char mysqlex_get_field_char( struct mysqlex_resultset *rs, const char *field_name )
{
	unsigned int index;

	if ( !mysqlex_get_field_index( rs, field_name, &index ) )
	{
		return 0;
	}

	return mysqlex_get_field_char_I( rs, index );
}

char mysqlex_get_field_char_I( struct mysqlex_resultset *rs, unsigned int field_index )
{
	if ( rs->current->field_count > field_index )
	{
		return mysqlex_column_get_char( &rs->current->rows[rs->current->cursor][field_index] );
	}

	return 0;
}

unsigned char mysqlex_get_field_uchar( struct mysqlex_resultset *rs, const char *field_name )
{
	unsigned int index;

	if ( !mysqlex_get_field_index( rs, field_name, &index ) )
	{
		return 0;
	}

	return mysqlex_get_field_uchar_I( rs, index );
}

unsigned char mysqlex_get_field_uchar_I( struct mysqlex_resultset *rs, unsigned int field_index )
{
	if ( rs->current->field_count > field_index )
	{
		return mysqlex_column_get_uchar( &rs->current->rows[rs->current->cursor][field_index] );
	}

	return 0;
}

int16_t mysqlex_get_field_int16( struct mysqlex_resultset *rs, const char *field_name )
{
	unsigned int index;

	if ( !mysqlex_get_field_index( rs, field_name, &index ) )
	{
		return 0;
	}

	return mysqlex_get_field_int16_I( rs, index );
}

int16_t mysqlex_get_field_int16_I( struct mysqlex_resultset *rs, unsigned int field_index )
{
	if ( rs->current->field_count > field_index )
	{
		return mysqlex_column_get_int16( &rs->current->rows[rs->current->cursor][field_index] );
	}

	return 0;
}

uint16_t mysqlex_get_field_uint16( struct mysqlex_resultset *rs, const char *field_name )
{
	unsigned int index;

	if ( !mysqlex_get_field_index( rs, field_name, &index ) )
	{
		return 0;
	}

	return mysqlex_get_field_uint16_I( rs, index );
}

uint16_t mysqlex_get_field_uint16_I( struct mysqlex_resultset *rs, unsigned int field_index )
{
	if ( rs->current->field_count > field_index )
	{
		return mysqlex_column_get_uint16( &rs->current->rows[rs->current->cursor][field_index] );
	}

	return 0;
}

int32_t mysqlex_get_field_int32( struct mysqlex_resultset *rs, const char *field_name )
{
	unsigned int index;

	if ( !mysqlex_get_field_index( rs, field_name, &index ) )
	{
		return 0;
	}

	return mysqlex_get_field_int32_I( rs, index );
}

int32_t mysqlex_get_field_int32_I( struct mysqlex_resultset *rs, unsigned int field_index )
{
	if ( rs->current->field_count > field_index )
	{
		return mysqlex_column_get_int32( &rs->current->rows[rs->current->cursor][field_index] );
	}

	return 0;
}

uint32_t mysqlex_get_field_uint32( struct mysqlex_resultset *rs, const char *field_name )
{
	unsigned int index;

	if ( !mysqlex_get_field_index( rs, field_name, &index ) )
	{
		return 0;
	}

	return mysqlex_get_field_uint32_I( rs, index );
}

uint32_t mysqlex_get_field_uint32_I( struct mysqlex_resultset *rs, unsigned int field_index )
{
	if ( rs->current->field_count > field_index )
	{
		return mysqlex_column_get_uint32( &rs->current->rows[rs->current->cursor][field_index] );
	}

	return 0;
}

int64_t mysqlex_get_field_int64( struct mysqlex_resultset *rs, const char *field_name )
{
	unsigned int index;

	if ( !mysqlex_get_field_index( rs, field_name, &index ) )
	{
		return 0;
	}

	return mysqlex_get_field_int64_I( rs, index );
}

int64_t mysqlex_get_field_int64_I( struct mysqlex_resultset *rs, unsigned int field_index )
{
	if ( rs->current->field_count > field_index )
	{
		return mysqlex_column_get_int64( &rs->current->rows[rs->current->cursor][field_index] );
	}

	return 0;
}

uint64_t mysqlex_get_field_uint64( struct mysqlex_resultset *rs, const char *field_name )
{
	unsigned int index;

	if ( !mysqlex_get_field_index( rs, field_name, &index ) )
	{
		return 0;
	}

	return mysqlex_get_field_uint64_I( rs, index );
}

uint64_t mysqlex_get_field_uint64_I( struct mysqlex_resultset *rs, unsigned int field_index )
{
	if ( rs->current->field_count > field_index )
	{
		return mysqlex_column_get_uint64( &rs->current->rows[rs->current->cursor][field_index] );
	}

	return 0;
}


float mysqlex_get_field_float( struct mysqlex_resultset *rs, const char *field_name )
{
	unsigned int index;

	if ( !mysqlex_get_field_index( rs, field_name, &index ) )
	{
		return 0;
	}

	return mysqlex_get_field_float_I( rs, index );
}

float mysqlex_get_field_float_I( struct mysqlex_resultset *rs, unsigned int field_index )
{
	if ( rs->current->field_count > field_index )
	{
		return mysqlex_column_get_float( &rs->current->rows[rs->current->cursor][field_index] );
	}

	return 0;
}


double mysqlex_get_field_double( struct mysqlex_resultset *rs, const char *field_name )
{
	unsigned int index;

	if ( !mysqlex_get_field_index( rs, field_name, &index ) )
	{
		return 0;
	}

	return mysqlex_get_field_double_I( rs, index );
}

double mysqlex_get_field_double_I( struct mysqlex_resultset *rs, unsigned int field_index )
{
	if ( rs->current->field_count > field_index )
	{
		return mysqlex_column_get_double( &rs->current->rows[rs->current->cursor][field_index] );
	}

	return 0;
}

MYSQL_TIME mysqlex_get_field_time( struct mysqlex_resultset *rs, const char *field_name )
{
	unsigned int index;
	MYSQL_TIME tm = { 0 };

	if ( !mysqlex_get_field_index( rs, field_name, &index ) )
	{
		return tm;
	}

	return mysqlex_get_field_time_I( rs, index );
}

MYSQL_TIME mysqlex_get_field_time_I( struct mysqlex_resultset *rs, unsigned int field_index )
{
	MYSQL_TIME tm = { 0 };

	if ( rs->current->field_count > field_index )
	{
		return mysqlex_column_get_time( &rs->current->rows[rs->current->cursor][field_index] );
	}

	return tm;
}

bool mysqlex_get_field_string( struct mysqlex_resultset *rs, const char *field_name, char *buffer, size_t length )
{
	unsigned int index;

	if ( !mysqlex_get_field_index( rs, field_name, &index ) )
	{
		return false;
	}

	return mysqlex_get_field_string_I( rs, index, buffer, length );
}

bool mysqlex_get_field_string_I( struct mysqlex_resultset *rs, unsigned int field_index, char *buffer, size_t length )
{
	if ( rs->current->field_count > field_index )
	{
		return mysqlex_column_get_string( &rs->current->rows[rs->current->cursor][field_index], buffer, length );
	}

	return false;
}


char* mysqlex_get_field_string2( struct mysqlex_resultset *rs, const char *field_name )
{
	unsigned int index;

	if ( !mysqlex_get_field_index( rs, field_name, &index ) )
	{
		return NULL;
	}

	return mysqlex_get_field_string2_I( rs, index );
}

char* mysqlex_get_field_string2_I( struct mysqlex_resultset *rs, unsigned int field_index )
{
	if ( rs->current->field_count > field_index )
	{
		return mysqlex_column_get_string2( &rs->current->rows[rs->current->cursor][field_index] );
	}

	return NULL;
}

bool mysqlex_get_field_blob( struct mysqlex_resultset *rs, const char *field_name, char *buffer, size_t blobsize )
{
	unsigned int index;

	if ( !mysqlex_get_field_index( rs, field_name, &index ) )
	{
		return false;
	}

	return mysqlex_get_field_blob_I( rs, index, buffer, blobsize );
}

bool mysqlex_get_field_blob_I( struct mysqlex_resultset *rs, unsigned int field_index, char *buffer, size_t blobsize )
{
	if ( rs->current->field_count > field_index )
	{
		return mysqlex_column_get_blob( &rs->current->rows[rs->current->cursor][field_index], buffer, blobsize );
	}

	return false;
}


void* mysqlex_get_field_blob2( struct mysqlex_resultset *rs, const char *field_name, size_t *blobsize )
{
	unsigned int index;

	if ( !mysqlex_get_field_index( rs, field_name, &index ) )
	{
		return NULL;
	}

	return mysqlex_get_field_blob2_I( rs, index, blobsize );
}

void* mysqlex_get_field_blob2_I( struct mysqlex_resultset *rs, unsigned int field_index, size_t *blobsize )
{
	if ( rs->current->field_count > field_index )
	{
		return mysqlex_column_get_blob2( &rs->current->rows[rs->current->cursor][field_index], blobsize );
	}

	return NULL;
}