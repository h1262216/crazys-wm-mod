/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "utils/DirPath.h"
#include "CLog.h"

/*
 * kind of trivial...
 */
#ifdef LINUX
#include <dirent.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

const char    DirPath::sep[] = "/";

bool DirPath::is_directory() const {
    DIR* dir = opendir(path.c_str());
    if (dir) {
        /* Directory exists. */
        closedir(dir);
        return true;
    } else if (ENOENT == errno) {
       return false;
    } else {
        /* opendir() failed for some other reason. */
        g_LogFile.warning("io", "Error (", errno, ") when opening '", path, "': ", std::strerror(errno));
        return false;
    }
}

#else
#include <shlwapi.h>
const char    DirPath::sep[] = "\\";

bool DirPath::is_directory() const {
    return PathIsDirectoryA(path.c_str());
}

#endif

ImagePath::ImagePath(const std::string& filename)
        : DirPath()
{
    (*this) << "Resources" << "Images" << filename;
}

ButtonPath::ButtonPath(const std::string& filename)
        : DirPath()
{
    (*this) << "Resources" << "Buttons" << filename;
}

namespace {
#if defined(LINUX)

  // look up my current home directory in /etc/passwd (or NIS or
  // wherever).
  std::unique_ptr<std::string> find_pwd_home()
  {
    size_t bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if(bufsize == static_cast<size_t>(-1))
      bufsize = 16384;		// Should be big enough

    struct passwd pwd;
    std::vector<char> buf(bufsize);
    struct passwd* result;

    auto err = getpwuid_r(getuid(), &pwd, buf.data(), buf.size(), &result);
    if(!result)
      {
	if(err == 0)
	  g_LogFile.warning("io", "User has no home directory.");
	else
	  g_LogFile.error("io", "getpwuid_r() failed: ", std::strerror(err), " (", err, ")");

	return {};
      }

    return std::make_unique<std::string>(pwd.pw_dir);
  }

  // Expand "~/some/dir" to "<my home-dir>/some/dir"
  //
  // TODO: Expand "~mike/dir" to "<mike's home-dir>/dir"
  std::string tildeexpand(std::string path)
  {
    if(path.empty())
      return path;

    if(path.substr(0, 2) == "~/")
      if(auto env_home = std::getenv("HOME"))
	return path.replace(0, 1, env_home);
      else if(auto pwd_home = find_pwd_home())
	return path.replace(0, 1, *pwd_home);

    return path;
  }
#endif
}

std::string DirPath::expand_path(std::string path)
{
#if defined(LINUX)
  return tildeexpand(std::move(path));
#else
  return path;
#endif
}
