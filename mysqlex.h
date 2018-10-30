#ifndef _MYSQLEX_H_
#define _MYSQLEX_H_

#ifdef __cplusplus
extern "C"
{
#endif
	struct mysqlex_list_entry
	{
		struct mysqlex_list_entry *prev, *next;
	};

	struct mysqlex_blob
	{
		void *data;
		unsigned long length;
	};

	struct mysqlex_column
	{
		bool initialized;
		unsigned long length;
		void *buffer;
		enum enum_field_types buffer_type;
		bool error_value;
		bool is_unsigned;
		bool is_null;
	};

	struct mysqlex_result
	{
		struct mysqlex_list_entry entry;

		my_ulonglong affected_rows;
		my_ulonglong insert_id;


		MYSQL_FIELD *fields;
		unsigned int field_count;

		my_ulonglong num_rows;
		struct mysqlex_column **rows;

		my_ulonglong num_elements;
		struct mysqlex_column *elements;

		my_ulonglong cursor;
	};

	struct mysqlex_resultset
	{
		int num_results;
		struct mysqlex_list_entry results;

		struct mysqlex_result *current;
	};

	struct mysqlex
	{
		bool connected;
		MYSQL db;


		//error
		int eno;
		char error[MYSQL_ERRMSG_SIZE];
		char sqlstate[SQLSTATE_LENGTH + 1];

		my_ulonglong affected_rows;
		my_ulonglong insert_id;
	};


	void mysqlex_list_init( struct mysqlex_list_entry *list );

	void mysqlex_list_add_tail( struct mysqlex_list_entry *head,
								struct mysqlex_list_entry *entry );

	void mysqlex_list_add_head( struct mysqlex_list_entry *head,
								struct mysqlex_list_entry *entry );

	void mysqlex_list_del( struct mysqlex_list_entry *entry );

	bool mysqlex_list_empty( const struct mysqlex_list_entry *head );

	int mysqlex_list_is_last( const struct mysqlex_list_entry *list,
							  const struct mysqlex_list_entry *head );


	void mysqlex_column_init( struct mysqlex_column *column );
	void mysqlex_column_format( struct mysqlex_column *output_data, MYSQL_BIND *input_data );
	void mysqlex_column_close( struct mysqlex_column *column );

	struct mysqlex_resultset * mysqlex_resultset_new( );
	void mysqlex_resultset_free( struct mysqlex_resultset *rs );


	void mysqlex_init( struct mysqlex *dbex );
	void mysqlex_close( struct mysqlex *dbex );
	bool mysqlex_connect( struct mysqlex *dbex, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, const char *charset );

	//transaction
	bool mysqlex_begin_transaction( struct mysqlex *dbex );
	bool mysqlex_commit( struct mysqlex *dbex );
	bool mysqlex_rollback( struct mysqlex *dbex );


	/*
	format:
		u = uint
		i = int
		s = string( const char * )
		I = int64
		U = uint64
		f = float
		d = double
		b = blob( struct mysqlex_blob * )
		d = date( MYSQL_TIME * )
		c = char
		x = uchar
		o = short
		z = unsigned short
	*/

	//execute only
	bool mysqlex_execute( struct mysqlex *database, const char *sql, const char *fmt, ... );
	struct mysqlex_resultset* mysqlex_query( struct mysqlex *database, const char *sql, const char *fmt, ... );


#ifdef __cplusplus
};
#endif

#endif  // _MYSQLEX_H_