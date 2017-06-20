/*
 * JNI interface for the EnhancePlus connector - definitions
 */

#ifndef _FGL_ENHANCE_PLUS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Log debug message to the console
 * 
 * @param tag tag to use
 * @param message message to log
 */
extern void FglEnhancePlus_logMessage(const char *str_tag, const char *str_message);

/**
 * Get app's version name
 * 
 * @param out_buffer output buffer for app's version name string
 * @param out_buffer_size size of output buffer in bytes
 */
extern void FglEnhancePlus_getAppVersionName(char *out_buffer, size_t out_buffer_size);

/**
 * Get app's version code
 * 
 * @return version code
 */
extern int FglEnhancePlus_getAppVersionCode();

/**
 * Check the app's target market
 * 
 * @param out_buffer output buffer for target market string (such as googleplay or amazon)
 * @param out_buffer_size size of output buffer in bytes
 */
extern void FglEnhancePlus_getMarket(char *out_buffer, size_t out_buffer_size);

/**
 * Get arbitrary configuration variable, as passed in by Enhance
 * 
 * @param str_varName variable name (case-sensitive)
 * @param out_buffer output buffer for value string
 * @param out_buffer_size size of output buffer in bytes
 */
extern void FglEnhancePlus_getConfigVariable(const char *str_varName, char *out_buffer, size_t out_buffer_size);

/**
 * Update the value of a particular SDK key
 *
 * @param str_id ID of sdk to set key for
 * @param str_key key to set
 * @param str_value new value
 */
extern void FglEnhancePlus_setSdkValue(const char *str_id, const char *str_key, const char *str_value);

/**
 * Set current screen
 * 
 * @param str_name name of the currently displayed screen
 */
extern void FglEnhancePlus_setCurrentScreen(const char *str_name);

/**
 * Get id of sdk that granted the most recent reward
 * 
 * @param out_buffer output buffer for value string
 * @param out_buffer_size size of output buffer in bytes
 */
extern void FglEnhancePlus_getSdkIdForLastReward(char *out_buffer, size_t out_buffer_size);

#ifdef __cplusplus
}
#endif

#endif /* _FGL_ENHANCE_PLUS_H */
