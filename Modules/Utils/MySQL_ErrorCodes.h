#ifndef MYSQL_ERROR_CODES_H
#define MYSQL_ERROR_CODES_H

typedef enum MySQL_ErrorCode
{
	ER_NO = 1002,
	ER_YES=1003,
	ER_NO_DB_ERROR = 1046,
	ER_BAD_DB_ERROR = 1049,
	ER_NO_SUCH_TABLE = 1146

}MySQL_ErrorCode;


#endif