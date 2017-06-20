/*
 * JNI interface for the EnhanceInappPurchases connector - definitions
 */

#ifndef _FGL_ENHANCE_INAPP_PURCHASES_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Check if in-app purchases are supported
 * 
 * @return true if supported, false if not
 */
extern bool FglEnhanceInAppPurchases_isSupported();

/**
 * Attempt purchase
 * 
 * @param str_sku product to purchase
 * @param callback_onPurchaseSuccess function called when the purchase is successful
 * @param callback_onPurchaseFailed function called when the purchase is declined
 */
extern void FglEnhanceInAppPurchases_attemptPurchase(const char *str_sku, void(*callback_onPurchaseSuccess)(void),
													 void(*callback_onPurchaseFailed)(void));

/**
 * Consume purchase
 * 
 * @param str_sku product to consume
 * @param callback_onConsumeSuccess function called when the purchase is successful
 * @param callback_onConsumeFailed function called when the purchase is declined
 */
extern void FglEnhanceInAppPurchases_consume(const char *str_sku, void(*callback_onConsumeSuccess)(void),
											 void(*callback_onConsumeFailed)(void));

/**
 * Get product price
 * 
 * @param str_sku product to get price of
 * @param str_default_price default price, in case the information isn't available
 * @param out_buffer output buffer for value string
 * @param out_buffer_size size of output buffer in bytes
 */
extern void FglEnhanceInAppPurchases_getDisplayPrice(const char *str_sku, const char *str_default_price, char *out_buffer, size_t out_buffer_size);

/**
 * Check if product is owned
 * 
 * @param str_sku product to check
 *
 * @return true if owned, false if not
 */
extern bool FglEnhanceInAppPurchases_isItemOwned(const char *str_sku);

/**
 * Check number of copies owned of the product
 * 
 * @param str_sku product to check
 *
 * @return number of copies owned
 */
extern int FglEnhanceInAppPurchases_getOwnedItemCount(const char *str_sku);

#ifdef __cplusplus
}
#endif

#endif /* _FGL_ENHANCE_INAPP_PURCHASES_H */
