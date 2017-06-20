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

bool Defold_Enhance_isOfferwallReady() {
   return true;
}

void Defold_Enhance_showOfferwall() {
   dmLogInfo("EnhanceDefold[Mac]: show offerwall");
}

bool Defold_Enhance_isSpecialOfferReady() {
   return true;
}

void Defold_Enhance_showSpecialOffer() {
   dmLogInfo("EnhanceDefold[Mac]: show special offer");
}

bool Defold_Enhance_isFullscreenAdShowing() {
   return false;
}

bool Defold_Enhance_isOverlayAdReady() {
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

#endif
