/*  SceneLib: software for real-time SLAM

    settings.h
    Copyright (C) 2005 University of Oxford

    Author
    Paul Smith
    pas@robots.ox.ac.uk

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <map>
#include <string>
#include <iostream>

/**
Набор настроек из потока, обычно из файла. Этот класс предоставляет операции
для загрузки настроек и доступа к ним.
@ingroup gScene
*/
class Settings
{

public:
  /**
  Секция из конфигурационного потока. Секции обозначаются квадратными
  скобками и состоят из набора записей. Каждая запись начинается с новой
  строки и содержит символ '='.
  */
  class Section
  {
  public:
    Section(std::string& label) : m_label(label) {}

    const std::string& Label() const {return m_label;}
    
    bool add_entry(const std::string& entry, const std::string& data);
    bool add_line_to_entry(const std::string& entry, const std::string& data);
    const std::string& get_entry(const std::string& entry) const;

    friend std::ostream& operator <<(
      std::ostream& os, const Settings::Section& s);
    
    typedef std::map<std::string,std::string> StringStringMap;
  private:
    std::string m_label;
    StringStringMap m_entries;
  };
  typedef std::map<std::string,Section*> StringSectionMap;

public:
  Settings() {};
  Settings(std::istream& stream) {load(stream);}
  ~Settings();

  void load(std::istream& stream);

  const Section* get_section(const std::string& section) const;
  const std::string& get_entry(
    const std::string& section, const std::string& entry) const;

  friend std::ostream& operator <<(std::ostream& os, const Settings& s);

private:
  bool get_next_line(std::istream& stream, std::string& line) const;
  std::string get_section_name(const std::string& line) const;
  std::string get_entry_name(const std::string& line, std::string& data) const;

private:
  StringSectionMap m_sections;
  static const std::string m_empty;
};

std::ostream& operator <<(std::ostream& os, const Settings& s);
std::ostream& operator <<(std::ostream& os, const Settings::Section& s);

#endif
