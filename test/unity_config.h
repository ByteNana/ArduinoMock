#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

void gtest_output_char(char c);
void gtest_output_flush(void);
void gtest_output_start(void);
void gtest_exec_time_start(void);
void gtest_exec_time_stop(void);

#define UNITY_OUTPUT_CHAR(a)     gtest_output_char(a)
#define UNITY_OUTPUT_FLUSH()     gtest_output_flush()
#define UNITY_OUTPUT_START()     gtest_output_start()
#define UNITY_EXEC_TIME_START()  gtest_exec_time_start()
#define UNITY_EXEC_TIME_STOP()   gtest_exec_time_stop()

#endif /* UNITY_CONFIG_H */
