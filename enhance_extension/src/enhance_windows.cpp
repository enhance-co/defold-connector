#if defined(DM_PLATFORM_WINDOWS)

#include <stdio.h>
#include "enhance_internal.h"

EnhanceListener *g_callback_interstitial;
EnhanceListener *g_callback_offerwall;

void Defold_Enhance_Initialize() {
    dmLogInfo("Enhance: init");
}

void Defold_Enhance_pumpEvents() {}

bool Defold_Enhance_isEnhanced() {
    return false;
}

void Defold_Enhance_setInterstitialCallback(EnhanceListener *pCompleted) {
    dmLogInfo("Enhance: set interstitial callback")
    g_callback_interstitial = pCompleted;
}

void Defold_Enhance_setCurrencyCallback(EnhanceListener *pGranted) {
    dmLogInfo("Enhance: set received currency callback");
    g_callback_offerwall = pGranted;
}

bool Defold_Enhance_isInterstitialReady(const char *pszPlacement) {
    return true;
}

void Defold_Enhance_showInterstitial(const char *pszPlacement) {
    dmLogInfo("Enhance: show interstitial, placement: %s", pszPlacement);

    if (g_callback_interstitial)
        g_callback_interstitial->callWithNoParam();
}

bool Defold_Enhance_isRewardedAdReady(const char *pszPlacement) {
    return true;
}

void Defold_Enhance_showRewardedAd(const char *pszPlacement, EnhanceListener *pGranted, EnhanceListener *pDeclined, EnhanceListener *pUnavailable) {
    dmLogInfo("Enhance: show rewarded ad, placement: %s", pszPlacement);
    
    if(pGranted)
        pGranted->callWithIntStrParam(0, "REWARDTYPE_ITEM");
}

bool Defold_Enhance_isOfferwallReady(const char *pszPlacement) {
    return true;
}

void Defold_Enhance_showOfferwall(const char *pszPlacement) {
    dmLogInfo("Enhance: show offerwall, placement: %s", pszPlacement);

    if(g_callback_offerwall)
        g_callback_offerwall->callWithIntParam(99);
}

bool Defold_Enhance_isSpecialOfferReady(const char *pszPlacement) {
    return true;
}

void Defold_Enhance_showSpecialOffer(const char *pszPlacement) {
    dmLogInfo("Enhance: show special offer, placement: %s", pszPlacement);
}

bool Defold_Enhance_isBannerAdReady(const char *pszPlacement) {
    return true;
}

bool Defold_Enhance_isFullscreenAdShowing() {
    return false;
}

void Defold_Enhance_showBannerAd(const char *pszPlacement, const char *pszPosition) {
    dmLogInfo("Enhance: show banner ad, position: %s, placement: %s", pszPosition, pszPlacement);
}

void Defold_Enhance_hideBannerAd() {
    dmLogInfo("Enhance: hide banner ad");
}

void Defold_Enhance_logCustomEvent(const char *pszEventName, const char *pszParamKey, const char *pszParamValue) {
    if (pszParamKey && pszParamValue) {
        dmLogInfo("Enhance: log event: %s with param key: %s and value: %s", pszEventName, pszParamKey, pszParamValue);
    }
    else {
        dmLogInfo("Enhance: log event: %s", pszEventName);
    }
}

void Defold_Enhance_requestLocalNotificationPermission(EnhanceListener *pGranted, EnhanceListener *pRefused) {
    dmLogInfo("Enhance: request local notification permission");

    if(pGranted)
        pGranted->callWithNoParam();
}

void Defold_Enhance_enableLocalNotification(const char *pszTitle, const char *pszMessage, int delay) {
    dmLogInfo("Enhance: enable local notification with title: %s message: %s delay: %d", pszTitle, pszMessage, delay);
}

void Defold_Enhance_disableLocalNotification() {
    dmLogInfo("Enhance: disable local notification");
}

void Defold_Enhance_requiresDataConsentOptIn(EnhanceListener *callback_onServiceOptInRequirement) {
    if(callback_onServiceOptInRequirement)
        callback_onServiceOptInRequirement->callWithIntParam(0);
}

void Defold_Enhance_serviceTermsOptIn(const char *str_sdks) {
    dmLogInfo("Enhance: service terms opt in");
}

void Defold_Enhance_showServiceOptInDialogs(const char *str_sdks, EnhanceListener *callback_onDialogComplete) {
    dmLogInfo("Enhance: show service opt in dialogs");
}

void Defold_Enhance_serviceTermsOptOut() {
    dmLogInfo("Enhance: service terms opt out");
}

bool Defold_EnhanceInAppPurchases_isSupported() {
    return true;
}

void Defold_EnhanceInAppPurchases_attemptPurchase(const char *str_sku, EnhanceListener *callback_onPurchaseSuccess, EnhanceListener *callback_onPurchaseFailed) {
    dmLogInfo("Enhance: attempt purchase, product name: %s", str_sku);

    if(callback_onPurchaseSuccess)
        callback_onPurchaseSuccess->callWithNoParam();
}

void Defold_EnhanceInAppPurchases_consume(const char *str_sku, EnhanceListener *callback_onConsumeSuccess, EnhanceListener *callback_onConsumeFailed) {
    dmLogInfo("Enhance: consume purchase, product name: %s", str_sku);

    if(callback_onConsumeSuccess)
        callback_onConsumeSuccess->callWithNoParam();
}

const char* Defold_EnhanceInAppPurchases_getDisplayPrice(const char *str_sku, const char *str_default_price) {
    return str_default_price;
}

bool Defold_EnhanceInAppPurchases_isItemOwned(const char *str_sku) {
    return false;
}

int Defold_EnhanceInAppPurchases_getOwnedItemCount(const char *str_sku) {
    return 0;
}

void Defold_EnhanceInAppPurchases_manuallyRestorePurchases(EnhanceListener *callback_onRestoreSuccess, EnhanceListener *callback_onRestoreFailed) {
    dmLogInfo("Enhance: manually restore purchases");

    if(callback_onRestoreSuccess)
        callback_onRestoreSuccess->callWithNoParam();
}

const char* Defold_EnhanceInAppPurchases_getDisplayTitle(const char *str_sku, const char *str_default_title) {
    return str_default_title;
}

const char* Defold_EnhanceInAppPurchases_getDisplayDescription(const char *str_sku, const char *str_default_description) {
    return str_default_description;
}

#endif
