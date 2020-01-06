
// This is the pure C wrapper of the DataLog class.

#include "audio_engine/system_wrappers/interface/data_log_c.h"

#include <string>

#include "audio_engine/system_wrappers/interface/data_log.h"

extern "C" int VoipDataLog_CreateLog() {
  return VoIP::DataLog::CreateLog();
}

extern "C" void VoipDataLog_ReturnLog() {
  return VoIP::DataLog::ReturnLog();
}

extern "C" char* VoipDataLog_Combine(char* combined_name, size_t combined_len,
                                       const char* table_name, int table_id) {
  if (!table_name) return NULL;
  std::string combined = VoIP::DataLog::Combine(table_name, table_id);
  if (combined.size() >= combined_len) return NULL;
  std::copy(combined.begin(), combined.end(), combined_name);
  combined_name[combined.size()] = '\0';
  return combined_name;
}

extern "C" int VoipDataLog_AddTable(const char* table_name) {
  if (!table_name) return -1;
  return VoIP::DataLog::AddTable(table_name);
}

extern "C" int VoipDataLog_AddColumn(const char* table_name,
                                       const char* column_name,
                                       int multi_value_length) {
  if (!table_name || !column_name) return -1;
  return VoIP::DataLog::AddColumn(table_name, column_name,
                                    multi_value_length);
}

extern "C" int VoipDataLog_InsertCell_int(const char* table_name,
                                            const char* column_name,
                                            int value) {
  if (!table_name || !column_name) return -1;
  return VoIP::DataLog::InsertCell(table_name, column_name, value);
}

extern "C" int VoipDataLog_InsertArray_int(const char* table_name,
                                             const char* column_name,
                                             const int* values,
                                             int length) {
  if (!table_name || !column_name) return -1;
  return VoIP::DataLog::InsertCell(table_name, column_name, values, length);
}

extern "C" int VoipDataLog_InsertCell_float(const char* table_name,
                                              const char* column_name,
                                              float value) {
  if (!table_name || !column_name) return -1;
  return VoIP::DataLog::InsertCell(table_name, column_name, value);
}

extern "C" int VoipDataLog_InsertArray_float(const char* table_name,
                                               const char* column_name,
                                               const float* values,
                                               int length) {
  if (!table_name || !column_name) return -1;
  return VoIP::DataLog::InsertCell(table_name, column_name, values, length);
}

extern "C" int VoipDataLog_InsertCell_double(const char* table_name,
                                               const char* column_name,
                                               double value) {
  if (!table_name || !column_name) return -1;
  return VoIP::DataLog::InsertCell(table_name, column_name, value);
}

extern "C" int VoipDataLog_InsertArray_double(const char* table_name,
                                                const char* column_name,
                                                const double* values,
                                                int length) {
  if (!table_name || !column_name) return -1;
  return VoIP::DataLog::InsertCell(table_name, column_name, values, length);
}

extern "C" int VoipDataLog_InsertCell_int32(const char* table_name,
                                              const char* column_name,
                                              int32_t value) {
  if (!table_name || !column_name) return -1;
  return VoIP::DataLog::InsertCell(table_name, column_name, value);
}

extern "C" int VoipDataLog_InsertArray_int32(const char* table_name,
                                               const char* column_name,
                                               const int32_t* values,
                                               int length) {
  if (!table_name || !column_name) return -1;
  return VoIP::DataLog::InsertCell(table_name, column_name, values, length);
}

extern "C" int VoipDataLog_InsertCell_uint32(const char* table_name,
                                               const char* column_name,
                                               uint32_t value) {
  if (!table_name || !column_name) return -1;
  return VoIP::DataLog::InsertCell(table_name, column_name, value);
}

extern "C" int VoipDataLog_InsertArray_uint32(const char* table_name,
                                                const char* column_name,
                                                const uint32_t* values,
                                                int length) {
  if (!table_name || !column_name) return -1;
  return VoIP::DataLog::InsertCell(table_name, column_name, values, length);
}

extern "C" int VoipDataLog_InsertCell_int64(const char* table_name,
                                              const char* column_name,
                                              int64_t value) {
  if (!table_name || !column_name) return -1;
  return VoIP::DataLog::InsertCell(table_name, column_name, value);
}

extern "C" int VoipDataLog_InsertArray_int64(const char* table_name,
                                               const char* column_name,
                                               const int64_t* values,
                                               int length) {
  if (!table_name || !column_name) return -1;
  return VoIP::DataLog::InsertCell(table_name, column_name, values, length);
}

extern "C" int VoipDataLog_NextRow(const char* table_name) {
  if (!table_name) return -1;
  return VoIP::DataLog::NextRow(table_name);
}
