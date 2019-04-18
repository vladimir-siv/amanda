#include <suit/def.h>

extern void _type_traits();
extern void _qbuffer_int();
extern void _exceptions();
extern void _raii();
extern void _vector_int();
extern void _vector_coord();
extern void _bitvector();
extern void _queue_int();
extern void _queue_coord();
extern void _list_int();
extern void _list_coord();
extern void _list_enumerator();
extern void _vlist_int();
extern void _vlist_coord();
extern void _vlist_enumerator();
extern void _tuple();
extern void _events();
extern void _object_allocator();
extern void _vmultilist_int();
extern void _xml_parser();

void setup()
{
	suit::add("type_traits", &_type_traits);
	suit::add("qbuffer_int", &_qbuffer_int);
	suit::add("exceptions", &_exceptions);
	suit::add("raii", &_raii);
	suit::add("vector_int", &_vector_int);
	suit::add("vector_coord", &_vector_coord);
	suit::add("bitvector", &_bitvector);
	suit::add("queue_int", &_queue_int);
	suit::add("queue_coord", &_queue_coord);
	suit::add("list_int", &_list_int);
	suit::add("list_coord", &_list_coord);
	suit::add("list_enumerator", &_list_enumerator);
	suit::add("vlist_int", &_vlist_int);
	suit::add("vlist_coord", &_vlist_coord);
	suit::add("vlist_enumerator", &_vlist_enumerator);
	suit::add("tuple", &_tuple);
	suit::add("events", &_events);
	suit::add("object_allocator", &_object_allocator);
	suit::add("vmultilist_int", &_vmultilist_int);
	suit::add("xml_parser", &_xml_parser);
}
