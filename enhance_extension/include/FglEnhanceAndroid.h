/*
 * JNI interface for the Enhance connector - definitions
 */

#ifndef _FGL_ENHANCE_H

#ifdef __cplusplus
extern "C" {
#endif

/** Reward type */

typedef enum {
	REWARDTYPE_ITEM = 0,
	REWARDTYPE_COINS = 1,
} RewardType;

/** Overlay ad position */

typedef enum {
	OVERLAY_AD_TOP = 0,			// Place at the top, using the entire screen's width
	OVERLAY_AD_BOTTOM = 1		// Place at the bottom, using the entire screen's width
} OverlayAdPosition;

/**
 * Pump Enhance events. Call this every frame from your app thread to receive callbacks
 */
extern void FglEnhance_pumpEvents (void);

/**
 * Check if the Enhance SDK is injected into the host app
 *
 * @param context application context
 *
 * @return true if injected, false if not
 */
extern bool FglEnhance_isEnhanced (void);

/**
 * Request permission from the user to show local notifications
 *
 * @param callback_onInterstitialCompleted function called when an interstitial is completed
 */
extern void FglEnhance_setInterstitialCallback(void(*callback_onInterstitialCompleted)(void));

/**
 * Check if an interstitial ad is ready to show
 *
 * @param str_placement placement to check readiness of, NULL for default placement
 *
 * @return true if ready, false if not
 */
extern bool FglEnhance_isInterstitialReady (const char *str_placement);

/**
 * Show interstitial ad
 *
 * @param str_placement placement to show interstitial for, NULL for default placement
 */
extern void FglEnhance_showInterstitialAd (const char *str_placement);

/**
 * Check if a rewarded ad is ready to show
 *
 * @param str_placement placement to check readiness of, NULL for default placement
 *
 * @return true if ready, false if not
 */
extern bool FglEnhance_isRewardedAdReady(const char *str_placement);

/**
 * Show rewarded ad
 *
 * @param str_placement placement to show rewarded ad for, NULL for default placement
 * @param callback_onRewardGranted function called when a reward is granted
 * @param callback_onRewardDeclined function called when a reward is declined
 * @param callback_onRewardUnavailable function called when a reward isn't available to show
 */
extern void FglEnhance_showRewardedAd(const char *str_placement,
	void (*callback_onRewardGranted)(int rewardValue, RewardType rewardType),
	void (*callback_onRewardDeclined)(void),
	void (*callback_onRewardUnavailable)(void));

/**
 * Set callback for receiving currency, for instance when getting asynchronous rewards from a special offer or offerwall
 *
 * @param callback_onCurrencyGranted method called when currency is granted to the user
 */
extern void FglEnhance_setCurrencyCallback(void(*callback_onCurrencyGranted)(int currencyAmount));

/**
 * Check if a special offer is ready to show
 * 
 * @return true if ready, false if not
 */
extern bool FglEnhance_isSpecialOfferReady(void);

/**
 * Show special offer
 */
extern void FglEnhance_showSpecialOffer(void);

/**
 * Check if an offerwall is ready to show
 * 
 * @return true if ready, false if not
 */
extern bool FglEnhance_isOfferwallReady(void);

/**
 * Show offerwall
 */
extern void FglEnhance_showOfferwall (void);

/**
 * Check if a fullscreen, modal ad is currently showing
 *
 * @return true if currently showing, false if not
 */
extern bool FglEnhance_isFullscreenAdShowing();

/**
 * Check if an overlay is ready to show
 * 
 * @return true if ready, false if not
 */
extern bool FglEnhance_isOverlayAdReady(void);

/**
 * Show overlay ad
 *
 * @param position ad position (top or bottom)
 */
extern void FglEnhance_showOverlayAdWithPosition(OverlayAdPosition position);

/**
 * Show overlay ad
 *
 * @param x X coordinate of top, left of rectangle to show ad at, in screen coordinates
 * @param y Y coordinate of top, left of rectangle to show ad at, in screen coordinates
 * @param width width of rectangle to show ad at, in screen coordinates
 * @param height height of rectangle to show ad at, in screen coordinates
 */
extern void FglEnhance_showOverlayAdWithRect(int x, int y, int width, int height);

/**
 * Hide overlay ad, if one is showing
 */
extern void FglEnhance_hideOverlayAd();

/**
 * Show a new ad in the overlay, if one is showing
 */
extern void FglEnhance_refreshOverlayAd();

/**
 * Log custom event
 *
 * @param str_eventType event type
 * @param str_paramKey optional parameter key, NULL for none
 * @param str_paramValue optional parameter value, NULL for none
 */
extern void FglEnhance_logEvent(const char *str_eventType, const char *str_paramKey, const char *str_paramValue);

/**
 * Request permission from the user to show local notifications
 *
 * @param callback_onPermissionGranted function called when the permission is granted
 * @param callback_onPermissionRefused function called when the permission is declined
 */
extern void FglEnhance_requestLocalNotificationPermission(void(*callback_onPermissionGranted)(void),
                                                          void(*callback_onPermissionRefused)(void));

/**
 * Enable local notifications
 *
 * @param str_title notification title
 * @param str_message notification message
 * @param delay notification delay in seconds
 */
extern void FglEnhance_enableLocalNotification(const char *str_title, const char *str_message, int delay);

/**
 * Disable local notifications
 */
extern void FglEnhance_disableLocalNotification();

#ifdef __cplusplus
}
#endif

#endif /* _FGL_ENHANCE_H */
