#if defined(DM_PLATFORM_OSX)

#include <stdio.h>
#include "enhance_internal.h"

static EnhanceListener *g_pInterstitialCompleted;

void Defold_Enhance_Initialize() {
   dmLogInfo("EnhanceDefold[Mac]: initialize");
}

bool Defold_Enhance_isEnhanced() {
   return false;
}

void Defold_Enhance_setInterstitialCallback(EnhanceListener *pCompleted) {
   g_pInterstitialCompleted = pCompleted;
}

void Defold_Enhance_setCurrencyCallback(EnhanceListener *pGranted) {
}

bool Defold_Enhance_isInterstitialReady(const char *pszPlacement) {
   return true;
}

void Defold_Enhance_showInterstitial(const char *pszPlacement) {
   dmLogInfo("Enhance stub[Mac]: show interstitial with placement: %s", pszPlacement);
   if (g_pInterstitialCompleted)
      g_pInterstitialCompleted->callWithNoParam();
}

bool Defold_Enhance_isRewardedAdReady(const char *pszPlacement) {
   return true;
}

void Defold_Enhance_showRewardedAd(const char *pszPlacement, EnhanceListener *pGranted, EnhanceListener *pDeclined, EnhanceListener *pUnavailable) {
   dmLogInfo("EnhanceDefold[Mac]: show rewarded ad with placement: %s", pszPlacement);
   pGranted->callWithIntStrParam(0, "REWARD_ITEM");
}

bool Defold_Enhance_isOfferwallReady(const char *pszPlacement) {
   return true;
}

void Defold_Enhance_showOfferwall(const char *pszPlacement) {
   dmLogInfo("EnhanceDefold[Mac]: show offerwall at: %s", pszPlacement);
}

bool Defold_Enhance_isSpecialOfferReady(const char *pszPlacement) {
   return true;
}

void Defold_Enhance_showSpecialOffer(const char *pszPlacement) {
   dmLogInfo("EnhanceDefold[Mac]: show special offer at: %s", pszPlacement);
}

bool Defold_Enhance_isFullscreenAdShowing() {
   return false;
}

bool Defold_Enhance_isOverlayAdReady() {
   return false;
}

bool Defold_Enhance_isBannerAdReady(const char *pszPlacement) {
   return false;
}

void Defold_Enhance_showOverlayAd(const char *pszPosition) {
   dmLogInfo("EnhanceDefold[Mac]: show overlay ad at: %s", pszPosition);
}

void Defold_Enhance_hideOverlayAd() {
   dmLogInfo("EnhanceDefold[Mac]: hide overlay ad");
}

void Defold_Enhance_logCustomEvent(const char *pszEventName, const char *pszParamKey, const char *pszParamValue) {
   if (pszParamKey && pszParamValue) {
      dmLogInfo("EnhanceDefold[Mac]: log event: %s with %s=%s", pszEventName, pszParamKey, pszParamValue);
   }
   else {
      dmLogInfo("EnhanceDefold[Mac]: log event: %s", pszEventName);
   }
}

void Defold_Enhance_requestLocalNotificationPermission(EnhanceListener *pGranted, EnhanceListener *pRefused) {
   pGranted->callWithNoParam();
}

void Defold_Enhance_enableLocalNotification(const char *pszTitle, const char *pszMessage, int delay) {
   dmLogInfo("EnhanceDefold[Mac]: enable local notification title: %s message: %s delay: %d", pszTitle, pszMessage, delay);
}

void Defold_Enhance_disableLocalNotification() {
   dmLogInfo("EnhanceDefold[Mac]: disable local notification");
}

void Defold_Enhance_showBannerAd(const char *pszPlacement, const char *pszPosition) {
  dmLogInfo("EnhanceDefold[Mac]: show banner ad at : %s %s",pszPlacement, pszPosition);
}

void Defold_Enhance_hideBannerAd() {
   dmLogInfo("EnhanceDefold[Mac]: hide banner ad");
}

void Defold_Enhance_pumpEvents() {

}


bool Defold_EnhanceInAppPurchases_isSupported() {
   return false;
}

void Defold_EnhanceInAppPurchases_attemptPurchase(const char *str_sku, EnhanceListener *callback_onPurchaseSuccess, EnhanceListener *callback_onPurchaseFailed) {
	callback_onPurchaseSuccess->callWithNoParam();  
	dmLogInfo("EnhanceDefold[Mac]: attemptPurchase");
}

void Defold_EnhanceInAppPurchases_consume(const char *str_sku, EnhanceListener *callback_onConsumeSuccess, EnhanceListener *callback_onConsumeFailed) {
   dmLogInfo("EnhanceDefold[Mac]: consume");

}

const char*  Defold_EnhanceInAppPurchases_getDisplayPrice(const char *str_sku, const char *str_default_price) {
   dmLogInfo("EnhanceDefold[Mac]: getDisplayPrice");
   return "0.0";
}

bool Defold_EnhanceInAppPurchases_isItemOwned(const char *str_sku) {
   return false;
}

int Defold_EnhanceInAppPurchases_getOwnedItemCount(const char *str_sku) {
   return 0;
}

void Defold_EnhanceInAppPurchases_restorePurchases(EnhanceListener *pSuccess, EnhanceListener *pFailed){
   dmLogInfo("EnhanceDefold[Mac]: restorePurchases");
}


#endif
