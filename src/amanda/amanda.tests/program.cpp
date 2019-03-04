#include <suit/def.h>

extern void _type_traits();
extern void _qbuffer_int();
extern void _vector_int();
extern void _vector_coord();
extern void _list_int();
extern void _list_coord();
extern void _exceptions();

void setup()
{
	suit::add("type_traits", &_type_traits);
	suit::add("qbuffer_int", &_qbuffer_int);
	suit::add("vector_int", &_vector_int);
	suit::add("vector_coord", &_vector_coord);
	suit::add("list_int", &_list_int);
	suit::add("list_coord", &_list_coord);
	suit::add("exceptions", &_exceptions);
}
