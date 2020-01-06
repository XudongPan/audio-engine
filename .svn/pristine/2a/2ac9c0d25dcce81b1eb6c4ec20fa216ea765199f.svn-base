
#ifndef SRC_SYSTEM_WRAPPERS_INTERFACE_DATA_LOG_C_H_
#define SRC_SYSTEM_WRAPPERS_INTERFACE_DATA_LOG_C_H_

#include <stddef.h>  // size_t

#include "audio_engine/include/typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

// All char* parameters in this file are expected to be null-terminated
// character sequences.
int VoipDataLog_CreateLog();
void VoipDataLog_ReturnLog();
char* VoipDataLog_Combine(char* combined_name, size_t combined_len,
                            const char* table_name, int table_id);
int VoipDataLog_AddTable(const char* table_name);
int VoipDataLog_AddColumn(const char* table_name, const char* column_name,
                            int multi_value_length);

int VoipDataLog_InsertCell_int(const char* table_name,
                                 const char* column_name,
                                 int value);
int VoipDataLog_InsertArray_int(const char* table_name,
                                  const char* column_name,
                                  const int* values,
                                  int length);
int VoipDataLog_InsertCell_float(const char* table_name,
                                   const char* column_name,
                                   float value);
int VoipDataLog_InsertArray_float(const char* table_name,
                                    const char* column_name,
                                    const float* values,
                                    int length);
int VoipDataLog_InsertCell_double(const char* table_name,
                                    const char* column_name,
                                    double value);
int VoipDataLog_InsertArray_double(const char* table_name,
                                     const char* column_name,
                                     const double* values,
                                     int length);
int VoipDataLog_InsertCell_int32(const char* table_name,
                                   const char* column_name,
                                   int32_t value);
int VoipDataLog_InsertArray_int32(const char* table_name,
                                    const char* column_name,
                                    const int32_t* values,
                                    int length);
int VoipDataLog_InsertCell_uint32(const char* table_name,
                                    const char* column_name,
                                    uint32_t value);
int VoipDataLog_InsertArray_uint32(const char* table_name,
                                     const char* column_name,
                                     const uint32_t* values,
                                     int length);
int VoipDataLog_InsertCell_int64(const char* table_name,
                                   const char* column_name,
                                   int64_t value);
int VoipDataLog_InsertArray_int64(const char* table_name,
                                    const char* column_name,
                                    const int64_t* values,
                                    int length);

int VoipDataLog_NextRow(const char* table_name);

#ifdef __cplusplus
}  // end of extern "C"
#endif

#endif  // SRC_SYSTEM_WRAPPERS_INTERFACE_DATA_LOG_C_H_  // NOLINT
