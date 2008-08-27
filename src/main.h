/*
 * Kaolin - Exfoliate your face
 * Copyright (c) 2008 Devin Torres
 * Licensed under the MIT license.
 */
 
#ifndef MAIN_H
#define MAIN_H



/*
 * Class prototypes.
 */
class help;
class room;
class player;
class server;

/*
 * Type defines.
 */
typedef bool (*fun_ptr)(player* const&, const std::string&);

/*
 * External objects.
 */
extern server *s;



#endif // MAIN_H
