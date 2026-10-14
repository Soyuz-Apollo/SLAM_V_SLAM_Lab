/*  SceneLib: software for real-time SLAM

    settings.cpp
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

#include "settings.h"

#include <iostream>

using namespace std;

const string Settings::m_empty = "";

/**
Считывает настройки из потока. Функция разбирает поток на набор строк,
которые сохраняются как секции и записи.
*/
void Settings::load(std::istream& stream)
{
  string line;
  string section;
  string entry;
  string data;
  Section* current_section = 0;
  string current_entry;
  
  do
  {
    // Считываем следующую строку.
    if(!get_next_line(stream, line))
      return;

    // Это начало секции?
    section = get_section_name(line);

    if(!section.empty())
    {
      // Создаём новую секцию и считываем её записи.
      // Если секция с таким именем уже есть, используем её
      // и объединяем записи.
      if(m_sections.find(section) != m_sections.end())
        current_section = m_sections[section];
      else
      {
        current_section = new Section(section);
        m_sections[section] = current_section;
      }
      // Это новая секция, значит текущей записи пока нет.
      current_entry = "";
    }
    else
    {
      // Это начало записи?
      entry = get_entry_name(line, data);
      if(!entry.empty())
      {
        // Создаём новую запись и добавляем в неё данные.
        // Есть ли такая запись уже?
        if(current_section->add_entry(entry,data) == false)
        {
          cerr << "Settings Warning: More than one entry with name \"" << entry
               << "\" in section [" << current_section->Label()
               << "]. Only the first will be used." << endl;
          cerr << "Settings Warning: Ignoring data:" << endl << line << endl;
        }
        else
        {
          current_entry = entry;
        }
      }
      else
      {
        // Это должны быть данные, добавляемые к текущей секции и записи,
        // если они есть. Если их нет, значит что-то пошло не так.
        if(current_section == 0 || current_entry == "")
        {
            cerr << "Settings Warning: Ignoring data:" << endl << line << endl;
        }
        else
        {
          current_section->add_line_to_entry(current_entry, line);
        }
      }
    }
  }
  while(stream.good());
}

/**
Деструктор. Освобождает память, выделенную под секции.*/
Settings::~Settings()
{
  for(Settings::StringSectionMap::const_iterator i = m_sections.begin();
      i != m_sections.end(); i++)
  {
    delete i->second;
  }
}

/**
Получает следующую строку из потока, пропуская пустые строки и строки
комментариев, то есть начинающиеся с '#'.
*/
bool Settings::get_next_line(istream& stream, string& line) const
{
  do
  {
    getline(stream, line);
  }
  while((line.empty() || line[0] == '#') && stream.good());
  return stream.good();
}

/**
Получает имя секции из следующей корректной строки потока. Если следующая
корректная строка не содержит имени секции, возвращается пустая строка.
Секция — это строка в квадратных скобках, например <code>[Motion]</code>.
*/
string Settings::get_section_name(const string& line) const
{
  string section;

  if(line[0] == '[')
  {
    // Мы нашли имя секции. Определяем его длину.
    string::size_type end_bracket_idx = line.find_first_of("]");
    if(end_bracket_idx != string::npos)
    {
      section = line.substr(1, end_bracket_idx - 1);
    }
  }

  return section;
}

/**
Получает имя записи из следующей корректной строки потока. Запись — это
строка, начинающаяся в начале строки и заканчивающаяся символом '=',
например <code>Speed=</code>. Остаток строки и все последующие строки,
не содержащие '=', считаются данными этой записи. Если следующая корректная
строка не содержит имени записи, возвращается пустая строка, а data
заполняется строкой из потока.
@param stream Поток для разбора
@param data Строка, заполняемая данными записи: либо данными после '=',
либо всей строкой, если строка не является началом новой записи
@returns Имя записи или пустая строка, если имя не найдено
*/
string Settings::get_entry_name(const string& line, string& data) const
{
  string section;

  // Мы нашли имя записи. Определяем его длину.
  string::size_type equals_idx = line.find_first_of("=");
  if(equals_idx != string::npos)
  {
    section = line.substr(0, equals_idx);
    data = line.substr(equals_idx + 1);
  }

  return section;
}

/**
Возвращает секцию с заданной меткой. Если секции не существует,
возвращается 0.
*/
const Settings::Section* Settings::get_section(const std::string& section) const
{
  map<string, Section*>::const_iterator pos = m_sections.find(section);
  if(pos == m_sections.end())
  {
    return 0;
  }
  else
    return pos->second;
}

/**
Возвращает данные, связанные с конкретной секцией и записью.
*/
const std::string& Settings::get_entry(
  const std::string& section, const std::string& entry) const
{
  const Section* psection = get_section(section);
  if(psection == 0)
    return m_empty;
  else
    return psection->get_entry(entry);
}


/**
Создаёт новую запись в этой секции и добавляет в неё данные. Если запись уже
существует, ничего не делает.
@param entry Создаваемая запись
@param data Данные, добавляемые в новую запись
@returns <code>true</code> при успехе или <code>false</code>, если запись
уже существует.
*/
bool Settings::Section::add_entry(
  const std::string& entry, const std::string& data)
{
  // Есть ли эта запись уже?
  if(m_entries.find(entry) != m_entries.end())
    return false;

  m_entries[entry] = data;

  return true;
}

/**
Добавляет данные к существующей записи с новой строки. Если записи
не существует, ничего не делает.
@param entry Запись, к которой добавляются данные
@param data Добавляемые данные
@returns <code>true</code> при успехе или <code>false</code>, если запись
не существует.
*/
bool Settings::Section::add_line_to_entry(
  const std::string& entry, const std::string& data)
{
  // Есть ли эта запись уже?
  if(m_entries.find(entry) == m_entries.end())
    return false;

  m_entries[entry] += '\n';
  m_entries[entry] += data;

  return true;
}

/**
Возвращает данные, связанные с заданной записью.
*/
const std::string& Settings::Section::get_entry(const std::string& entry) const
{
  map<string, string>::const_iterator pos = m_entries.find(entry);
  if(pos == m_entries.end())
  {
    return m_empty;
  }
  else
    return pos->second;
}

/** Записывает настройки в поток.
@param os Поток
@param x Секция
*/
std::ostream& operator <<(std::ostream& os, const Settings& s)
{
  for(Settings::StringSectionMap::const_iterator i = s.m_sections.begin();
    i != s.m_sections.end(); i++)
  {
    os << *i->second;
    os << endl;
  }
  return os;
}

/** Записывает секцию в поток.
@param os Поток
@param x Секция
*/
std::ostream& operator <<(std::ostream& os, const Settings::Section& s)
{
  os << "[" << s.Label() << "]" << endl;
  for(
    Settings::Section::StringStringMap::const_iterator i = s.m_entries.begin();
      i != s.m_entries.end(); i++)
  {
    os << i->first << "=" << i->second << endl;
  }
  return os;
}
