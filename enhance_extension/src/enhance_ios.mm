#if defined(DM_PLATFORM_IOS)

#include "enhance_internal.h"
#include "FglEnhance.h"

static EnhanceListener *g_pInterstitialCompleted;
static EnhanceListener *g_pCurrencyGranted;
static EnhanceListener *g_pRewardGranted;
static EnhanceListener *g_pRewardDeclined;
static EnhanceListener *g_pRewardUnavailable;
static EnhanceListener *g_pPermissionGranted;
static EnhanceListener *g_pPermissionRefused;

@interface DefoldEnhanceDelegate : NSObject<InterstitialDelegate, RewardDelegate, CurrencyGrantedDelegate, PermissionDelegate> {
}
@end

@implementation DefoldEnhanceDelegate;

-(void)onInterstitialCompleted {
   dmLogInfo("EnhanceDefold[iOS]: onInterstitialCompleted");
   if (g_pInterstitialCompleted)
      g_pInterstitialCompleted->callWithNoParam();
}

-(void)onCurrencyGranted:(int)amount {
   dmLogInfo("EnhanceDefold[iOS]: onCurrencyGranted: %d", amount);
   if (g_pCurrencyGranted)
      g_pCurrencyGranted->callWithIntParam(amount);
}

-(void)onRewardGranted:(int)rewardValue rewardType:(RewardType)rewardType {
   dmLogInfo("EnhanceDefold[iOS]: onRewardGranted");
   if (g_pRewardGranted) {
      if (rewardType == REWARDTYPE_COINS)
         g_pRewardGranted->callWithIntStrParam(rewardValue, "REWARDTYPE_COINS");
      else
         g_pRewardGranted->callWithIntStrParam(0, "REWARDTYPE_ITEM");
      
   }
}

-(void)onRewardDeclined {
   dmLogInfo("EnhanceDefold[iOS]: onRewardDeclined");
   if (g_pRewardDeclined) {
      g_pRewardDeclined->callWithNoParam();
   }
}

-(void)onRewardUnavailable {
   dmLogInfo("EnhanceDefold[iOS]: onRewardUnavailable");
   if (g_pRewardUnavailable) {
      g_pRewardUnavailable->callWithNoParam();
   }
}

-(void)onPermissionGranted {
   dmLogInfo("EnhanceDefold[iOS]: onPermissionGranted");
   if (g_pPermissionGranted) {
      g_pPermissionGranted->callWithNoParam();
   }
}

-(void)onPermissionRefused {
   dmLogInfo("EnhanceDefold[iOS]: onPermissionRefused");
   if (g_pPermissionRefused) {
      g_pPermissionRefused->callWithNoParam();
   }
}

@end

static DefoldEnhanceDelegate *g_delegate;

void Defold_Enhance_Initialize() {
   dmLogInfo("EnhanceDefold[iOS]: initialize");
   g_delegate = [[DefoldEnhanceDelegate alloc] init];
}

bool Defold_Enhance_isEnhanced() {
   return [[FglEnhance sharedInstance] isEnhanced];
}

void Defold_Enhance_setInterstitialCallback(EnhanceListener *pCompleted) {
   g_pInterstitialCompleted = pCompleted;
}

void Defold_Enhance_setCurrencyCallback(EnhanceListener *pGranted) {
   g_pCurrencyGranted = pGranted;
}

bool Defold_Enhance_isInterstitialReady(const char *pszPlacement) {
   NSString *placement = [NSString stringWithUTF8String:pszPlacement];
   
   return [[FglEnhance sharedInstance] isInterstitialReady:placement];
}

void Defold_Enhance_showInterstitial(const char *pszPlacement) {
   NSString *placement = [NSString stringWithUTF8String:pszPlacement];
   
   dmLogInfo("EnhanceDefold[iOS]: show interstitial");
   [[FglEnhance sharedInstance] showInterstitial:placement];
}

