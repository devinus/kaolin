/*
 * Kaolin - Exfoliate your face
 * Copyright (c) 2008 Devin Torres
 * Licensed under the MIT license.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>



/*
 * Maximum buffer length (if changed, recompile the entire project).
 */
static const unsigned short int MAX_BUFFER = 1024;

/*
 * Default mud port.
 */
extern unsigned short int port;

/*
 * Kaolin version string.
 */
extern const std::string CEMENT_VERSION;

/*
 * Directory locations.
 */
extern const std::string LOG_DIR;
extern const std::string HELP_DIR;
extern const std::string PLAYER_DIR;
extern const std::string ZONE_DIR;



#endif // CONFIG_H
