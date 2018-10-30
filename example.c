#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "mysqlex.h"
#include "mysqlex-util.h"


int main(int argc, char **argv)
{
	struct mysqlex my_db;
	struct mysqlex_resultset *rs;
	
	mysql_library_init( argc, argv, NULL );
	
	mysqlex_init( &my_db );
	
	if ( !mysqlex_connect( &my_db, "localhost", "root", "123456", "lotterys", 3306, NULL, "utf8" ) )
	{
		printf( "%s\n", my_db.error );
		return 0;
	}


	rs = mysqlex_query( &my_db, "SELECT * FROM lottery_data_513_257 where date = ?;", "i", 20170718 );

	if ( mysqlex_next_result( rs ) )
	{
		if ( mysqlex_move_first( rs ) )
		{
			do
			{
				char value[15] = { 0 };
				int32_t issue_no = mysqlex_get_field_int32( rs, "issue_no" );
				mysqlex_get_field_string( rs, "value", value, sizeof( value ) );

				printf( "issue_no :%d\n", issue_no );
				printf( "value :%s\n", value );
			}
			while ( mysqlex_move_next( rs ) );
		}
	}

	if ( rs )
	{
		mysqlex_resultset_free( rs );
	}
	
	mysqlex_close( &my_db );
	
	return 0;
}