bool Defold_Enhance_isRewardedAdReady(const char *pszPlacement) {
   NSString *placement = [NSString stringWithUTF8String:pszPlacement];
   
   return [[FglEnhance sharedInstance] isRewardedAdReady];
}

void Defold_Enhance_showRewardedAd(const char *pszPlacement, EnhanceListener *pGranted, EnhanceListener *pDeclined, EnhanceListener *pUnavailable) {
   NSString *placement = [NSString stringWithUTF8String:pszPlacement];
   
   g_pRewardGranted = pGranted;
   g_pRewardDeclined = pDeclined;
   g_pRewardUnavailable = pUnavailable;
   
   dmLogInfo("EnhanceDefold[iOS]: show rewarded ad");
   [[FglEnhance sharedInstance] showRewardedAd:g_delegate placement:placement];
}

bool Defold_Enhance_isOfferwallReady() {
   return [[FglEnhance sharedInstance] isOfferwallReady];
}

void Defold_Enhance_showOfferwall() {
   dmLogInfo("EnhanceDefold[iOS]: show offerwall");
   [[FglEnhance sharedInstance] showOfferwall];
}

bool Defold_Enhance_isSpecialOfferReady() {
   return [[FglEnhance sharedInstance] isSpecialOfferReady];
}

void Defold_Enhance_showSpecialOffer() {
   dmLogInfo("EnhanceDefold[iOS]: show special offer");
   [[FglEnhance sharedInstance] showSpecialOffer];
}

bool Defold_Enhance_isFullscreenAdShowing() {
   return [[FglEnhance sharedInstance] isFullscreenAdShowing];
}

bool Defold_Enhance_isOverlayAdReady() {
   return [[FglEnhance sharedInstance] isOverlayAdReady];
}

void Defold_Enhance_showOverlayAd(const char *pszPosition) {
   Position pos = POSITION_TOP;
   
   dmLogInfo("EnhanceDefold[iOS]: show overlay ad at: %s", pszPosition);
   
   if (pszPosition && !strcasecmp(pszPosition, "bottom"))
      pos = POSITION_BOTTOM;
   [[FglEnhance sharedInstance] showOverlayAd:pos];
}

void Defold_Enhance_hideOverlayAd() {
   dmLogInfo("EnhanceDefold[iOS]: hide overlay ad");
   [[FglEnhance sharedInstance] hideOverlayAd];
}

void Defold_Enhance_logCustomEvent(const char *pszEventName, const char *pszParamKey, const char *pszParamValue) {
   NSString *eventName = [NSString stringWithUTF8String:pszEventName];
   
   if (pszParamKey && pszParamValue) {
      NSString *paramKey = [NSString stringWithUTF8String:pszParamKey];
      NSString *paramValue = [NSString stringWithUTF8String:pszParamValue];
      
      [[FglEnhance sharedInstance] logEvent:eventName withParameter:paramKey andValue:paramValue];
   }
   else {
      [[FglEnhance sharedInstance] logEvent:eventName];
   }
}

void Defold_Enhance_requestLocalNotificationPermission(EnhanceListener *pGranted, EnhanceListener *pRefused) {
   g_pPermissionGranted = pGranted;
   g_pPermissionRefused = pRefused;
   
   [[FglEnhance sharedInstance] requestLocalNotificationPermission:g_delegate];
}

void Defold_Enhance_enableLocalNotification(const char *pszTitle, const char *pszMessage, int delay) {
   NSString *title = [NSString stringWithUTF8String:pszTitle];
   NSString *msg = [NSString stringWithUTF8String:pszMessage];
   
   dmLogInfo("EnhanceDefold[iOS]: enable local notification title: %s message: %s delay:%s", pszTitle, pszMessage, delay);
   [[FglEnhance sharedInstance] enableLocalNotificationWithTitle:title message:msg delay:delay];
}

void Defold_Enhance_disableLocalNotification() {
   dmLogInfo("EnhanceDefold[iOS]: disable local notification");
   [[FglEnhance sharedInstance] disableLocalNotification];
}

void Defold_Enhance_pumpEvents() {

}

#endif
