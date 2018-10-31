#ifndef _MYSQLEX_UTIL_H_
#define _MYSQLEX_UTIL_H_

#ifdef __cplusplus
extern "C"
{
#endif

char mysqlex_column_get_char( struct mysqlex_column *col );
unsigned char mysqlex_column_get_uchar( struct mysqlex_column *col );

int16_t mysqlex_column_get_int16( struct mysqlex_column *col );
uint16_t mysqlex_column_get_uint16( struct mysqlex_column *col );

int32_t mysqlex_column_get_int32( struct mysqlex_column *col );
uint32_t mysqlex_column_get_uint32( struct mysqlex_column *col );

int64_t mysqlex_column_get_int64( struct mysqlex_column *col );
uint64_t mysqlex_column_get_uint64( struct mysqlex_column *col );

MYSQL_TIME mysqlex_column_get_time( struct mysqlex_column *col );

float mysqlex_column_get_float( struct mysqlex_column *col );
double mysqlex_column_get_double( struct mysqlex_column *col );

bool mysqlex_column_get_string( struct mysqlex_column *col, char *buffer, size_t length );
size_t mysqlex_column_get_blob( struct mysqlex_column *col, void *buffer, size_t blobsize );
char *mysqlex_column_get_string2( struct mysqlex_column *col );
void *mysqlex_column_get_blob2( struct mysqlex_column *col, size_t *blobsize );


/*
	multi result
*/

bool mysqlex_first_result( struct mysqlex_resultset *rs );
bool mysqlex_next_result( struct mysqlex_resultset *rs );


/*
	single result
*/
bool mysqlex_move_first( struct mysqlex_resultset *rs );
bool mysqlex_move_next( struct mysqlex_resultset *rs );

bool mysqlex_get_field_index( struct mysqlex_resultset *rs, const char *field_name, unsigned int *index );

char mysqlex_get_field_char( struct mysqlex_resultset *rs, const char *field_name );
char mysqlex_get_field_char_I( struct mysqlex_resultset *rs, unsigned int field_index );
unsigned char mysqlex_get_field_uchar( struct mysqlex_resultset *rs, const char *field_name );
unsigned char mysqlex_get_field_uchar_I( struct mysqlex_resultset *rs, unsigned int field_index );

int16_t mysqlex_get_field_int16( struct mysqlex_resultset *rs, const char *field_name );
int16_t mysqlex_get_field_int16_I( struct mysqlex_resultset *rs, unsigned int field_index );
uint16_t mysqlex_get_field_uint16( struct mysqlex_resultset *rs, const char *field_name );
uint16_t mysqlex_get_field_uint16_I( struct mysqlex_resultset *rs, unsigned int field_index );

int32_t mysqlex_get_field_int32( struct mysqlex_resultset *rs, const char *field_name );
int32_t mysqlex_get_field_int32_I( struct mysqlex_resultset *rs, unsigned int field_index );
uint32_t mysqlex_get_field_uint32( struct mysqlex_resultset *rs, const char *field_name );
uint32_t mysqlex_get_field_uint32_I( struct mysqlex_resultset *rs, unsigned int field_index );

int64_t mysqlex_get_field_int64( struct mysqlex_resultset *rs, const char *field_name );
int64_t mysqlex_get_field_int64_I( struct mysqlex_resultset *rs, unsigned int field_index );
uint64_t mysqlex_get_field_uint64( struct mysqlex_resultset *rs, const char *field_name );
uint64_t mysqlex_get_field_uint64_I( struct mysqlex_resultset *rs, unsigned int field_index );

float mysqlex_get_field_float( struct mysqlex_resultset *rs, const char *field_name );
float mysqlex_get_field_float_I( struct mysqlex_resultset *rs, unsigned int field_index );

double mysqlex_get_field_double( struct mysqlex_resultset *rs, const char *field_name );
double mysqlex_get_field_double_I( struct mysqlex_resultset *rs, unsigned int field_index );

MYSQL_TIME mysqlex_get_field_time( struct mysqlex_resultset *rs, const char *field_name );
MYSQL_TIME mysqlex_get_field_time_I( struct mysqlex_resultset *rs, unsigned int field_index );

bool mysqlex_get_field_string( struct mysqlex_resultset *rs, const char *field_name, char *buffer, size_t length );
bool mysqlex_get_field_string_I( struct mysqlex_resultset *rs, unsigned int field_index, char *buffer, size_t length );

char* mysqlex_get_field_string2( struct mysqlex_resultset *rs, const char *field_name );
char* mysqlex_get_field_string2_I( struct mysqlex_resultset *rs, unsigned int field_index );


bool mysqlex_get_field_blob( struct mysqlex_resultset *rs, const char *field_name, char *buffer, size_t blobsize );
bool mysqlex_get_field_blob_I( struct mysqlex_resultset *rs, unsigned int field_index, char *buffer, size_t blobsize );

void* mysqlex_get_field_blob2( struct mysqlex_resultset *rs, const char *field_name, size_t *blobsize );
void* mysqlex_get_field_blob2_I( struct mysqlex_resultset *rs, unsigned int field_index, size_t *blobsize );

#ifdef __cplusplus
};
#endif

#endif