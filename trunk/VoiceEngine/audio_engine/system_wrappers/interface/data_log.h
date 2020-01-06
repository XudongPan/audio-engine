
#ifndef VOIP_SYSTEM_WRAPPERS_INTERFACE_DATA_LOG_H_
#define VOIP_SYSTEM_WRAPPERS_INTERFACE_DATA_LOG_H_

#include <string>

#include "audio_engine/system_wrappers/interface/data_log_impl.h"

namespace VoIP {

class DataLog {
 public:
  // Creates a log which uses a separate thread (referred to as the file
  // writer thread) for writing log rows to file.
  //
  // Calls to this function after the log object has been created will only
  // increment the reference counter.
  static int CreateLog();

  // Decrements the reference counter and deletes the log when the counter
  // reaches 0. Should be called equal number of times as successful calls to
  // CreateLog or memory leak will occur.
  static void ReturnLog();

  // Combines the string table_name and the integer table_id into a new string
  // table_name + _ + table_id. The new string will be lower-case.
  static std::string Combine(const std::string& table_name, int table_id);

  // Adds a new table, with the name table_name, and creates the file, with the
  // name table_name + ".txt", to which the table will be written.
  // table_name is treated in a case sensitive way.
  static int AddTable(const std::string& table_name);

  // Adds a new column to a table. The column will be a multi-value-column
  // if multi_value_length is greater than 1.
  // table_name and column_name are treated in a case sensitive way.
  static int AddColumn(const std::string& table_name,
                       const std::string& column_name,
                       int multi_value_length);

  template<class T>
  static int InsertCell(const std::string& table_name,
                        const std::string& column_name,
                        T value) {
    DataLogImpl* data_log = DataLogImpl::StaticInstance();
    if (data_log == NULL)
      return -1;
    return data_log->InsertCell(
             table_name,
             column_name,
             new ValueContainer<T>(value));
  }

  // Inserts an array of values into a table with name table_name at the
  // column specified by column_name, which must be a multi-value-column.
  // Note that the MultiValueContainer makes use of the copy constructor,
  // operator= and operator<< of the type T, and that the template type
  // must implement a deep copy copy constructor and operator=.
  // Copy constructor and operator= must not be disabled for the type T.
  // table_name and column_name are treated in a case sensitive way.
  template<class T>
  static int InsertCell(const std::string& table_name,
                        const std::string& column_name,
                        const T* array,
                        int length) {
    DataLogImpl* data_log = DataLogImpl::StaticInstance();
    if (data_log == NULL)
      return -1;
    return data_log->InsertCell(
             table_name,
             column_name,
             new MultiValueContainer<T>(array, length));
  }

  // For the table with name table_name: Writes the current row to file.
  // Starts a new empty row.
  // table_name is treated in a case-sensitive way.
  static int NextRow(const std::string& table_name);
};

}  // namespace VoIP

#endif  // VOIP_SYSTEM_WRAPPERS_INTERFACE_DATA_LOG_H_
