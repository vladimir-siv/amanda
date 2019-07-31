#include "event_parser.h"

#include <dependency.h>

//#define _DEBUG_EVENT_PARSER_

// level legend:
//	l2 == 1 => requirements
//	{
//		l3 no meaning
//		
//		l5 == 1 => equ
//		l5 == 2 => neq
//		l5 == 3 => gtr
//		l5 == 4 => lss
//		l5 == 5 => geq
//		l5 == 6 => leq
//	}
//	l2 == 2 => actions
//	{
//		l3 == 1 => raise
//		l3 == 2 => expire
//		
//		l5 == 1 => value
//		l5 == 2 => state
//	}

void EventParser::reset()
{
	level = 0;
	l2 = l3 = l5 = 0;

	evt = nullptr;
	pck = nullptr;
	cnd = nullptr;
	cmp = nullptr;
	comp = nullptr;
	unt.reset();
	comp_details.reset();
}
void EventParser::oncancel()
{
	level = 0;
	l2 = l3 = l5 = 0;

	if (evt != nullptr)
	{
		evt->~event();
		D::sdds->dealloc(evt);
		evt = nullptr;
	}

	if (pck != nullptr)
	{
		pck->~pack();
		D::sdds->dealloc(pck);
		pck = nullptr;
	}

	if (cnd != nullptr)
	{
		cnd->~cond();
		D::sdds->dealloc(cnd);
		cnd = nullptr;
	}

	cmp = nullptr;
	comp = nullptr;
	unt.reset();
	comp_details.reset();
}
void EventParser::tag_opened(const char* tagname)
{
	++level;

	switch (level)
	{
		case 1:
		{
			if (strcmp_P(tagname, PSTR("event")) != 0) cancel(); // invalid tag
		} break;
		case 2:
		{
			if (strcmp_P(tagname, PSTR("requirements")) == 0) l2 = 1;
			else if (strcmp_P(tagname, PSTR("actions")) == 0) l2 = 2;
			else cancel(); // invalid tag
		} break;
		case 3:
		{
			switch (l2)
			{
				case 1: // inside requirements
				{
					if (strcmp_P(tagname, PSTR("pack")) == 0)
					{
#ifdef _DEBUG_EVENT_PARSER_
						if (pck == nullptr)
						{
							cli();
							Serial.println(F("Pack created."));
							sei();
						}
#endif

						if (pck != nullptr) cancel(); // previous pack not closed
						else pck = pack::_new();
					}
					else cancel(); // invalid tag
				} break;
				case 2: // inside actions
				{
					if (strcmp_P(tagname, PSTR("raise")) == 0) l3 = 1;
					else if (strcmp_P(tagname, PSTR("expire")) == 0) l3 = 2;
					else cancel(); // invalid tag
				} break;

				default: // it shouldn't be possible for this to happen
				{
					cancel(); // invalid state
				} break;
			}
		} break;
		case 4:
		{
			switch (l2)
			{
				case 1: // inside requirements
				{
					if (strcmp_P(tagname, PSTR("condition")) == 0)
					{
						if (cnd != nullptr) cancel(); // previous condition not closed
					}
					else cancel(); // invalid tag
				} break;
				case 2: // inside actions
				{
					if (strcmp_P(tagname, PSTR("write")) != 0)
					{
						cancel(); // invalid tag
					}
				} break;

				default: // it shouldn't be possible for this to happen
				{
					cancel(); // invalid state
				} break;
			}
		} break;
		case 5:
		{
			switch (l2)
			{
				case 1: // inside requirements
				{
					if (cmp == nullptr)
					{
						cmp = comparator::resolve(tagname);
						if (cmp == nullptr) cancel(); // invalid tag
						else if (cmp == comparator::equ()) l5 = 1;
						else if (cmp == comparator::neq()) l5 = 2;
						else if (cmp == comparator::gtr()) l5 = 3;
						else if (cmp == comparator::lss()) l5 = 4;
						else if (cmp == comparator::geq()) l5 = 5;
						else if (cmp == comparator::leq()) l5 = 6;
						else cancel(); // what?

#ifdef _DEBUG_EVENT_PARSER_
						if (cmp != nullptr)
						{
							cli();
							Serial.print(F("Comparator resolved: "));
							Serial.println(tagname);
							sei();
						}
#endif
					}
					else cancel(); // comparator already set
				} break;
				case 2: // inside actions
				{
					if (strcmp_P(tagname, PSTR("value")) == 0) l5 = 1;
					else if (strcmp_P(tagname, PSTR("state")) == 0) l5 = 2;
					else cancel(); // invalid tag
				} break;

				default: // it shouldn't be possible for this to happen
				{
					cancel(); // invalid state
				} break;
			}
		} break;

		default:
		{
			cancel(); // invalid nesting level
		} break;
	}
}
void EventParser::attribute_spec(const char* attrname, const char* attrvalue)
{
	switch (level)
	{
		case 1: // event
		{
			if (strcmp_P(attrname, PSTR("repeat")) == 0)
			{
#ifdef _DEBUG_EVENT_PARSER_
				if (evt == nullptr)
				{
					cli();
					Serial.print(F("Event created with repeat: "));
					Serial.println(strtol(attrvalue, nullptr, 0));
					sei();
				}
#endif

				if (evt != nullptr) cancel(); // tag not closed (shouldn't be possible) or repeat attribute duplicate
				else evt = event::_new(strtol(attrvalue, nullptr, 0));
			}
			else cancel(); // invalid attribute name
		} break;
		case 4: // vid & ctype
		{
			if (strcmp_P(attrname, PSTR("vid")) == 0)
			{
				if (comp_details.vid == 0)
				{
					comp_details.vid = strtoul(attrvalue, nullptr, 0);
				}
				else cancel(); // vid attribute duplicate
			}
			else if (strcmp_P(attrname, PSTR("ctype")) == 0)
			{
				if (strcmp_P(comp_details.ctype, PSTR("")) == 0)
				{
					if (attrvalue[2] == 0)
					{
						comp_details.ctype[0] = attrvalue[0];
						comp_details.ctype[1] = attrvalue[1];
						comp_details.ctype[2] = attrvalue[2]; // 0
					}
					else cancel(); // invalid ctype (must be 2 chars long)
				}
				else cancel(); // ctype attribute duplicate
			}
			else cancel(); // invalid attribute name
		} break;
		case 5:
		{
			if (l2 == 2 && l5 == 1)
			{
				if (strcmp_P(attrname, PSTR("unit")) == 0)
				{
					if (unt.isset()) cancel(); // unit already set (duplicate attribute)
					else if (!unt.set(attrvalue)) cancel(); // invalid or unknown unit

#ifdef _DEBUG_EVENT_PARSER_
					if (unt.isset())
					{
						cli();
						Serial.print(F("Unit resolved: "));
						Serial.print(unt.prefix());
						Serial.println(unt.measure()->label);
						sei();
					}
#endif
				}
				else cancel(); // invalid attribute name
			}
			else cancel(); // at level 5, attributes are allowed only on <value>
		} break;

		default:
		{
			cancel(); // attribute specification not valid
		} break;
	}
}
void EventParser::attribute_spec_end()
{
	switch (level)
	{
		case 4: // vid & ctype (for both <condition> and <write>)
		{
			// [DECISION DEPENDENCY: component vid must not be equal to 0]
			if (comp_details.vid != 0 && strcmp_P(comp_details.ctype, PSTR("")) != 0)
			{
				if (comp == nullptr)
				{
					if (controller != nullptr)
					{
						// find a way to optimize this?
						comp = controller->find((VID)comp_details.vid, comp_details.ctype);
					}

#ifdef _DEBUG_EVENT_PARSER_
					if (comp != nullptr)
					{
						cli();
						Serial.print(F("Component resolved: "));
						Serial.print(comp->description());
						Serial.print('[');
						Serial.print(comp->ID());
						Serial.println(']');
						sei();
					}
#endif

					if (comp != nullptr)
					{
						switch (l2)
						{
							case 1: // inside requirements
							{
								ISensor* sensor = ComponentCaster::sensor(comp);

#ifdef _DEBUG_EVENT_PARSER_
								if (sensor != nullptr)
								{
									cli();
									Serial.print(F("Condition created with sensor: "));
									Serial.print(sensor->description());
									Serial.print('[');
									Serial.print(sensor->ID());
									Serial.println(']');
									sei();
								}
#endif

								if (sensor != nullptr) cnd = cond::_new(sensor);
								else cancel(); // component is not a sensor (should not be possible for this to happen)
							} break;
							case 2: // inside actions
							{
								// ok
							} break;

							default: // it shouldn't be possible for this to happen
							{
								cancel(); // invalid state
							} break;
						}
					}
					else cancel(); // controller not set or couldn't find such component
				}
				else cancel(); // component already set (shouldn't be possible)
			}
			else cancel(); // invalid attribute specification
		} break;
	}
}
void EventParser::text_value(const char* value)
{
	switch (level)
	{
		case 5:
		{
			switch (l2)
			{
				case 1: // inside requirements
				{
					if (cmp != nullptr)
					{
#ifdef _DEBUG_EVENT_PARSER_
						if (cnd != nullptr)
						{
							cli();
							Serial.print(F("Condition comparator set to compare: "));
							Serial.println((float)strtod(value, nullptr));
							sei();
						}
#endif

						if (cnd != nullptr) cnd->compare(cmp, (float)strtod(value, nullptr));
						else cancel(); // condition not opened
					}
					else cancel(); // invalid cmp
				} break;
				case 2: // inside actions
				{
					if (l3 == 1 || l3 == 2)
					{
						if (evt != nullptr)
						{
							if (comp != nullptr)
							{
								IElement* element = ComponentCaster::element(comp);
								if (element != nullptr)
								{
									switch (l5)
									{
										case 1: // <value>
										{
											if (unt.isset())
											{
												action* act = action::_new(element, (float)unt.convert(strtod(value, nullptr)));

#ifdef _DEBUG_EVENT_PARSER_
												if (act != nullptr)
												{
													cli();
													Serial.print(F("Action set on <value>: "));
													Serial.print(l3 == 1 ? F("Raise") : F("Expire"));
													Serial.print(' ');
													Serial.print(element->description());
													Serial.print('[');
													Serial.print(element->ID());
													Serial.print(']');
													Serial.print(' ');
													Serial.println((float)unt.convert(strtod(value, nullptr)));
													sei();
												}
#endif

												if (l3 == 1) evt->appendRaise(act);
												else if (l3 == 2) evt->appendExpire(act);
												else cancel(); // fatal error => act memory leak [system failure if this happens]
											}
											else cancel(); // unit not set (should not be possible for this to happen)
										} break;
										case 2: // <state>
										{
											action* act = action::_new(element, (float)strtol(value, nullptr, 0));

#ifdef _DEBUG_EVENT_PARSER_
											if (act != nullptr)
											{
												cli();
												Serial.print(F("Action set on <state>: "));
												Serial.print(l3 == 1 ? F("Raise") : F("Expire"));
												Serial.print(' ');
												Serial.print(element->description());
												Serial.print('[');
												Serial.print(element->ID());
												Serial.print(']');
												Serial.print(' ');
												Serial.println((float)strtol(value, nullptr, 0));
												sei();
											}
#endif

											if (l3 == 1) evt->appendRaise(act);
											else if (l3 == 2) evt->appendExpire(act);
											else cancel(); // fatal error => act memory leak [system failure if this happens]
										} break;

										default: // should not be possible for this to happen
										{
											cancel();
										} break;
									}
								}
								else cancel(); // component is not an element (should not be possible for this to happen)
							}
							else cancel(); // component not set (should not be possible for this to happen)
						}
						else cancel(); // event not set (should not be possible for this to happen)
					}
					else cancel(); // attribute not inside <raise> or <expire>
				} break;

				default: // it shouldn't be possible for this to happen
				{
					cancel(); // invalid state
				} break;
			}
		} break;

		default:
		{
			cancel(); // invalid nesting level
		} break;
	}
}
void EventParser::tag_closed(const char* tagname)
{
	switch (level)
	{
		case 1:
		{
			if (strcmp_P(tagname, PSTR("event")) != 0) cancel(); // invalid tag
		} break;
		case 2:
		{
			if (strcmp_P(tagname, PSTR("requirements")) == 0)
			{
				if (l2 == 1) l2 = 0;
				else cancel(); // closing tag mismatch
			}
			else if (strcmp_P(tagname, PSTR("actions")) == 0)
			{
				if (l2 == 2) l2 = 0;
				else cancel(); // closing tag mismatch
			}
			else cancel(); // invalid tag
		} break;
		case 3:
		{
			switch (l2)
			{
				case 1: // inside requirements
				{
					if (strcmp_P(tagname, PSTR("pack")) == 0)
					{
						if (evt != nullptr)
						{
#ifdef _DEBUG_EVENT_PARSER_
							if (pck != nullptr)
							{
								cli();
								Serial.println(F("Pack appended on event."));
								sei();
							}
#endif

							if (pck != nullptr)
							{
								evt->append(pck);
								pck = nullptr;
							}
							else cancel(); // pack not set (should not be possible for this to happen)
						}
						else cancel(); // event not set (should not be possible for this to happen)
					}
					else cancel(); // invalid tag
				} break;
				case 2: // inside actions
				{
					if (strcmp_P(tagname, PSTR("raise")) == 0)
					{
						if (l3 == 1) l3 = 0;
						else cancel(); // closing tag mismatch
					}
					else if (strcmp_P(tagname, PSTR("expire")) == 0)
					{
						if (l3 == 2) l3 = 0;
						else cancel(); // closing tag mismatch
					}
					else cancel(); // invalid tag
				} break;

				default: // it shouldn't be possible for this to happen
				{
					cancel(); // invalid state
				} break;
			}
		} break;
		case 4:
		{
			switch (l2)
			{
				case 1: // inside requirements
				{
					if (strcmp_P(tagname, PSTR("condition")) == 0)
					{
						if (pck != nullptr)
						{
#ifdef _DEBUG_EVENT_PARSER_
							if (cnd != nullptr)
							{
								cli();
								Serial.println(F("Condition appended on pack."));
								sei();
							}
#endif

							if (cnd != nullptr)
							{
								pck->append(cnd);
								cnd = nullptr;
								comp = nullptr;
								comp_details.reset();
							}
							else cancel(); // condition not set (shouldn't be possible to happen)
						}
						else cancel(); // pack not set (shouldn't be possible to happen)
					}
					else cancel(); // invalid tag
				} break;
				case 2: // inside actions
				{
					if (strcmp_P(tagname, PSTR("write")) == 0)
					{
						comp = nullptr;
						comp_details.reset();
					}
					else cancel(); // invalid tag
				} break;

				default: // it shouldn't be possible for this to happen
				{
					cancel(); // invalid state
				} break;
			}
		} break;
		case 5:
		{
			switch (l2)
			{
				case 1: // inside requirements
				{
					if (cmp != nullptr)
					{
						if (cmp == comparator::resolve(tagname))
						{
							l5 = 0;
							cmp = nullptr;
						}
						else cancel(); // closing tag mismatch or invalid tag
					}
					else cancel(); // comparator not set (shouldn't be possible to happen)
				} break;
				case 2: // inside actions
				{
					if (strcmp_P(tagname, PSTR("value")) == 0)
					{
						if (l5 == 1)
						{
							l5 = 0;
							if (unt.isset()) unt.reset();
							else cancel(); // unit not set (shouldn't be possible to happen)
						}
						else cancel(); // closing tag mismatch
					}
					else if (strcmp_P(tagname, PSTR("state")) == 0)
					{
						if (l5 == 2) l5 = 0;
						else cancel(); // closing tag mismatch
					}
					else cancel(); // invalid tag
				} break;

				default: // it shouldn't be possible for this to happen
				{
					cancel(); // invalid state
				} break;
			}
		} break;

		default:
		{
			cancel(); // invalid nesting level
		} break;
	}

	if (!canceled())
	{
		--level;
		if (level == 0) finish(); // end parsing event
	}
}

event* EventParser::extractEvent() const
{
	return evt;
}
