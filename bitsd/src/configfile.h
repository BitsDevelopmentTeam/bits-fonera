/***************************************************************************
 *   Copyright (C) 2010 by Terraneo Federico                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   As a special exception, if other files instantiate templates or use   *
 *   macros or inline functions from this file, or you compile this file   *
 *   and link it with other works to produce a work based on this file,    *
 *   this file does not by itself cause the resulting work to be covered   *
 *   by the GNU General Public License. However the source code for this   *
 *   file must still be made available in accordance with the GNU General  *
 *   Public License. This exception does not invalidate any other reasons  *
 *   why a work based on this file might be covered by the GNU General     *
 *   Public License.                                                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <map>
#include <string>

/**
 * Handles a simple config file with the syntax
 * option=value
 */
class ConfigFile
{
public:

	/**
	 * Constructor
	 * \param configfile file name + path of configuration file.
	 * \throws if file not found
	 */
	ConfigFile(std::string configfile);

	/**
	 * \return true if the option is present in the file
	 */
	bool hasOption(std::string option);
	
	/**
	 * \param option option name in the config file
	 * \return the option value as string
	 * \throws if option not present
	 */
	std::string getString(std::string option);

private:
	std::map<std::string,std::string> options;
};

#endif //CONFIGFILE_H
